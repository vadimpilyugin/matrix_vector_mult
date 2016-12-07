#include "params.h"
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

int rank, proc_num;
int i_am_the_master;
int row_cnt;

float *a, *v, *result;
float *small_a, *small_result;

void do_operation(float *a, float *v, float *result, int row_cnt)
{
	int i, j;
	float sum = 0;
	for(i = 0; i < row_cnt; i++)
	{
		for(j = 0; j < n; j++)
			sum += a[i*n+j]*v[j];
		result[i] = sum;
		sum = 0;
	}
}

void mymalloc()
{
	if(i_am_the_master)
	{
		a = (float *) malloc(m*n*sizeof(float));
		v = (float *) malloc(n*sizeof(float));
		result = (float *)malloc(m*sizeof(float));
	}
	else
	{
		a = NULL;
		v = (float *) malloc(n*sizeof(float));
		result = NULL;
	}
	if(row_cnt > 0)
	{
		small_a = (float *)malloc(row_cnt*n*sizeof(float));
		small_result = (float *)malloc(row_cnt*sizeof(float));
	}
	else
	{
		small_a = (float *)malloc(1*n*sizeof(float));
		small_result = (float *)malloc(1*sizeof(float));		
	}
}

void myfree()
{
	if(i_am_the_master)
	{
		free(a);
		free(result);
	}
	free(v);
	free(small_a);
	free(small_result);
}

int main(int argc, char **argv)
{

	MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &proc_num);

	i_am_the_master = rank == 0;
	row_cnt = m / proc_num;

    mymalloc();

    if(i_am_the_master)
	{
		// read from files
		FILE *f = fopen(matrix_fn, "rb");
		FILE *fv = fopen(vector_fn, "rb");
		if(f == NULL || fv == NULL)
		{
			fprintf(stderr, "%s\n", "Could not open file");
			exit(1);
		}
		if(a == NULL)
			fprintf(stderr, "%s\n", "This is very bad!"); 
		fread(a, sizeof(float), m*n, f);
		fread(v, sizeof(float), n, fv);
		fclose(f);
		fclose(fv);
	}

	// broadcast vector
	MPI_Bcast(v, n, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if(row_cnt > 0)
	{
		// scatter matrix, each process has m / p lines
		MPI_Scatter(a, row_cnt*n, MPI_FLOAT, small_a, row_cnt*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
		// do operation
		do_operation(small_a, v, small_result, row_cnt);
		// gather sum, each process passes m / p floats 
		MPI_Gather(small_result, row_cnt, MPI_FLOAT, result, row_cnt, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}
	// create new communicator, other processes exit
	int threshold = m % proc_num;
	MPI_Comm newcomm;
	MPI_Comm_split(MPI_COMM_WORLD, rank < threshold ? 1 : MPI_UNDEFINED, rank, &newcomm);
	// each process whose rank is higher than threshold exits
	if(rank >= threshold)
	{
		if(i_am_the_master)
		{
			FILE *f = fopen(output_file, "wb");
			fwrite(result, sizeof(float), m, f);
			fclose(f);
		}
    printf("Hello, world! rank = %d\n", rank);
		if(i_am_the_master && threshold != 0)
			fprintf(stderr, "Something bad happened! Master has exited where it shouldn't\n");
		myfree();
		MPI_Finalize();
		exit(0);
	}
	// broadcast one line (n floats) from matrix
	MPI_Scatter(a+(row_cnt*proc_num)*n, n, MPI_FLOAT, small_a, n, MPI_FLOAT, 0, newcomm);
	// do operation
	do_operation(small_a, v, small_result, 1);
	// gather back
	MPI_Gather(small_result, 1, MPI_FLOAT, result+row_cnt*proc_num, 1, MPI_FLOAT, 0, newcomm);
	// write to file
	if(i_am_the_master)
	{
		FILE *f = fopen(output_file, "wb");
		fwrite(result, sizeof(float), m, f);
		fclose(f);
		printf("Master has written answer to file %s\n", output_file);
	}
	// exit
	myfree();
	MPI_Finalize();
	exit(0);
}