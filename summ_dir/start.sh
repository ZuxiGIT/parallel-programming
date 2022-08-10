rm -v summ.*
mpicc main.c -std=c99 -lm
qsub ex.sh
