CC=icc
MPICC=mpicc
INC=-I./
CFLAGS_SER=$(INC)
CFLAGS_OMP=-openmp $(INC)
EXSRC_SER=floatoper_omp_ser.c
EXSRC_PAR=floatoper_omp_par.c
SRC_LP_OMP=lp_omp.c
SRC_LP_MPI=lp_mpi.c
SRC_HELLO_OMP=hello_omp.c
SRC_HELLO_MPI=hello_mpi.c

all: eg_serial eg_par_omp lp_omp lp_mpi hello_omp hello_mpi

hello_omp:
	$(CC) $(CFLAGS_OMP) $(SRC_HELLO_OMP) -o $@.out
	
hello_mpi:
	$(MPICC) $(SRC_HELLO_MPI) -o $@.out

eg_serial: $(EXSRC)
	$(CC) $(CFLAGS_OMP) $(EXSRC_SER) -o $@.out

eg_par_omp: $(EXSRC)
	$(CC) $(CFLAGS_OMP) $(EXSRC_PAR) -o $@.out

lp_omp: $(SRC_LP_OMP)
	$(CC) $(CFLAGS_OMP) $(SRC_LP_OMP) -o $@.out

lp_mpi: $(SRC_LP_MPI)
	$(MPICC) $(SRC_LP_MPI) -o $@.out

clean:
	rm -rf *.out
