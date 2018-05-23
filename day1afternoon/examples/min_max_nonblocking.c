#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 1000000
 
  int    num_procs;
  double x[MAX];			// Input array
  
  int main(int argc, char *argv[])
  {
     int start, stop;
     int myid, i;
     double my_min;
     double others_min[MAX];		    // Save minimum separately
     double my_max;
     double others_max[MAX];		    // Save maximum separately
     
     MPI_Request rq_min[MAX], rq_max[MAX];  // Status variables
     	 
     MPI_Init(&argc,&argv);                      // Initialize
  
     MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  // Get # processors
     MPI_Comm_rank(MPI_COMM_WORLD, &myid);       // Get my rank (id)
  
    for (i=0; i<MAX; i++){
        x[i]= (i+1);
    //    printf("%f\t",x[i]);
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
  
     for (i = start+1; i < stop; i++)// = i + n )
     {
        if ( x[i] < my_min )
              my_min = x[i];
     }
   //  printf("process %d min=%f\n",myid, my_min); 
     if ( myid == 0 )
     {
        /* -------------------------------------
           Get the min from others and compare
           ------------------------------------- */
        for (i = 0; i < num_procs; i++)
        {
  	   MPI_Irecv(&others_min[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &rq_min[i]);
        }
     }
     else
     {
        MPI_Isend(&my_min, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &rq_min[0]);
      //  MPI_Send(&my_min, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);//, &rq_min[0]);
     }

   /*****************************************************************
     NO Synchronization: all programs proceeds to the next stage
     of computation independently and will not wait on each other
     due to the asynchronous send/receive operations used
   *****************************************************************
  
     /* --------------------------------------
        Now find the max. among my numbers
        -------------------------------------- */
     my_max = x[start];
  
     for (i = start+1; i < stop; i++)// = i + n )
     {
        if ( x[i] > my_max )
              my_max = x[i];
     }
  
// printf("process %d max=%f\n",myid, my_max); 
     if ( myid == 0 )
     {
        /* -------------------------------------
           Get the max from others and compare
           ------------------------------------- */
        for (i = 1; i < num_procs; i++)
        {
           MPI_Irecv(&others_max[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &rq_max[i]);
        }
    
     }
     else
     {
        MPI_Isend(&my_max, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &rq_max[0]);
      //  MPI_Send(&my_max, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);//, &rq_max[0]);
     }

   /*****************************************************************
    Now we must gather the min[] and max[] from different processes
    HERE we must synchronize (we have no choice)
    That's OK, we are at the END of the processing !

    Waiting for others to finish at the END to complete the whole
    job is a necessity, it's not wasting time
   *****************************************************************
  
     /* --------------------------------------
        Now synchronize to compute results
        -------------------------------------- */
  MPI_Status status1, status2;
    // Receive at most MAX_NUMBERS from process zero
  //  MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    // After receiving the message, check the status to determine how many
    // numbers were actually received
  //  MPI_Get_count(&status, MPI_INT, &number_amount);
    // Print off the amount of numbers, and also print additional information
    // in the status object


   if ( myid == 0 )
     {  // The Master processor waits for all min[] and max[]
        // messages to arrive

        for ( i = 1; i < num_procs; i++)
        {
           MPI_Wait( &rq_min[i], &status1 );

           if ( others_min[i] < my_min )
              my_min = others_min[i];
        }

//  printf("my_min=%f\t num_procs=%d\n", my_min,num_procs);
        for ( i = 1; i < num_procs; i++)
        {
           MPI_Wait( &rq_max[i], &status2 );

           if ( others_max[i] > my_max )
              my_max = others_max[i];
        }

        // Print result....
        printf("min = %f\n" ,my_min);
        printf( "max = %f\n" ,my_max);
     }
     else
     {  // The other processes must wait until their messages
	// has been received before exiting !!!
	
        MPI_Wait(&rq_min[0], &status1);//NULL );
        MPI_Wait(&rq_max[0], &status2); //NULL );
  //      printf("1 received from 0. Message source = %d, tag = %d\n", status1.MPI_SOURCE, status1.MPI_TAG);  
      }

     MPI_Finalize();
  }
