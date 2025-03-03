#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include "gtmpi.h"

int main(int argc, char** argv)
{
  int num_processes, num_rounds = 100000;

  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc < 2){
    fprintf(stderr, "Usage: ./harness [NUM_PROCS]\n");
    exit(EXIT_FAILURE);
  }

  num_processes = strtol(argv[1], NULL, 10);

  gtmpi_init(num_processes);
  
  int k;
  double total_time = 0;
  struct timeval start, end;
  for(k = 0; k < num_rounds; k++){
    gettimeofday(&start, NULL); 
    gtmpi_barrier();
    gettimeofday(&end, NULL);

    total_time += (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
  }

  double avg_time = total_time / num_rounds;
  if (rank == 0) {
    printf("Avg time: %f\n", avg_time);
  }

  gtmpi_finalize();  

  MPI_Finalize();

  return 0;
}
