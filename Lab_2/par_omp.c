/******************************************************************
*	IN4026 Lab B: Simple Merge
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: seq.c
*	Description: The sequential version of the prefix and suffix
*	minima algorithm. It takes as input an array A and computes the
*	min of the prefix array and suffix array for a given index from
*	1 to the array size. The results are saved to P and S respectively. 
*	THe mimimum of the sub arrays are calculated using a balanced tree
*	method. 
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "fileIO.h"
#include <omp.h>


void simpleMerge(int *A, int *B, int *C, int n, int m);
int rank(int a, int *B, int start, int end);
int write_Array(int* A, int n);


int RUNS;
int MAX_THREADS;
int CHUNKSIZE = 256;

/****************************************************************
*
*	Function: main
*	Input:	int argc 	number of command line arguements
*		char **arg	pointer to those arguements
*
*	Output: int 0 for success and 1 for error
*
*	Description: Runs the simple merge algorithm multiple
*	times averaging the results and printing them to terminal. 
*
*****************************************************************/
int main(int argc, char **argv)
{
	struct timeval startt, endt, result;
	
	char name[8] = "omp/";
	
	int status;
	int n, m;
	int* A;
	int* B;
	int* C;
	
	//Check if app was given enough input
	if(argc < 8){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq path_input_A path_input_B A_size B_size  C_ans_Path RUNS Max_Threads\n");
		return 1;
	}
	
	//Save args to memory and allocate memory for arrays
	n = atoi(argv[3]);
	m = atoi(argv[4]);
	if((n<256) && (m<256)){
		CHUNKSIZE =4; /*For small N*/
	}
	RUNS = atoi(argv[6]);
	MAX_THREADS = atoi(argv[7]);
	A = malloc(n*sizeof(int));
	B = malloc(m*sizeof(int));
	C = malloc((n+m)*sizeof(int));

	if(A==NULL){
		printf("Failed to Allocate Memory for Input Array A");	
	}
	if(B==NULL){
		printf("Failed to Allocate Memory for Input Array B");	
	}
	if(C==NULL){
		printf("Failed to Allocate Memory for Input Array C");	
	}

	//Read the input array from file and save to memory
	status = read_input(A, n, argv[1]);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Read Input A \n");
		#endif
		return 1;
	}

	//Read the input array from file and save to memory
	status = read_input(B, m, argv[2]);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Read Input B \n");
		#endif
		return 1;
	}
	
	//Start of testing of the algorithm
	int j;
	double average;
	for(j=0; j<RUNS; j++){
		memset(C, 0, (n+m)*sizeof(int));

		/*Start Timer*/
		result.tv_sec=0;
		result.tv_usec=0;
		gettimeofday (&startt, NULL);

		simpleMerge(A, B, C, n, m);

		/*Stop Timer*/
		gettimeofday (&endt, NULL);
		result.tv_usec = (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
		average += result.tv_usec;
	}
	average = average/RUNS; //Average the execution times

	//print results to terminal
	printf("%d 	%f	us \n",n,average);

	if(atoi(argv[5])!=1)
	{
		status = outputCheck(C, argv[5], n+m);
		if(status){
			printf("Incorrect Answer\n");
		}
		else{
			printf("Correct Answer\n");
		}
	}

	/*Print Output only if less than 50 elements*/	
	if( (n<50) && (m<=50) ){
		status = write_output(A, B, C, n, m, name);
	}

	if(status){	
		printf("Failed to Write Output \n");
		return 1;
	}

	/*Used to generate Inputs for the algorithm*/	
//	generateInputs();

	free(A);
	free(B);
	free(C);

	
	
    	return 0;
}
/****************************************************************
*
*	Function: simpleMerge
*	Input:	int *A	Pointer to Input A
*		int *B	Pointer to Input B
		int *C	Pointer to the Merged Array C
*		int n	Size of Array A
*		int m	Size of Array B
*
*	Output: void
*
*	Description: Merges two sorted Arrays and A and B into a
*	a single Array C of size n+m. The Merge is done by computing
*	the rank of each element in the opposing Array. For example
*	Rank(A[i]:B) and vice versa. 
*
*****************************************************************/
void simpleMerge(int *A, int *B, int *C, int n, int m){
	int *AA = malloc(n*sizeof(int));
	int *BB = malloc(m*sizeof(int));
	int i;
	int chunk = CHUNKSIZE;

	omp_set_dynamic(0); //Makes sures the number of threads available is fixed    
	omp_set_num_threads(MAX_THREADS); //Set thread number
	#pragma omp parrallel shared(AA, BB, chunk) private(i)
	{
		#pragma omp for schedule(dynamic, chunk) nowait
		for(i=0; i<n; i++){
			AA[i] = rank(A[i]-1, B, 0, m-1);
			if(C[i+AA[i]] > 0){
				if(C[i+AA[i]]< A[i]){
					C[i+AA[i]+1] = A[i];
				}
				else{
					int temp = C[i+AA[i]];
					C[i+AA[i]] = A[i];
					C[i+AA[i]+1] = temp;
			
				}		
			}
			else{
				C[i+AA[i]] = A[i];		
			}
		}
		#pragma omp for schedule(dynamic, chunk) nowait
		for(i=0; i<m; i++){
			BB[i] = rank(B[i], A, 0, n-1);
			if(C[i+BB[i]] > 0){
				if(C[i+BB[i]]< B[i]){
					C[i+BB[i]+1] = B[i];
				}
				else{
					int temp = C[i+BB[i]];
					C[i+BB[i]] = B[i];
					C[i+BB[i]+1] = temp;
			
				}		
			}
			else{
				C[i+BB[i]] = B[i];		
			}
		}
	}	
}
/****************************************************************
*
*	Function: rank
*	Input:	int a	Pointer to Input A
*		int *B	Pointer to Input B
*
*	Output: int position of a in array B
*
*	Description: Determines the position of a in array B such
*	that all elements larger than an index i in B is greater 
*	than a. And all elements smaller than or equal to the index
*	i is less than or equal to a. 
*
*****************************************************************/
int rank(int a, int *B, int start, int end){
	if(start>end){
		return start;	//was not found in array
	}	
	if(end==start){
		if(B[start]>a){
			return start;
		}
		if(B[start]<=a){
			return start+1;
		}
	}
	else{
		int mid;
		int n = end - start+1;
		mid = start+(n>>1);
		if(B[mid] > a){
			return rank(a, B, start, mid-1);
		}
		if (B[mid] < a){
			return rank(a, B, mid+1, end);
		}
		if (B[mid] == a){
			return mid;
		}
	}
}
