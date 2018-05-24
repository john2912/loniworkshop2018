#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define M 5
#define N 5

int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    int i,j;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float a[M][N];

    for (i=0;i<M;i++)
        for (j=0;j<N;j++)
            a[i][j]=0;

    if (rank==root){
        int idx=0;
        for (i=0;i<M;i++)
            for (j=0;j<N;j++)
                a[i][j]=++idx;
    }

    int blocklen[M],displs[M];
    for (i=0;i<M;i++) {
        blocklen[i]=N-i;
        displs[i]=N*i+i;
    }

    MPI_Datatype upper_tri;
    MPI_Type_indexed(M,blocklen,displs,MPI_FLOAT,&upper_tri);
    MPI_Type_commit(&upper_tri);

    if (rank==0)
        MPI_Send(a,1,upper_tri,1,0,MPI_COMM_WORLD);
    else //rank==1
        MPI_Recv(a,1,upper_tri,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    printf("\n");
    for (i=0;i<M;i++) {
        for (j=0;j<N;j++)
            printf("%4.1f\t",a[i][j]);
        printf("\n");
    }

    MPI_Type_free(&upper_tri);
    MPI_Finalize();
    return 0;
}
