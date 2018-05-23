#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char* argv[])
{

  // Initialize MPI.

  int nprocs, myid;
  MPI_Status status;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  // Initialize the array and find the local max.

  int i, a[10];;
  int max_local=0, max_global, max_id
;
 // workload for all ranks to generate 10 random numbers
  for (i=0;i<10;i++) {
    a[i]=random()%1000;
    printf("%d ",a[i]);
    if (max_local <= a[i])
      max_local=a[i];
  }
  //make sure all processes finish their tasks 
  MPI_Barrier(MPI_COMM_WORLD);

  // Send local max to the root process to find the global error.
  if (myid == 0) {
     max_global = max_local;
     max_id = 0;

     for(i=1;i<nprocs;i++) {
  // blank 1: the root process receives local maxima from other processes

	if ( max_local > max_global){
                max_global = max_local;
                max_id = i;
        }
    }
    printf("Finally, global maximun %d from process %d\n\n", max_global, max_id);

  }
  else
  // blank 2: send the local maximum to the root process


  // Terminate MPI.
  MPI_Finalize();
  
  return 0;

}
