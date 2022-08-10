rm -v pthread_s0191913.*
gcc main.c -std=c99 -lm -pthread -Werror && qsub ex.sh
