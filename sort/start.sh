rm -v sort.*
mpicc main.c -std=c99 -lm -Werror && qsub ex.sh
