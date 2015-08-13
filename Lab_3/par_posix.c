/******************************************************************
*	IN4026 Lab C: List Ranking
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: par_posix.c
*	Description: The parallel version of the List Ranking
*	algorthim using the PThread library. List Ranking computes
*	the distance of a node in a tree to the final parent node
*	which has no outgoing pointer.  
*	The List Ranking is done by pointer doubling until the final 
*	node 0 is is reached. The threads are set up such that each 
*	thread grabs from the available queue of work. 
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

//Make the barrier variable global to all threads
pthread_barrier_t   barrier;
pthread_barrier_t   init; /*Initialization Barrier*/

typedef struct args {
	int *S;
	int *R;
	int *P;
	int n;
	int s1, e1;
	int id;
} args;

typedef struct queue {
	int queue1[2]; 
} queue;

void* nodeLength(void* argv);
void* arrayInit(void* argv);
int checkQueue(args *input);
void generateArrays(void);


int RUNS;
int MAX_THREADS;

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t c_lock = PTHREAD_MUTEX_INITIALIZER;
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
	struct timespec start, end;
	double cpu_time_used;
	
	char name[8] = "posix/";
	
	int status;
	int n;
	int* S;
	int* R;
	int* P;
	
	//Check if app was given enough input
	if(argc < 6){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq path_input input_size ans_Path RUNS MAX_THREADS\n");
		return 1;
	}
	
	//Save args to memory and allocate memory for arrays
	n = atoi(argv[2])+1;
	RUNS = atoi(argv[4]);
	MAX_THREADS = atoi(argv[5]);
	S = malloc(n*sizeof(int));
	R = malloc(n*sizeof(int));
	P = malloc(n*sizeof(int));	

	if(S==NULL){
		printf("Failed to Allocate Memory for Input Array S");	
	}
	if(R==NULL){
		printf("Failed to Allocate Memory for Input Array R");	
	}

	//Read the input array from file and save to memory
	status = read_input(S, n, argv[1]);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Read Input S\n");
		#endif
		return 1;
	}

	//Start of testing of the algorithm
	int j, i;
	double average;
	for(j=0; j<RUNS; j++){
		memset(R, 0, n*sizeof(int));
		memset(P, 0, n*sizeof(int));
		clock_gettime(CLOCK_MONOTONIC, &start); //start timer
	
		int rc;
		pthread_t thread_id[MAX_THREADS];
		pthread_attr_t attr[MAX_THREADS];

		int k;
		for(k=0; k<MAX_THREADS; k++){
			pthread_attr_init(&attr[k]);
			pthread_attr_setdetachstate(&attr[k], PTHREAD_CREATE_DETACHED);
		}

		//Initiate the barrier variable for threads
		pthread_barrier_init (&barrier, NULL, MAX_THREADS);
		pthread_barrier_init (&init, NULL, MAX_THREADS);

		args input[MAX_THREADS];
		for(k=0; k<MAX_THREADS; k++){
			input[k].S = S;
			input[k].R = R;
			input[k].P = P;
			input[k].n = n;
			input[k].id = k;
		}

		/*Initialize the Array in Parrallel*/
		jobs.queue1[0] = 0;
		jobs.queue1[1] = n;
		
		for(k=1; k<MAX_THREADS; k++){
			pthread_create(&thread_id[k], &attr[k], &arrayInit, &input[k]);
		}
		arrayInit(&input[0]);

		/*Compute the Distance of each Node*/
		jobs.queue1[0] = 0;
		jobs.queue1[1] = n;
		
		for(k=1; k<MAX_THREADS; k++){
			pthread_create(&thread_id[k], &attr[k], &nodeLength, &input[k]);
		}
		nodeLength(&input[0]);

		clock_gettime(CLOCK_MONOTONIC, &end); //end timer
		cpu_time_used = (end.tv_sec-start.tv_sec);
		cpu_time_used += (end.tv_nsec-start.tv_nsec)/1000000000.0;
		average += cpu_time_used;

		//Release barrier resources
		pthread_barrier_destroy(&barrier);
		pthread_barrier_destroy(&init);
		
		
	}
	average = average/RUNS; //Average the execution times

	//print results to terminal
	printf("%d 	%f	s \n",n,average);

	if(atoi(argv[3])!=1)
	{
		status = outputCheck(R, argv[3], n);
		if(status){
			printf("Incorrect Answer\n");
		}
		else{
			printf("Correct Answer\n");
		}
	}
	

	status = write_output(S, R, n, name);

	if(status){	
		printf("Failed to Write Output \n");
		return 1;
	}
	
	/*Used to generate the input files for the batch run*/
	generateArrays();

	free(S);
	free(R);
	free(P);	
	
    	return 0;
}
/****************************************************************
*
*	Function: arrayInit
*	Input:	arg *argv	Pointer to Thread Struct
*
*	Output: returns nothing
*
*	Description: Initializes the array with the initial distance
*	values. Done in parallel.
*
*****************************************************************/
void* arrayInit(void* argv){
	int i;
	int *S,*P,*R;

	args* input = (args*)argv;
	S = input->S;
	P = input->P;
	R = input->R;

	int status;
	status = checkQueue(input);

	if(status){

		/*Copy Contents into working Array and initalize R*/
		for(i=(input->s1); i<(input->e1); i++){
			P[i] = S[i];
			if(S[i] > 0){
				R[i] = 1;
			}
		}
		arrayInit(input);	

	}
	else{	
		pthread_barrier_wait(&init);
		if(input->id){		
			pthread_exit(NULL);
		}
	}
}
/****************************************************************
*
*	Function: nodeLength
*	Input:	int *S	Pointer to Input Array S
*		int *R	Pointer to Output Array R
*		int n   Size of the Arrays S and R
*
*	Output: returns nothing
*
*	Description: Takes a linked list array S and finds the
*	distance of each node in S to the final node 0. The 
*	distance is saved in the output array R. 
*
*****************************************************************/
void* nodeLength(void* argv){
	int i;
	int *S,*P,*R;

	args* input = (args*)argv;
	S = input->S;
	P = input->P;
	R = input->R;

	int status;
	status = checkQueue(input);
	
	if(status){

		/*Process each node sequential*/
		for(i=(input->s1); i<(input->e1); i++){
			while(P[i] > 0){	
				R[i] = R[i]+R[P[i]];
				P[i] = P[P[i]];
			}	
		}

		nodeLength(input);	

	}
	else{	
		pthread_barrier_wait(&barrier);
		if(input->id){		
			pthread_exit(NULL);
		}
	}

}
/****************************************************************
*
*	Function: checkQueue
*	Input:	args *input	Pointer to thread struct 
*
*	Output: int nempty	returns 0 if no jobs are left
*
*	Description: Allocates a piece of the total work to the 
*	threads based on a predefined chunksize. When
*
*****************************************************************/
int checkQueue(args *input){
	int nempty = 1;

	int status = pthread_mutex_lock(&queue_lock);

	if(status){
		printf("ERROR in CheckQueue(): pthread_mutex_lock \n");	
	}

	//Check to see if there are any jobs left
	//printf("Thread ID %d \n", input->id);
	//printf("S1 %d and E1 %d \n",jobs.queue1[0] , jobs.queue1[1]);
	if((jobs.queue1[0]) < (jobs.queue1[1])){
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
	}
	else{
		input->s1 = input->e1;
		nempty -= 1;	
	}

	status = pthread_mutex_unlock(&queue_lock);
	if(status){
		printf("ERROR in CheckQueue(): pthread_mutex_unlock \n");
	}
	return nempty;
}
void generateArrays(){
	int i,j;
	int *out;
	int n=2048;

	for(i=1; i<8; i++){
		n = n*2;
		out = malloc(n*sizeof(int));
		for(j=0; j<n; j++){
			out[i] = rand ();	
		}
		write_Array(out, n);
		free(out);
	}
}
