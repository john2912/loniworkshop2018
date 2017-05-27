#!/bin/bash +x

ARR=(1 2 4 8 16)
b=()
for j in "${ARR[@]}";
    do  
    export OMP_NUM_THREADS=$j
    # a warm-up run
    TIC=$(date +%s.%N);
    ./eg_par_omp.out $j
    TOC=$(date +%s.%N);
    min_duration=$(echo "$TOC - $TIC" | bc -l)

    END=5
    for i in $(seq 1 $END);
        do 
            TIC=$(date +%s.%N);
            ./eg_par_omp.out $j
            TOC=$(date +%s.%N);
            duration=$(echo "$TOC - $TIC" | bc -l)
            res=$(echo "${min_duration} > ${duration}" | bc -l)
            if [ "$res" -eq 1 ] 
            then
               min_duration=$duration
            fi
        done
    b+=("$j $min_duration")
    done

printf '%s\n' "${b[@]}" > perf_omp_weak.dat
