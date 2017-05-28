#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char ** argv)
{
    int pid, p, root = 0;
    int n=13;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    int errcode=0;
    if (argc==2) {
        n=atoi(argv[1]);
        if (n<p) { errcode=1;}
    }
    else { errcode=2;}
    if (errcode>0) {
        if (pid==0) {
            printf("Usage: mpirun -np <p> ./a.out <n>, p is the number of procs, n is the problem size!\n");
            if (errcode==1)
                printf("Error, n must greater than p!\n");
        }
        MPI_Finalize();       
        exit(0);
    }

    int my_start=0,my_end=0;
    int base_size,extra_size,my_size,remainder;

    base_size = n/p;
    extra_size=base_size+1;
    remainder = n%p;

    // FIXME, complete the following if-else so that each process gets roughly
    // the same amount of workload
    if ( pid < remainder ) {
        my_size = ;
        my_start = ;
    }
    else {
        my_size = ;
        my_start = ;
    }
    my_end = my_start + my_size -1;

    printf("pid:%d,\tmy_start=%d,\tmy_size=%d,\tmy_end=%d\n",pid,my_start,my_size,my_end);

    MPI_Finalize();
    return 0;
}
