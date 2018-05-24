#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "dynamic_2d_array.h"

/*transpose a matrix based on 1D row decomposition*/

int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
    int rank, psize, root = 0;
    int i,j,M,N;
    int nrows;
    real *sub_mat;
    real **A, **AT;

    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //default values for M,N
    M=4,N=6;
    if (argc>2) {
        M=atoi(argv[1]);
        N=atoi(argv[2]);
    }
    if (M % psize !=0) {
        if (rank==0)
            printf("total number of rows is not multiples of nprocs!\n");
        MPI_Finalize();
        return 0;
    }
    if (rank==0) {
        A = allocate_dynamic_2d_array(M,N);        
        printf("before transpose:\n");
        int idx=0;
        for (i=0;i<M;i++){
            for (j=0;j<N;j++) {
                A[i][j]=idx++;
                printf("%4.1f\t",A[i][j]);
            }
            printf("\n");
        }
    }

    nrows = M/psize;
    if (rank==0) {
        for (i=0;i<psize;i++)
            MPI_Send(&(A[i*nrows][0]),N*nrows,MPI_REALNUM,i,0,MPI_COMM_WORLD);
    }

    sub_mat = (real*)malloc(N*nrows*sizeof(real));
    MPI_Recv(sub_mat,N*nrows,MPI_REALNUM,root,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    //for (i=0;i<nrows*N;i++)
    //    printf("%3.1f\t",sub_mat[i]);
    //printf("\n");

    MPI_Datatype one_col,mul_cols;
    MPI_Aint lb,ext_real;
    //TODO: complete the following user defined type for transposed matrix
    MPI_Type_vector(   ,  ,  ,MPI_REALNUM,&one_col);
    MPI_Type_commit(&one_col);
    MPI_Type_get_extent(MPI_REALNUM,&lb,&ext_real);
    MPI_Type_create_hvector(  ,  ,  ,one_col,&mul_cols);
    MPI_Type_commit(&mul_cols);

    MPI_Send(sub_mat,N*nrows,MPI_REALNUM,root,0,MPI_COMM_WORLD);

    if (rank==0) {
        AT = allocate_dynamic_2d_array(N,M);        
        for (i=0;i<psize;i++)
            MPI_Recv(&(AT[0][i*nrows]),1,mul_cols,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        printf("after transpose:\n");
        for (i=0;i<N;i++){
            for (j=0;j<M;j++)
                printf("%4.1f\t",AT[i][j]);
            printf("\n");
        }
        free_dynamic_2d_array(AT); 
        free_dynamic_2d_array(A); 
    }

    free(sub_mat);
    MPI_Type_free(&one_col);
    MPI_Type_free(&mul_cols);
    MPI_Finalize();
    return 0;
}
