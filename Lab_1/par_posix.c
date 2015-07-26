#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>
#include <errno.h>

//#define DEBUG

#define MAX_THREADS 8

int read_input(int* A, int n, char* file);
int write_output(int* P, int* S, int n);
int outputCheck(int *P, int *S, char* pfile, char* sfile, int n);
void* psMin(void* args);
int minArray(int *A, int n);
void minima(int *A, int n);

typedef struct data {
	int *A;
	int *P;
	int *S;
	int n; //Size of the arrays
	int start; //iteration to j<=n
	int end;
	int id;
} data;

int main(int argc, char **argv)
{
	double start, end;
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
	for(j=0; j<5000; j++){
		start = omp_get_wtime();
		pthread_t thread_id[MAX_THREADS];
		int k;
		data thread_args[MAX_THREADS];

		for(k=0; k<MAX_THREADS; k++){
			thread_args[k].A = A;
			thread_args[k].P = P;
			thread_args[k].S = S;
			thread_args[k].n = n;
			thread_args[k].start = k*(n/MAX_THREADS);
			thread_args[k].end = (k+1)*(n/MAX_THREADS);
			thread_args[k].id = k;
		}
		
		for(k=0; k<MAX_THREADS-1; k++){
			pthread_create(&thread_id[k], NULL, &psMin, &thread_args[k]);
		}

		psMin(&thread_args[MAX_THREADS-1]);
		
		for(k=0; k<MAX_THREADS-1; k++){
			pthread_join(thread_id[k], NULL);
		}
		end = omp_get_wtime();
		cpu_time_used = end-start;
		if(j==0) average = cpu_time_used;
		else	 average = (average+cpu_time_used)/2;
	}	
	//End of Algorithm
	printf("%d 	%f	s \n",n,average);

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

    	return 0;
}

void* psMin(void* args){
	int i;
	int *A, *P, *S, n, end, start, id;
	data *input = (data*)args;
	A = input->A;
	P = input->P;
	S = input->S;
	n = input->n;
	start = input->start;
	end = input->end;
	id = input->id;

	//printf("THREAD ID %d \n", id);

	for(i=start; i<end; i++){
		int *B = malloc(n*sizeof(int));		
		memcpy(B, A, n*sizeof(int));
		P[i] = minArray(B, i+1);
		B[i] = A[i];
		S[i] = minArray(&B[i], n-i);
		free(B);	
	}
	return NULL;
}

int minArray(int *A, int n){
	int j, min, m;
	min = INT_MAX;
	m = ceil(log2(n));

	if(n==1){
		return A[0];
	}
	for(j=1;j<=m;j++){
		if(n%2){
			min = A[n-1];
		}
		n = n>>1;		
		minima(A,n);
		if(A[0]>min) A[0] = min;		
	}

	if( min > A[0]) min = A[0];
	return min;
}
void minima(int *A, int n){
	int p,l;
	for(l=0; l<n; l++){
		p = 2*l;
		if(A[p]>A[p+1]) A[l] = A[p+1];
		else A[l] = A[p];
	}

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
	output = fopen("results/results_posix.txt", "w");
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
