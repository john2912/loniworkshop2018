#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "dynamic_2d_array.h"
#include "mpi.h"

#define MAXITER 1000000

int main(int argc, char** argv)
{
    //int nr=10,nc=6,niter=100,iprint=1,debug_global_matrix=0;
    int nr=10,nc=6,niter=1000,iprint=1,debug_global_matrix=0,errcode=99;
    int ndiv_rows=5,ndiv_cols=2;
    //int nr=10,nc=6,niter=1000,iprint=1,debug_global_matrix=1,errcode=99;
    //int nr=4096,nc=4096,niter=1000,iprint=10,debug_global_matrix=0,errcode=99;
    //int ndiv_rows=4,ndiv_cols=4;
    real relerr=0.1;

    int i,j,iter;
    real dt, dt_global;
    real start_time, end_time;
    int psize, rank, rk_left, rk_right, rk_top, rk_bottom;

    int i_blk, j_blk;
    int n_sub_row,n_sub_col;
    real **global_told,**global_t;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&psize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if (rank == 0)
        printf("usage: nr nc ndiv_rows ndiv_cols relerr niter iprint debug_global_matrix.\n");

    int input_error=0;
    if (argc == 9) {
        nr =atoi(argv[1]);
        nc =atoi(argv[2]);
        ndiv_rows=atoi(argv[3]);
        ndiv_cols=atoi(argv[4]);
        //ndiv_cols=psize/ndiv_rows;
        relerr=atof(argv[5]);
        niter=atoi(argv[6]);
        iprint=atoi(argv[7]);
        debug_global_matrix=atoi(argv[8]);

        if (rank==0) {
            printf("using: nr=%d,\nnc=%d,\nndiv_rows=%d,\nndiv_cols=%d,\nnprocs=%d\n",nr,nc,ndiv_rows,ndiv_cols,psize);
            printf("relerr=%.4f,niter=%d,\niprint=%d,\ndebug_global_matrix=%d\n",relerr,niter,iprint,debug_global_matrix);
        }

        if (nr % ndiv_rows !=0) {
            if (rank==0) {
                printf("number of rows not multiples of ndiv_rows!\n");
                printf("nr = %d, ndiv_rows = %d, nr/ndiv_rows=%.1f\n",nr,ndiv_rows,nr/(real)ndiv_rows);
            }
            input_error=1;
        }

        if (nc % ndiv_cols !=0) {
            if (rank==0) {
                printf("number of rows not multiples of ndiv_rows!\n");
                printf("nc = %d, ndiv_cols = %d, nc/ndiv_cols=%.1f\n",nc,ndiv_cols,nc/(real)ndiv_cols);
            }
            input_error=1;
        }

        if (psize != ndiv_rows*ndiv_cols) {
            if (rank==0) {
                printf("Use ndiv_rows*ndiv_cols= %d procs!, current procs is %d\n",ndiv_rows*ndiv_cols, psize);
            }
            input_error=1;
        }
    }
    else if (argc==1) {
        if (rank==0) {
            printf("using: \nnr=%d,\nndiv_rows=%d,\nnc=%d,\nndiv_cols=%d,\nnprocs=%d\n",nr,ndiv_rows,nc,ndiv_cols,psize);
            printf("relerr=%.4f,niter=%d,\niprint=%d,\ndebug_global_matrix=%d\n",relerr,niter,iprint,debug_global_matrix);
        }
        if (psize != ndiv_rows*ndiv_cols) {
            if (rank==0) {
                printf("require to use %d procs!, current procs is %d\n",ndiv_rows*ndiv_cols, psize);
            }
            input_error=1;
        }
    }
    else {
        if (rank==0) {
            printf("incorrect number of arguments! required 8 but supplied %d\n",argc-1);
            printf("example: mpirun -np 10 ./a.out 10 6 5 2 0.1 100 1 1\n");
        }
        input_error=1;
    }
    if (input_error==1) {
        MPI_Finalize();
        return 0;
    }

    int nr2 = nr+2;
    int nc2 = nc+2;

    n_sub_row = nr/ndiv_rows;
    n_sub_col = nc/ndiv_cols;
    int n_sub_row2 = n_sub_row+2;
    int n_sub_col2 = n_sub_col+2;

    real *recvp=NULL;
    if (rank==0) {
        global_t    = allocate_dynamic_2d_array(n_sub_row2*ndiv_rows,n_sub_col2*ndiv_cols);
        global_told = allocate_dynamic_2d_array(n_sub_row2*ndiv_rows,n_sub_col2*ndiv_cols);
        recvp = &(global_told[0][0]);
    }

    real **t   =allocate_dynamic_2d_array(n_sub_row2,n_sub_col2);
    real **told=allocate_dynamic_2d_array(n_sub_row2,n_sub_col2);

    real *bcl=(real*)malloc(nr2*sizeof(real));
    real *bcr=(real*)malloc(nr2*sizeof(real));
    real *bct=(real*)malloc(nc2*sizeof(real));
    real *bcb=(real*)malloc(nc2*sizeof(real));

    for (i=1;i<=nr;i++) 
        bcl[i]=i*100.0/nr,bcr[i]=0;
    for (i=1;i<=nc;i++)
        bct[i]=0,bcb[i]=(nc-i+1)*100.0/nc;

    for (i=0;i<n_sub_row2;i++)
        for (j=0;j<n_sub_col2;j++) {
            t[i][j]=0.0;
            told[i][j]=0.0;
        }
    i_blk = rank / ndiv_cols;
    j_blk = rank % ndiv_cols;

    //left bc
    if (j_blk == 0) {
        rk_left   = MPI_PROC_NULL;
        for (i=1;i<=n_sub_row;i++){
            told[i][0] = bcl[i_blk*n_sub_row + i]; 
        }
    } else {
        rk_left   = (rank-1)/ndiv_cols*ndiv_cols + (rank-1)%ndiv_cols;
    }

    //right bc
    if (j_blk == ndiv_cols-1) {
        rk_right   = MPI_PROC_NULL;
        for (i=1;i<=n_sub_row;i++)
            told[i][n_sub_col+1] = bcr[i_blk*n_sub_row + i]; 
    } else {
        rk_right  = (rank+1)/ndiv_cols*ndiv_cols + (rank+1) % ndiv_cols;
    }

    //top bc
    if (i_blk == 0) {
        rk_top   = MPI_PROC_NULL;
        for (j=1;j<=n_sub_col;j++)
            told[0][j] = bct[j_blk*n_sub_col + j]; 
    } else {
        rk_top  =(rank-ndiv_cols)/ndiv_cols*ndiv_cols + (rank-ndiv_cols)%ndiv_cols;
    }

    //bottom bc
    if (i_blk == ndiv_rows-1) {
        rk_bottom  = MPI_PROC_NULL;
        for (j=1;j<=n_sub_col;j++)
            told[n_sub_row+1][j] = bcb[j_blk*n_sub_col + j]; 
    } else {
        rk_bottom =(rank+ndiv_cols)/ndiv_cols*ndiv_cols + (rank+ndiv_cols)%ndiv_cols;
    }

    free(bcl);
    free(bcr);
    free(bct);
    free(bcb);

    // completion of the global -> local boundary conditions assignment
    MPI_Datatype bc_top_bottom;
    MPI_Type_contiguous(n_sub_col,MPI_REALNUM,&bc_top_bottom);
    MPI_Type_commit(&bc_top_bottom);

    MPI_Datatype bc_left_right;
    MPI_Type_vector(n_sub_row,1,n_sub_col+2,MPI_REALNUM,&bc_left_right);
    MPI_Type_commit(&bc_left_right);

    MPI_Datatype sub_mat,rs_sub_mat;
    int sizes[]={n_sub_row2*ndiv_rows,n_sub_col2*ndiv_cols};
    int subsizes[]={n_sub_row2,n_sub_col2};
    int starts[]={0,0};
    MPI_Type_create_subarray(2,sizes,subsizes,starts,MPI_ORDER_C,MPI_REALNUM,&sub_mat);
    MPI_Aint ext_real;
    MPI_Type_extent(MPI_REALNUM,&ext_real);
    MPI_Type_create_resized(sub_mat,0,n_sub_col2*ext_real,&rs_sub_mat);
    MPI_Type_commit(&rs_sub_mat);

    int done=0;
    dt_global = relerr*2;
    for (iter=1;iter<=niter && (dt_global > relerr) ;iter++) {

        // exchange domain boundary info
        //right boundary
        MPI_Request req_r,req_l,req_t,req_b;

        MPI_Irecv(&(told[1][n_sub_col+1]),1,bc_left_right,rk_right, 0,MPI_COMM_WORLD, &req_r);
        MPI_Irecv(&(told[1][0]),          1,bc_left_right,rk_left,  0,MPI_COMM_WORLD, &req_l);
        MPI_Irecv(&(told[0][1]),          1,bc_top_bottom,rk_top,   0,MPI_COMM_WORLD, &req_t);
        MPI_Irecv(&(told[n_sub_row+1][1]),1,bc_top_bottom,rk_bottom,0,MPI_COMM_WORLD, &req_b);

        MPI_Send(&(told[1][n_sub_col]), 1, bc_left_right, rk_right, 0, MPI_COMM_WORLD);
        MPI_Send(&(told[1][1]),         1, bc_left_right, rk_left,  0, MPI_COMM_WORLD);
        MPI_Send(&(told[1][1]),         1, bc_top_bottom, rk_top,   0, MPI_COMM_WORLD);
        MPI_Send(&(told[n_sub_row][1]), 1, bc_top_bottom, rk_bottom,0, MPI_COMM_WORLD);

        MPI_Wait(&req_l,MPI_STATUS_IGNORE);
        MPI_Wait(&req_r,MPI_STATUS_IGNORE);
        MPI_Wait(&req_t,MPI_STATUS_IGNORE);
        MPI_Wait(&req_b,MPI_STATUS_IGNORE);

        // Main loop.
        for (i=1;i<=n_sub_row;i++) {
            for (j=1;j<=n_sub_col;j++) {
                t[i][j]=0.25*(told[i+1][j]+told[i-1][j]+told[i][j-1]+told[i][j+1]);
            }
        }

        dt=0;

        for (i=1;i<=n_sub_row;i++) {
            for (j=1;j<=n_sub_col;j++) {
                dt=fmax(fabs(t[i][j]-told[i][j]),dt);
                told[i][j]=t[i][j];
            }
        }
        //printf("rk[%d],dt:%.2f \n",rank,dt);
        if (debug_global_matrix) {

            int recv_counts[psize];
            int recv_displs[psize];

            if (rank==0) {

                int grid_rowid,grid_colid;
                for (i=0;i<psize;i++) {
                    grid_rowid = i / ndiv_cols;
                    grid_colid = i % ndiv_cols;
                    recv_counts[i]=1;
                    recv_displs[i]=grid_rowid*ndiv_cols*n_sub_row2 + grid_colid;
                    //printf("displ[%d]=%d\n",i,recv_displs[i]);
                }
            }
            MPI_Gatherv(&(told[0][0]),n_sub_row2*n_sub_col2,MPI_REALNUM,
                    recvp,recv_counts,recv_displs,rs_sub_mat,0,MPI_COMM_WORLD);
        }

        //MPI_Reduce(&dt,&dt_global,1,MPI_REALNUM,MPI_MAX,0,MPI_COMM_WORLD);
        MPI_Allreduce(&dt,&dt_global,1,MPI_REALNUM,MPI_MAX,MPI_COMM_WORLD);
        // exchange info among blocks
        if (rank == 0) {
            // Check if output is required.
            if (iprint != 0)
                if (iter%iprint == 0) {
                    printf("Iteration: %6d; Convergence Diff: %f\n",iter,dt_global);
                }
        }
    } // end for loop

    if (rank==0) {
        if (debug_global_matrix) {
            //try to print a global matrix
            print_matrix(global_told,n_sub_row2*ndiv_rows,n_sub_col2*ndiv_cols,"%6.1f ");
        }
        free_dynamic_2d_array(global_t);
        free_dynamic_2d_array(global_told);
        if (dt_global <= relerr)
            printf("\nSolution has converged.\n");
    }

    free_dynamic_2d_array(t);
    free_dynamic_2d_array(told);

    // Print out the execution time.
    //end_time = omp_get_wtime();
    //printf ("total time in sec: %f\n", end_time - start_time);
    //printf("%s\n",MPI_PROC_NULL);
    MPI_Finalize();
    return 0;
}
