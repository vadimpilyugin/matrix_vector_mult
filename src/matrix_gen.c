#include "params.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

float frandom()
{
	return (float)rand() / RAND_MAX * 10;
}

int main()
{
	float *a = (float *) malloc(m*n*sizeof(float));
	float *v = (float *) malloc(n*sizeof(float));
	srand(time(NULL));
	int i, j;
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			a[i*n+j] = (int)frandom();
	for(j = 0; j < n; j++)
		v[j] = (int)frandom();
	FILE *f, *fv;
	f = fopen(matrix_fn, "wb");
	fv = fopen(vector_fn, "wb");
	fwrite(a, sizeof(float), m*n, f);
	fwrite(v, sizeof(float), n, fv);
	fclose(f);
	fclose(fv);
	free(a);
	free(v);
	return(0);
}

