#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>


static const int NUM_THREADS = 20;
bool f[20] = {};
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    int tid;
    double stuff;
} thread_data_t;

void* thr_func(void* arg)
{
    thread_data_t* data = (thread_data_t*)arg;

    while(true)
    {
        pthread_mutex_lock(&m);

        if(data->tid == 0 || f[data->tid-1])
        {
            pthread_mutex_unlock(&m);
            break;
        }
        pthread_mutex_unlock(&m);
    }
    printf("hello from thr_func; id = %3d [overall=%d]\n", data->tid, NUM_THREADS);
    pthread_mutex_lock(&m);
    f[data->tid] = true;
    pthread_mutex_unlock(&m);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    thread_data_t thr_data[NUM_THREADS];

    pthread_t thr[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++)
    {
        thr_data[i].tid = i;

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
