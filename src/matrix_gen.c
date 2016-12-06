#include "params.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	float *a = (float *) malloc(m*n*sizeof(float));
	float *v = (float *) malloc(m*sizeof(float));
	srand(time(NULL));
	int i, j;
	for(i = 0; i < m; i++)
	{
		for(j = 0; j < n; j++)
			a[i*m+j] = rand()*100;
		v[i] = rand()*100;
	}
	FILE *f, *fv;
	f = fopen(matrix_fn, "wb");
	fv = fopen(vector_fn, "wb");
	fwrite(a, sizeof(float), m*n, f);
	fwrite(v, sizeof(float), m, fv);
	fclose(f);
	fclose(fv);
}