#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include "helper.h"

unsigned int gol(unsigned char *grid, unsigned int dim_x, unsigned int dim_y, unsigned int time_steps)
{

	int np, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int upSender, upReceiver, dowSender, dowReceiver;
	int *disp = malloc(sizeof(int) * np);
	int *send = malloc(sizeof(int) * np);
	int *rdim_y = malloc(sizeof(int) * np);
	int T = 0;
	
	for(int i = 0; i < np; i++)
	{
		disp[i] = T;
		rdim_y[i] = dim_y / np + (i < dim_y % np);
		send[i] = rdim_y[i] * dim_x;
		T += send[i];
	}
	unsigned char *membuffer = calloc(sizeof(unsigned char), dim_x * (rdim_y[rank] + 2));

	MPI_Scatterv(grid, send, disp, MPI_UNSIGNED_CHAR, membuffer + dim_x, send[rank], MPI_CHAR, 0, MPI_COMM_WORLD);

	unsigned char *grid_in, *grid_out, *grid_tmp;
	size_t size = sizeof(unsigned char) * dim_x * (rdim_y[rank] + 2);

	grid_tmp = calloc(sizeof(unsigned char), dim_x * (rdim_y[rank] + 2));
	if (grid_tmp == NULL)
		exit(EXIT_FAILURE);

	grid_in = membuffer;
	grid_out = grid_tmp;
	
	dowReceiver = upSender = (rank + 1) % np;
	dowSender = upReceiver = (rank - 1 + np) % np;

	for (int t = 0; t < time_steps; ++t)
	{	
		MPI_Sendrecv(
				grid_in + dim_x,
				dim_x,
				MPI_UNSIGNED_CHAR,
				upReceiver,
				rank,
				grid_in + (rdim_y[rank] + 1) * dim_x,
				dim_x,
				MPI_UNSIGNED_CHAR,
				upSender,
				upSender,
				MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);

		MPI_Sendrecv(
				grid_in + rdim_y[rank] * dim_x,
				dim_x,
				MPI_UNSIGNED_CHAR,
				dowReceiver,
				rank,
				grid_in,
				dim_x,
				MPI_UNSIGNED_CHAR,
				dowSender,	
				dowSender, 
				MPI_COMM_WORLD, 
				MPI_STATUS_IGNORE);
		
		for (int y = 0; y < rdim_y[rank] + 2; ++y)
		{
			for (int x = 0; x < dim_x; ++x)
			{
				evolve(grid_in, grid_out, dim_x, rdim_y[rank] + 2, x, y);
			}
		}
		swap((void**)&grid_in, (void**)&grid_out);

	}

	if (membuffer != grid_in)
		memcpy(membuffer, grid_in, size);

	free(grid_tmp);

	MPI_Gatherv(
			membuffer + dim_x, 
			send[rank],
			MPI_UNSIGNED_CHAR,
                	grid,
			send,
			disp,
                	MPI_UNSIGNED_CHAR,
			0,
			MPI_COMM_WORLD);

	if(rank == 0)
	return cells_alive(membuffer, dim_x, rdim_y[rank]);
	else
	return 0;
}
