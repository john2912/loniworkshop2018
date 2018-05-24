#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

// Creates an array of random numbers. Each number has a value from 0 - 1
void fill_rand_nums(float *rand_nums, int num_elements) {
    //float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
    //assert(rand_nums != NULL);
    int i;
    for (i = 0; i < num_elements; i++) {
        rand_nums[i] = (rand() / (float)RAND_MAX);
        printf("%3.6f ", rand_nums[i]);
    }
    printf("\n");
}


float calc_sub_avg(float *sub_rand_nums, int num_elems, int rank) {
    int i;
    float sub_sum = sub_rand_nums[0];
    for (i=1;i<num_elems;i++) {
        sub_sum += sub_rand_nums[i];
    }
    return sub_sum/num_elems;
}

float calc_sub_diff2(float *sub_rand_nums, int num_elems, float glb_avg) {
    int i;
    //printf("sub_rand_nums[0]=%f\n",sub_rand_nums[0]);
    float sub_sum_diff2 = 0.0;
    for (i=0;i<num_elems;i++) {
        sub_sum_diff2 += (sub_rand_nums[i] - glb_avg)*(sub_rand_nums[i] - glb_avg);
    }
    return sub_sum_diff2;
}

int main(int argc, char ** argv)
{
    int rank, psize, root = 0;
    int elem_per_proc = 3;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float sub_avg, glb_avg;
    float sub_diff2, glb_diff2;
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
    sub_avg = calc_sub_avg(sub_rand_nums, elem_per_proc, rank);

    MPI_Allreduce(&sub_avg, &glb_avg, 1,  MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    glb_avg /= psize;
    //printf("glb_avg,rk[%d]=%f\n", rank, glb_avg);
    
    sub_diff2 = calc_sub_diff2(sub_rand_nums, elem_per_proc, glb_avg);

    //printf("sub_diff2,rk[%d]=%f\n", rank, sub_diff2);

    MPI_Reduce(&sub_diff2, &glb_diff2, 1,  MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);

    if (rank == root)
    {
        glb_diff2 /= psize*elem_per_proc;
        glb_diff2 = sqrt(glb_diff2);
        printf("glb_diff2,rk[%d]=%f\n", rank, glb_diff2);
        free(tot_rand_nums);
    }
    free(sub_rand_nums);
    MPI_Finalize();
    return 0;
}
