/******************************************************************
*	IN4026 Lab C: List Ranking (Pointer Jumping)
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: par_omp.c
*	Description: The OpenMP parallel version of the linked list 
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
#include <math.h>
#include "fileIO.h"
#include <omp.h>

#define CHUNKSIZE 256

void nodeLength(int* S, int* R, int n, int *P_temp, int *R_temp, int *P);

int MAX_THREADS;
int chunk = CHUNKSIZE;


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

	int status=0;
	int n;
	int* S;
	int* R;
	
	int RUNS;
	
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

	if(n<50){
		chunk = 4; /*For Small N*/
	}

	omp_set_dynamic(0); //Makes sure the number of threads available is fixed    
	omp_set_num_threads(MAX_THREADS); //Set thread number


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
	
	int *P_temp = malloc(n*sizeof(int));	
	int *R_temp = malloc(n*sizeof(int));
	int *P = malloc(n*sizeof(int));

	//Start of testing of the algorithm
	int j;
	double average;
	for(j=0; j<RUNS; j++){
		memset(R, 0, n*sizeof(int));

		/*Start Timer*/
		result.tv_sec=0;
		result.tv_usec=0;
		gettimeofday (&startt, NULL);

		/*Start Algorithm*/
		nodeLength(S, R, n, P_temp, R_temp, P);

		/*Stop Timer*/
		gettimeofday (&endt, NULL);
		result.tv_usec = (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
		average += result.tv_usec;
		
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
	free(P_temp);
	free(R_temp);
	free(P);	
	
    	return 0;
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
void nodeLength(int* S, int* R, int n, int *P_temp, int *R_temp, int *P){	
	int i,j, m;
	m = floor(log2(n));

	/*Copy Contents into working Array*/
	//#pragma omp parallel for schedule(dynamic, chunk) shared(S) private(i) num_threads(MAX_THREADS)
	for(i=0; i<n; i++){
		P[i] = S[i];
		if(S[i] > 0){
			R[i] = 1;
			R_temp[i] = 1;
		}
		else{
			R[i] = 0;
			R_temp[i] = 0;
		}
	}

	/*Process each node step by step*/
	for(j=1; j<=m; j++){
		//#pragma omp parallel for schedule(dynamic, chunk) shared(R, P, R_temp, P_temp) private(i) num_threads(MAX_THREADS)
		for(i=0; i<n; i++){
			if(P[i]>0){
				R_temp[i] = R[i]+R[P[i]];			
				P_temp[i] = P[P[i]];
			}
		}
		//#pragma omp parallel for schedule(dynamic, chunk) shared(R, P, R_temp, P_temp) private(i) num_threads(MAX_THREADS)
		for(i=0; i<n; i++){
			R[i] = R_temp[i];
			//P[i] = P_temp[i];		
		}
		
	}
}

