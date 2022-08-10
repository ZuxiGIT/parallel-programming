#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define r "[rank=%d] "

int main(int argc, char* argv[])
{
    int commsize, my_rank;                                                      
    MPI_Init(&argc, &argv);                                                     
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank == 0)
    {
        if(argc < 2)
        {
            printf(r"\"N\" was not provided\n", my_rank);
            exit(EXIT_FAILURE);
        }

        int N = atoi(argv[1]);
        if(N == 0)
        {
            printf(r"\"N\" is not a number or equal 0\n", my_rank);
            exit(EXIT_FAILURE);
        }

        long double sum = 0;

        for(int i = 1; i <= commsize-1; i ++) 
        { 
            long double temp = 0;

            int res = MPI_Recv(&temp, 1, MPI_LONG_DOUBLE, i, MPI_ANY_TAG,
                               MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            if(res != MPI_SUCCESS)
                fprintf(stderr, r"Error ocurred while getting number from %d\n",
                                            my_rank, i);
            else
                fprintf(stdout, r"Got number \"%Lf\" from rank %d\n",
                                my_rank, temp, i);

            sum += temp;

        }

        fprintf(stdout, r"Result is \"%Lf\"\n", my_rank, sum);
    }
    else
    {
        if(argc < 2)
            exit(EXIT_FAILURE);

        int N = 0;

        if((N = atoi(argv[1])) == 0)
            exit(EXIT_FAILURE);

        int range = N / (commsize - 1);

        long double sum = 0;

        int start = (my_rank - 1) * range;
        int finish = my_rank * range;

        if(my_rank == (commsize - 1))
            finish = N;

        fprintf(stdout, r"My range is (%d, %d]\n",
                        my_rank, start, finish);

        
        for(int i = finish; i > start; i--) 
            sum += 1. / (double)i;

        int res = MPI_Send(&sum, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);

        if(res != MPI_SUCCESS)
            fprintf(stderr, r"Error ocurred while sending from %d to %d\n",
                                        my_rank, my_rank, 0);
        else
            fprintf(stdout, r"Sent \"%Lf\" to rank %d\n",
                            my_rank, sum, 0);

    }

    MPI_Finalize();

    return 0;
}
