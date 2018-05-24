#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_2d_array.h"

int main (int argc, char **argv)
{
    int nra=4,
        nca=5,
        ncb=6,
        nprocs,
        myrank,
        mpi_err=99,
        errcode;
    
    int i,j,k;
    int ipeek,jpeek;
    
    int rows,ira_start,ira_end;
    
//    real flops,init_time,start_time,end_time;
    
    MPI_Status status;

    real** a;
    real** b;
    real** c;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   
    int input_error=0;
    if (argc==1){
    if (myrank==0) {
            printf("using: \nnra=%d,\nnca=%d,\nncb=%d\n",nra,nca,ncb);
    }
    }
    else if (4!=argc && 1!=argc ) {
        if (0==myrank)
            printf("Usage:mpirun -np N ./a.out nra nca ncb\n");
        input_error=1;
    }
    else {
        nra=atoi(argv[1]);
        nca=atoi(argv[2]);
        ncb=atoi(argv[3]);
    }
    if (0!=(nra % nprocs)) {
        if (0==myrank)
           printf("Nrows is not a multiple of nprocs!\n");
        input_error=1;
    }

    if (input_error){
        MPI_Finalize();
        return 0;
    }

 //   flops = 2.0*nra*nca*ncb;
 //   init_time = MPI_Wtime();

    // allocate memory space for A, B and C
    a = allocate_dynamic_2d_array(nra,nca);
    b = allocate_dynamic_2d_array(nca,ncb);
    c = allocate_dynamic_2d_array(nra,ncb);

    // initialize the values
    for (i=0;i<nra;i++)
        for (j=0;j<nca;j++)
            a[i][j]=i+j;

    for (i=0;i<nca;i++)
        for (j=0;j<ncb;j++)
            b[i][j]=i*j;

    for (i=0;i<nra;i++)
        for (j=0;j<ncb;j++)
            c[i][j]=0.0;    
    
    // find out current time
//    start_time = MPI_Wtime();

    rows=nra/nprocs;
    ira_start=myrank*rows;
    ira_end  =(myrank+1)*rows-1;
    
    //printf("ira_start=%d,ira_end=%d\n",ira_start,ira_end);

    for (i=ira_start;i<=ira_end;i++)
        for (j=0;j<ncb;j++)
            for (k=0;k<nca;k++)
                c[i][j] += a[i][k]*b[k][j];

    //printf("rows*ncb=%d,rk=%d\n",rows*ncb,myrank);
    //MPI_Gather(&(c[myrank*rows][0]),rows*ncb,MPI_REALNUM,&(c[0][0]),rows*ncb,MPI_REALNUM,0,MPI_COMM_WORLD);
    MPI_Gather(c[myrank*rows],rows*ncb,MPI_REALNUM,c[0],rows*ncb,MPI_REALNUM,0,MPI_COMM_WORLD);
    //if (0==myrank) {
    //    for (i=1;i<=nprocs-1;i++) {
    //        MPI_Recv(c[i*rows],rows*nca,MPI_DOUBLE,i,0,MPI_COMM_WORLD,&status);
    //    }
    //}
    //else {
    //    MPI_Send(c[myrank*rows],rows*nca,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
    //}
    
    // find out current time
 //   end_time = MPI_Wtime();
 
   if (0==myrank) {
 //      printf("Init Time: %7.3e\n", start_time - init_time);
 //      printf("Calc Time: %7.3e\n", end_time - start_time);
 //      printf("GFlops: %7.3e\n", 1.0e-9* flops/(end_time - start_time));
       print_matrix(a,nra,nca,"%4.1f\t");
       print_matrix(b,nca,ncb,"%4.1f\t");
       print_matrix(c,nra,ncb,"%4.1f\t");
   }

   // free A-C
   free_dynamic_2d_array(a);
   free_dynamic_2d_array(b);
   free_dynamic_2d_array(c);

   MPI_Finalize(); 
}
