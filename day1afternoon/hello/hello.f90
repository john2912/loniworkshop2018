program hello

use mpi
implicit none

!include "mpif.h"

integer::nprocs, ierr, myid
integer::status(mpi_status_size)

!Initialize MPI
call mpi_init(ierr)
call mpi_comm_size(mpi_comm_world, nprocs, ierr)
call mpi_comm_rank(mpi_comm_world, myid, ierr)

write(*, '("Hello World from process ",I3," /",I3)') myid, nprocs

call mpi_finalize(ierr)

stop
end program hello
