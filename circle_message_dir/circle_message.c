#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int commsize, my_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    char buff[100] = {};

    if(my_rank == 0)
    {
        if(commsize > 1)
        {
            char* str = "TestString";

            int res = MPI_Send(str, strlen(str) + 1, MPI_CHAR, my_rank+1,
                               1, MPI_COMM_WORLD);

            if(res != MPI_SUCCESS)
                fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                                my_rank, my_rank, my_rank + 1);
            else
                fprintf(stdout, "[rank=%d] Sent string \"%s\" to rank %d\n",
                                my_rank, str, my_rank+1);
        }
        else
            fprintf(stderr, "[rank=%d] Too few compute elements\n");


        int res = MPI_Recv(buff, sizeof(buff), MPI_CHAR, MPI_ANY_SOURCE,
                           MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(res != MPI_SUCCESS)
            fprintf(stderr, "[rank=%d] Error ocurred while receiving\n",
                            my_rank);
        else
            fprintf(stdout, "[rank=%d] Received string \"%s\"\n",
                            my_rank, buff);

        if(strncmp(buff, "TestString", strlen("TestString")) == 0)
            fprintf(stderr, "[rank=%d] Everything is OK!\n", my_rank);
        else
            fprintf(stderr, "[rank=%d] Everything is NOT OK!\n", my_rank);
    }
    else
    {
        int dest = 0;

        if(my_rank < commsize - 1)
            dest = my_rank + 1;

        int res = MPI_Recv(buff, sizeof(buff), MPI_CHAR, MPI_ANY_SOURCE,
                           MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(res != MPI_SUCCESS)
            fprintf(stderr, "[rank=%d] Error ocurred while receiving\n",
                            my_rank);
        else
            fprintf(stdout, "[rank=%d] Received string \"%s\"\n",
                            my_rank, buff);

        res = MPI_Send(buff, sizeof(buff), MPI_CHAR, dest,
                       1, MPI_COMM_WORLD);

        if(res != MPI_SUCCESS)
            fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                            my_rank, my_rank, dest);
        else
            fprintf(stdout, "[rank=%d] Sent string \"%s\" to rank %d\n",
                            my_rank, buff, dest);
    }


    MPI_Finalize();
	return 0;
}
