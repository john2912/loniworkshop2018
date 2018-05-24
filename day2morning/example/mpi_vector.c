#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int count=3,stride=2,i;
    double *source,*target;

    source = (double*) malloc(stride*count*sizeof(double));
    target = (double*) malloc(count*sizeof(double));

    if (rank==0) {
        for (i=0;i<stride*count;i++) {
            source[i]=i+1;
            printf("source[%d]=%3.1lf\t",i,source[i]);
        }
        printf("\n");
        MPI_Datatype newvectortype;
        MPI_Type_vector(count,1,stride,MPI_DOUBLE,&newvectortype);
        MPI_Type_commit(&newvectortype);
        MPI_Send(source,1,newvectortype,1,0,MPI_COMM_WORLD);
        MPI_Type_free(&newvectortype);
    } else if (rank==1) {
        MPI_Status recv_status;
        int recv_count;
        MPI_Recv(target,count,MPI_DOUBLE,0,0,MPI_COMM_WORLD,
                &recv_status);
        MPI_Get_count(&recv_status,MPI_DOUBLE,&recv_count);
        printf("recv_count=%d\n",recv_count);
        for (i=0;i<recv_count;i++)
            printf("target[%d]=%3.1lf\t",i,target[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
