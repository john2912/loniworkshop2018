#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "dynamic_2d_array.h"

int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
    int rank, psize, root = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //MT: total rows, NT: total cols
    int i,j,MT,NT,mdiv,ndiv,M,N;
    int input_error=0;

    // default values for M,N
    if (argc==5) {
        MT=atoi(argv[1]);
        NT=atoi(argv[2]);
        mdiv=atoi(argv[3]);
        ndiv=atoi(argv[4]);
        if (MT % mdiv !=0)    input_error=1;
        if (NT % ndiv !=0)    input_error=2;
        if (mdiv*ndiv!=psize) input_error=3;
    }
    else if (argc ==1) {
        MT=4,NT=6;
        mdiv=2,ndiv=psize/mdiv;
        if (psize % mdiv !=0) input_error=4;
        if (NT % ndiv !=0)    input_error=5;
    }
    else { //argc is not correct
        input_error=6;
    } 
    if (input_error) {
        if (rank==0) {
            switch (input_error) {
                case 1:
                    printf("MT is not multiples of mdiv!\n");
                    break;
                case 2:
                    printf("NT is not multiples of ndiv!\n");
                    break;
                case 3:
                    printf("mdiv*ndiv != psize!\n");
                    break;
                case 4:
                    printf("MT=%d,NT=%d,mdiv=%d, ndiv=%d, psize %% mdiv != 0!\n",MT,NT,mdiv, ndiv);
                    break;
                case 5:
                    printf("MT=%d,NT=%d,mdiv=%d, ndiv=%d, NT %% ndiv != 0!\n",MT,NT,mdiv, ndiv);
                    break;
                case 6:
                    printf("usage: MT NT mdiv ndiv\n");
                    break;
            } 
        }
        MPI_Finalize();
        exit(0);
    }
    M = MT/mdiv, N = NT/ndiv;

    // only contains the submatrix before transpose
    MPI_Aint lb,ext_real;
    MPI_Datatype smat_orig, rs_smat_orig;
    MPI_Type_vector(M,N,NT,MPI_REALNUM,&smat_orig);
    MPI_Type_get_extent(MPI_REALNUM,&lb,&ext_real);
    MPI_Type_create_resized(smat_orig,0,N*ext_real,&rs_smat_orig);
    MPI_Type_commit(&rs_smat_orig);

    real **A, **AT; 
    int send_counts[psize], send_displs[psize];
    if (rank==0) {
        // default values for M,N
        A = allocate_dynamic_2d_array(MT,NT);        
        printf("before transpose:\n");
        int idx=0;
        for (i=0;i<MT;i++){
            for (j=0;j<NT;j++) {
                A[i][j]=idx++;
            }
        }
        print_matrix(A,MT,NT,"%4.1f\t");
        int grid_row_id,grid_col_id;
        for (i=0;i<psize;i++) {
            // row and col start id of the submatrix in global matrix
            send_counts[i]=1;
            grid_row_id = (i / ndiv);
            grid_col_id = (i % ndiv);
            send_displs[i] = grid_row_id*ndiv*M + grid_col_id;
            //printf("send_displs[%d]=%d\n",i,send_displs[i]);
        }
    }

    real *sendp=NULL;
    if (rank==root) 
        sendp = &(A[0][0]);

    //real *sub_mat = (real*)malloc(M*N*sizeof(real));
    real **sub_mat = allocate_dynamic_2d_array(M,N);

    //MPI_Scatterv(sendp,send_counts,send_displs,rs_smat_orig,sub_mat,M*N,MPI_REALNUM,root,MPI_COMM_WORLD);
    MPI_Scatterv(sendp,send_counts,send_displs,rs_smat_orig,
            &(sub_mat[0][0]),M*N,MPI_REALNUM,root,MPI_COMM_WORLD);

    if (rank==root) {
        free_dynamic_2d_array(A); 
    }

    real **sub_mat_trans=allocate_dynamic_2d_array(N,M);
    for (i=0;i<M;i++)
        for (j=0;j<N;j++)
            sub_mat_trans[j][i]=sub_mat[i][j];

    //printf("rank=%d:\t",rank);
    //for (i=0;i<M*N;i++)
    //    printf("%4.1f\t",sub_mat[i]);
    //printf("\n");

    // local tranpose, do not need the column major sub-matrix
    MPI_Datatype smat_trans, rs_smat_trans;
    MPI_Type_vector(N,M,MT,MPI_REALNUM,&smat_trans);
    MPI_Type_get_extent(MPI_REALNUM,&lb,&ext_real);
    MPI_Type_create_resized(smat_trans,0,M*ext_real,&rs_smat_trans);
    MPI_Type_commit(&rs_smat_trans);

    real *recvp=NULL;
    if (rank==root) {
        AT = allocate_dynamic_2d_array(NT,MT);
        recvp = &(AT[0][0]);
    }

    int recv_counts[psize];
    int recv_displs[psize];
    if (rank==root) {
        int grid_row_id,grid_col_id;
        for (i=0;i<psize;i++) {
            recv_counts[i]=1;
            // this will be the transposed grid ids
            grid_col_id = (i / ndiv);
            grid_row_id = (i % ndiv);
            recv_displs[i]=grid_row_id*mdiv*N + grid_col_id; 
            //printf("recv_displs[%d]=%d\n",i,recv_displs[i]);
        }
    }

    //MPI_Gatherv(sub_mat,M*N,MPI_REALNUM,recvp,recv_counts,recv_displs,rs_smat_trans,root,MPI_COMM_WORLD);
    MPI_Gatherv(&(sub_mat_trans[0][0]),M*N,MPI_REALNUM,
            recvp,recv_counts,recv_displs,rs_smat_trans,root,MPI_COMM_WORLD);
    free_dynamic_2d_array(sub_mat);
    free_dynamic_2d_array(sub_mat_trans);

    if (rank == root)
    {
        printf("after transpose:\n");
        print_matrix(AT,NT,MT,"%4.1f\t");
        free_dynamic_2d_array(AT);
    }

    MPI_Type_free(&rs_smat_orig);
    MPI_Type_free(&rs_smat_trans);
    MPI_Finalize();
    return 0;
}
