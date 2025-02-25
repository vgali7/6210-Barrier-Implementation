#include <omp.h>
#include "gtmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int global_num_threads;
static int global_num_rounds;
static int global_sense = 0;

typedef enum {
    winner,
    loser,
    bye,
    champion,
    dropout
} role_;

typedef struct {
    role_ role;
    int opponent;
    int flag;
} round_t;
static round_t** rounds;

void print_barrier(round_t** rounds) {
    for (int k = 0; k < global_num_rounds; k++) {
        for (int i = 0; i < global_num_threads; i++) {
            if (rounds[i][k].role == 4) {continue;}
            printf("Thread %d at round %d is a %d and its opponent is %d\n----------------\n", i, k, rounds[i][k].role, rounds[i][k].opponent);
        }
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
} 


void gtmp_init(int num_threads){
    printf("Tournament Barrier\n");
    global_num_threads = num_threads;
    global_num_rounds = (int) ceil(log2(num_threads));

    rounds = calloc(num_threads, sizeof(round_t *));
    for (int i = 0; i < num_threads; i++) {
        rounds[i] = calloc(global_num_rounds, sizeof(round_t));
    }

    for (int i = 0; i < num_threads; i++) {
        for (int k = 0; k < global_num_rounds; k++) {
            rounds[i][k].flag = 0;
            rounds[i][k].opponent = -1;
            int pow_k  = 1 << k;
            int pow_k1 = 1 << (k+1);

            if ((i == 0) && (k == global_num_rounds - 1)) {
                rounds[i][k].role = champion;
                rounds[i][k].opponent = i + pow_k;
            } else if ((i % pow_k1) == 0) {
                if ((i + pow_k < num_threads)) {
                    rounds[i][k].role = winner;
                    rounds[i][k].opponent = i + pow_k;
                } else {
                    rounds[i][k].role = bye;
                }
            } else if ((i % pow_k1) == pow_k) {
                rounds[i][k].role = loser;
                rounds[i][k].opponent = i - pow_k;
            } else {
                rounds[i][k].role = dropout;
            }
        }
    }
    // print_barrier(rounds);
}

void gtmp_barrier(){
    int tid = omp_get_thread_num();
    printf("The current thread at the barrier is %d\n", tid);
    role_ cur_role;
    int cur_opponent;
    int local_sense = 1 - global_sense;
    

    //barrier
    for (int k = 0; k < global_num_rounds; k++) {
        // printf("The thread %d is the barrier in level %d\n---------\n", tid, k);
        cur_role = rounds[tid][k].role;
        cur_opponent = rounds[tid][k].opponent;
        if (cur_role == loser) { //loser
            #pragma omp atomic write
            rounds[cur_opponent][k].flag = local_sense;
            #pragma omp flush
            // printf("I (%d) just lost, let me wake up %d at %d\n", tid, cur_opponent, k);
            while (rounds[tid][k].flag != local_sense) {
                #pragma omp flush
            }
            break;

        } else if (cur_role == 0) { //winner
            // printf("%d starting win at %d\n", tid, k);
            while (rounds[tid][k].flag != local_sense) {
                #pragma omp flush
            }
            // printf("%d ending win\n", tid);

        } else if (cur_role == 2 || cur_role == dropout) {
            rounds[tid][k].flag = 1 - local_sense;
            //bye
        } else if (cur_role == 3) { //champion
            while (rounds[tid][k].flag != local_sense) {
                #pragma omp flush
            }
            printf("----------All %d of the threads have arrived :)----------\n", global_num_threads);
            #pragma omp atomic write
            rounds[cur_opponent][k].flag = local_sense;
            global_sense = 1 - global_sense;
            #pragma omp flush
            break;
        }
    }
    for (int k = global_num_rounds-1; k >= 0; k--) {
        role_ cur_role = rounds[tid][k].role;
        int cur_opponent = rounds[tid][k].opponent;
        if (cur_role == winner) {
            #pragma omp atomic write
            rounds[cur_opponent][k].flag = local_sense;
            #pragma omp flush
        }
    }
}


void gtmp_finalize(){
    for (int i = 0; i < global_num_threads; i++){
        free(rounds[i]);
    }
    free(rounds);
}

