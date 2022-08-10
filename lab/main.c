#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>

//u_t + a * u_x = f(x, t)
//u(0, t) = phi(t) = exp(-t)
//u(x, 0) = psi(x) = cos(pi * x)
//0 <= x <= 1
//0 <= t <= 1

double precise(double x, double t)
{
    if (x >= 2 * t)
        return x * t - t * t / 2 + cos(PI * (2 * t - x));
    else
        return x * t - t * t / 2 + (2 * t - x) * (2 * t - x) / 8 + exp(x / 2 - t);
}

// debug define
#define $ //fprintf(stdout, r" DEBUG [%d]\n", rank, __LINE__);

const double a = 2;

const double x0 = 0;
const double x1 = 1;

const double t0 = 0;
const double t1 = 1;

double dx = 0.01;
double dt = 0.01;


void print_column(int rank, double** u, int column, int to)
{
    for (int i = 0; i < to; i++)
        printf(r"u[%d][%d] = %lf\n", rank, column, i, u[column][i]); 
}

void print_row(int rank, double** u, int row, int to)
{
    for (int i = 0; i < to; i++)
        printf(r"u[%d][%d] = %lf\n", rank, i, row, u[i][row]); 
}

double f(double x, double t)
{
    return x + t;
}

double psi(double t)
{
    return exp(-t);
}

double phi(double x)
{
    return cos(M_PI * x);
}


double left_corner(double u_0_0, double u_1_0, double f_val)
{
    return u_1_0 - dt * ((u_1_0 - u_0_0) / dx - f_val);
}

int main(int argc, char * argv[])
{
   int commsize, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int count_x = (int)round((x1 - x0) / dx);
    int count_t = (int)round((t1 - t0) / dt);

    //fprintf(stdout, r" count_t = %lf[%d]\n", rank, 1.f / dt, __LINE__);

    int range = (int)round((double)count_x / (double)commsize);

    int start  = rank * range;
    int finish = rank != commsize - 1 ? start + range - 1 : count_x - 1;

    range = finish - start + 1;

    //Allocating memory for function's values

    $
    double ** u = (double **)calloc(range, sizeof(double *));
    if(u == NULL)
    {
        fprintf(stderr, r" Cannot allocate memory (double**) [range=%d]\n", 
                        rank, range);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < range; i++)
    {
        u[i] = (double *)calloc(count_t, sizeof(double));
        if(u[i] == NULL)
        {
            fprintf(stderr, r" Cannot allocate memory (double*) [count_t=%d]\n",
                            rank, (int)count_t);
            exit(EXIT_FAILURE);
        }
    }
    $

    //Filling boundary conditions

    for (int i = 0; i < range; i++)
    {
        u[i][0] = phi((i + start) * dx + x0);
    }

    //print_row(rank, u, 0, range);

    $
    if (rank == 0)
    {
        for (int i = 0; i < count_t; i++)
        {
            u[0][i] = psi(i * dt + t0);
        }
        //print_column(rank, u, 0, count_t);
    }

    //Filling first layer

    for (int i = 1; i < range; i++)
    { 
        u[i][1] = left_corner(u[i - 1][0], u[i][0], f((i + start) * dx + x0, dt));
    }

    $
    if (rank != commsize - 1)
    { 
        if (MPI_Send(&u[range - 1][1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD) != MPI_SUCCESS)
        {
            fprintf(stderr, r" Failed to send to node %d\n", rank, rank + 1);
            exit(EXIT_FAILURE);
        }
    }

    $

    MPI_Status status = {};

    if (rank != 0)
    {
        double u_out = 0;

        if (MPI_Recv(&u_out, 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &status) != MPI_SUCCESS)
        {
            fprintf(stderr, r" Failed to recieve from node %d\n", rank, rank - 1);
            exit(EXIT_FAILURE);
        }

        u[0][1] = left_corner(u_out, u[0][0], f(start * dx + x0, 0));
    }

    $

    //Filling other layers

    for (int i = 1; i < count_t - 1; i++)
    {
        //Filling central knots

        for (int j = 1; j < range - 1; j++)
        {
            u[j][i + 1] = u[j][i - 1] - dt * ((u[j + 1][i] - u[j - 1][i]) / dx - 2 * f(j * dx, i * dt));
        }


        //Filling starting and finishing knots

        //finishing
        u[range-1][i+1] = left_corner(u[range-2][i], u[range-1][i], f(finish * dx, i * dt));

        if(rank != commsize - 1)
        {
            if (MPI_Send(&u[range - 1][i], 1, MPI_DOUBLE, rank + 1, i, MPI_COMM_WORLD) != MPI_SUCCESS)
            {
                fprintf(stderr, r" Failed to send to node %d\n", rank, rank + 1);
                exit(EXIT_FAILURE);
            }
        }

        //starting
        if(rank != 0)
        {
            double u_out = 0;
            if (MPI_Recv(&u_out, 1, MPI_DOUBLE, rank - 1, i, MPI_COMM_WORLD, &status) != MPI_SUCCESS)
            {
                fprintf(stderr, r" Failed to recieve from node %d\n", rank, rank - 1);
                exit(EXIT_FAILURE);
            }
            u[0][i+1] = left_corner(u_out, u[0][i], f(start * dx + x0, i * dt));
        }

        $
        
    }

    //print_row(rank, u, count_t - 1, range);
    fprintf(stdout, r" count_t = %d; range = %d; pid = %d\n",
                    rank, count_t, range, getpid());

    $
    
    double** res;
    if(rank == 0)
    {
        res = (double**)calloc(count_x, sizeof(double*));

        if(res == NULL)
        {
            fprintf(stderr, r" Cannot allocate memory (double**) [range=%d]\n", 
                            rank, count_x);
            exit(EXIT_FAILURE);
        }

        MPI_Gather(u, range, MPI_LONG, res, range, MPI_LONG, 0, MPI_COMM_WORLD);
    }
    else
        MPI_Gather(u, range, MPI_LONG, NULL, range, MPI_LONG, 0, MPI_COMM_WORLD);

    $
    
    if(rank == 0) for(int i = 0; i < count_x; i ++) printf(r"res[%d] = %x\n", rank, i, res[i]);
    if(rank == 0) print_row(rank, res, count_t - 1, count_x);

    MPI_Finalize();

    if(rank == 0)
        free(res);

    for(int i = 0; i < range; i ++)
        free(u[i]);

    free(u);

    return 0;
}
