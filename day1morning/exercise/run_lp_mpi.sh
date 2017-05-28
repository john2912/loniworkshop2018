#!/bin/bash
# compile and run the mpi laplacian solver
mpicc lp_mpi.c -o lp_mpi.out
# running problem size 4096x4096 grid, 4x4 procs, relative error 0.0001
# 10000 timesteps, print information every 100 steps, do not print 
# matrix 0
mpirun -np 16 ./lp_mpi.out 4096 4096 4 4 0.0001 10000 100 0
# open another terminal and go to the compute node, then type “top”

