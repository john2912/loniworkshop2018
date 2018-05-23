#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MATSIZE 500

int main (int argc, char *argv[])
{
    int nra, nca, ncb, nrb;
    int nprocs, myrank;
    int mpi_err=99;
    int i,j,k;
    int rpeek,cpeek;
    
    //matrix A & B & C have the same dimensions:
    nra = nca = MATSIZE,
    nrb = ncb = MATSIZE;

// blank 1: Initialize MPI   


// // blank 2: Find out the total number of process and the rank.


    if ( argc != 3 && myrank == 0 ){
        printf("\nUsage: mpirun -np xx %s row-to-check col-to-check \n\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
    else {
        nra = MATSIZE; 
        rpeek=atoi(argv[1]);
        cpeek=atoi(argv[2]);
    }

    if ((nra % nprocs) != 0 && myrank == 0) {
        printf("\nNrows is not a multiple of nprocs!\n\n");
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

   //blank 3: fill in the indices for i
    for (i =     ){
        for (j = 0; j < ncb; j++){
            for (k=0;k<nca;k++){
                C[i][j] += A[i][k]*B[k][j];
            }
         }
    }
 
 //calculate C[rpeek][cpeek] to check accuracy
   double value_rpeek_cpeek = 0.0;
   for (k=0; k<nca; k++){
	value_rpeek_cpeek += A[rpeek][k] * B[k][cpeek];
   }
 
//only myrank calculated C[rpeek][cpeek] has the value
   if (myrank == rpeek/rows_per_process) {
       printf("\nValue calculated by the program:\n");
       printf("C[%d][%d]=%14.6e\n",rpeek,cpeek,C[rpeek][cpeek]);
       printf("Compared with the correct value:\n");
       printf("C[%d][%d]=%14.6e\n\n",rpeek,cpeek,value_rpeek_cpeek);
    }
    
// blank 4: Finalize MPI

}
