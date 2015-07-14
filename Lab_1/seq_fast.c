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
	for(j=0; j<20000; j++){
		start = clock();
		psMin(A, P, S, n);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		if(j==0) average = cpu_time_used;
		else	 average = (average+cpu_time_used)/2;
	}	
	//End of Algorithm

	printf("Average Computation Time %fs\n",average);

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

	free(P);
	free(S);
	free(A);

    	return 0;
}

void psMin(int *A, int *P, int *S, int n){
	int p_min = INT_MAX;
	int i;
	for(i=0; i<n; i++){		
		if(A[i] < p_min){ //Seg fault occurs here?
			p_min = A[i];		
		}
		P[i] = p_min;
		S[i] = minArray(A, n, i);
	}
}

int minArray(int *A, int n, int i){
	int j;	
	int min = INT_MAX;
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
		printf("%s \n",buf);
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
