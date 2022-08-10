#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

#define STACK_IS_FREE(stk)\
    (stk.sp == 0)

#define PUSH(_stk, _a, _b, _fa, _fb, _sAB)\
    do\
    {\
        if(_stk.sp == 1000)\
        {\
            printf("Oops %s is full;\n\ta = %lf b = %lf\n", #_stk, _a, _b);\
            exit(EXIT_FAILURE);\
        }\
        _stk.data[_stk.sp++] = (segment_data_t){_a, _b, _fa, _fb, _sAB};\
    } while(0)

#define POP(_stk, _a, _b, _fa, _fb, _sAB)\
    do\
    {\
        if(_stk.sp == 0)\
        {\
            printf("Oops %s is empty;\n", #_stk);\
            exit(EXIT_FAILURE);\
        }\
        segment_data_t temp = _stk.data[--_stk.sp];\
        _a = temp.a;\
        _b = temp.b;\
        _fa = temp.fa;\
        _fb = temp.fb;\
        _sAB = temp.sAB;\
    } while(0)

#define MOVE(stk_1, stk_2)\
    memcpy(stk_1.data + stk_1.sp++, stk_2.data + --stk_2.sp, sizeof(segment_data_t));

#define _thr "[thr=%3d] "
#define $ printf("[line=%3d] debug...\n", __LINE__);
#define $$ printf(_thr"[line=%3d] debug...\n", id, __LINE__);


static int NUM_THREADS;

double _eps;

const double start = 1e-5;
const double finish = 1;
static int _SPK;

static inline double f(double a)
{
    double s = sin(1/a);
    double r = 1/a;
    return r*r*s*s;
    //return sin(1/a);
}

typedef struct{
    double a;
    double b;
    double fa;
    double fb;
    double sAB;

} segment_data_t;

typedef struct{
    int sp;
    segment_data_t  data[1000];
} segment_stk_t;
    

typedef struct 
{
    int tid; 
    double eps;
    int spk;
    long double ret;
} thread_data_t;

sem_t sem_task_present;
sem_t sem_list;

int nactive = 0;
segment_stk_t global_stk = {};

void* thr_func(void* arg)
{
    thread_data_t* data = (thread_data_t*)arg;

    segment_stk_t local_stk = {};

    long double J = 0;

    int nlpush = 0;
    int nlpop = 0;
    int ngpush = 0;
    int ngpop = 0;
    int counted = 0;

    int id = data->tid;
    int SPK = data->spk;
    double eps = data->eps;

    double len = finish - start;

    double a = 0;
    double b = 0;

    double fa = 0;
    double fb = 0;

    double sAB = 0;

    time_t wall_time = time(NULL);

    while(1)
    {
        sem_wait(&sem_task_present);

        sem_wait(&sem_list);

        POP(global_stk, a, b, fa, fb, sAB);
        ngpop++;

        if(a < b) nactive++;

        sem_post(&sem_list);

        if(a >= b)
            break;

        while(1)
        {
            double c = (a + b) / 2;
            double fc = f(c);

            double sAC = (fa + fc) * (c - a) / 2;
            double sCB = (fc + fb) * (b - c) / 2;

            double sACB = sAC + sCB;
            
            if(fabs(sAB-sACB) >= eps * (b-a) / len)
            {
                PUSH(local_stk, a, c, fa, fc, sAC);
                nlpush++;

                a = c;
                fa = fc;
                sAB = sCB;
            }
            else
            {
                J += sACB;
                counted++;

                if(STACK_IS_FREE(local_stk))
                    break;

                POP(local_stk, a, b, fa, fb, sAB);
                nlpop++;
            }

            if(local_stk.sp > SPK)
            {
                sem_wait(&sem_list);

                if(STACK_IS_FREE(global_stk))
                {
                    while(local_stk.sp > 1)
                    {
                        MOVE(global_stk, local_stk);

                        nlpop++;
                        ngpush++;

                        sem_post(&sem_task_present);
                    }
                }

                sem_post(&sem_list);
            }
        }
        
        sem_wait(&sem_list);

        nactive--;

        if(!nactive && STACK_IS_FREE(global_stk))
        {
            printf(_thr"filling global stack with bad values to stop threads\n",
                        id);

            for(int i = 0; i < NUM_THREADS; i++)
            {
                PUSH(global_stk, 2.f, 1.f, 0.f, 0.f, 0.f);
                sem_post(&sem_task_present);
            }
            

            ngpush += NUM_THREADS;
        }

        sem_post(&sem_list);
    }
    wall_time = time(NULL) - wall_time;

    data->ret = J;

    printf(_thr "\tlocal_stack:  pushed: %d times; poped %d times;\n"
                "\t\tglobal_stack: pushed: %d times; poped %d times; res = %Lf\n"
                "\t\t[Wall time] ellapsed %ld minutes %ld seconds\n"
                "\t\tcounted %d times\n",
                id, nlpush, nlpop, ngpush, ngpop, J, 
                (wall_time) / 60, (wall_time) % 60,
                counted);

    return data;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Number of threads is not provided\n");
        exit(EXIT_FAILURE);
    }

    NUM_THREADS = atoi(argv[1]);

    if(!NUM_THREADS)
    {
        printf("Wrong input\n");
        exit(EXIT_FAILURE);
    }

    if(argc < 3)
    {
        printf("SPK is not provided\n");
        exit(EXIT_FAILURE);
    }

    _SPK = atoi(argv[2]);

    if(!_SPK)
    {
        printf("Wrong input\n");
        exit(EXIT_FAILURE);
    }

    if(argc < 4)
    {
        printf("eps is not provided\n");
        exit(EXIT_FAILURE);
    }

    _eps = strtod(argv[3], NULL);

    if(!_eps)
    {
        printf("Wrong input\n");
        exit(EXIT_FAILURE);
    }

    thread_data_t thr_data[NUM_THREADS];
    pthread_t thr[NUM_THREADS];

    double range = (double) (finish - start) / (double) NUM_THREADS;

    double a =  start;
    double b = finish;
    double fa = f(a);
    double fb = f(b);
    double sAB = (fa + fb) * (b - a) / 2;

    sem_init(&sem_task_present, 0, 1);
    sem_init(&sem_list, 0, 1);

    PUSH(global_stk, a, b, fa, fb, sAB);

    clock_t cpu_time = clock();
    time_t wall_time = time(NULL);

    for(int i = 0; i < NUM_THREADS; i++)
    {
        thr_data[i].tid = i;
        thr_data[i].spk = _SPK;
        thr_data[i].eps = _eps;

        int rc;

        if((rc = pthread_create(thr + i, NULL, thr_func, thr_data + i)))
        {
            fprintf(stderr, "Failed to create thread; rc = %d\n,", rc);
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }

    long double res = 0;

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(thr[i], NULL);
        res += thr_data[i].ret;
    }

    cpu_time = clock() - cpu_time;
    wall_time = time(NULL) - wall_time;

    printf("\n\n[CPU time] ellapsed %ld minutes %ld seconds\n",
            ((cpu_time) / CLOCKS_PER_SEC) / 60, ((cpu_time) / CLOCKS_PER_SEC) % 60);
    printf("[Wall time] ellapsed %ld minutes %ld seconds\n",
            (wall_time) / 60, (wall_time) % 60);

    printf("number of threads: %d\n", NUM_THREADS);
    printf("eps: %lf; start: %lf; finish: %lf\n", _eps, start, finish);

    printf("res = %Lf\n", res);
    printf("SPK = %d\n\n\n", _SPK);

    return 0;
}
