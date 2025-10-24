#include "../inc/gemm_2d.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <cblas.h>
#include <math.h>
void init_matrix(double* mat,int size){
    for(int i=0;i<size*size;i++)
    mat[i]=(double)rand()/ RAND_MAX;
}
//每个线程固定工作
void* gemm_worker_2d(void* arg){
    gemm_thread_2d_t* data=(gemm_thread_2d_t*)arg;
    int N=data->N;
    int tile_size=data->tile_size;
    int num_dim=data->num_dim;
    int start_idx=data->start_idx;
    int end_idx=data->end_idx;

    for(int k=start_idx;k<end_idx;k++){
        int i=k/num_dim;//行号
        int j=k%num_dim;//列号
        //起始坐标
        int row_start=i*tile_size;
        int col_start=j*tile_size;
        int row_end=(i+1)*tile_size>N?N:(i+1)*tile_size;
        int col_end=(j+1)*tile_size>N?N:(j+1)*tile_size;
        //计算C的tile
        for(int i=row_start;i<row_end;i++){
            
            for(int j=col_start;j<col_end;j++){
                double sum=0.0;
                for(int k_gemm=0;k_gemm<N;k_gemm++)
                sum+=(data->A[i*N+k_gemm])*(data->B[k_gemm*N+j]);
            }
            data->C[i*N+j]=sum;
        }
    }
}

int main(int argc,char *argv[]){
    //初始化并检查参数正确
    if(argc!=3)
    {
    fprintf(stderr,"Usage:%s <matrix_size> <num_threads>\n",argv[0]);
    return 1;}

    int N=atoi(argv[1]);
    int T=atoi(argv[2]);
    printf("Matrix size:%d x %d,Threads:%d\n",N,N,T);
    pthread_t threads[T];
    gemm_thread_data_t thread_data[T];
    double* A=(double*)malloc(N*N*sizeof(double));
    double* B=(double*)malloc(N*N*sizeof(double));
    double* C=(double*)malloc(N*N*sizeof(double));
    double* C_parallel = (double*)malloc(N * N * sizeof(double));
    double* C_blas = (double*)malloc(N * N * sizeof(double));
    srand(0);
    init_matrix(A,N);
    init_matrix(B,N);
    //将任务分配到每个线程上
    int rows_per_thread=N/T;
    for(int i=0;i<T;i++){
        thread_data[i].N=N;
        thread_data[i].A=A;
        thread_data[i].B=B;
        thread_data[i].C=C_parallel;
        thread_data[i].start_row=i*rows_per_thread;
        //让最后一个线程把没做完的部分都做完，防止无法整除
        if(i==T-1){
            thread_data[i].end_row=N;
        }
        else 
            thread_data[i].end_row=(i+1)*rows_per_thread;
    }
    struct timeval start,end;
    gettimeofday(&start,NULL);
    //创建线程
    for(int i=0;i<T;i++){
        pthread_create(&threads[i],NULL,gemm_worker,(void*)&thread_data[i]);
    }
    for(int i=0;i<T;i++){
        pthread_join(threads[i],NULL);
    }
    printf("Matrix multiplication completed.\n");
    gettimeofday(&end,NULL);
    double elapsed=(end.tv_sec - start.tv_sec)+(end.tv_usec - start.tv_usec)/1000000.0;
    printf("Elapsed time:%.6f seconds\n",elapsed);
    cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,N,N,N,1.0,A,N,B,N,0.0,C_blas,N);
    double error=0.0;
    for(int i=0;i<N*N;i++){
        double diff=C_parallel[i]-C_blas[i];
        error+=diff*diff;
    }
    printf("Verification error (Frobenius norm):%.6e\n",sqrt(error));
    double gflops=(2.0*N*N*N)/(elapsed*1e9);
    printf("Performance:%.2f GFLOPS\n",gflops);
    free(A);
    free(B);
    free(C_parallel);
    free(C_blas);
    return 0;
}