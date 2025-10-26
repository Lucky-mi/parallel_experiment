#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <cblas.h> // For verification
#include "gemm.h" // 包含自己的头文件

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);

    printf("Matrix size: %d x %d\n", N, N);

    double* A = (double*)malloc(N * N * sizeof(double));
    double* B = (double*)malloc(N * N * sizeof(double));
    double* C_serial = (double*)malloc(N * N * sizeof(double));
    double* C_blas = (double*)malloc(N * N * sizeof(double));

    srand(0);
    init_matrix(A, N);
    init_matrix(B, N);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    gemm_serial(A, B, C_serial, N);
    gettimeofday(&end, NULL);
    double time_gettimeofday = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    printf("Time (gettimeofday): %f seconds\n", time_gettimeofday);

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, A, N, B, N, 0.0, C_blas, N);

    double error = 0.0;
    for (int i = 0; i < N * N; ++i) {
        double diff = C_serial[i] - C_blas[i];
        error += diff * diff;
    }
    printf("Verification error (sum of squared diff): %e\n", error);

    double gflops = (2.0 * N * N * N) / (time_gettimeofday * 1e9);
    printf("GFLOPS: %f\n", gflops);

    free(A);
    free(B);
    free(C_serial);
    free(C_blas);

    return 0;
}