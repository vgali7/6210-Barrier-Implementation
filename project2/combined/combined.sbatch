#!/bin/bash

#SBATCH -J cs6210-proj2-combined_exp
#SBATCH --array=0-76
#SBATCH -N 12 --ntasks-per-node=1 --cpus-per-task=8
#SBATCH --mem-per-cpu=1G
##SBATCH -t 5
#SBATCH -q coc-ice
#SBATCH -o out/combined_exp_%A_%a.out

num_threads=()
num_procs=()
for proc in {2..12}; do
    for thread in {2..8}; do
        ((i++))
        num_procs+=($proc)
        num_threads+=($thread)
    done
done

num_procs=${num_procs[$SLURM_ARRAY_TASK_ID]}
num_threads=${num_threads[$SLURM_ARRAY_TASK_ID]}

echo $num_procs
echo $num_threads

echo "Started on `/bin/hostname` with task id $SLURM_ARRAY_TASK_ID with $num_procs processors and $num_threads threads"

cd ~/scratch/6210-Barrier-Implementation/project2/combined

module load gcc/12.3.0 mvapich2/2.3.7-1

mpiexec -n $num_procs ./combined $num_threads
