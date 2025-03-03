#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>
#include "gtcombined.h"

// MPI Centralized Sense-Reversing Barrier
static int mpi_global_sense = 1;
static int mpi_global_num_processes;

// OpenMP Tournament Barrier
typedef enum { winner, loser, bye, champion, dropout } role_;
typedef struct { role_ role; int opponent; int flag; } round_t;

static int omp_global_num_threads;
static int omp_global_num_rounds;
static int omp_global_sense = 0;
static round_t** omp_rounds;

void gtcombined_init(int num_processes, int num_threads) {
    gtmpi_init(num_processes);
    gtmp_init(num_threads);
}

void gtcombined_barrier() {
    #pragma omp parallel shared(omp_global_num_threads)
    {
        gtmp_barrier();
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // printf("Threads done for %d\n", rank);

    gtmpi_barrier();
}

void gtcombined_finalize() {
    gtmpi_finalize();
    gtmp_finalize();
}

void gtmpi_init(int num_processes) {
    mpi_global_num_processes = num_processes;
}

void gtmpi_barrier() {
    int local_sense = mpi_global_sense;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int msg;
        for (int i = 1; i < mpi_global_num_processes; i++)
            MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        mpi_global_sense = 1 - mpi_global_sense;
    } else {
        MPI_Send(&local_sense, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Bcast(&mpi_global_sense, 1, MPI_INT, 0, MPI_COMM_WORLD);
    while (local_sense == mpi_global_sense);
}

void gtmpi_finalize() {}


void gtmp_init(int num_threads) {
    omp_global_num_threads = num_threads;
    omp_global_num_rounds = (int)ceil(log2(num_threads));

    omp_rounds = calloc(num_threads, sizeof(round_t*));
    for (int i = 0; i < num_threads; i++)
        omp_rounds[i] = calloc(omp_global_num_rounds, sizeof(round_t));

    for (int i = 0; i < num_threads; i++) {
        for (int k = 0; k < omp_global_num_rounds; k++) {
            int pow_k = 1 << k;
            int pow_k1 = 1 << (k+1);

            if (i == 0 && k == omp_global_num_rounds - 1) {
                omp_rounds[i][k] = (round_t){champion, i + pow_k, 0};
            } else if (i % pow_k1 == 0) {
                omp_rounds[i][k] = (i + pow_k < num_threads) ? 
                    (round_t){winner, i + pow_k, 0} : (round_t){bye, -1, 0};
            } else if (i % pow_k1 == pow_k) {
                omp_rounds[i][k] = (round_t){loser, i - pow_k, 0};
            } else {
                omp_rounds[i][k] = (round_t){dropout, -1, 0};
            }
        }
    }
}

void gtmp_barrier() {
    int tid = omp_get_thread_num();
    int local_sense = 1 - omp_global_sense;

    for (int k = 0; k < omp_global_num_rounds; k++) {
        role_ role = omp_rounds[tid][k].role;
        int opponent = omp_rounds[tid][k].opponent;

        if (role == loser) {
            #pragma omp atomic write
            omp_rounds[opponent][k].flag = local_sense;
            #pragma omp flush
            while (omp_rounds[tid][k].flag != local_sense) {
                #pragma omp flush
            }
            break;
        } 
        else if (role == winner) {
            while (omp_rounds[tid][k].flag != local_sense) {
                #pragma omp flush
            }
        }
        else if (role == champion) {
            while (omp_rounds[tid][k].flag != local_sense) {
                #pragma omp flush
            }
            #pragma omp atomic write
            omp_rounds[opponent][k].flag = local_sense;
            omp_global_sense = local_sense;
            break;
        }
    }

    for (int k = omp_global_num_rounds-1; k >= 0; k--) {
        if (omp_rounds[tid][k].role == winner) {
            #pragma omp atomic write
            omp_rounds[omp_rounds[tid][k].opponent][k].flag = local_sense;
            #pragma omp flush
        }
    }
}

void gtmp_finalize() {
    for (int i = 0; i < omp_global_num_threads; i++){
        free(omp_rounds[i]);
    }
    free(omp_rounds);
}