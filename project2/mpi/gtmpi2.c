#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include "gtmpi.h"

int global_num_processes;

void gtmpi_init(int num_processes) {
    printf("MCS Barrier\n");
    global_num_processes = num_processes;
}

void gtmpi_barrier() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int local_sense = 0;
    printf("%d just arrived\n", rank);

    int start = (rank * 4) + 1;
    
    for (int i = start; i < start + 4; i++) {
        if (i >= global_num_processes) {
            break;
        }
        // printf("%d waiting on %d\n", rank, i);
        int msg = 0;
        MPI_Recv(&msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("%d received from %d\n", rank, i);
        fflush(stdout);
    }

    int parent = (rank - 1) / 4;;
    if (rank != 0) {
        int msg = 0;
        // printf("%d sent to parent: %d\n", rank, parent);
        fflush(stdout);
        MPI_Send(&msg, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
    }


    if (rank == 0) {
        for (int i = 1; i <= 4; i++) {
            if (i < global_num_processes) {
                int msg = 0;
                // printf("sTARTING THE DOWNFAL--\n");
                MPI_Send(&msg, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            }
        }
        // printf("---- ALL DONE :)---\n");
    } else {
        int msg = 0;
        MPI_Recv(&msg, 1, MPI_INT, parent, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("%d going DOWN from %d\n", rank, parent);
        for (int i = start; i < start + 4; i++) {
            if (i < global_num_processes) {
                int msg = 0;
                MPI_Send(&msg, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            }
        }
    }
    

}

void gtmpi_finalize() {
}
