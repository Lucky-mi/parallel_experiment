#ifndef SPMV_H
#define SPMV_H

// CSR 稀疏矩阵结构体定义
typedef struct {
    int num_rows;
    int num_cols;
    int num_non_zeros;
    double* values;
    int* col_indices;
    int* row_ptr;
} csr_matrix;

// 函数声明
void generate_csr(csr_matrix* A, int size, double density);
void spmv_serial_csr(const csr_matrix* A, const double* x, double* y);
void free_csr(csr_matrix* A); // 添加一个释放内存的辅助函数

#endif // SPMV_H