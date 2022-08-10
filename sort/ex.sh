#!/bin/bash

#PBS -l walltime=00:02:00,nodes=1:ppn=4
#PBS -N sort 
#PBS -q batch

cd $PBS_O_WORKDIR
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 10
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 100
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 1000
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 10000
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 100000
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 1000000
mpirun --hostfile $PBS_NODEFILE -np 1 ./a.out 10000000

mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 10
mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 100
mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 1000
mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 10000
mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 100000
mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 1000000
mpirun --hostfile $PBS_NODEFILE -np 2 ./a.out 10000000

mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 10
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 100
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 1000
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 10000
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 100000
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 1000000
mpirun --hostfile $PBS_NODEFILE -np 3 ./a.out 10000000

mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 10
mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 100
mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 1000
mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 10000
mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 100000
mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 1000000
mpirun --hostfile $PBS_NODEFILE -np 4 ./a.out 10000000
