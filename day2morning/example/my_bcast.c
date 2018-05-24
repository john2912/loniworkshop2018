#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int i,j;
    int number=0;

    if (rank==root) {
        number=2;
        for (i=0;i<psize;i++)
            if (i!=root)
                MPI_Send(&number,1,MPI_INT,i,0,MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(&number,1,MPI_INT,root,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }

    printf("rank_%d,number=%d\n",rank,number);

    MPI_Finalize();
    return 0;
}
