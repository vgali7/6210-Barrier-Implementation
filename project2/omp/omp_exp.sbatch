#!/bin/bash

#SBATCH -J cs6210-proj2-omp_exp
#SBATCH --array=2-8
#SBATCH -N 1 --cpus-per-task=8
#SBATCH --mem-per-cpu=1G
##SBATCH -t 5
#SBATCH -q coc-ice
#SBATCH -o out/omp_exp_%A_%a.out

echo "Started on `/bin/hostname` with task id $SLURM_ARRAY_TASK_ID"


cd ~/scratch/6210-Barrier-Implementation/project2/omp

module load gcc/12.3.0 mvapich2/2.3.7-1
# make clean
# make all

./mp1 $SLURM_ARRAY_TASK_ID
./mp2 $SLURM_ARRAY_TASK_ID
