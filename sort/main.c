#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

#define r "[rank = %d]"
#define $ ;//fprintf(stdout, r" Debug (%d)\n", rank, __LINE__);

int select_min(int* arr, int arr_size, int commsize, int* p)
{
    int min = 0x7fffffff;
    int t = -1;
    int range = (int)ceil((double)arr_size / (double)commsize);

    for(int i = 0; i < commsize; i++)
    {
        if(p[i] >= range*(i+1) || p[i] >= arr_size )
            continue;

        if(arr[p[i]] < min)
        {
            min = arr[p[i]];
            t = i;
        }
    }

    //fprintf(stdout, "min is in arr[%d] [i = %d, min = %d]", p[t], t, min);
    //fprintf(stdout, "\tp[%d] is %d\n", t, p[t]);

    p[t] += 1;

    return min;
}

void merge_multiple(int* arr, int commsize, int arr_size)
{
    int* res = (int*)calloc(commsize + arr_size, sizeof(int));
    int* p = res + arr_size; 

    int range = (int)ceil((double)arr_size / (double)commsize);

    for(int i = 0; i < commsize-1; i ++)
        p[i] = i * range;

    p[commsize-1] = (commsize-1)*range;

    for(int i = 0; i < arr_size; i++)
    {
        res[i] = select_min(arr, arr_size, commsize, p);
    }

    memcpy(arr, res, arr_size*sizeof(int));
    free(res);

    return;
}

void print_arr(int rank, int* arr, int size)
{
    for(int i = 0; i < size; i ++)
        printf(r" arr[%d] = %d\n", rank, i, arr[i]);
}

int* merge(int* a , int* b, int a_size, int b_size)
{
    int* res = (int*) calloc(a_size, sizeof(int));
    memcpy(res, a, a_size*sizeof(int));

    // replacing "a" with "res"

    int* temp = a;
    a = res;
    res = temp;

    int a_ind = 0;
    int b_ind = 0;

    for(int i = 0; i < a_size + b_size; i++)
        if(a_ind < a_size && b_ind < b_size)
        {
            if(a[a_ind] < b[b_ind])
                res[i] = a[a_ind++];
            else
                res[i] = b[b_ind++];
        }
        else
        {
            if(a_ind < a_size)
                res[i] = a[a_ind++];
            else
                res[i] = b[b_ind++];
        }

    free(a);

    return res;
}

int* mergeSort(int* arr, int size)
{
    if(size == 2)
    {
        if(arr[0] > arr[1])
        {
            int temp = arr[0];
            arr[0] = arr[1];
            arr[1] = temp;
        }
        return arr;
    }

    if(size == 1)
        return arr;

    int mid = size / 2;

    int* a = mergeSort(arr, mid);
    int* b = mergeSort(arr + mid, size - mid);
    int* res = merge(a, b, mid, size-mid);

    return res;
}

int main(int argc, char** argv)
{
    int commsize, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));


    if(argc < 2)
    {
        fprintf(stderr, r" Array size was not provided\n", rank);
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);

    if(N == 0)
    {
        fprintf(stderr, r" Wrong input\n", rank);
        exit(EXIT_FAILURE);
    }

    double time;

    int range = (int)ceil((double)N / (double)commsize);

    if(rank == commsize - 1)
        range = N - (commsize - 1) * range;

    //fprintf(stdout, r"---------range = %d\n", rank, range);

    int* arr = NULL;

    if(!rank)
    {
        arr = (int*) calloc(N, sizeof(int));

        if(!arr)
        {
            fprintf(stderr, r" Failed to allocate memore (int) [range=%d]\n", rank, range);
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < N; i++)
            arr[i] = rand() % N;
        
        time = MPI_Wtime();
    }
    else
    {
        arr = (int*)calloc(range, sizeof(int));

        if(!arr)
        {
            fprintf(stderr, r" Failed to allocate memore (int) [range=%d]\n", rank, range);
            exit(EXIT_FAILURE);
        }
    }



    $
    int* counts = NULL;
    int* displs = NULL;

    if(!rank)
    {
        counts = (int*)calloc(commsize, sizeof(int));
        displs = (int*)calloc(commsize, sizeof(int));

        for(int i = 0; i < commsize; i++)
        {
            counts[i] = range;
            displs[i] = range*i;
        }
        counts[commsize-1] = N - (commsize - 1)*range;

    //    printf(r" Sending array (size = %d)\n", rank, range);
    }
    
    //printf(r" arr = %p, range = %d, counts = %p, displs = %p\n", rank, arr, range, counts, displs);
    //
    MPI_Scatterv(arr, counts, displs, MPI_INT, arr, range, MPI_INT, 0, MPI_COMM_WORLD);
    $

    //printf(r"-----------------Got array (size = %d)\n", rank, range);
    //print_arr(rank, arr, range);

    int* res = mergeSort(arr, range);

    //if(!rank)
    //    printf(r" res = %X arr = %X equal: %d\n", rank, res, arr, res == arr ? 1 : 0);

    //if(rank) 
    //{
    //    printf(r"-------------Sending array (size=%d)\n", rank, range);
    //    print_arr(rank, arr, range);
    //}

    $

    MPI_Gatherv(res, range, MPI_INT, res, counts, displs, MPI_INT, 0, MPI_COMM_WORLD); 
    $

    if(!rank)
    {
        time = MPI_Wtime() - time;
        printf(r" commsize = %d, N = %d\n", rank, commsize, N);
        //printf(r "------------------before---------\n", rank);
        //print_arr(rank, arr, N);
        //merge_multiple(arr, commsize, N);
        //printf(r "------------------after---------\n", rank);
        //print_arr(rank, arr, N);
        printf(r" ellapsed time %lf seconds\n", rank, time);

        free(displs);
        free(counts);
    }

    free(arr);

    MPI_Finalize();

    return 0;
}
