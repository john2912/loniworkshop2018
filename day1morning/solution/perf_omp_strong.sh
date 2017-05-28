#!/bin/bash +x
#make

mkdir -p scaling_strong
BASE=100000000
MULTI=(01 02 04 08 16 32)
P_ARR=(1 2 4 8 16)
for k in "${MULTI[@]}";
    do
    b=()
    for j in "${P_ARR[@]}";
        do  
        export OMP_NUM_THREADS=$j
        # a warm-up run
        TIC=$(date +%s.%N);
        ./eg_par_omp.out $BASE $k #> /dev/null 2>&1#> /dev/null 2>&1
        TOC=$(date +%s.%N);
        min_duration=$(echo "$TOC - $TIC" | bc -l)
    
        # run the test 3 times, get the best performance
        END=3
        for i in $(seq 1 $END);
            do 
                TIC=$(date +%s.%N);
                ./eg_par_omp.out $BASE $k #> /dev/null 2>&1
                TOC=$(date +%s.%N);
                duration=$(echo "$TOC - $TIC" | bc -l)
                #echo "dur=$duration sec"
                res=$(echo "${min_duration} > ${duration}" | bc -l)
                if [ "$res" -eq 1 ] 
                then
                   min_duration=$duration
                fi
            done
        b+=("$j $min_duration")
    done
    printf '%s\n' "${b[@]}" > ./scaling_strong/perf_omp_strg_$k.dat
done

paste ./scaling_strong/perf_omp_strg_* > ./perf_omp_strg.dat

