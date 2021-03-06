/******************************************************************
*	IN4026 Lab C: List Ranking (Pointer Jumping)
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: par_posix.c
*	Description: The PThread parallel version of the linked list 
*	distance calculation. Takes an input array S of size n
*	and attempts to find the distance of each node in S to 
*	the final node 0. The solution is then saved in the output
*	array R of size n.
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "fileIO.h"
#include <pthread.h>

#define CHUNKSIZE 256

//Make the barrier variable global to all threads
pthread_barrier_t   barrier;
pthread_barrier_t   init; /*Initialization Barrier*/

typedef struct args {
	int *S;
	int *R;
	int *P;
	int *R_temp;
	int *P_temp;
	int n;
	int s1, e1;
	int id;
} args;

typedef struct queue {
	int queue1[2];
	int m; 
} queue;

void* nodeLength(void* argv);
void* arrayInit(void* argv);
int checkQueue(args *input);


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
	struct timeval startt, endt, result;
	
	char name[8] = "posix/";
	
	int status = 0;
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
		printf("Failed to Read Input S\n");
		return 1;
	}
	
	/*Declaring Temporary Arrays*/
	int *P_temp = malloc(n*sizeof(int));	
	int *R_temp = malloc(n*sizeof(int));

	//Start of testing of the algorithm
	int j, i;
	double average;
	for(j=0; j<RUNS; j++){
		memset(R, 0, n*sizeof(int));
		memset(P, 0, n*sizeof(int));

		/*Start Timer*/
		result.tv_sec=0;
		result.tv_usec=0;
		gettimeofday (&startt, NULL);
	
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

		/*Setup variables to be passed to threads*/
		args input[MAX_THREADS];
		for(k=0; k<MAX_THREADS; k++){
			input[k].S = S;
			input[k].R = R;
			input[k].P = P;
			input[k].n = n;
			input[k].id = k;
			input[k].R_temp = R_temp;
			input[k].P_temp = P_temp;
		}

		/*Initialize the Array in Parrallel*/

		jobs.queue1[0] = 0;
		jobs.queue1[1] = n;
		jobs.m = ceil(log2(n)); /*Set the number of steps*/
		
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

		/*Stop Timer*/
		gettimeofday (&endt, NULL);
		result.tv_usec = (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
		average += result.tv_usec;

		//Release barrier resources
		pthread_barrier_destroy(&barrier);
		pthread_barrier_destroy(&init);
		
		
	}
	average = average/RUNS; //Average the execution times

	//print results to terminal
	printf("%d 	%f	us \n",n-1,average);

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
	
	/*Save the Results if the output is less than 50 elements*/
	if(n<=50){
		status = write_output(S, R, n, name);
	}

	if(status){	
		printf("Failed to Write Output \n");
		return 1;
	}
	
	free(S);
	free(R);
	free(P);
	free(R_temp);
	free(P_temp);	
	
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
	int *S,*P,*R, *R_temp, *P_temp;

	args* input = (args*)argv;
	S = input->S;
	P = input->P;
	R = input->R;
	R_temp = input->R_temp;
	P_temp = input->P_temp;

	int status;
	status = checkQueue(input);

	if(status>0){

		/*Copy Contents into working Array and initalize R*/
		for(i=(input->s1); i<(input->e1); i++){
			P[i] = S[i];
			P_temp[i] = S[i];
			if(S[i] > 0){
				R[i] = 1;
				R_temp[i] = 1;
			}
			else{
				R[i] = 0;
				R_temp[i] = 0;
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
*	Function: arrayCopy
*	Input:	arg *argv	Pointer to Thread Struct
*
*	Output: returns nothing
*
*	Description: Copys the results from the temporary arrays
*	to their respective output arrays
*
*****************************************************************/
void* arrayCopy(void* argv){
	int i;
	int *S,*P,*R, *R_temp, *P_temp;

	args* input = (args*)argv;
	S = input->S;
	P = input->P;
	R = input->R;
	R_temp = input->R_temp;
	P_temp = input->P_temp;

	int status;
	status = checkQueue(input);

	if(status>0){

		for(i=(input->s1); i<(input->e1); i++){
			R[i] = R_temp[i];
			P[i] = P_temp[i];		
		}
		arrayCopy(input);	

	}
	else{	
		/*Wait for all Threads to Finish*/
		pthread_barrier_wait(&init);
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
	int *S,*P,*R, *R_temp, *P_temp;

	args* input = (args*)argv;
	S = input->S;
	P = input->P;
	R = input->R;
	R_temp = input->R_temp;
	P_temp = input->P_temp;

	int status;
	status = checkQueue(input);
	
	if(status>0){

		/*Process each node */
		for(i=(input->s1); i<(input->e1); i++){
			if(P[i] > 0){	
				R_temp[i] = R[i]+R[P[i]];			
				P_temp[i] = P[P[i]];
			}	
		}

		/*Check to see if more work is left*/
		nodeLength(input);
	}
	else{	
		/*Wait for other threads to finish step*/
		pthread_barrier_wait(&barrier);

		/*Reset Work Queue so we can start Copying Results Back*/
		if(input->id == 0){ /*Only master touches it*/
			jobs.queue1[0] = 0;
			jobs.queue1[1] = input->n;
		}

		/*Wait for Master to update the Queue*/
		pthread_barrier_wait(&barrier);

		/*Start Copying array back*/		
		arrayCopy(input);

		/*Wait for other threads to finish*/
		pthread_barrier_wait(&barrier);

		/*Finished all steps*/
		if(status==-1){
			if(input->id){		
				pthread_exit(NULL);
			}
		}
		else{
			/*Reset the work queue*/
			if(input->id == 0){ /*Only master touches it*/
				jobs.m -= 1;		
				jobs.queue1[0] = 0;
				jobs.queue1[1] = input->n;
			}
			/*Wait for other threads to finish step*/
			pthread_barrier_wait(&barrier);
			nodeLength(input);
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
	int chunk;

	if( (input->n) <= 256){
		chunk = 4; /*Used for small N*/
	}
	else{
		chunk = CHUNKSIZE;	
	}

	int status = pthread_mutex_lock(&queue_lock);

	if(status){
		printf("ERROR in CheckQueue(): pthread_mutex_lock \n");	
	}

	/*Check to see if there are any jobs left*/
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
			input->e1 = jobs.queue1[0]+chunk;
			jobs.queue1[0]+=chunk;
		} 
	}
	else{
		input->s1 = input->e1;
		nempty -= 1;	
	}
	if(jobs.m == 0){
		nempty = -1; /*Signal that we're done all the log(n) steps*/
	}

	status = pthread_mutex_unlock(&queue_lock);
	if(status){
		printf("ERROR in CheckQueue(): pthread_mutex_unlock \n");
	}
	return nempty;
}
