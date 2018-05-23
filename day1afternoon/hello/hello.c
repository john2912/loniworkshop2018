#include "mpi.h"

int main(int argc, char* argv[])
{
  //Initialize MPI

  int nprocs, myid;
  char* name = "dear";
  if (argc == 2){
  	name = argv[1];
  }
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);  
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  printf("Hi %s, Hello World from process %d/%d \n", name, myid, nprocs);

  MPI_Finalize();
  
  return 0;
}
