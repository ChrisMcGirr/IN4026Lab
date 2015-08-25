/******************************************************************
*	IN4026 Lab A: Prefix and Suffix Minima
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: par_posix.c
*	Description: The parallel version of the sequential algorithm
*	using the Pthread library. The algorithm has been parallized
*	around the main loop for computing the prefix and suffix values.
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>
#include <errno.h>
#include "fileIO.h"

//Make the barrier variable global to all threads
pthread_barrier_t   barrier;

//Parameters are given in Makefile
int RUNS; //Number of Runs of the algorithm
int MAX_THREADS; //Number of threads available

void* psMin(void* args);
int minArray(int *A, int n);
void minima(int *A, int n, int *B);

//Arguements to be passed to all threads
typedef struct data {
	int *A;
	int *P;
	int *S;
	int n; //Size of the arrays
	int start; //start of the loop
	int end; //end of the loop
	int id; //thread id
} data;


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
*
*****************************************************************/
int main(int argc, char **argv)
{
	struct timeval startt, endt, result;

	char name[16] = "posix";
	
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

	//Start algorithm testing
	int j;
	double average;
	for(j=0; j<RUNS; j++){

		/*Start Timer*/
		result.tv_sec=0;
		result.tv_usec=0;
		gettimeofday (&startt, NULL);

		//setup the threads and args for threads
		pthread_t thread_id[MAX_THREADS];
		int k;
		data thread_args[MAX_THREADS];
		
		//figure out the amount of work for each thread
		int m = n/MAX_THREADS;
		int rem = n%MAX_THREADS;
		int allocated=0;

		//Set the threads to Detached state so they release resources immediately
		//when terminated
		pthread_attr_t attribute;
		pthread_attr_init(&attribute);
		pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
		
		//Make sure the number of barriers is correct in case of work is less than
		//amount of threads available.
		int numbarriers = MAX_THREADS;
		if(m==0){
			numbarriers = rem;
		}
		
		//Initiate the barrier variable for threads
		pthread_barrier_init (&barrier, NULL, numbarriers);

		//Setup all the thread arguements with their respective workloads
		for(k=0; k<MAX_THREADS; k++){
			thread_args[k].A = A;
			thread_args[k].P = P;
			thread_args[k].S = S;
			thread_args[k].n = n;
			if(rem){
				thread_args[k].start = k*m+allocated;
				thread_args[k].end = (k+1)*m+1+allocated;
				rem--;
				allocated++;
			}
			else{
				thread_args[k].start = k*m+allocated;
				thread_args[k].end = (k+1)*m+allocated;
				
			}
			thread_args[k].id = k;
		}

		//Create all the threads
		for(k=1; k<MAX_THREADS; k++){
			//Check to see if they have work
			if(thread_args[k].start != thread_args[k].end )
			pthread_create(&thread_id[k], &attribute, &psMin, &thread_args[k]);
		}
		
		//Run main thread with arguements
		psMin(&thread_args[0]);

		//Release barrier resources
		pthread_barrier_destroy(&barrier);

		/*Stop Timer*/
		gettimeofday (&endt, NULL);
		result.tv_usec = (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
		average += result.tv_usec;
	}
	average = average/RUNS;	//Find average execution time
	
	//Print the results	
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
	/*Output Results if N is small*/
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
*	Input:	void* args	pointer to data struct
*
*	Output: void*
*
*	Description: Runs the prefix and suffix algorthim.Main Loop
*	is parallelized and so each thread is given where is should
*	start and end its work. These values are given through the
*	void* args which is the pointer to the struct of type data
*	which contains all the information. At the end the thread
*	waits at the barrier until all the other threads who are 
*	running are done
*
*****************************************************************/
void* psMin(void* args){
	int i;
	int *A, *P, *S, n, end, start, id;
	data *input = (data*)args;
	A = input->A;
	P = input->P;
	S = input->S;
	n = input->n;
	start = input->start;
	end = input->end;
	id = input->id;

	for(i=start; i<end; i++){
		P[i] = minArray(A, i+1);
		S[i] = minArray(&A[i], n-i);
			
	}
	pthread_barrier_wait(&barrier);
	return NULL;
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
	for(l=0; l<n; l++){
		p = 2*l;
		if(A[p]>A[p+1]) B[l] = A[p+1];
		else B[l] = A[p];
	}

}

