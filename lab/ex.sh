#!/bin/bash

#PBS -l walltime=00:01:00,nodes=4:ppn=4
#PBS -N lab_01_s0191913 
#PBS -q batch

cd $PBS_O_WORKDIR
mpirun --hostfile $PBS_NODEFILE -np 8 ./a.out
