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
int outputCheck(int *C,char* cfile, int n){

	//need to allocate memory for the answers from file 
	int *C_ans = malloc(n*sizeof(int));
	
	//Default return value is 0 (success)
	int correct = 0;

	read_input(C_ans, n, cfile);
	
	//Cycle through arrays P & S to check if the answers match
	int i;
	for(i=0; i<n; i++){
		if(C[i] != C_ans[i]){
			correct=1;		
		}	
	}

	free(C_ans);

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
	char buf[20];
	
	input =fopen(file,"r");

	if (!input){
		printf("Error File Does not Exist. \n");
		return 1;
	}

	//Get the first line of the file and discard it
	if(fgets(buf,20, input)!=NULL){
		//printf("Successfully Read Input\n");	
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
*	Input:	int* S	Pointer to array A 
*		int* R	Pointer to array B
*		int n	Size of the array A
		char *name Name of Program Running, e.g. seq
*
*	Output: int 0 if successful or 1 if failure
*
*	Description: Writes the output of the linked list along
*	with the input.
*
*
*****************************************************************/
int write_output(int *S, int *R, int n, char *name){

	FILE *output;
	char value[32];
	sprintf(value,"results_%d.txt", n);

	//create output file for arrays
	char file[128] = "output/";
	strcat(file, name);
	strcat(file, value);
	output = fopen(file, "w");
	if(output==NULL){	
		printf("Failed to create the Output File \n");
		return 1;
	}

	//Start printing P to file
	fprintf(output, "S = {");
	
	int i;
	for(i=0; i<n; i++){
		if(i==n-1){
			fprintf(output, "%d, ", S[i]);		
		}
		else{
			fprintf(output, "%d, ", S[i]);
		}
		if((i%16 == 0) && (i!=0)){
			fprintf(output, "\n");
		}
		
	}
	fprintf(output, "} \n\n");
	fprintf(output, "R = {");
	for(i=0; i<n; i++){
		if(i==n-1){
			fprintf(output, "%d, ", R[i]);		
		}
		else{
			fprintf(output, "%d, ", R[i]);
		}
		if((i%16 == 0) && (i!=0)){
			fprintf(output, "\n");
		}
		
	}
	
	//Finish writing and close file
	fclose(output);

	return 0;
}
/****************************************************************
*
*	Function: generateInputs
*	Input:	int *A	Pointer to input array
*		int n	Size of the array
*
*	Output: void
*
*	Description: Generates input files to be used. 
*
*****************************************************************/
void generateInputs(int *A, int n){
	int i;
	for(i=1; i<=n; i++){
		write_Array(A, i);
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
	snprintf(name, sizeof(name), "input_data/input_%d.txt", n-1);
	output = fopen(name, "w");
	if(output==NULL){
		printf("Failed to create the Input File \n");
		return 1;
	}
	fprintf(output, "B = {\n");
	
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
*	Function: generateArrays
*	Input:	int *A		Pointer to valid 16 length array
*
*	Output: void	
*
*	Description: Creates inputs for the algorithm of varying
*	sizes from 4096 doubling the size each time by 2. Outputs
*	them as text files to be read back by the algorithm.	
*
*****************************************************************/
void generateArrays(){
	int i,j;
	int *out;
	int count,p,s;
	int n=2048;

	srand ( time(NULL) );
	printf("Generating Inputs...\n");
	for(i=1; i<8; i++){
		n = n*2+1;
		printf("Input size %d \n",n-1);
		out = (int *)calloc(n, sizeof(int));
		/*Create the array*/
		for(j=0; j<n; j++){
			out[j] = j;
		}
		for(j=0; j<n; j++){
			p = rand()%(n-1);
			s = out[j];
			/*Swap the nodes*/
			out[j] = out[p];
			out[p] = s;
		}
		/*Sanity Check: Make sure length of all nodes is less than n*/
		for(j=0; j<n; j++){
			p = out[j];
			while(count < n){
				count++;
				p = out[p];
				if(p==0){
					count = 0;
					break;
				}
				if(count >= n){
					printf("List has no end \n");
					count=0;
					break;
				}
			}
		}
		write_Array(out, n);
		free(out);
		n = n-1;
	}
}
