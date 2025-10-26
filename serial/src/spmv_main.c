#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "spmv.h" // 包含自己的头文件

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <matrix_size> <density>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    double DENSITY = atof(argv[2]);

    csr_matrix A;
    double* x = (double*)malloc(N * sizeof(double));
    double* y = (double*)malloc(N * sizeof(double));

    srand(0);
    generate_csr(&A, N, DENSITY);
    for (int i = 0; i < N; ++i) {
        x[i] = (double)rand() / RAND_MAX;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    spmv_serial_csr(&A, x, y);
    gettimeofday(&end, NULL);
    double time_gettimeofday = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    double gflops = (2.0 * A.num_non_zeros) / (time_gettimeofday * 1e9);

    printf("Size: %d, Density: %f, NNZ: %d, Time: %f s, GFLOPS: %f\n",
           N, DENSITY, A.num_non_zeros, time_gettimeofday, gflops);
    
    free_csr(&A);
    free(x);
    free(y);
    
    return 0;
}