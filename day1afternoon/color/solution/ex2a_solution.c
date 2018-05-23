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
  int max_local=0, max_global, max_id;
  int seed;
  
// workload for all ranks 
// pick max from randomly generated 10 numbers
  seed= time(NULL) + myid*100;
  srand(seed);

  for (i=0;i<10;i++) {
    a[i] = rand() % 1000;
    printf("%d ",a[i]);
    if (a[i] > max_local)
      max_local = a[i];
  }
   printf("from process %d\n\n", myid);

  MPI_Barrier(MPI_COMM_WORLD);
  
// root receive local max from each rank
  if (myid == 0) {
     max_global = max_local;
     max_id = 0;	
     for(i = 1;i< nprocs;i++) {
         MPI_Recv(&max_local, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
         if ( max_local > max_global){
		max_global = max_local;
        	max_id = i;
         }
     }
     printf("Finally, global maximun %d from process %d\n\n", max_global, max_id);
  }
  else    // myid != 0 Send local max to the root process
     MPI_Send(&max_local, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);


  // Terminate MPI.
  MPI_Finalize();
 
  return 0;
}
