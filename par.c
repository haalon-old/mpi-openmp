#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
//-fopenmp

int nthreads;

int** construct( int n, int m)
{
	int** matrix = (int**) malloc(n*sizeof(int*));
	int i;
	for ( i = 0; i < n; ++i)
	{
		*(matrix+i) = (int*) malloc(m*sizeof(int));
	}
	return matrix;
}

void freeM(int** matrix, int n)
{
	int i;
	for ( i = 0; i < n; ++i)
	{
		free(matrix[i]);
	}
	free(matrix);
}

int** transpose(int** oldMatrix, int n, int m)
{
	int** matrix = construct(m,n);
	int i,j,ij;

	#pragma omp parallel private(i,j,ij) num_threads(nthreads)
	{
		printf("%d\n", omp_get_num_threads());
		#pragma omp for 	
		for ( ij = 0; ij < m*n; ++ij)
		{
			j=ij%n;
			i=ij/n;			
			*(*(matrix+i)+j) =  *(*(oldMatrix+j)+i);				
		}
	}
	
	
	
	freeM(oldMatrix,n);
	return matrix;
}

void printm(int** matrix, int n, int m)
{
	int i,j;
	for ( i = 0; i < m; ++i)
	{
		for ( j = 0; j < n; ++j)
		{			
			printf("%02d ", *(*(matrix+i)+j) );
		}
		printf("\n" );
	}
}

int main(int argc, char const *argv[])
{
	int n,m,i,j;
	int** matrix;
	time_t old,new;
	//clock_t uptime, downtime;

	srand((unsigned) time(NULL));

	if (argc==1)
	{
		n=m=10;	
		nthreads=1;	
	}
	if (argc==2)
	{
		n=m= atoi( argv[1] );
		nthreads=1;
	}
	if (argc==3)
	{
		n=m=atoi( argv[1] );
		nthreads=atoi( argv[2] );
		//omp_set_num_threads(4);
	}
	printf("%d * %d \n", n,m );
	matrix = construct(n,m);
	for ( i = 0; i < n; ++i)
	{
		for ( j = 0; j < m; ++j)
		{
			*(*(matrix+i)+j) =  rand()%100;
			//printf("%02d ", *(*(matrix+i)+j) );
		}
		//printf("\n" );
	}
	//printm(matrix,n,m);
	time(&old);
	for (i = 0; i < 10000; ++i)
	{
		matrix = transpose(matrix,n,m);
		j=n;
		n=m;
		m=j;
	}
	time(&new);

	printf("%f",difftime(new,old));

	free(matrix);
	
}