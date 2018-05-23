#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[])
{

int nprocs, myid;

// Initialize MPI.
MPI_Init(&argc, &argv);
// Get size
MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
// Get rank/id
MPI_Comm_rank(MPI_COMM_WORLD, &myid);

printf("Process %d has the color red.\n", myid);

printf("Process %d has the color green.\n", myid);

//Finalize
MPI_Finalize();
  return 0;

}
