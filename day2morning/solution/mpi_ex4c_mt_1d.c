#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "dynamic_2d_array.h"

int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
    int rank, psize, root = 0;
    int i,j;
    int M,N;
    int nrows;
    real *sub_mat;
    real **A, **AT;
    real *sendp=NULL,*recvp=NULL;

    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // default values for M,N
    if (argc==3) {
        M=atoi(argv[1]);
        N=atoi(argv[2]);
    }
    else if (argc ==1) {
        M=4,N=3;
    }
    else {
        if (rank==0) 
            printf("usage: M N\n");
        MPI_Finalize();
        exit(0);
    }
    if (rank==0) {
        A = allocate_dynamic_2d_array(M,N);        
        sendp = &(A[0][0]);
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
    sub_mat = (real*)malloc(N*nrows*sizeof(real));

    MPI_Scatter(sendp,N*nrows,MPI_REALNUM,sub_mat,N*nrows,MPI_REALNUM,root,MPI_COMM_WORLD);

    //for (i=0;i<nrows*N;i++)
    //    printf("%3.1f\t",sub_mat[i]);
    //printf("\n");

    MPI_Datatype one_col,mul_cols, rs_mul_cols;
    MPI_Aint lb,ext_float;
    MPI_Type_vector(N,1,M,MPI_REALNUM,&one_col);
    MPI_Type_commit(&one_col);
    MPI_Type_get_extent(MPI_REALNUM,&lb,&ext_float);
    MPI_Type_create_hvector(nrows,1,ext_float,one_col,&mul_cols);
    MPI_Type_commit(&mul_cols);
    MPI_Type_create_resized(mul_cols,0,nrows*ext_float,&rs_mul_cols);
    MPI_Type_commit(&rs_mul_cols);

    recvp=NULL;
    int recv_counts[psize],recv_displs[psize];
    if (rank==0) {
        AT = allocate_dynamic_2d_array(N,M);        
        recvp = &(AT[0][0]);
        for (i=0;i<psize;i++) {
            recv_counts[i]=1;
            recv_displs[i]=i;
        }
    }

    MPI_Gatherv(sub_mat,N*nrows,MPI_REALNUM,recvp,recv_counts,recv_displs,rs_mul_cols,root,MPI_COMM_WORLD);

    if (rank==0) {
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
    MPI_Type_free(&rs_mul_cols);
    MPI_Finalize();
    return 0;

}
