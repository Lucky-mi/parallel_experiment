#include "../inc/gemm.h"
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
void* gemm_worker(void* arg){
    gemm_thread_data_t* data=(gemm_thread_data_t*)arg;
    for(int i=data->start_row;i<data->end_row;i++){
        for(int j=0;j<data->N;j++){
            double sum=0.0;
            for(int k=0;k<data->N;k++){
                sum+=(data->A[i*(data->N)+k])*(data->B[k*(data->N)+j]);
            }
            data->C[i*(data->N)+j]=sum;
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