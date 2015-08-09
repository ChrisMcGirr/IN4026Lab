/******************************************************************
*	IN4026 Lab C: List Ranking (Pointer Jumping)
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: seq.c
*	Description: The sequential version of the linked list 
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

void nodeLength(int* S, int* R, int n);


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
	
	char name[8] = "seq/";

	int status;
	int n;
	int* S;
	int* R;
	
	int RUNS;
	
	//Check if app was given enough input
	if(argc < 5){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq path_input input_size ans_Path RUNS\n");
		return 1;
	}
	
	//Save args to memory and allocate memory for arrays
	n = atoi(argv[2]);
	RUNS = atoi(argv[4]);
	S = malloc(n*sizeof(int));
	R = malloc(n*sizeof(int));


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
	int j;
	double average;
	for(j=0; j<RUNS; j++){
		memset(R, 0, n*sizeof(int));
		start = omp_get_wtime(); //start timer
		nodeLength(S, R, n);
		end = omp_get_wtime(); //end timer
		cpu_time_used = end - start;
		average += cpu_time_used;
		
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

	free(S);
	free(R);	
	
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
void nodeLength(int* S, int* R, int n){
	int i;
	int pointer;
	for(i=0; i<n; i++){
		if(S[i]!=i){
			R[i] = 1;
		}
		else{
			R[i] = 0;
		}
	}

	for(i=0; i<n; i++){
		while(S[i]!= S[S[i]]){	
			R[i] = R[i]+R[S[i]];
			S[i] = S[S[i]];
		}	
	}

}
