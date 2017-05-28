#!/bin/bash
# compile and run the openmp laplacian solver
icc -openmp lp_omp.c -o lp_omp.out
# run problem size 4096x4096 grid for 10000 steps using default 
# 16 threads, print information every 100 steps, relative error 0.0001
env OMP_NUM_THREADS=16 ./lp_omp.out 4096 4096 10000 100 0.0001
# open another terminal and ssh to the compute node, then type “top -H”

