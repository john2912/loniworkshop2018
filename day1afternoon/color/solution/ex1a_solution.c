#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[])
{

  // Initialize MPI.

  MPI_Init(&argc,&argv);
  int nprocs, myid;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  // Print out to screen.
  char host[10];
  gethostname(host, 255);
  if (myid%2 == 0) 
    printf("Process %d from mahcine %s has the color red.\n",myid, host);
  else
    printf("Process %d from machine %s has the color green.\n",myid, host);

  // Finalize MPI.

  MPI_Finalize();

  return 0;

}
