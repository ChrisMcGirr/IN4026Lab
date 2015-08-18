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
	struct timeval startt, endt, result;
	
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
	n = atoi(argv[2])+1;
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

		/*Start Timer*/
		result.tv_sec=0;
		result.tv_usec=0;
		gettimeofday (&startt, NULL);

		nodeLength(S, R, n);

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
	int *P = malloc(n*sizeof(int));	
	int i;

	/*Copy Contents into working Array*/
	for(i=0; i<n; i++){
		P[i] = S[i];
		if(S[i] > 0){
			R[i] = 1;
		}
	}
	/*Process each node sequential*/
	for(i=0; i<n; i++){
		while(P[i] > 0){	
			R[i] = R[i]+R[P[i]];
			P[i] = P[P[i]];
		}	
	}

	free(P);

}

