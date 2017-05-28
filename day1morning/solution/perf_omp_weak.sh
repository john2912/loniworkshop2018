#!/bin/bash +x

MULTI=(01 02 04 08 16 32)

#echo ${MULTI[@]} > perf_omp_wk.dat 

BASE=100000000
ARR=(1 2 4 8 16)
for k in "${MULTI[@]}";
    do
    NSIZE=$(echo "$BASE*$k" | bc -l)
    echo "NSIZE="$NSIZE
    b=()
    for j in "${ARR[@]}";
        do  
        export OMP_NUM_THREADS=$j
        # a warm-up run
        TIC=$(date +%s.%N);
        ./eg_par_omp.out $NSIZE $j
        TOC=$(date +%s.%N);
        min_duration=$(echo "$TOC - $TIC" | bc -l)
    
        END=5
        for i in $(seq 1 $END);
            do 
                TIC=$(date +%s.%N);
                ./eg_par_omp.out $NSIZE $j
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
    printf '%s\n' "${b[@]}" > perf_omp_wk_${k}x.dat
    done

paste perf_omp_wk_*.dat >> perf_omp_wk.dat
