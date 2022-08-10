#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define handle_error_en(en, msg)\
    do{ errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

const int NUM_THREADS = 20;

typedef struct 
{
    int tid;
} thread_data_t;

pthread_mutex_t lock_x;


static void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
   int s, i;
   size_t v;
   void *stkaddr;
   struct sched_param sp;

   s = pthread_attr_getdetachstate(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getdetachstate");
   printf("%sDetach state        = %s\n", prefix,
           (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
           (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
           "???");

   s = pthread_attr_getscope(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getscope");
   printf("%sScope               = %s\n", prefix,
           (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
           (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
           "???");

   s = pthread_attr_getinheritsched(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getinheritsched");
   printf("%sInherit scheduler   = %s\n", prefix,
           (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
           (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
           "???");

   s = pthread_attr_getschedpolicy(attr, &i);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getschedpolicy");
   printf("%sScheduling policy   = %s\n", prefix,
           (i == SCHED_OTHER) ? "SCHED_OTHER" :
           (i == SCHED_FIFO)  ? "SCHED_FIFO" :
           (i == SCHED_RR)    ? "SCHED_RR" :
           "???");

   s = pthread_attr_getschedparam(attr, &sp);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getschedparam");
   printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);

   s = pthread_attr_getguardsize(attr, &v);
   if (s != 0)
       handle_error_en(s, "pthread_attr_getguardsize");
   printf("%sGuard size          = %d bytes\n", prefix, v);

   //s = pthread_attr_getstack(attr, &stkaddr, &v);
   //if (s != 0)
   //    handle_error_en(s, "pthread_attr_getstack");
   //printf("%sStack addres        = %p bytes\n", prefix, stkaddr);
   //printf("%sStack size          = 0x%X bytes\n", prefix, v);

   printf("---------------------------------------------\n");
}

void* thr_func(void* arg)
{
    thread_data_t* data = (thread_data_t*)arg;

    
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    char buff[32] = {};
    sprintf(buff, "[thr=%3d]", data->tid);

    pthread_mutex_lock(&lock_x);

    display_pthread_attr(&attr, buff);

    pthread_mutex_unlock(&lock_x);
    pthread_attr_destroy(&attr);

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    thread_data_t thr_data[NUM_THREADS];

    pthread_t thr[NUM_THREADS];

    pthread_mutex_init(&lock_x, NULL);

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
