#include "../utils/utils.hh"
#include "../xphi-init/mic_init.hh"
#include "leibniz_init.hh"
#include <offload.h>
#include <cstdio>
#include <omp.h>

void leibniz1(){
	int nmic;
	mic_init(nmic);
	long n = 1l*1000*1000*800;
	long nbytes = n*8;
	printf("            nbytes = %ld\n",nbytes);
	double* v = (double *)_mm_malloc(nbytes, 64);
	leibniz_init(v, n);
	printf("    host pointer v = %p \n", v);

#pragma offload target(mic:0)					\
	in(v:length(n) align(64) alloc_if(1) free_if(0))
	{}

#pragma offload target(mic:0) nocopy(v:length(n) alloc_if(0) free_if(0))
	hostmic_scale(v, n);

#pragma offload target(mic:0)					\
	out(v:length(n) align(64) alloc_if(0) free_if(0))
	hostmic_scale(v, n);
	
	hostmic_scale(v, n);
	
	double sum;
#pragma offload target(mic:0)					\
	in(v:length(n) align(64) alloc_if(0) free_if(1))
	sum = mic_sum(v, n);

	printf("               sum = %f\n", sum);
	_mm_free(v);
	mic_exit();
}

void leibniz2(){
	int nmic;
	mic_init(nmic);
	long n = 1l*1000*1000*800;
	long nbytes = n*8;
	double* v = (double *)_mm_malloc(nbytes, 64);
	leibniz_init(v, n);
	
	double sum; 
#pragma offload target(mic:0)					\
	in(v:length(n) align(64))	
	{
		hostmic_scale(v, n);
		hostmic_scale(v, n);
		hostmic_scale(v, n);
		sum = mic_sum(v, n);
	}

	printf("    leibniz2:  sum = %f\n", sum);
	_mm_free(v);
	mic_exit();
}

void leibniz3(){
	int nmic;
	mic_init(nmic);
	long n = 1l*1000*1000*800;
	long nbytes = n*8;
	double* v = (double *)_mm_malloc(nbytes, 64);
	leibniz_init(v, n);
	
	double sum=-1;
#pragma offload target(mic:0)					\
	in(v:length(n) align(64))				\
	signal(v)
	{
		hostmic_scale(v, n);
		hostmic_scale(v, n);
		hostmic_scale(v, n);
		sum = mic_sum(v, n);
	}

#pragma offload_wait target(mic:0)  wait(v)
	printf("    leibniz3:  sum = %f\n", sum);

	_mm_free(v);
	mic_exit();
}

void leibniz4(){
	int nmic;
	mic_init(nmic);
	long n = 1l*1000*1000*800;
	long nbytes = n*8;
	double* v = (double *)_mm_malloc(nbytes, 64);
	leibniz_init(v, n);

	double sum[nmic];
	
	for(int mc=0; mc < nmic; mc++){ 
		long shft = mc*n/nmic;
		long len = (mc+1)*n/nmic - mc*n/nmic;
#pragma offload target(mic:mc)						\
	in(v[shft:len]:align(64))					\
	out(sum[mc:1])							\
	signal(1+mc)
		{
			hostmic_scale(v+shft, len);
			hostmic_scale(v+shft, len);
			hostmic_scale(v+shft, len);
			sum[mc] = mic_sum(v+shft, len);
		}
	}

	for(int mc=0; mc < nmic; mc++){
#pragma offload_wait target(mic:mc)  wait(1+mc)
	}

	double ans = 0;
	for(int mc=0; mc < nmic; mc++)
		ans += sum[mc];
	printf("     leibniz4: sum = %e\n", ans);

	_mm_free(v);
	mic_exit();
}

int main(){
	//leibniz1();
	//leibniz2();
	//leibniz3();
	leibniz4();
}