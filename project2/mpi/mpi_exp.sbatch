#!/bin/bash

#SBATCH -J cs6210-proj2-mpi_exp
#SBATCH --array=2-12
#SBATCH -N 12 --ntasks-per-node=1
#SBATCH --mem-per-cpu=1G
##SBATCH -t 5
#SBATCH -q coc-ice
#SBATCH -o out/mpi_exp_%A_%a.out

echo "Started on `/bin/hostname` with task id $SLURM_ARRAY_TASK_ID"


cd ~/scratch/6210-Barrier-Implementation/project2/mpi

module load gcc/12.3.0 mvapich2/2.3.7-1
# make clean
# make all


srun -N $SLURM_ARRAY_TASK_ID mpi1 $SLURM_ARRAY_TASK_ID
srun -N $SLURM_ARRAY_TASK_ID mpi2 $SLURM_ARRAY_TASK_ID
