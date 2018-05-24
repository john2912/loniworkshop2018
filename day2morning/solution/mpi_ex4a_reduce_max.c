#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

// Creates an array of random numbers. Each number has a value from 0 - 1
void fill_rand_nums(float *rand_nums, int num_elements) {
    int i;
    for (i = 0; i < num_elements; i++) {
        rand_nums[i] = (rand() / (float)RAND_MAX);
        printf("%4.3f\t", rand_nums[i]);
    }
    printf("\n");
}


float calc_sub_max(float *sub_rand_nums, int num_elems, int rank) {
    int i;
    float sub_max = sub_rand_nums[0];
    for (i=1;i<num_elems;i++) {
        if (sub_rand_nums[i] > sub_max) {
            sub_max = sub_rand_nums[i];
        }
    }
    return sub_max;
}

int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    int elem_per_proc = 3;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float sub_max, glb_max;
    float *tot_rand_nums;
    float *sub_rand_nums = malloc(elem_per_proc*sizeof(float));
    if (rank ==root )
    {
        tot_rand_nums = malloc(elem_per_proc*psize*sizeof(float));
        fill_rand_nums(tot_rand_nums, elem_per_proc*psize);
    }
    MPI_Scatter(tot_rand_nums, elem_per_proc, MPI_FLOAT, 
                sub_rand_nums, elem_per_proc, MPI_FLOAT,
                root, MPI_COMM_WORLD);
    sub_max = calc_sub_max(sub_rand_nums, elem_per_proc, rank);

    MPI_Reduce(&sub_max, &glb_max, 
               1,  MPI_FLOAT, 
               MPI_MAX, root, MPI_COMM_WORLD);

    if (rank == root)
    {
        printf("global_max=%4.3f\n", glb_max);
        free(tot_rand_nums);
    }
    free(sub_rand_nums);
    MPI_Finalize();
    return 0;
}
