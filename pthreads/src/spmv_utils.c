#include "../inc/spmv_pthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 生成随机CSR矩阵
void generate_csr(csr_matrix* A, int size, double density) {
    A->num_rows = size;
    A->num_cols = size;
    A->num_non_zeros = 0;
    // 使用 calloc 自动将 row_ptr 初始化为 0
    A->row_ptr = (int*)calloc(size + 1, sizeof(int));
    
    long long estimated_nnz = (long long)(size * size * density);
    if (estimated_nnz == 0) estimated_nnz = 1; // 至少分配1个

    // 预留一些空间，防止 realloc 过于频繁
    long long capacity = (long long)(estimated_nnz * 1.2 + 100);
    A->values = (double*)malloc(capacity * sizeof(double));
    A->col_indices = (int*)malloc(capacity * sizeof(int));

    if (A->row_ptr == NULL || A->values == NULL || A->col_indices == NULL) {
        fprintf(stderr, "Error: 内存分配失败 (Memory allocation failed).\n");
        exit(1);
    }

    int nnz_count = 0;
    A->row_ptr[0] = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if ((double)rand() / RAND_MAX < density) {
                // 动态扩容检查
                if (nnz_count >= capacity) {
                    capacity = (long long)(capacity * 1.5);
                    A->values = (double*)realloc(A->values, capacity * sizeof(double));
                    A->col_indices = (int*)realloc(A->col_indices, capacity * sizeof(int));
                     if (A->values == NULL || A->col_indices == NULL) {
                        fprintf(stderr, "Error: 内存重新分配失败 (Memory reallocation failed).\n");
                        exit(1);
                    }
                }
                A->values[nnz_count] = (double)rand() / RAND_MAX;
                A->col_indices[nnz_count] = j;
                nnz_count++;
            }
        }
        A->row_ptr[i+1] = nnz_count;
    }
    A->num_non_zeros = nnz_count;

    // 缩减内存至实际大小
    A->values = (double*)realloc(A->values, nnz_count * sizeof(double));
    A->col_indices = (int*)realloc(A->col_indices, nnz_count * sizeof(int));
}

// 串行SpMV实现 (用于验证)
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


// 初始化向量 (从 gemm_row.c 复制而来)
void init_vector(double* x, int N) {
    for (int i = 0; i < N; ++i) {
        x[i] = (double)rand() / RAND_MAX;
    }
}