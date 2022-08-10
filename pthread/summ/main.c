#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

const int NUM_THREADS = 100;
int N;

typedef struct 
{
    int tid;
} thread_data_t;

long double shared_x;

pthread_mutex_t lock_x;

void* thr_func(void* arg)
{
    thread_data_t* data = (thread_data_t*)arg;

    long double res = 0;

    int range = (int)ceil((double) N / (double) NUM_THREADS); 

    int start = range * data->tid;
    int finish = data->tid == NUM_THREADS - 1 ? N : range * (data->tid + 1);

    if(!data->tid)
        start ++;

    for(int i = finish - 1 ; i >= start; i --) 
        res += 1.f / (double) i;

    pthread_mutex_lock(&lock_x);

    shared_x = shared_x + res;

    pthread_mutex_unlock(&lock_x);

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Error: N was not provided\n");
        exit(EXIT_FAILURE);
    }
    
    N = atoi(argv[1]);

    if(!N)
    {
        fprintf(stderr, "Error: wrong input\n");
        exit(EXIT_FAILURE);
    }

    thread_data_t thr_data[NUM_THREADS];

    pthread_t thr[NUM_THREADS];

    shared_x = 0;

    pthread_mutex_init(&lock_x, NULL);

    for(int i = 0; i < NUM_THREADS; i++)
    {
        thr_data[i].tid = i;

        int rc;

        if((rc = pthread_create(thr + i, NULL, thr_func, thr_data + i)))
        {
            fprintf(stderr, "Failed to create thread; rc = %d\n", rc);
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++)
        pthread_join(thr[i], NULL);

    printf("res = %Lf\n", shared_x);

    return 0;
}
