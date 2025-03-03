#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include "gtmp.h"

int main(int argc, char** argv)
{
  int num_threads, num_iter=100000;

  if (argc < 2){
    fprintf(stderr, "Usage: ./harness [NUM_THREADS]\n");
    exit(EXIT_FAILURE);
  }
  num_threads = strtol(argv[1], NULL, 10);

  omp_set_dynamic(0);
  if (omp_get_dynamic())
    printf("Warning: dynamic adjustment of threads has been set\n");

  omp_set_num_threads(num_threads);
  
  gtmp_init(num_threads);

  double total_time = 0;
  struct timeval start, end;
#pragma omp parallel shared(num_threads)
   {
     for(int i = 0; i < num_iter; i++){
      #pragma omp single
      gettimeofday(&start, NULL); 

      gtmp_barrier();

      #pragma omp single
      {
        gettimeofday(&end, NULL);
        total_time += (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
      }
    }
   }

  double avg_time = total_time / num_iter;
  printf("Avg time: %f\n", avg_time);

   gtmp_finalize();

   return 0;
}
