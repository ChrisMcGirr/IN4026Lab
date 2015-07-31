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
#include <pthread.h>

#define CHUNKSIZE 4

typedef struct args {
	int *A;
	int *B;
	int *C;
	int n;
	int m;
	int s1, e1, s2, e2;
} args;

typedef struct queue {
	int queue1[2]; //queue for AinB
	int queue2[2]; //queue for BinA
} queue;

void* simpleMerge(void* argv);
void AinB(int *A, int *B, int *C, int n, int m, int start, int end);
int checkQueue(args *input);
void BinA(int *A, int *B, int *C, int n, int m, int start, int end);
int rank(int a, int *B, int start, int end);
int write_Array(int* A, int n);
void generateInputs(int *A, int n);


int RUNS;
int MAX_THREADS;
int *AA, *BB;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
queue jobs;


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
	double start, end;
	double cpu_time_used;
	
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
		printf("Failed to Read Input A \n");
		return 1;
	}

	//Read the input array from file and save to memory
	status = read_input(B, m, argv[2]);

	if(status){
		printf("Failed to Read Input B \n");
		return 1;
	}
	
	AA = malloc(n*sizeof(int));
	BB = malloc(m*sizeof(int));

	//Start of testing of the algorithm
	int j;
	double average;
	for(j=0; j<RUNS; j++){
		start = omp_get_wtime(); //start timer

/**************************************************************
		//NEED TO ADD THREAD CREATION CODE!!
**************************************************************/

		//simpleMerge(A, B, C, n, m);

		end = omp_get_wtime(); //end timer
		cpu_time_used = end - start;
		average += cpu_time_used;
	}
	average = average/RUNS; //Average the execution times

	//print results to terminal
	printf("%d 	%f	s \n",n,average);

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

	status = write_output(A, B, C, n, m, name);

	if(status){	
		printf("Failed to Write Output \n");
		return 1;
	}

	free(A);
	free(B);
	free(C);
	free(AA);
	free(BB);
	
	
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
void* simpleMerge(void* argv){
	args *input = (args*)argv;
	int status;

	AinB(input->A, input->B, input->C, input->n, input->m, input->s1, input->e1);
	BinA(input->A, input->B, input->C, input->n, input->m, input->s2, input->e2);
	
	status = checkQueue(input);
	if(status){
		simpleMerge(input);	
	}
	
}
void AinB(int *A, int *B, int *C, int n, int m, int start, int end){
	int i;
	if(start!=end){	
		for(i=start; i<end; i++){
			AA[i] = rank(A[i]-1, B, 0, m-1);
			C[i+AA[i]] = A[i];
		}
	}
}
void BinA(int *A, int *B, int *C, int n, int m, int start, int end){
	int i;	
	if(start!=end){	
		for(i=start; i<end; i++){
			BB[i] = rank(B[i], A, 0, n-1);
			C[i+BB[i]] = B[i];
		}
	}
}
int checkQueue(args *input){
	int empty = 1;
	int status = pthread_mutex_lock(&queue_lock);
	if(status){
		perror("pthread_mutex_lock");
    		pthread_exit(NULL);	
	}
	printf("Job Queue 1 is %d to %d\n", jobs.queue1[0], jobs.queue1[1]);
	//Check to see if there are any jobs left for AinB
	if((jobs.queue1[0]) != (jobs.queue1[1])){
		int n = jobs.queue1[1]-jobs.queue1[0];
		int m = n%CHUNKSIZE;
		if(m){
			input->s1 = jobs.queue1[0];
			input->e1 = jobs.queue1[0]+m;
			jobs.queue1[0]+=m;
		}
		else{
			input->s1 = jobs.queue1[0];
			input->e1 = jobs.queue1[0]+CHUNKSIZE;
			jobs.queue1[0]+=CHUNKSIZE;
		} 
		empty = 0;
	}
	else{
		input->s1 = input->e1;	
	}
	printf("Job Queue 2 is %d to %d\n", jobs.queue2[0], jobs.queue2[1]);
	//Check to see if there are any jobs left for BinA
	if((jobs.queue2[0]) != (jobs.queue2[1])){
		int n = jobs.queue2[1]-jobs.queue2[0];
		int m = n%CHUNKSIZE;
		if(m){
			input->s2 = jobs.queue2[0];
			input->e2 = jobs.queue2[0]+m;
			jobs.queue2[0]+=m;
		}
		else{
			input->s2 = jobs.queue2[0];
			input->e2 = jobs.queue2[0]+CHUNKSIZE;
			jobs.queue2[0]+=CHUNKSIZE;
		} 
		empty = 0;
	}
	else{
		input->s2 = input->e2;	
	}

	status = pthread_mutex_unlock(&queue_lock);
	if(status){
		perror("pthread_mutex_unlock");
   		pthread_exit(NULL);
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
