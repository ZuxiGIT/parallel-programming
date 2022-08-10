#!/bin/bash

#PBS -l walltime=00:10:00,nodes=1:ppn=4
#PBS -N integr_s0191913 
#PBS -q batch

$PBS_O_WORKDIR/a.out 4 20 2
