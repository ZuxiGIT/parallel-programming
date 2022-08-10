rm -v put_job.*
mpicc main.c -std=c99 -lm
qsub ex.sh
