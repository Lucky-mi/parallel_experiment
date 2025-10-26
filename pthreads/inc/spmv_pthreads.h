#ifndef SPMV_PTHREADS_H
#define SPMV_PTHREADS_H

#include <pthread.h>

// --- CSR 矩阵结构体 (从 chuanxing/inc/spmv.h 复制而来) ---

// CSR 稀疏矩阵结构体定义
typedef struct {
    int num_rows;
    int num_cols;
    int num_non_zeros;
    double* values;
    int* col_indices;
    int* row_ptr;
} csr_matrix;

// --- 辅助函数 (在 spmv_utils.c 中实现) ---

// 函数声明
void generate_csr(csr_matrix* A, int size, double density);
void spmv_serial_csr(const csr_matrix* A, const double* x, double* y);
void free_csr(csr_matrix* A);
void init_vector(double* x, int N); // 初始化向量

// --- 策略1：静态行块的线程数据 ---
typedef struct {
    const csr_matrix* A;
    const double* x;
    double* y;
    int start_row; // 负责的起始行
    int end_row;   // 负责的结束行 (不包含)
} spmv_thread_static_t;


// --- 策略2：动态调度的线程数据 ---
typedef struct {
    const csr_matrix* A;
    const double* x;
    double* y;
    int N_rows;              // 总行数
    int* next_row_ptr;       // 指向下一个待处理行索引的 *共享指针*
    pthread_mutex_t* mutex;  // 指向 *同一个* 互斥锁
} spmv_thread_dynamic_t;


#endif // SPMV_PTHREADS_H