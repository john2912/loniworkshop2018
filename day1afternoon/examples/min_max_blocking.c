#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 100

  int    num_procs;
  int x[MAX];			// Input array
  
  int main(int argc, char *argv[])
  {
     int start, stop;
     int myid, i;
     int my_min, others_min[100];		// Minimum
     int my_max, others_max[100];		// Maximum
  
     MPI_Init(&argc,&argv);                      // Initialize
  
     MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  // Get # processors
     MPI_Comm_rank(MPI_COMM_WORLD, &myid);       // Get my rank (id)
  
    for (i=0; i<MAX; i++){
        x[i]= (i+1);
     }
     /* --------------------------------------
        Find the min. among my numbers
        -------------------------------------- */
     int n = MAX/num_procs;
  
     start = myid * n;
  
     if ( myid != (num_procs-1) )
     {
        stop = start + n;
     }
     else
     {
        stop = MAX;
     }
  
     my_min = x[start];
     for (i = start+1; i < stop; i++)
     {
        if ( x[i] < my_min )
              my_min = x[i];
     }
    MPI_Status stat1, stat2;
     if ( myid == 0 )
     {
        /* -------------------------------------
           Get the min from others and compare
           ------------------------------------- */
        for (i = 1; i < num_procs; i++)
        {
  	   MPI_Recv(&others_min[i], 1, MPI_DOUBLE, i, 10, MPI_COMM_WORLD, &stat1);
  
           if ( others_min[i] < my_min )
              my_min = others_min[i];
        }
	printf("min =%d\n", my_min);
     }
     else
     {
        MPI_Send(&my_min, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD);
     }

   /************************************************************
     Synchronization point: all programs are stopped here
     due to the synchronous send/receive operations used
   ************************************************************
  
     /* --------------------------------------
        Now find the max. among my numbers
        -------------------------------------- */
     my_max = x[start];
  
     for (i = start+1; i < stop; i++)
     {
        if ( x[i] > my_max )
              my_max = x[i];
     }
  
     if ( myid == 0 )
     {
        /* -------------------------------------
           Get the max from others and compare
           ------------------------------------- */
        for (i = 1; i < num_procs; i++)
        {
           MPI_Recv(&others_max[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &stat2);    

           if ( others_max[i] > my_max )
              my_max = others_max[i];
        }
	printf("max =%d\n", my_max);
     }
     else
     {
        MPI_Send(&my_max, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
     }
  
     MPI_Finalize();
  }
