int rank, proc_num;

int main()
{
	MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &proc_num);

    int i_am_the_master = rank == 0;
    float *a, *v;
    if(i_am_the_master)
	{ //master
		FILE *f = fopen(matrix_fn, "rb");
		FILE *fv = fopen(vector_fn, "rb");
		float *a = (float *) malloc(m*n*sizeof(float));
		float *v = (float *) malloc(m*sizeof(float));
		fread(a, sizeof(float), m*n, f);
		fread(v, sizeof(float), m, fv);
		fclose(f);
		fclose(fv);
		// broadcast vector
		MPI_Bcast(v, m, MPI_FLOAT, 0, MPI_COMM_WORLD);
		// scatter matrix, each process has m / p lines
		int row_cnt = m / proc_num;
		MPI_Scatter(a, row_cnt*n, MPI_FLOAT, a, row_cnt*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
		float *result = (float *)malloc(m*sizeof(float));
		// do operation
		// gather sum, each process passes m / p floats 
		MPI_Gather(result, row_cnt, MPI_FLOAT, result, row_cnt, )
		MPI_Gather(
		    void* send_data,
		    int send_count,
		    MPI_Datatype send_datatype,
		    void* recv_data,
		    int recv_count,
		    MPI_Datatype recv_datatype,
		    int root,
		    MPI_Comm communicator)

	}

	{ //slave
		int row_cnt = m / proc_num;
		// receive vector
		float *v = (float *)malloc(m*sizeof(float));
		MPI_Bcast(v, m, MPI_FLOAT, 0, MPI_COMM_WORLD);
		// receive matrix rows
		float *a = (float *)malloc(row_cnt*n*sizeof(float));
		MPI_Scatter(a, row_cnt*n, MPI_FLOAT, a, row_cnt*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
		// do operation
		// send back
		// divide processes from 0 to p-1
		int p = m % proc_num;
		MPI_Comm newcomm;
		MPI_Comm_split(MPI_COMM_WORLD, rank < p ? 1 : MPI_UNDEFINED, rank, &newcomm);
		// each process whose rank is higher than p exits
		if(rank >= p)
			MPI_Finalize();
		else
		{
			// broadcast one line from matrix
			MPI_Bcast(a, n, MPI_FLOAT, 0, newcomm);
			// do operation
			MPI_Bcast(result, 1, MPI_FLOAT, 0, newcomm);
			MPI_Finalize();
		}
	}
}