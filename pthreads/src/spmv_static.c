#include "../inc/spmv_pthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h> // 用于 sqrt

// 策略1：静态行块 "工人" 函数
void* spmv_worker_static(void* arg) {
    spmv_thread_static_t* data = (spmv_thread_static_t*)arg;
    const csr_matrix* A = data->A;
    const double* x = data->x;
    
    // 【** 你的任务 1：实现 worker 逻辑 **】
    // 遍历分配给这个线程的行 (从 data->start_row 到 data->end_row)
    // 对于每一行 i，计算 y[i] 的值
    // (提示：内部逻辑与 spmv_serial_csr 的外层循环内部完全相同)
    
    // (我已经帮你填好了，逻辑和 gemm_row 一样)
    for (int i = data->start_row; i < data->end_row; ++i) {
        double sum = 0.0;
        for (int j = A->row_ptr[i]; j < A->row_ptr[i+1]; ++j) {
            sum += A->values[j] * x[A->col_indices[j]];
        }
        data->y[i] = sum;
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

    printf("SpMV 静态行块策略\n");
    printf("Matrix size: %d, Density: %f, Threads: %d\n", N, DENSITY, T);

    csr_matrix A;
    double* x = (double*)malloc(N * sizeof(double));
    double* y_parallel = (double*)malloc(N * sizeof(double));
    double* y_serial = (double*)malloc(N * sizeof(double));

    srand(0);
    generate_csr(&A, N, DENSITY);
    init_vector(x, N);

    pthread_t threads[T];
    spmv_thread_static_t thread_data[T];

    // 【** 你的任务 2：实现任务划分 **】
    // 计算每个线程的 start_row 和 end_row。
    // (提示：这和 gemm_row.c 中的任务划分逻辑完全一样)
    int rows_per_thread = N / T;
    for (int i = 0; i < T; i++) {
        thread_data[i].A = &A;
        thread_data[i].x = x;
        thread_data[i].y = y_parallel;
        thread_data[i].start_row = i * rows_per_thread;
        
        if (i == T - 1) {
            thread_data[i].end_row = N; // 最后一个线程包办余下的行
        } else {
            thread_data[i].end_row = (i + 1) * rows_per_thread;
        }
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 【** 你的任务 3：创建 (Fork) 线程 **】
    // (传入 spmv_worker_static)
    for (int i = 0; i < T; i++) {
        pthread_create(&threads[i], NULL, spmv_worker_static, (void*)&thread_data[i]);
    }

    // 【** 你的任务 4：同步 (Join) 线程 **】
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("并行计算完成 (Parallel SpMV completed).\n");
    printf("运行时间 (Elapsed time): %.6f seconds\n", elapsed);

    // 性能计算 (SpMV 的 GFLOPS 定义为 2 * nnz)
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
