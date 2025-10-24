typedef struct{
    int N;
    const double* A;
    const double* B;
    double* C;
    int tile_size;
    int num_dim;
    int start_idx;
    int end_idx;
}gemm_thread_2d_t;

void* gemm_worker(void* arg);

void gemm_parallel(const double*A,const double*B,double* C,int F,int num_threads);