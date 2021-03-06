#include "test_utils.hh"

void skew2x2(double *c, int m, int n){
	int ldc = m;
	for(int i=0; i < m; i+=2)
		for(int j=0; j < n; j+=2){
			double temp;
			temp = c[i+1+j*ldc];
			c[i+1+j*ldc] = c[i+1+(j+1)*ldc];
			c[i+1+(j+1)*ldc] = c[i+(j+1)*ldc];
			c[i+(j+1)*ldc] = temp;
		}
}

//Here b[] is in packed format.
void transpose200x4(double *bb, int ldbb, double *b){
	for(int i=0; i < 200; i++)
		for(int j=0; j < 4; j++){
			b[j+i*4] = bb[i+j*ldbb];
		}
}

void easymult(double *restrict a, double *restrict b, double *restrict c,
	      int m, int n, int p){
	int lda = m;
	int ldb = n;
	int ldc = m;
	for(int i=0; i < m; i++)
		for(int j=0; j < n; j++)
			for(int k=0; k < p; k++)
				c[i+k*ldc] += a[i+j*lda]*b[j+k*ldb];
}

