/*
 * Copyright Divakar Viswanath, 2009-2014
 */

/*     
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as 
 * published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __dhstmem24sept2014__
#define __dhstmem24sept2014__
#include <cstdio>
template<class ttype> class dhstmem{
private:
	ttype *devicemem;
	ttype *hostmem;
	int n;
	cudaError_t errcode;
public:
	dhstmem(long int nin){
		n = nin;
		errcode = cudaMalloc((void **)&devicemem, n*sizeof(ttype));
		hostmem = new ttype[n];
	}
	~dhstmem(){
		delete[] hostmem;
		cudaFree(devicemem);
	}
	ttype *device(){return devicemem;}
	ttype *host(){return hostmem;}
	void device2host(){
		errcode = cudaMemcpy(hostmem, devicemem, 
			 n*sizeof(ttype),cudaMemcpyDeviceToHost);
	}
	void host2device(){
		errcode = cudaMemcpy(devicemem, hostmem, 
			 n*sizeof(ttype),cudaMemcpyHostToDevice);
	}
};
#endif