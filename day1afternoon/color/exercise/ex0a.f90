program pcolor 

implicit none

include "mpif.h"

integer :: nprocs, ierr, myid

! blank 1: Initialize MPI
call mpi_init(ierr)
! blank 2: Find the size and rank
call mpi_comm_size(mpi_comm_world, nprocs, ierr)
call mpi_comm_rank(mpi_comm_world, myid, ierr)

!print color
write(*,'("Process", I3, "has the color of red")') myid

write(*,'("Process", I3, "has the color of green")') myid

! blank 4: Finalize MPI
call mpi_finalize(ierr)

stop
end program pcolor

