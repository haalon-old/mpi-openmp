#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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


int main (int argc, char* argv[])
{
  	int myRank,numprocs;
	int errCode,i,j,i2;
  	int n,m;
  	
    int **matrix;
    int **new_matrix;
    int **matrix_piece;
    
    MPI_Request * req;
 
    int startcol, lastcol, ncol;
  	double start, end;

    if ((errCode = MPI_Init(&argc, &argv)) != 0)
    {
        return errCode;
    }

  	if (argc==1)
	{
		n=m=10;	
	}
	if (argc==2)
	{
		n=m= atoi( argv[1] );
	}
	if (argc==3)
	{
		n=atoi( argv[1] );
		m=atoi( argv[2] );
	}

  

  	matrix = construct(n,m);
  	for ( i = 0; i < n; ++i)
  	{
  		for ( j = 0; j < m; ++j)
  		{
  			matrix[i][j]=i-j;
  		}
  	}

  	

   
    start = MPI_Wtime();
    

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
    if(myRank==0)
        new_matrix = construct(m,n);

  	
    if(myRank==0)
    {
        printf("there is so much processes: %d\n",numprocs);
    }
  	startcol = (myRank *m) / numprocs;
    lastcol = (((myRank + 1) * m) / numprocs)-1;
    ncol = lastcol - startcol + 1;

    req = malloc(ncol* sizeof(MPI_Request));

    matrix_piece = construct(ncol,n);
 	
  	
 
  	//printf("Hello, my rank is: %d! \n",myRank);
  	
  	for (i = 0; i < n; ++i)
    {
        for (j = 0; j < ncol; ++j)
        {
            matrix_piece[j][i] = matrix[i][startcol+j];
        }
    }

    for (i = 0; i < ncol; ++i)
    {
        MPI_Isend(matrix_piece[i], n, MPI_INT, 0, startcol+i, MPI_COMM_WORLD, req+i );
    }

    if(myRank==0)
    {

        for (i = 0; i < m; ++i)
        {
            MPI_Recv(new_matrix[i], n, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD,MPI_STATUS_IGNORE );
        }
        end = MPI_Wtime();
        printf("Done in %f seconds\n", end-start );
        /*
        for ( i = 0; i < n; ++i)
        {
            for ( j = 0; j < m; ++j)
            {
                printf("%03d ", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        for ( i = 0; i < m; ++i)
        {
            for ( j = 0; j < n; ++j)
            {
                printf("%03d ", new_matrix[i][j]);
            }
            printf("\n");
        }
        */
        freeM(new_matrix, m);
    }
    else
    {
        MPI_Waitall(ncol, req, MPI_STATUS_IGNORE);
    }
  
    freeM(matrix,n);
    freeM(matrix_piece, ncol);
    
    
    MPI_Finalize();
 	
  	
  	return 0;
}
