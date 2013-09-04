#include "../utils/utils.hh"
#include "../utils/StatVector.hh"
#include "../utils/TimeStamp.hh"
#include "../pyplot/pyplot.hh"
#include "fft_mkl.hh"
#include "fft_fftw.hh"
#include "nr.hh"
#include <mkl.h>
#include <unistd.h>
#include <iostream>

static enum yesno_type {YES, NO};

static StatVector *stat_mkl = NULL, *stat_fftw = NULL, *stat_nr = NULL;

/*
 * no cache effects if march_on == YES
 */
void time_fft(int n, enum yesno_type march_on){
	/*
	 * check if n is a power of 2
	 */
	int nn = n;
	enum yesno_type pow2 = YES;
	while(nn > 1){
		if(nn%2 == 1)
			pow2 = NO;
		nn /= 2;
	}
	
	/*
	 * determine count of ffts so that 10 GB of data is accessed
	 */
	long bytes = 10l*1000*1000*1000;
	double *space = (double *)MKL_malloc(bytes, 64);
	long count = bytes/(1l*2*n*sizeof(double));

	/*
	 * allocate stat objects
	 */
	if(stat_mkl != NULL){
		delete stat_mkl;
		stat_mkl = new StatVector(count);
	}

	if(stat_fftw != NULL){
		delete stat_fftw;
		stat_fftw = new StatVector(count);
	}

	if(stat_nr != NULL){
		delete stat_nr;
		stat_nr = new StatVector(count);
	}

	/*
	 * initialize data
	 */
	for(long i = 0; i < count; i++){
		double *v = (march_on == YES) ? space + i*(2*n) : space;
		for(int j = 0; j < n; j++){//"random" and cheap
			v[2*j] = 7.0/(8.0+i+j);
			v[2*j+1] = 77/0/(8.0+i*i+j);
		}
	}
	
	TimeStamp clk;
	/*
	 * collect mkl stat
	 */
	fft_mkl fftmkl(n);
	for(long i = 0; i < count; i++){
		double *v = (march_on == YES) ? space + i*(2*n) : space;
		clk.tic();
		fft_mkl.fwd(v);
		double cycles = clk.toc();
		stat_mkl->insert(cycles);
	}

	/*
	 * collect fftw stat
	 */
	fft_fftw fftw(n);
	for(long i = 0; i < count; i++){
		double *v = (march_on == YES) ? space + i*(2*n) : space;
		clk.tic();
		fftw.fwd(v);
		double cycles = clk.toc();
		stat_fftw->insert(cycles);
	}

	/*
	 * collect nr data
	 */
	switch(pow2){
	case NO:
		stat_nr->insert(0);
		break;
	case YES:
		for(long i=0; i < count; i++){
			double *v = (march_on == YES) ? space + i*(2*n)
				                      : space;
			clk.tic();
			nrfwd(v, n);
			double cycles = clk.toc();
			stat_nr->insert(cycles);
		}
		break;
	}
	
	MKL_free(space);
}

void make_table(enum yesno_flag march_onoff){
	int n[] = {32, 64, 80, 8*3*7, 192, 1024, 1024*128, 1024*1024};
	
	for(int i=0; i < 9; i++){
		time_fft(n[i], march_on);
		std::cout<<"\n\nn= "<<n[i]<<std::endl;
		std::cout<<stat_mkl->median()<<std::endl;
		std::cout<<stat_fftw->median()<<std::endl;
		std::cout<<stat_nr->median()<<std::endl;
	}
}

int main(){
	make_table(YES);
	make_table(NO);
}