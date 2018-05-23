#include <stdio.h>
#include <math.h>
#include <mpi.h>                       

#define NC       1000                   /* Number of cols        */
#define NR       1000                   /* Number of rows        */
#define DOWN     100                    /* Tag for messages down */
#define UP       101                    /* Tag for messages up   */
#define ROOT     0                      /* The root PE           */
#define NITER    100000                  /* Number of iteration */
#define ERR      0.01                    /* default error */
#define MAX(x,y) ( ((x) > (y)) ? x : y )

int NRL;                                 // variable to hold rows/process
void initialize(float t[NRL+2][NC+2]);
void set_bcs(float t[NRL+2][NC+2], int, int);

int main( int argc, char **argv ){
 
  int nprocs, myrank, stat=0;                   
  MPI_Status status;
  float dt, dtg;                    /* Delta t/global       */
  int i, j, iter;
  
  /*  Initialize MPI */
  MPI_Init(&argc, &argv);
  /*  Determine size of global communicator */
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs );
  /*  Determine my rank in the global communicator */
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank );

  NRL = NR / nprocs;   // assign number of rows/process
  if ( NR % nprocs != 0 && myrank ==0){
	printf("\nError: the number of rows %d is not multiple of nprocs %d!\n", NR, nprocs);
        MPI_Abort(MPI_COMM_WORLD, stat);
        exit(0);
  }
  
  float t[NRL+2][NC+2], t_old[NRL+2][NC+2];
  
 /* Give initial guesss of 0. */
  initialize(t);     
  /* Set the Boundary values   */
  set_bcs(t, myrank, nprocs); 
  
  for( i=0; i<=NRL+1; i++ )  
    for( j=0; j<=NC+1; j++ )
      t_old[i][j] = t[i][j];

/* Computation on Sub-grid for NITER iterations */
for( iter=1; iter<=NITER; iter++ ) {
     
      for( i=1; i<=NRL; i++ ){
         for( j=1; j<=NC; j++ )
            t[i][j] = 0.25*(t_old[i+1][j]+t_old[i-1][j]+t_old[i][j+1]+t_old[i][j-1]);
      }

  /***************************************************************************************/
               /* exchange boundary data among differnt myranks */
// Blank 1:
  /* Send my data down to the processor below me except last myrank  */
      if( myrank < nprocs-1 )

// Blank 2:
  /* Sending my data up to the processor above me except when myrank=0  */
      if( myrank != 0 )

// Blank 3:
  /*  Receive new data from UP processor of any source except when myrank=0  */
      if( myrank != 0 )

// Blank 4:
  /*  Receive new data from DOWN processsor of any source except last myrank */
      if( myrank != nprocs-1 )

               /* end of exchanging boundary data among differnt myranks */

  /***************************************************************************************/
  /*  Detemermine max value from all processors, followed by update back to all processors*/
  			/* begining of updating global max */   
      dt = 0.;

      for( i=1; i<=NRL; i++ )
          for( j=1; j<=NC; j++ ){
              dt = MAX(fabs(t[i][j]-t_old[i][j]), dt);
              t_old[i][j] = t[i][j];
          }
     // Find the global max convergence error.  
       if (myrank == 0) {
          for(i=1;i<nprocs;i++) {
 // Blank 5: receive err max from rank != 0

		if (dtg > dt) dt=dtg;
          }
           dtg=dt;
       }
       else
 // Blank 6: send err max to rank=0  
         
      
   // Send the global max convergence error to all processes.
       if (myrank == 0) {
           for(i=1;i<nprocs;i++)
// Blank 7: send err max to all ranks!=0
       
        }
 
       else
// Blank 7: receive err max from rank=0

  
/***************************************************************************************/
  			/* end of updating global max */   

       /* Print some test values */
        if ( myrank == 0 ){
            if ( iter%100 == 0 ) 
                  printf("Iter = %4d: Convergence error: %.5E\n", iter, dtg);
            if (dtg < ERR){
  	         printf("\nSolution has converged at iteration (%d) with error (%.5E) < %.5E\n\n", iter, dtg, ERR);
                 MPI_Abort(MPI_COMM_WORLD, stat);
            }
         }
      //  MPI_Barrier( MPI_COMM_WORLD );
  } //end of iteration
 
  MPI_Finalize(); 
}

/************** functions ***********************/
/* Initialize all the values to 0. as a starting value */
void initialize( float t[NRL+2][NC+2] ){
  int i, j;
         
  for( i=0; i<=NRL+1; i++ )       
    for ( j=0; j<=NC+1; j++ )
      t[i][j] = 0.0;
}

/* Set the values at the boundary.  Values at the boundary do not */
/* Change through out the execution of the program                */
void set_bcs( float t[NRL+2][NC+2], int myrank, int nprocs){
  int i, j;
  double n = 100.0/NRL;
  
  for( i=0; i<=NRL+1; i++ ) {    
    t[i][0] = n*i;                //left boundary
    t[i][NC+1] = n * i;           //right boundary 
  }
  
  if( myrank == 0 )                  /* Top boundary */
     for( j=0; j<=NC+1; j++ )
  	t[0][j] = n*j;

  if( myrank == nprocs-1 )             /* Bottom boundary */
      for( j=0; j<=NC+1; j++ )
         t[NRL+1][j] = n * j;
}

