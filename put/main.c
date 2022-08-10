#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define r "[rank=%d]"

int main(int argc, char* argv[])
{
    int commsize, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Group  comm_group, group_acc, group_exp;

    MPI_Comm_group(MPI_COMM_WORLD, &comm_group);

    int buff = 0;

    MPI_Win win;
    MPI_Win_create(&buff, sizeof(buff), sizeof(buff), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    MPI_Win_fence(0, win);

    //MPI_Barrier(MPI_COMM_WORLD);

    fprintf(stdout, r" window was created [wind=%d]\n", rank, win);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        if(commsize > 1)
        {
            int dest = rank + 1;

            MPI_Group_incl(comm_group, 1, &dest, &group_acc);


            buff++;

            //access epoch
            MPI_Win_start(group_acc, 0, win);

            int res = MPI_Put(&buff, 1, MPI_INT, rank+1, 0, 1, MPI_INT, win);

            MPI_Win_complete(win);

            if(res != MPI_SUCCESS)
                fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                                rank, rank, rank + 1);
            else
                fprintf(stdout, "[rank=%d] Sent number \"%d\" to rank %d\n",
                                rank, buff, rank+1);
        }
        else
            fprintf(stderr, "[rank=%d] Too few compute elements\n");


        int exposure = commsize - 1;
        MPI_Group_incl(comm_group, 1, &exposure, &group_exp);

        //exposure epoch
        MPI_Win_post(group_exp, 0, win);
        MPI_Win_wait(win);

        fprintf(stdout, "res = \"%d\"\n", buff);
    }
    else
    {

        int exposure = rank - 1;
        MPI_Group_incl(comm_group, 1, &exposure, &group_exp);

        //exposure epoch
        MPI_Win_post(group_exp, 0, win);
        MPI_Win_wait(win);


        buff += 2;

        int dest = rank != commsize - 1 ? rank + 1 : 0;
        MPI_Group_incl(comm_group, 1, &dest, &group_acc);

        //access epoch
        MPI_Win_start(group_acc, 0, win);

        int res = MPI_Put(&buff, 1, MPI_INT, dest, 0, 1, MPI_INT, win);

        MPI_Win_complete(win);

        if(res != MPI_SUCCESS)
            fprintf(stderr, "[rank=%d] Error ocurred while sending from %d to %d\n",
                            rank, rank, dest);
        else
            fprintf(stdout, "[rank=%d] Sent number \"%d\" to rank %d\n",
                            rank, buff, dest);

    }

    MPI_Group_free(&comm_group);
    MPI_Group_free(&group_exp);
    MPI_Group_free(&group_acc);
    MPI_Win_free(&win);

    MPI_Finalize();
	return 0;
}
