#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include "gtmpi.h"

int global_sense = 1;
int global_num_processes;

void gtmpi_init(int num_processes) {
    printf("Centralized Barrier with Sense Reversal\n");
    global_num_processes = num_processes;
}

void gtmpi_barrier() {
    int local_sense = global_sense;
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // printf("%d just arrived\n", rank);

    if (rank == 0) {
        int msg;
        for (int i = 1; i < global_num_processes; i++) {
            MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        // printf("All processes have arrived at the barrier. Starting up again...\n");
        global_sense = 1 - global_sense;
    } else {
        MPI_Send(&local_sense, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Bcast(&global_sense, 1, MPI_INT, 0, MPI_COMM_WORLD);

    while (local_sense == global_sense);
}

void gtmpi_finalize() {
}
