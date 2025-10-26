#include "../inc/spmv_pthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h> // 用于 sqrt

// 策略2：动态调度 "工人" 函数
void* spmv_worker_dynamic(void* arg) {
    spmv_thread_dynamic_t* data = (spmv_thread_dynamic_t*)arg;
    const csr_matrix* A = data->A;
    const double* x = data->x;
    
    int my_row;

    // 【1：实现 worker 逻辑 】
    // 这是一个 "任务池" 模型。
    // 线程需要在一个循环中不断地 "领取" 任务（行号），直到所有行都被处理完毕。
    while (1) {
        
        // --- 1. 领取任务（临界区） ---
        // 【加锁】
        pthread_mutex_lock(data->mutex);

        // 从共享计数器获取当前行号
        my_row = *(data->next_row_ptr);
        // 更新共享计数器
        (*(data->next_row_ptr))++;
        
        // 【解锁 】
        pthread_mutex_unlock(data->mutex);


        // --- 2. 检查任务是否已领完 ---
        if (my_row >= data->N_rows) {
            break; // 所有行都处理完了，退出循环
        }

        // --- 3. 执行任务 ---
        // (这部分与静态版本 worker 相同)
        double sum = 0.0;
        for (int j = A->row_ptr[my_row]; j < A->row_ptr[my_row+1]; ++j) {
            sum += A->values[j] * x[A->col_indices[j]];
        }
        data->y[my_row] = sum;
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "用法: %s <matrix_size> <density> <num_threads>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    double DENSITY = atof(argv[2]);
    int T = atoi(argv[3]);
    
    if (N <= 0 || DENSITY <= 0 || DENSITY > 1 || T <= 0) {
        fprintf(stderr, "错误：参数无效 (Invalid arguments).\n");
        return 1;
    }

    printf("SpMV 动态调度策略\n");
    printf("Matrix size: %d, Density: %f, Threads: %d\n", N, DENSITY, T);

    csr_matrix A;
    double* x = (double*)malloc(N * sizeof(double));
    double* y_parallel = (double*)malloc(N * sizeof(double));
    double* y_serial = (double*)malloc(N * sizeof(double));

    srand(0);
    generate_csr(&A, N, DENSITY);
    init_vector(x, N);

    pthread_t threads[T];
    spmv_thread_dynamic_t thread_data[T];

    // 【2：设置共享资源 】
    // 1. 共享的 "下一个行" 计数器
    int next_row = 0;
    // 2. 共享的互斥锁
    pthread_mutex_t mutex;
    
    // 【初始化互斥锁】
    pthread_mutex_init(&mutex, NULL);


    // 任务划分：所有线程共享相同的资源
    for (int i = 0; i < T; i++) {
        thread_data[i].A = &A;
        thread_data[i].x = x;
        thread_data[i].y = y_parallel;
        thread_data[i].N_rows = N;
        thread_data[i].next_row_ptr = &next_row; // 传入 *地址*
        thread_data[i].mutex = &mutex;         // 传入 *地址*
    }
      
    struct timeval start, end;
    gettimeofday(&start, NULL);
  
    // 【创建 (Fork) 线程】
    // (传入 spmv_worker_dynamic)  
    for (int i = 0; i < T; i++) {
        pthread_create(&threads[i], NULL, spmv_worker_dynamic, (void*)&thread_data[i]);    
    }

    // 【同步 (Join) 线程 】  
    for (int i = 0; i < T; i++) {  
        pthread_join(threads[i], NULL);  
    }
    
    gettimeofday(&end, NULL);
    printf("并行计算完成 (Parallel SpMV completed).\n");
    
    // 【** 你的任务 5：销毁互斥锁 **】
    pthread_mutex_destroy(&mutex);  

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("运行时间 (Elapsed time): %.6f seconds\n", elapsed);

    // 性能计算
    double gflops = (2.0 * A.num_non_zeros) / (elapsed * 1e9);  
    printf("性能 (Performance): %.2f GFLOPS\n", gflops);

    // 验证
    printf("正在验证 (Verifying)... \n");
    spmv_serial_csr(&A, x, y_serial);
    double error = 0.0;
    for (int i = 0; i < N; ++i) {
        double diff = y_parallel[i] - y_serial[i];
        error += diff * diff;
    }
    printf("验证误差 (Frobenius norm): %.6e\n", sqrt(error));
  
    // 清理
    free_csr(&A);
    free(x);
    free(y_parallel);
    free(y_serial);

    return 0;
}
  