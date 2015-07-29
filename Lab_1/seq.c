/******************************************************************
*	IN4026 Lab A: Prefix and Suffix Minima
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


int minArray(int *A, int n);
void psMin(int *A, int *P, int *S, int n);
void randArray(int *A, int n);
int write_Array(int* A, int n);
void generateArrays(void);
void minima(int *A, int n);

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
	double start, end;
	double cpu_time_used;
	
	int status;
	int n;
	int* A;
	int* P;
	int* S;
	
	int RUNS;
	
	//Check if app was given enough input
	if(argc < 6){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq file_path array_size P_ans_path S_ans_Path RUNS\n");
		return 1;
	}
	
	//Save args to memory and allocate memory for arrays
	n = atoi(argv[2]);
	RUNS = atoi(argv[5]);
	A = malloc(n*sizeof(int));
	P = malloc(n*sizeof(int));
	S = malloc(n*sizeof(int));

	if(A==NULL){
		printf("Failed to Allocate Memory for Input Array");	
	}

	//Read the input array from file and save to memory
	status = read_input(A, n, argv[1]);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Read Input \n");
		#endif
		return 1;
	}
	
	//Start of testing of the algorithm
	int j;
	double average;
	for(j=0; j<RUNS; j++){
		start = omp_get_wtime(); //start timer
		psMin(A, P, S, n);
		end = omp_get_wtime(); //end timer
		cpu_time_used = end - start;
		average += cpu_time_used;
	}
	average = average/RUNS; //Average the execution times

	//print results to terminal
	printf("%d 	%f	s \n",n,average);

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
	
	//Used to generate random data sets and save to disk as text files
	//generateArrays();

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
*	sets the number of threads available to OpenMP.
*	Each Loop iteration a copy of A is made to B where the work
*	is done on. As the minArray writes to B and saves the result
*	to B[0] due to the balanced tree method.
*
*****************************************************************/
void psMin(int *A, int *P, int *S, int n){
	int i;
	int *B = malloc(n*sizeof(int));
	for(i=0; i<n; i++){		
		memcpy(B, A, n*sizeof(int));
		P[i] = minArray(B, i+1); //find prefix min
		B[i] = A[i]; //make sure the boundary is still correct
		S[i] = minArray(&B[i], n-i); //find suffix min
	}
	free(B);
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
	int i;
	min = INT_MAX;
	m = ceil(log2(n));
	
	//Simple case
	if(n==1){
		return A[0];
	}
	//Cycle through Log2 levels of the array
	for(j=1;j<=m;j++){
		//Check to see if odd and save odd element
		if(n%2){
			min = A[n-1];
		}
		//calculate the min using balanced tree with even n
		n = n>>1;		
		minima(A,n);
		//check to see if the result of the tree is greater than min
		if(A[0]>min) A[0] = min;		

	}
	
	//just to make sure
	if( min > A[0]) min = A[0];

	return min;
}
/****************************************************************
*
*	Function: minima
*	Input:	int *A	Pointer to input array
*		int n	Size of the array
*
*	Output: void
*
*	Description: Finds the minimum of a given Array A of size
*	n using the balanced tree method. Saves the result in the
*	array position A[0]. Returns no value. 
*
*****************************************************************/
void minima(int *A, int n){
	int p,l;
	for(l=0; l<n; l++){
		p = 2*l;
		if(A[p]>A[p+1]) A[l] = A[p+1];
		else A[l] = A[p];
	}

}
/****************************************************************
*
*	Function: generateArrays
*	Input:	void
*
*	Output: void
*
*	Description: Generates random arrays used to be given as
*	input to the algorithm. Only ran once to create the arrays
*
*****************************************************************/
void generateArrays(){
	int i;
	//Generates arrays of size 1 to 48
	//and saves them to file.
	for(i=1; i<=48; i++){
		int *A;
		A = malloc(i*sizeof(int));
		randArray(A, i);
		write_Array(A,i);
		free(A);
	}
}
/****************************************************************
*
*	Function: write_Array
*	Input:	int *A	Pointer to input array
*		int n	Size of the array
*
*	Output: void
*
*	Description: Writes a generated array to file. 
*
*****************************************************************/
int write_Array(int* A, int n){

	FILE *output;
	char name[16];
	snprintf(name, sizeof(name), "Input_Data/input_%d.txt", n);
	output = fopen(name, "w");
	if(output==NULL){
		#ifdef DEBUG	
		printf("Failed to create the Output File \n");
		#endif
		return 1;
	}
	fprintf(output, "P = {\n");
	
	int i;
	for(i=0; i<n; i++){
		fprintf(output, "%d,\n", A[i]);
	}
	fprintf(output, "};");
	fclose(output);

	return 0;
}
/****************************************************************
*
*	Function: randArray
*	Input:	int *A	Pointer to input array
*		int n	Size of the array
*
*	Output: void
*
*	Description: Generates a random array of size n with values
*	from 0 to 255 and saves it to memory location of A which is
*	given as input
*
*****************************************************************/
void randArray(int *A, int n){
	if(A==NULL){
		printf("Failed to allocate RandArray\n");	
	}
	int i;	
	for(i=0; i<n; i++){
		A[i] = rand() % 256;
	}
}
