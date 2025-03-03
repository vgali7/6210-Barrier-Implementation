#ifndef GTCOMBINED
#define GTCOMBINED

void gtcombined_init(int num_processes, int num_threads);
void gtcombined_barrier();
void gtcombined_finalize();
void gtmpi_init(int num_processes);
void gtmpi_barrier();
void gtmpi_finalize();
void gtmp_init(int num_threads);
void gtmp_barrier();				
void gtmp_finalize();

#endif
