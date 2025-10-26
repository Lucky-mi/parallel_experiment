#include<stdlib.h>
#include"gemm.h"

void init_matrix(double* mat,int size){
    for(int i=0;i<size*size;++i){
        mat[i]=(double)rand()/RAND_MAX;
    }
}
void gemm_serial(const double* A,const double* B,double* C,int size){
    for(int i=0;i<size;++i){
        for(int j=0;j<size;++j){
            double sum=0.0;
            for(int k=0;k<size;++k){
                sum+=A[i*size+k]*B[k*size+j];
            }
            C[i*size+j]=sum; 
        }
    }
}