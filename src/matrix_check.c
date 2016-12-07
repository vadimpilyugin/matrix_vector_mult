#include "params.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

const float eps = 1e-5;

void do_operation(float *a, float *v, float *result)
{
	int i, j;
	float sum = 0;
	for(i = 0; i < m; i++)
	{
		for(j = 0; j < n; j++)
			sum += a[i*n+j]*v[j];
		result[i] = sum;
		sum = 0;
	}
}
void do_check(float *v_computed, float *v_real)
{
	int i;
	float diff = 0, max_diff = 0, a, b;
	printf("%s\n", "Computed\tReal\n");
	for(i = 0; i < m; i++)
	{
		a = v_computed[i];
		b = v_real[i];
		diff = a > b ? a-b : b-a;
		if(diff > max_diff)
			max_diff = diff;
		printf("%5.2f\t%5.2f\n", a, b);
	}
	if(max_diff > eps)
		fprintf(stderr, "\nМаксимальное отклонение: %f\nВсе хорошо? Нет\n", max_diff);
	else
		fprintf(stderr, "\nМаксимальное отклонение: %f\nВсе хорошо? Да\n", max_diff);		
}

void output_matrix(float *a, int rows, int cols, const char *descr)
{
	int i, j;
	printf("%s: \n", descr);
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
			printf("%5.2f ", a[i*cols+j]);
		printf("\n");
	}
}

int main()
{
	// read from files
	FILE *f = fopen(matrix_fn, "rb");
	FILE *fv = fopen(vector_fn, "rb");
	FILE *freal = fopen(output_file, "rb");

	// allocate memory for 3 inputs and 1 computed result
	float *a = (float *) malloc(m*n*sizeof(float));
	float *v = (float *) malloc(n*sizeof(float));
	float *real = (float *) malloc(m*sizeof(float));
	float *result = (float *)malloc(m*sizeof(float));

	// read matrix and 2 vectors
	fread(a, sizeof(float), m*n, f);
	fread(v, sizeof(float), n, fv);
	fread(real, sizeof(float), m, freal);

	fclose(f);
	fclose(fv);
	fclose(freal);


	do_operation(a, v, result);

	// output_matrix(result, m, 1, "Computed vector");
	// output_matrix(real, m, 1, "Real vector");
	printf("Hello, World!\n");
	//output_matrix(a, m, n, "Original matrix");
	//output_matrix(v, n, 1, "Original vector");

	do_check(result, real);



	FILE *fresult = fopen("data/checked_result.bin", "wb");
	fwrite(result, sizeof(float), m, fresult);
	fclose(fresult);

	free(a);
	free(v);
	free(result);
	free(real);
	exit(0);
}