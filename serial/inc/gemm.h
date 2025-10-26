#ifdef GEMM_H
#define GEMM_H

void init_matrix(double* mat,int size);
void gemm_serial(const double* A,const double* B,double* C,int N);

#endif