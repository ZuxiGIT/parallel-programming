#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define r "[rank=%d]"

int main(int argc, char* argv[])
{
    int commsize, my_rank;                                                      
    MPI_Init(&argc, &argv);                                                     
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(argc != 2)
        return -1;

    int N = atoi(argv[1]);

    int buff = 0;

    int res = 0;

    if(my_rank == 0)
    {
        fprintf(stdout, "[rank=%d] my rank is %d\n",
                                my_rank, my_rank);

        double time = MPI_Wtime();

        fprintf(stdout, "[rank=%d] Begun at %lf seconds [N = %d]\n",
                                my_rank, time, N);
        while(buff != N)
        {
            if(commsize > 1)
            {
                res = MPI_Send(&buff, 1, MPI_INT, 1,
                                   1, MPI_COMM_WORLD);

                if(res != MPI_SUCCESS)
                    fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                                    my_rank, my_rank, my_rank + 1);
                else
                    ;//fprintf(stdout, "[rank=%d] Sent number  \"%d\" to rank %d\n",
                     //                my_rank, buff, my_rank+1);
            }
            else
            {
                fprintf(stderr, "[rank=%d] Too few compute elements\n");
                return 0;
            }


            res = MPI_Recv(&buff, 1, MPI_INT, 1,
                               MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(res != MPI_SUCCESS)
                fprintf(stderr, "[rank=%d] Error ocurred while receiving\n",
                                my_rank);
            else
                ;//fprintf(stdout, "[rank=%d] Received number\"%d\"\n",
                 //                my_rank, buff);

            buff++;
        }
        double time2 = MPI_Wtime();

        fprintf(stdout, "[rank=%d] Ended at %lf seconds\n",
                                my_rank, time2);

        fprintf(stdout, "[rank=%d] Ellapsed time: %lf seconds [%lf msg/s]\n",
                                my_rank, time2 - time, (2*N)/(time2-time));
        buff = -1;

        res = MPI_Send(&buff, 1, MPI_INT, 1,
                           1, MPI_COMM_WORLD);

        if(res != MPI_SUCCESS)
            fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                            my_rank, my_rank, my_rank + 1);
        else
            ;//fprintf(stdout, "[rank=%d] Sent number  \"%d\" to rank %d\n",
             //                my_rank, buff, my_rank+1);
    }
    else
    {
        fprintf(stdout, "[rank=%d] my rank is %d\n",
                                my_rank, my_rank, 0);
        while(1)
        {
            int res = MPI_Recv(&buff, 1, MPI_INT, 0,
                               MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(res != MPI_SUCCESS)
                fprintf(stderr, "[rank=%d] Error ocurred while receiving\n",
                                my_rank);
            else
                ;//fprintf(stdout, "[rank=%d] Received number \"%d\"\n",
                 //                my_rank, buff);

            if(buff < 0 || buff == N)
            {
//                fprintf(stdout, "[rank=%d] buff is %d\n\tExit...\n",
                                //my_rank, buff);
                break;
            }

            res = MPI_Send(&buff, 1, MPI_INT, 0,
                           1, MPI_COMM_WORLD);

            if(res != MPI_SUCCESS)
                fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                                my_rank, my_rank, 0);
            else
                ;//fprintf(stdout, "[rank=%d] Sent number \"%d\" to rank %d\n",
                 //                my_rank, buff, 0);
        }
    }

    MPI_Finalize();
    return 0;
}
