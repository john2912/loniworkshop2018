// http://stackoverflow.com/questions/5362577/c-gettimeofday-for-computing-time
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(char *argc, char **argv) {
    //const long N = 10000000000;
    //orig
    //const long N = 1000000000;
    //double
    //const long N = 2000000000;
    //half
    //const long N =  500000000;
    //quarter
    const long N =  250000000;
    double t0, t1, t2, t3;
    double sum=0.0; 
    // fundamental arithmetic type representing clock tick counts.
    clock_t start, end; 
    struct timeval r_start, r_end;
    //int i;
    long i;
    //gettimeofday(&r_start, NULL);
    //start = clock();
    // openmp directives
    double start_time = omp_get_wtime();
    //#pragma omp parallel for reduction(+:sum)
    for (i=0;i<N;i++) 
        sum += i*2.0+i/2.0; // doing some floating point operations
            // b = i*2.0; // doing some floating point operations
    double time = omp_get_wtime() - start_time;
    //end = clock();
    //system("sleep 2");
    //gettimeofday(&r_end, NULL);
    //double cputime_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
    //double realtime_elapsed_in_seconds = ((r_end.tv_sec * 1000000 + r_end.tv_usec)
    //          - (r_start.tv_sec * 1000000 + r_start.tv_usec))/1000000.0;
    //printf("cputime_elapsed_in_sec: %e\n", cputime_elapsed_in_seconds);
    //printf("realtime_elapsed_in_sec: %e\n", realtime_elapsed_in_seconds);
    //printf("cputime / numcores: %e\n", cputime_elapsed_in_seconds/16);
    //printf("cputime / realtime=%.2f\n",cputime_elapsed_in_seconds/realtime_elapsed_in_seconds);
    printf("time= %4.3e\t", time);
    printf("sum= %4.3e\n", sum);
    //sum /=sum;
    return 0;
}
