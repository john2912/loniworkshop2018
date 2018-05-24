#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define M 4
#define N 3

int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    int i,j;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float a[M][N],at[N][M];

    if (rank==root){
        printf("rank=%d\n",rank);
        int idx=0;
        for (i=0;i<M;i++) {
            for (j=0;j<N;j++) {
                a[i][j]=idx++;
                printf("%4.1f\t",a[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Datatype one_col,sub_mat_tran;  
    MPI_Aint lb,ext_float;          
    //TODO: complete the definition of user-defined type
    MPI_Type_vector( , , ,MPI_FLOAT,&one_col);
    MPI_Type_get_extent(MPI_FLOAT,&lb,&ext_float);
    MPI_Type_create_hvector( , , ,one_col,&sub_mat_tran);
    // only need to commit the last type
    MPI_Type_commit(&sub_mat_tran); 

    if (rank==0)
        //TODO: complete the MPI_Send
        MPI_Send(a,   ,MPI_FLOAT,1,0,MPI_COMM_WORLD);
    else //rank==1
        //TODO:complete the MPI_Recv
        MPI_Recv(at, ,  ,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    if (rank==1) {
        printf("rank=%d\n",rank);
        for (i=0;i<N;i++) {
            for (j=0;j<M;j++)
                printf("%4.1f\t",at[i][j]);
            printf("\n");
        }
    }

    MPI_Type_free(&sub_mat_tran);
    MPI_Finalize();
    return 0;
}
