#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MATSIZE 512

int main (int argc, char *argv[])
{
    int nra, nca, ncb, nrb;
    int nprocs, myrank;
    int mpi_err=99;
    int errcode;
    int i,j,k;
    int rpeek,cpeek;
    MPI_Status status;

    myrank = 0;
    if ( argc != 3 && myrank == 0 ){
        printf("\nUsage: mpirun -np xx %s row-to-check col-to-check \n\n", argv[0]);
        exit(1);
    }
    else {
        nra = MATSIZE;
        rpeek=atoi(argv[1]);
        cpeek=atoi(argv[2]);
        if (rpeek >= MATSIZE || cpeek >=MATSIZE && myrank==0) {
                printf("\n peek row of %d or col of %d is greater than %d\n", rpeek, cpeek ,MATSIZE);
                exit(1);
       }
    }
 
    //matrix A & B & C have the same dimensions:
    nra = nca = MATSIZE,
    nrb = ncb = MATSIZE;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


    if ((nra % nprocs) != 0 && myrank == 0) {
        printf("\nNrows %d is not a multiple of nprocs %d!\n\n", MATSIZE, nprocs);
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }

// initialize the A/B/C matrix

    double A[nra][nca];
    double B[nrb][ncb];
    double C[nra][ncb];
    for (i=0;i<nra;i++)
        for (j=0;j<nca;j++)
            A[i][j]=i+j;
    
    for (i=0;i<nca;i++)
        for (j=0;j<ncb;j++)
            B[i][j]=i*j;
           
   for (i=0;i<nra;i++)
        for (j=0;j<ncb;j++) 
            C[i][j]=0.0;    
    
//divide work among ranks 
    int rows_per_process = nra / nprocs;
    int ira_start = myrank * rows_per_process;
    int ira_end = (myrank+1) * rows_per_process - 1;

    for (i=ira_start; i<ira_end; i++){
        for (j=0;j<ncb;j++){
            //c[i][j]=0.0;
            for (k=0;k<nca;k++){
                C[i][j] += A[i][k]*B[k][j];
            }
         }
    }
   if (myrank == 0){
	for(i = 1; i <= nprocs-1; i++)
 	     MPI_Recv(C[i*rows_per_process], rows_per_process*nca, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
   }
   else
	MPI_Send(C[myrank*rows_per_process], rows_per_process*nca, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);

 //calculate C[rpeek][cpeek] to check accuracy
   double value_rpeek_cpeek = 0.0;
   for (k=0; k<nca; k++){
	value_rpeek_cpeek += A[rpeek][k] * B[k][cpeek];
   }
 
//rank0 holds all the values C[rpeek][cpeek] 
   if (myrank == 0) {
       printf("\nValue calculated by the program:\n");
       printf("C[%d][%d]=%14.6e\n",rpeek,cpeek,C[rpeek][cpeek]);
       printf("Compared with the correct value:\n");
       printf("C[%d][%d]=%14.6e\n\n",rpeek,cpeek,value_rpeek_cpeek);
       if (C[rpeek][cpeek] == value_rpeek_cpeek)
           printf("MPI version generated correct values\n\n");

    }
    
    MPI_Finalize(); 
}
