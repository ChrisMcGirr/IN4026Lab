#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

//#define DEBUG

int read_input(int* A, int n, char* file);
int write_output(int* P, int* S, int n);
int minArray(int *A, int n, int i);
int outputCheck(int *P, int *S, char* pfile, char* sfile, int n);
void psMin(int *A, int *P, int *S, int n);
void randArray(int *A, int n);
int write_Array(int* A, int n);
void generateArrays(void);

int main(int argc, char **argv)
{
	clock_t start, end;
	double cpu_time_used;
	
	int status;
	int n;
	int* A;
	int* P;
	int* S;

	if(argc < 5){
		printf("Missing Arguement Parameters\n");
		printf("Format ./seq file_path array_size P_ans_path S_ans_Path\n");
		return 1;
	}

	n = atoi(argv[2]);
	A = malloc(n*sizeof(int));
	P = malloc(n*sizeof(int));
	S = malloc(n*sizeof(int));

	if(A==NULL){
		printf("Failed to Allocate Memory for Input Array");	
	}

	status = read_input(A, n, argv[1]);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Read Input \n");
		#endif
		return 1;
	}
	//Start of the Algorithm
	int j;
	double average;
	for(j=0; j<50000; j++){
		start = clock();
		psMin(A, P, S, n);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		if(j==0) average = cpu_time_used;
		else	 average = (average+cpu_time_used)/2;
	}	
	//End of Algorithm

	printf("Average Computation Time %fs for an input size of %d \n",average, n);
	if(atoi(argv[3]) && atoi(argv[4]))
	{
		status = outputCheck(P, S, argv[3], argv[4], n);
		if(status){
			printf("Incorrect Answer\n");
		}
		else{
			printf("Correct Answer\n");
		}
	}

	status = write_output(P, S, n);

	if(status){
		#ifdef DEBUG	
		printf("Failed to Write Output \n");
		#endif
		return 1;
	}

	free(P);
	free(S);
	
	//Used to generate random data sets and save to disk as text files
//	generateArrays();

/*
	//Multiple Runs
	time_t t;
	srand((unsigned) time(&t));
	int *B;

	printf("****TEST RESULTS OF VARYING N INPUT SIZE\n");
	printf("    Size N    Execution Time\n");
	
	int i;
	int k;
	for(i=1; i<=16; i++){
		k=n*i;
		randArray(B, k);
		P = malloc(k*sizeof(int));
		if(P==NULL){
			printf("Failed to allocate P\n");	
		}
		S = malloc(k*sizeof(int));
		if(S==NULL){
			printf("Failed to allocate S\n");	
		}
		start = clock();
		psMin(B, P, S, k);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("    %d    %fs \n", k, cpu_time_used);
		free(B);
		free(P);
		free(S);
	}
*/	
	free(A);

    	return 0;
}
void generateArrays(){
	int i;	
	for(i=16; i<=48; i++){
		int *A;
		A = malloc(i*sizeof(int));
		randArray(A, i);
		write_Array(A,i);
		free(A);
	}
}
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
void randArray(int *A, int n){
	if(A==NULL){
		printf("Failed to allocate RandArray\n");	
	}
	int i;	
	for(i=0; i<n; i++){
		A[i] = rand() % 256;
	}
}
void psMin(int *A, int *P, int *S, int n){
	int i;
	for(i=0; i<n; i++){		
		P[i] = minArray(A, i+1, 0);
		S[i] = minArray(A, n, i);
	}
}

int minArray(int *A, int n, int i){
	int j;
	int min = INT_MAX;	
	if((n-i) == 1){
		min = A[i];
	}
	for(j=i; j<n; j++){
		if(A[j] < min){
			min = A[j];
		}
	}
	return min;
}
int outputCheck(int *P, int *S, char* pfile, char* sfile, int n){

	int *P_ans = malloc(n*sizeof(int));
	int *S_ans = malloc(n*sizeof(int));

	int correct = 0;

	read_input(P_ans, n, pfile);
	read_input(S_ans, n, sfile);
	
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
int read_input(int* A, int n, char* file) {

	FILE *input;
	char buf[20];

	input =fopen(file,"r");

	if (!input){
		#ifdef DEBUG	
		printf("Error File Does not Exist. \n");
		#endif
		return 1;
	}

	if(fgets(buf,20, input)!=NULL){
		#ifdef DEBUG	
		printf("%s",buf);
		#endif	
	}
	else{
		#ifdef DEBUG	
		printf("First Line could not be read \n");
		#endif
		return 1;	
	}
	int i;
	for(i = 0; i<n; i++){
		if(fgets(buf,20, input)!=NULL){
			A[i] = atoi(buf);
			A[i] = A[i] - atoi(",");
			#ifdef DEBUG	
			printf("%d \n",A[i]);
			#endif	
		}
	}

	fclose(input);
	return 0;
}
int write_output(int* P, int* S, int n){

	FILE *output;
	output = fopen("results.txt", "w");
	if(output==NULL){
		#ifdef DEBUG	
		printf("Failed to create the Output File \n");
		#endif
		return 1;
	}
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
	fclose(output);

	return 0;
}
