#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#include <math.h>
#include "fileIO.h"

//#define DEBUG

int minArray(int *A, int n);
void psMin(int *A, int *P, int *S, int n);
void minima(int *A, int n);

int main(int argc, char **argv)
{
	double start, end;
	double cpu_time_used;
	
	int status;
	int n;
	int* A;
	int* P;
	int* S;

	if(argc < 5){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq file_path array_size P_ans_path S_ans_Path\n");
		return 1;
	}

	n = atoi(argv[2]);
	A = malloc(n*sizeof(int));
	P = malloc(n*sizeof(int));
	S = malloc(n*sizeof(int));

	if(A==NULL){
		printf("Failed to Allocate Memory for Input Array");	
	}

	status = read_input(A, n, argv[1]);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Read Input \n");
		#endif
		return 1;
	}
	//Start of the Algorithm
	int j;
	double average;
	for(j=0; j<50000; j++){
		start = omp_get_wtime();
		psMin(A, P, S, n);
		end = omp_get_wtime();
		cpu_time_used = end-start;
		if(j==0) average = cpu_time_used;
		else	 average = (average+cpu_time_used)/2;
	}	
	//End of Algorithm

	printf("%d 	%f	s \n",n,average);

	if((atoi(argv[3])!=1) && (atoi(argv[4])!=1))
	{
		status = outputCheck(P, S, argv[3], argv[4], n);
		if(status){
			printf("Incorrect Answer\n");
		}
		else{
			printf("Correct Answer\n");
		}
		status = write_output(P, S, n);
		if(status){
			#ifdef DEBUG	
			printf("Failed to Write Output \n");
			#endif
			return 1;
		}
	}

	free(P);
	free(S);
	free(A);

    	return 0;
}

void psMin(int *A, int *P, int *S, int n){
	int i;

	#pragma omp parallel for
	for(i=0; i<n; i++){
		int *B = malloc(n*sizeof(int));		
		memcpy(B, A, n*sizeof(int));
		P[i] = minArray(B, i+1);
		B[i] = A[i];
		S[i] = minArray(&B[i], n-i);
		free(B);
	}
	
}

int minArray(int *A, int n){
	int j, min, m;
	int i;
	min = INT_MAX;
	m = ceil(log2(n));
	
	if(n==1){
		return A[0];
	}
	for(j=1;j<=m;j++){
		if(n%2){
			min = A[n-1];
		}
		n = n>>1;		
		minima(A,n);
		if(A[0]>min) A[0] = min;		

	}

	if( min > A[0]) min = A[0];

	return min;
}
void minima(int *A, int n){
	int p,l;
	#pragma omp parallel for
	for(l=0; l<n; l++){
		p = 2*l;
		if(A[p]>A[p+1]) A[l] = A[p+1];
		else A[l] = A[p];
	}

}
