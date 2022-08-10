#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

const int NUM_THREADS = 20;

typedef struct 
{
    int tid;
    double stuff;
} thread_data_t;

double shared_x;
pthread_mutex_t lock_x;


void* thr_func(void* arg)
{
    thread_data_t* data = (thread_data_t*)arg;
    pthread_mutex_lock(&lock_x);
    shared_x += data->stuff;
    printf("[thr=%3d] hello from thr_func; id = %3d [overa=%d]\nx = %lf (+%lf)\n", 
            data->tid, data->tid, NUM_THREADS, shared_x, data->stuff);
    pthread_mutex_unlock(&lock_x);

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    thread_data_t thr_data[NUM_THREADS];

    pthread_t thr[NUM_THREADS];

    shared_x = 0;
    pthread_mutex_init(&lock_x, NULL);

    for(int i = 0; i < NUM_THREADS; i++)
    {
        thr_data[i].tid = i;
        thr_data[i].stuff = (i + 1) * NUM_THREADS;

        int rc;

        if((rc = pthread_create(thr + i, NULL, thr_func, thr_data + i)))
        {
            fprintf(stderr, "Failed to create thread; rc = %d\n,", rc);
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++)
        pthread_join(thr[i], NULL);

    return 0;
}
