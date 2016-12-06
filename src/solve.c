int rank, proc_num;

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

int main()
{
	MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &proc_num);

    int i_am_the_master = rank == 0;
    float *a, *v, *result;
    int row_cnt = m / proc_num;

    if(i_am_the_master)
	{
		// read from files
		FILE *f = fopen(matrix_fn, "rb");
		FILE *fv = fopen(vector_fn, "rb");
		a = (float *) malloc(m*n*sizeof(float));
		v = (float *) malloc(m*sizeof(float));
		result = (float *)malloc(m*sizeof(float));
		fread(a, sizeof(float), m*n, f);
		fread(v, sizeof(float), m, fv);
		fclose(f);
		fclose(fv);
	}
	else
	{	
		a = (float *)malloc(row_cnt*n*sizeof(float));
		v = (float *)malloc(m*sizeof(float));
		result = (float *)malloc(row_cnt*sizeof(float));
	}
	// broadcast vector
	MPI_Bcast(v, m, MPI_FLOAT, 0, MPI_COMM_WORLD);
	// scatter matrix, each process has m / p lines
	MPI_Scatter(a, row_cnt*n, MPI_FLOAT, a, row_cnt*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
	// do operation
	do_operation(a, v, result, row_cnt);
	// gather sum, each process passes m / p floats 
	MPI_Gather(result, row_cnt, MPI_FLOAT, result, row_cnt, MPI_FLOAT, 0, MPI_COMM_WORLD);
	// create new communicator, other processes exit
	int threshold = m % proc_num;
	MPI_Comm newcomm;
	MPI_Comm_split(MPI_COMM_WORLD, rank < threshold ? 1 : MPI_UNDEFINED, rank, &newcomm);
	// each process whose rank is higher than threshold exits
	if(rank >= threshold)
		MPI_Finalize();
	// broadcast one line (n floats) from matrix
	MPI_Scatter(a+(row_cnt*proc_num)*n, n, MPI_FLOAT, a, n, MPI_FLOAT, 0, newcomm);
	// do operation
	do_operation(a, v, result, 1);
	// gather back
	MPI_Gather(result, 1, MPI_FLOAT, result, 1, MPI_FLOAT, 0, newcomm);
	// write to file
	if(i_am_the_master)
	{
		FILE *f = fopen(output_file, "wb");
		fwrite(result, sizeof(float), m, f);
		fclose(f);
	}
	// exit
	MPI_Finalize();

}