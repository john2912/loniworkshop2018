// http://stackoverflow.com/questions/5362577/c-gettimeofday-for-computing-time
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(int argc, char **argv) {
    long N = 10000000000;
    if (2==argc) 
        N *= atof(argv[1]);
    double sum=0.0; 
    long i;
    double start_time = omp_get_wtime();
    // openmp directives
    #pragma omp parallel for reduction(+:sum)
    for (i=0;i<N;i++) 
        sum += i*2.0+i/2.0; // doing some floating point operations
    double time = omp_get_wtime() - start_time;
    printf("time= %4.3e\t", time);
    printf("sum= %4.3e\n", sum);
    return 0;
}
