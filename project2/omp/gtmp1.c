#include <omp.h>
#include "gtmp.h"
#include <stdio.h>
#include <stdlib.h>

static int global_num_threads;
static int global_sense;
static int count;
static int* local_sense;

void gtmp_init(int num_threads){
    printf("Centralized Barrier with Sense Reversal\n");
    printf("WE ARE STARTING %d with %d threads\n----------\n", 1, num_threads);

    global_num_threads = num_threads;
    count = num_threads;
    global_sense = 0;
    
    local_sense = calloc(num_threads, sizeof(int));
    for (int i = 0; i < num_threads; i++) {
        local_sense[i] = 0;
    }
}

void gtmp_barrier(){
    int tid = omp_get_thread_num();
    printf("The current thread at the barrier is %d\n", tid);
    
    local_sense[tid] = (local_sense[tid] + 1) % 2;
    int cur_sense = local_sense[tid];

    int num_left;
    #pragma omp atomic capture
    int num_left = --count;

    if (num_left == 0) {
        printf("----------All %d of the threads have arrived :)----------\n", global_num_threads);
        count = global_num_threads;
        global_sense = (global_sense + 1) % 2;
        #pragma omp flush(global_sense)
    } else {
        while (global_sense != cur_sense) {
            #pragma omp flush(global_sense)
        }
    }

}

void gtmp_finalize(){
    free(local_sense);
}

