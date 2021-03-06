/******************************************************************
*	IN4026 Lab A: Prefix and Suffix Minima
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: fileIO.c
*	Description: contains all the functions needed for file
*	input and output like reading the arrays and writing the 
*	arrays to file. Also includes a file check function to see
*	if the results of the algorithm matchs the answer given in
*	a text file. 
*
******************************************************************/

#include "fileIO.h"

/****************************************************************
*
*	Function: outputCheck
*	Input:	int *P Pointer to Prefix array, result of algorithm
*		int *S Pointer to Suffix array, result of algorithm
*		char* pfile	Pointer to file location with answer
*		char* sfile	Pointer to file location with answer
*		int n		Size of the arrays
*
*	Output: int 0 for match or 1 if values do not match
*
*	Description: Simply checks the results of the algorithms given
*	in the arrays P and S to the answers saved in the files pfile
*	sfile. Returns 0 if P and S are correct or 1 if they are not
*
*
*****************************************************************/
int outputCheck(int *P, int *S, char* pfile, char* sfile, int n){

	//need to allocate memory for the answers from file 
	int *P_ans = malloc(n*sizeof(int));
	int *S_ans = malloc(n*sizeof(int));
	
	//Default return value is 0 (success)
	int correct = 0;

	read_input(P_ans, n, pfile);
	read_input(S_ans, n, sfile);
	
	//Cycle through arrays P & S to check if the answers match
	int i;
	for(i=0; i<n; i++){
		if(P[i] != P_ans[i]){
			correct=1;		
		}	
	}
	for(i=0; i<n; i++){
		if(S[i] != S_ans[i]){
			correct=1;		
		}	
	}
	
	free(P_ans);
	free(S_ans);
	
	return correct;
}
/****************************************************************
*
*	Function: read_input
*	Input:	int* A	Pointer to location where array from file is
*			to be stored in memory. Make sure to malloc()
*			the array before calling this function.
*		int n	Size of the array
*		char* file	Pointer to file where array is stored
*
*	Output: int 0 if successful or 1 if failure
*
*	Description: Reads an array from file and saves it to the 
*	array A given as a pointer in the input.
*
*
*****************************************************************/
int read_input(int* A, int n, char* file) {

	FILE *input;
	char buf[128];
	
	input =fopen(file,"r");

	if (!input){
		printf("Error File Does not Exist. \n");
		return 1;
	}

	//Get the first line of the file and discard it
	if(fgets(buf,20, input)!=NULL){
		//printf("%s",buf);	
	}
	else{	
		printf("First Line could not be read \n");
		return 1;	
	}
	//Start copying array values from file to memory
	int i;
	for(i = 0; i<n; i++){
		if(fgets(buf,20, input)!=NULL){
			A[i] = atoi(buf);
			A[i] = A[i] - atoi(",");
		}
	}

	fclose(input);
	return 0;
}
/****************************************************************
*
*	Function: write_output
*	Input:	int* P	Pointer to Prefix array 
*		int* S	Pointer to Suffix array
*		int n	Size of the arrays.
*
*	Output: int 0 if successful or 1 if failure
*
*	Description: Writes the prefix and suffix array results to
*	file under the name file location "results/results.txt".
*
*
*****************************************************************/
int write_output(int* P, int* S, int n, char *name){

	FILE *output;
	char file[64] = "output/results_";
	char value[32];
	sprintf(value, "%d.txt", n);
	strcat(file, name);
	strcat(file, value);

	//create output file for arrays
	output = fopen(file, "w");
	if(output==NULL){	
		printf("Failed to create the Output File \n");
		return 1;
	}

	//Start printing P to file
	fprintf(output, "P = {");
	
	int i;
	for(i=0; i<n; i++){
		if(i==n-1){
			fprintf(output, "%d", P[i]);		
		}
		else{
			fprintf(output, "%d, ", P[i]);
		}
		if((i%16 == 0) && (i!=0)){
			fprintf(output, "\n");
		}
		
	}
	fprintf(output, "} \n");
	
	//Start printing S to same file
	fprintf(output, "S = {");
	for(i=0; i<n; i++){
		if(i==n-1){
			fprintf(output, "%d", S[i]);		
		}
		else{
			fprintf(output, "%d, ", S[i]);
		}
		if((i%16 == 0) && (i!=0)){
			fprintf(output, "\n");
		}
		
	}
	fprintf(output, "} \n");
	
	//Finish writing and close file
	fclose(output);

	return 0;
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
	int n=2048;
	int *A;

	//Generates arrays of size 1 to 48
	//and saves them to file.
	for(i=1; i<8; i++){
		n=n*2;
		A = malloc(n*sizeof(int));
		randArray(A, n);
		write_Array(A,n);
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
	char name[64];
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
