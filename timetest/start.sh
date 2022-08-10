rm -v time_test.*
mpicc main.c -std=c99 -lm
qsub ex.sh
