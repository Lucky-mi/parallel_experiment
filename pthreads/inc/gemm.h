typedef struct{
    int N;
    int start_row;
    int end_row;
    const double* A;
    const double* B;
    double* C;
}gemm_thread_data_t;

void* gemm_worker(void* arg);

void gemm_parallel(const double*A,const double*B,double* C,int F,int num_threads);