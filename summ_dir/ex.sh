#!/bin/bash

#PBS -l walltime=00:01:00,nodes=1:ppn=3
#PBS -N summ 
#PBS -q batch

cd $PBS_O_WORKDIR
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 1000000
