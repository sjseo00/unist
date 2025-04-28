#!/bin/sh
#SBATCH -J MPI_job
#SBATCH --time=01:00:00
#SBATCH -o %x_%j.out
#SBATCH -e %x_%j.err
#SBATCH -p gpu_k80
#SBATCH -N 2 # number of node
#SBATCH -n 8 # total process

mpirun ./test
