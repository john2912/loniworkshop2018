#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_2d_array.h"

int main (int argc, char **argv)
{
    int ns_rows=3, nca=5, ns_cols=7,
        ndiv_cols=3,
        ndiv_rows=2,
        tot_rows=ns_rows*ndiv_rows,
        tot_cols=ns_cols*ndiv_cols,
        root =0, nprocs, myrank,
        mpi_err=99,
        errcode;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    int i,j,k;
    int ipeek,jpeek;

 //   real flops,init_time,start_time,end_time;

    MPI_Status status;

    int input_error=0;
    if (argc==6) {
        tot_rows=atoi(argv[1]);
        nca     =atoi(argv[2]);
        tot_cols=atoi(argv[3]);
        ndiv_rows=atoi(argv[4]);
        ndiv_cols=atoi(argv[5]);
        if (tot_rows % ndiv_rows !=0) {
            if (myrank==0)
                printf("total rows not multiple of ndiv_rows!\n");
            input_error=1;
        }
        if (tot_cols % ndiv_cols !=0) {
            if (myrank==0)
                printf("total cols not multiple of ndiv_cols!\n");
            input_error=1;
        }
        if (nprocs != ndiv_rows*ndiv_cols) {
            if (myrank==0)
                printf("nprocs != ndiv_rows*ndiv_cols!\n");
            input_error=1;
        }
        ns_rows = tot_rows/ndiv_rows;
        ns_cols = tot_cols/ndiv_cols;
    }
    else if (argc==1) {
        if (ndiv_rows*ndiv_cols != nprocs) {
            if (myrank==0)
                printf("require %d procs, current np=%d!\n",ndiv_rows*ndiv_cols,nprocs);
            input_error=1;
        }
    }
    else {
        if (myrank==0)
            printf("usage: NRA NCA NCB ndiv_rows ndiv_cols\n");
        input_error=1;
    }

    if (input_error==1) {
        MPI_Finalize();
        exit(0);
    }

    //a, b, c are submatrix in each proc, 
    //global_a,global_b,global_c are global matrix, 
    //global_cv for verification with serial version on rank 0
    real **a, **b, **c, **global_a, **global_b, **global_c, **global_cv;


    if (myrank==0) {
        global_a = allocate_dynamic_2d_array(tot_rows,nca);
        for (i=0;i<tot_rows;i++)
            for (j=0;j<nca;j++)
                global_a[i][j] = i+j;

        global_b = allocate_dynamic_2d_array(nca,tot_cols);
        for (i=0;i<nca;i++)
            for (j=0;j<tot_cols;j++)
                global_b[i][j] = i*j;

        global_cv = allocate_dynamic_2d_array(tot_rows,tot_cols);
        for (i=0;i<tot_rows;i++)
            for (j=0;j<tot_cols;j++)
            {
                global_cv[i][j] = 0.0;
                for (k=0;k<nca;k++)
                    global_cv[i][j] += global_a[i][k]*global_b[k][j];
            }

        //print_matrix(global_a,tot_rows,nca,"%4.1f\t");
        //print_matrix(global_b,nca,tot_cols,"%4.1f\t");
        printf("serial result of global_c\n");
        print_matrix(global_cv,tot_rows,tot_cols,"%4.1f\t");

    }

 //   flops = 2.0*ns_rows*nca*ns_cols;
 //   init_time = MPI_Wtime();

    // allocate memory space for A, B and C
    a = allocate_dynamic_2d_array(ns_rows,nca);
    b = allocate_dynamic_2d_array(nca,ns_cols);
    c = allocate_dynamic_2d_array(ns_rows,ns_cols);
    // find out current time
  //  start_time = MPI_Wtime();

    MPI_Datatype smat_a,smat_b;
    MPI_Type_vector(ns_rows,nca,nca,MPI_REALNUM,&smat_a);
    MPI_Type_commit(&smat_a);
    int send_counts[nprocs];
    int send_displs[nprocs];

    real *sendp = (myrank==0?&(global_a[0][0]):NULL);
    if (myrank ==0) {
        int row_id;
        for (i=0;i<nprocs;i++) {
            send_counts[i]=1;
            row_id = (i / ndiv_cols);
            send_displs[i] = row_id;
        }
    }
    MPI_Scatterv(sendp,send_counts,send_displs,smat_a,&(a[0][0]),ns_rows*nca,MPI_REALNUM,root,MPI_COMM_WORLD);
    MPI_Type_free(&smat_a);

    // for scatterv b matrix
    MPI_Datatype rs_smat_b;
    MPI_Aint lb,ext_real;
    MPI_Type_get_extent(MPI_REALNUM,&lb,&ext_real);
    //MPI_Type_vector(nca,ns_cols,tot_cols,MPI_REALNUM,&smat_b);

    int sizes[2]={nca,tot_cols};
    int subsizes[2]={nca,ns_cols};
    int starts[2]={0,0};
    MPI_Type_create_subarray(2,sizes,subsizes,starts,MPI_ORDER_C,MPI_REALNUM,&smat_b);

    MPI_Type_create_resized(smat_b,0,ns_cols*ext_real,&rs_smat_b);
    MPI_Type_commit(&rs_smat_b);

    sendp = (myrank==0?&(global_b[0][0]):NULL);
    if (myrank ==0) {
        int col_id;
        for (i=0;i<nprocs;i++) {
            send_counts[i]=1;
            col_id = (i % ndiv_cols);
            send_displs[i] = col_id;
        }
    }

    MPI_Scatterv(sendp,send_counts,send_displs,rs_smat_b,
            &(b[0][0]),nca*ns_cols,MPI_REALNUM,root,MPI_COMM_WORLD);
    MPI_Type_free(&rs_smat_b);

    for (i=0;i<ns_rows;i++)
        for (j=0;j<ns_cols;j++) {
            c[i][j]=0.0;    
            for (k=0;k<nca;k++)
                c[i][j] += a[i][k]*b[k][j];
        }

    //print_matrix(a,ns_rows,nca,"%4.1f\t");
    //print_matrix(b,nca,ns_cols,"%4.1f\t");
    //print_matrix(c,ns_rows,ns_cols,"%4.1f\t");

    MPI_Datatype sub_mat,resized_sub_mat;
    sizes[0]=tot_rows,sizes[1]=tot_cols;
    subsizes[0]=ns_rows,subsizes[1]=ns_cols;
    starts[0]=0,starts[1]=0;
    MPI_Type_create_subarray(2,sizes,subsizes,starts,MPI_ORDER_C,MPI_REALNUM,&sub_mat);
    // the follwing can acheive the same function
    //MPI_Type_vector(ns_rows,ns_cols,tot_cols,MPI_REALNUM,&sub_mat);
    //MPI_Type_extent(MPI_REALNUM,&ext_real);
    MPI_Type_create_resized(sub_mat,0,ns_cols*ext_real,&resized_sub_mat);
    MPI_Type_commit(&resized_sub_mat);

    int recv_counts[nprocs];
    int recv_displs[nprocs];

    if (myrank==0) {
        for (i=0;i<nprocs;i++)
            recv_counts[i]=1;
        int row_id,col_id;
        for (i=0;i<nprocs;i++) {
            row_id = (i / ndiv_cols);
            col_id = (i % ndiv_cols);
            recv_displs[i] = row_id*ndiv_cols*ns_rows + col_id;
        }                

        global_c = allocate_dynamic_2d_array(tot_rows,tot_cols);
    }

    real *recvp; 
    recvp = NULL;
    if ( myrank == 0)
        recvp = &(global_c[0][0]);

    MPI_Gatherv(&(c[0][0]),ns_rows*ns_cols,MPI_REALNUM,
            recvp,recv_counts,recv_displs,resized_sub_mat,root,MPI_COMM_WORLD);
    MPI_Type_free(&resized_sub_mat);

    if (myrank==0) {
        printf("parallel result global_c\n");
        print_matrix(global_c,tot_rows,tot_cols,"%4.1f\t");
        free_dynamic_2d_array(global_c);

        free_dynamic_2d_array(global_a);
        free_dynamic_2d_array(global_b);
        free_dynamic_2d_array(global_cv);
    }

    // find out current time
 //   end_time = MPI_Wtime();

   // if (0==myrank) {
   //     printf("Init Time: %7.3e\n", start_time - init_time);
   //     printf("Calc Time: %7.3e\n", end_time - start_time);
   //     printf("GFlops: %7.3e\n", 1.0e-9* flops/(end_time - start_time));
  //  }

    // free A-C
    free_dynamic_2d_array(a);
    free_dynamic_2d_array(b);
    free_dynamic_2d_array(c);

    MPI_Finalize(); 
}
