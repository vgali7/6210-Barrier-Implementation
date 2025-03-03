#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "gtcombined.h"

int main(int argc, char *argv[])
{
    int num_threads, num_iter = 100000;

    if (argc < 2){
        fprintf(stderr, "Usage: ./harness [NUM_THREADS]\n");
        exit(EXIT_FAILURE);
    }
    num_threads = strtol(argv[1], NULL, 10);
    
    // mpi setup
    MPI_Init(NULL, NULL);

    int rank;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // openmp setup
    omp_set_dynamic(0);
    if (omp_get_dynamic())
        printf("Warning: dynamic adjustment of threads has been set\n");
    omp_set_num_threads(num_threads);

    gtcombined_init(size, num_threads);

    int i = 0;
    double total_time = 0;
    struct timeval start, end;
    for(i = 0; i < num_iter; i++) {
        gettimeofday(&start, NULL); 

        gtcombined_barrier();

        gettimeofday(&end, NULL);
        total_time += (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
    }

    double avg_time = total_time / num_iter;
    if (rank == 0) {
        printf("Avg time: %f\n", avg_time);
    }

    MPI_Finalize();
    return 0;
}
