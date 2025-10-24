#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spmv.h" // 包含自己的头文件

// 生成随机CSR矩阵
void generate_csr(csr_matrix* A, int size, double density) {
    A->num_rows = size;
    A->num_cols = size;
    A->num_non_zeros = 0;
    A->row_ptr = (int*)calloc(size + 1, sizeof(int));
    
    long long estimated_nnz = (long long)(size * size * density);
    if (estimated_nnz == 0) estimated_nnz = 1;

    A->values = (double*)malloc(estimated_nnz * sizeof(double));
    A->col_indices = (int*)malloc(estimated_nnz * sizeof(int));

    int nnz_count = 0;
    A->row_ptr[0] = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if ((double)rand() / RAND_MAX < density) {
                if (nnz_count < estimated_nnz) {
                    A->values[nnz_count] = (double)rand() / RAND_MAX;
                    A->col_indices[nnz_count] = j;
                    nnz_count++;
                }
            }
        }
        A->row_ptr[i+1] = nnz_count;
    }
    A->num_non_zeros = nnz_count;
}

// 串行SpMV实现
void spmv_serial_csr(const csr_matrix* A, const double* x, double* y) {
    for (int i = 0; i < A->num_rows; ++i) {
        double sum = 0.0;
        for (int j = A->row_ptr[i]; j < A->row_ptr[i+1]; ++j) {
            sum += A->values[j] * x[A->col_indices[j]];
        }
        y[i] = sum;
    }
}

// 释放CSR矩阵内存
void free_csr(csr_matrix* A) {
    free(A->values);
    free(A->col_indices);
    free(A->row_ptr);
}