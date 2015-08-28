
/******************************************************************
*	IN4026 Lab A: Prefix and Suffix Minima
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: par_omp.c
*	Description: The parallel version of the sequential algorithm
*	using the OpenMP library. The algorithm has been parallized
*	around the main loop for computing the prefix and suffix values
*	as well as around the balanced tree minima function. 
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include "fileIO.h"

/*Parameters are given in Makefile*/
int RUNS; /*Number of Runs of the algorithm*/
int MAX_THREADS; /*Number of threads available*/
int CHUNKSIZE = 256;
int m, k; /*Number of threads given to each parallel section*/

int minArray(int *A, int n);
void psMin(int *A, int *P, int *S, int n);
void minima(int *A, int n, int *B);
int minElement(int a, int b);

/****************************************************************
*
*	Function: main
*	Input:	int argc 	number of command line arguements
*		char **arg	pointer to those arguements
*
*	Output: int 0 for success and 1 for error
*
*	Description: Runs the prefix and suffix algorithm multiple
*	times averaging the results and printing them to terminal. 
*
*****************************************************************/
int main(int argc, char **argv)
{
	struct timeval startt, endt, result;

	char name[16] = "omp";

	int status;
	int n;
	int* A;
	int* P;
	int* S;
	
	//Check if app was given enough input
	if(argc < 7){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq file_path array_size P_ans_path S_ans_Path RUNS THREADS\n");
		return 1;
	}

	//Save args to memory and allocate memory for arrays
	n = atoi(argv[2]);
	RUNS = atoi(argv[5]);
	MAX_THREADS = atoi(argv[6]);
	A = malloc(n*sizeof(int));
	P = malloc(n*sizeof(int));
	S = malloc(n*sizeof(int));

	if(A==NULL){
		printf("Failed to Allocate Memory for Input Array");	
	}

	//Read the input array from file and save to memory
	status = read_input(A, n, argv[1]);

	if(status){	
		printf("Failed to Read Input \n");
		return 1;
	}

	//Start of testing of the algorithm
	int j;
	double average;
	for(j=0; j<RUNS; j++){

		/*Start Timer*/
		result.tv_sec=0;
		result.tv_usec=0;
		gettimeofday (&startt, NULL);

		psMin(A, P, S, n);

		/*Stop Timer*/
		gettimeofday (&endt, NULL);
		result.tv_usec = (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
		average += result.tv_usec;

	}
	average = average/RUNS;	//Average the execution times

	//print results to terminal
	printf("%d 	%f	us \n",n,average);

	//Check to see if answer is correct if told to do so in the input args
	if((atoi(argv[3])!=1) && (atoi(argv[4])!=1))
	{
		status = outputCheck(P, S, argv[3], argv[4], n);
		if(status){
			printf("Incorrect Answer\n");
		}
		else{
			printf("Correct Answer\n");
		}
	}

	if(n<=50){
		status = write_output(P, S, n, name);
		if(status){	
			printf("Failed to Write Output \n");
			return 1;
		}
	}

	//free the memory
	free(P);
	free(S);
	free(A);

    	return 0;
}
/****************************************************************
*
*	Function: psMin
*	Input:	int *A	Pointer to input array
*		int *P	Pointer to Prefix array
*		int *S	Pointer to Suffix array
*		int n	Size of the arrays
*
*	Output: void
*
*	Description: Runs the prefix and suffix algorthim. Also
*	sets the number of threads available to OpenMP. The value
*	is given through the command line when the application is
*	executed. Main Loop is parallelized and is up to the OpenMP
*	compiler to schedule the work to the available threads.
*	Each Loop iteration a copy of A is made to B which is local
*	to each thread and so no mutexes are needed when writing.
*
*****************************************************************/
void psMin(int *A, int *P, int *S, int n){
	int i;
	int tid, length, start, end, pmin, smin, remove;

	if(MAX_THREADS == 1){
		m = 1;
		k = 1;
	}
	if(MAX_THREADS == 2){
		m = 2;
		k = 1;
	}
	if(MAX_THREADS == 4){
		m = 2;
		k = 2;
	}
	if(MAX_THREADS == 8){
		m = 4;
		k = 2;
	}
	if(MAX_THREADS == 16){
		m = 4;
		k = 4;
	}

	#pragma omp parallel shared(A, P, S, n) private(i,tid, length, start, end, pmin, smin, remove) num_threads(m)
	{
		tid = omp_get_thread_num();
		length = n/m;
		
		/*Start and end of Thread's work*/
		start = tid*length;
		end = (tid+1)*length;
		
		/*Find the Min of the respective parts of the array*/
		pmin = minArray(&A[0], start+1);
		smin = minArray(&A[start], n-start);

		/*Flag to indicate the min will be removed next iteration*/
		remove = 0;

		for(i=start; i<end; i++){	
			P[i] = minElement(pmin, A[i]); /*find prefix min*/
			pmin = P[i];
			if(A[i] > smin){
				S[i] = smin;
			}
			if(remove){
				S[i] = minArray(&A[i], n-i);
				smin = S[i];
				remove = 0;
			}
			if(A[i] == smin ){
				S[i] = smin;
				remove = 1;
			}

		}
	}

	
}
/****************************************************************
*
*	Function: psMin
*	Input:	int a	element a
*		int b	element b
*
*	Output: int	the min of a and b
*
*	Description: Finds which element is the minimum of the two
*
*****************************************************************/
int minElement(int a, int b){
	if(a>=b){
		return b;
	}
	else{
		return a;
	}
}
/****************************************************************
*
*	Function: minArray
*	Input:	int *A	Pointer to input array
*		int n	Size of the array
*
*	Output: int
*
*	Description: Finds the minimum of a given Array A of size
*	n. This function uses the balanced tree method to determine
*	the minimum. It returns the minimum when the algorithm 
*	terminates.
*
*****************************************************************/
int minArray(int *A, int n){
	int j, min, m;
	int i, output;
	min = INT_MAX;
	m = ceil(log2(n));
	int *B = malloc((n>>1)*sizeof(int));

	if(n == 1){
		return A[0];
	}

	/*First Case to save A to B*/
	if(n%2){
		min = A[n-1];
	}

	n = n>>1;		
	minima(A, n, B);

	/*Cycle through the rest of Log2 levels of the B array*/
	for(j=2;j<=m;j++){

		/*Check to see if odd and save odd element*/
		if((n%2) && (B[n-1] < min)){
			min = B[n-1];
		}

		/*calculate the min using balanced tree with even n*/
		n = n>>1;		
		minima(B, n, B);

		/*check to see if the result of the tree is greater than min*/
		if(B[0]>min) B[0] = min;		

	}
	
	//just to make sure
	if( min > B[0]) min = B[0];

	free(B);
	return min;
}

/****************************************************************
*
*	Function: minima
*	Input:	int *A	Pointer to input array
*		int n	Size of the array
*		int *B 	Pointer to temporary array B
*
*	Output: void
*
*	Description: Finds the minimum of a given Array A of size
*	n using the balanced tree method. Saves the result in the
*	array position A[0]. Returns no value. 
*
*****************************************************************/
void minima(int *A, int n, int *B){
	int p,l;
	int chunk = CHUNKSIZE;
	if(n<=256){
		chunk=8;
	}

	#pragma omp parallel for schedule(dynamic, chunk) shared(A) private(l,p) num_threads(k)
	for(l=0; l<n; l++){
		p = 2*l;
		if(A[p]>A[p+1]) B[l] = A[p+1];
		else B[l] = A[p];
	}

}
