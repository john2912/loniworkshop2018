#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int M=4,N=3,i,j;

    int A[M][N],AT[N][M];

    if (rank==0){
        int idx=0;
        for(i=0;i<M;i++) {
            for (j=0;j<N;j++) {
                A[i][j]=idx++;
                printf("%4d\t",A[i][j]);
            }
            printf("\n");
        }
    }

    int nrows=M/psize;

    if (rank==0) {
        for (i=0;i<psize;i++)
            MPI_Send(&(A[i*nrows][0]),N*nrows,MPI_INT,i,0,MPI_COMM_WORLD);
    }

    int *submat=(int*)malloc(N*sizeof(int));
    MPI_Recv(submat,N*nrows,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    MPI_Datatype myvct;
    MPI_Type_vector(N,nrows,M,MPI_INT,&myvct);
    MPI_Type_commit(&myvct);

    MPI_Send(submat,N*nrows,MPI_INT,0,0,MPI_COMM_WORLD);

    if (rank==0) {
        for (i=0;i<psize;i++)
            MPI_Recv(&(AT[0][i*nrows]),1,myvct,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for (i=0;i<N;i++) {
            for (j=0;j<M;j++)
                printf("%4d\t",AT[i][j]);
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
