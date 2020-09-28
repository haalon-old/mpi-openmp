#include <stdio.h>
#include <stdlib.h>
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
	
		#pragma omp parallel for private(i,j,ij) num_threads(nthreads)	
		for ( ij = 0; ij < m*n; ++ij)
		{
			j=ij%n;
			i=ij/n;	
			*(*(matrix+i)+j) =  *(*(oldMatrix+j)+i);	
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

	double old,new;

	if (argc==2)

	{

		n=m=10;

		nthreads =atoi( argv[1] )	;

	}

	if (argc==3)

	{

		nthreads =atoi( argv[1] )	;

		n=m= atoi( argv[2] );

	}

	if (argc==4)

	{

		nthreads =atoi( argv[1] );	

		n=atoi( argv[2] );

		m=atoi( argv[3] );

	}

	printf("%d * %d \n", n,m );

	matrix = construct(n,m);

	for ( i = 0; i < n; ++i)

	{

		for ( j = 0; j < m; ++j)

		{

			*(*(matrix+i)+j) =  rand()%100;

		}

	}

	//printm(matrix,n,m);
	old =omp_get_wtime();

	for (i = 0; i <100 ; ++i)

	{

		matrix = transpose(matrix,n,m);

		j=n;

		n=m;

		m=j;

	}

	new = omp_get_wtime();	

	//printm(matrix,m,n);

	printf("done with %d threads in %f seconds\n", nthreads, (new-old) );

	freeM(matrix,n);	

}

