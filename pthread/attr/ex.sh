#!/bin/bash

#PBS -l walltime=00:02:00,nodes=1:ppn=1
#PBS -N pthread_s0191913 
#PBS -q batch

cd $PBS_O_WORKDIR
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 
