#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int M=6,N=8,i,j;
    int nrows=3,ncols=4,ntotcols=N;
    float A[M][N];
    float Asub[nrows][ncols];


    //printf("lb=%ld,ext=%ld\n",lb,ext);

    for (i=0;i<M;i++)
        for (j=0;j<N;j++)
            A[i][j]=0;

    //assign each Asub with the rank+1
    for (i=0;i<nrows;i++)
        for (j=0;j<ncols;j++)
            Asub[i][j]=rank+1;

    MPI_Datatype submat;
    MPI_Type_vector(nrows,ncols,ntotcols,MPI_FLOAT,&submat);
    MPI_Type_commit(&submat);
    MPI_Aint lb,ext;
    MPI_Type_get_extent(submat,&lb,&ext);

    MPI_Datatype rs_submat;
    MPI_Type_create_resized(submat,0,ncols*sizeof(float),&rs_submat);
    MPI_Type_commit(&rs_submat);
    
    int recv_counts[]={1,1};
    int recv_displs[]={0,1};

   MPI_Gatherv(&(Asub[0][0]),nrows*ncols,MPI_FLOAT,
//    MPI_Gatherv(&Asub,1,submat,
                &(A[0][0]),recv_counts,recv_displs,rs_submat,root,MPI_COMM_WORLD);

    if (rank==root) {
        printf("\nUsing MPI_Gatherv:\n");
        for (i=0;i<M;i++) {
            for (j=0;j<N;j++)
                printf("%.0f  ",A[i][j]);
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
