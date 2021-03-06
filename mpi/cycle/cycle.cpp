#include "../../utils/utils.hh"
#include "../init/mpi_init.hh"
#include "cycle.hh"
#include <mpi.h>

//cycle.hh.
Cycle::Cycle(int rank, int nprocs, int bsize)
{
	assrt(gl_mpi_onoff == MPION);
	assrt(bsize > 0);
	bufsize = bsize;
	
	sendbufl = new double[bufsize];
	sendbufr = new double[bufsize];
	recvbufl = new double[bufsize];
	recvbufr = new double[bufsize];

	int left = (rank==0)?nprocs-1:rank-1;
	int right = (rank==nprocs-1)?0:rank+1;
	int tagl = 0;
	int tagr = 1;
	MPI_Send_init(sendbufl, bufsize, MPI_DOUBLE, left, tagl, 
		      MPI_COMM_WORLD, reqlist+0);
	MPI_Send_init(sendbufr, bufsize, MPI_DOUBLE, right, tagr, 
		      MPI_COMM_WORLD, reqlist+1);
	MPI_Recv_init(recvbufl, bufsize, MPI_DOUBLE, left, tagr, 
		      MPI_COMM_WORLD, reqlist+2);
	MPI_Recv_init(recvbufr, bufsize, MPI_DOUBLE, right, tagl, 
	MPI_COMM_WORLD, reqlist+3);
}

Cycle::~Cycle(){
	assrt(gl_mpi_onoff == MPION);

	delete[] sendbufl;
	delete[] sendbufr;
	delete[] recvbufl;
	delete[] recvbufr;
	
	for(int i=0; i < 4; i++)
		MPI_Request_free(reqlist+i);
}

void Cycle::post(){
	MPI_Startall(4, reqlist);
}

void Cycle::wait(){
	MPI_Waitall(4, reqlist, MPI_STATUSES_IGNORE);
}

void Cycle::copy_r2s(){
	for(int i=0; i < bufsize; i++){
		sendbufr[i] = recvbufl[i];
		sendbufl[i] = recvbufr[i];
	}
}

