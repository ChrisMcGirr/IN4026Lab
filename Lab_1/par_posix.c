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
void* minArray(void* input);
int outputCheck(int *P, int *S, char* pfile, char* sfile, int n);
void psMin(int *A, int *P, int *S, int n);
void minima(int *A, int n);

typedef struct data {
	int *A;
	int n;
	int id;
	int p;
	int *out;
	pthread_cond_t *done;
	int *requests;
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
	for(j=0; j<100; j++){
		start = omp_get_wtime();
		psMin(A, P, S, n);
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

void psMin(int *A, int *P, int *S, int n){
	int i;
	int p = 8;
	int thr_id;
	int rc;
	int *status;
	int requests;
	pthread_t p_thread;
	pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t done = PTHREAD_COND_INITIALIZER;
	int *B = malloc(n*sizeof(int));	
	
	for(i=0; i<n; i++){
			
		status = memcpy(B, A, n*sizeof(int));
		if(!status){
			printf("MEMCPY Error \n");
		}

		data *pout = malloc(sizeof(data));
		if(pout==NULL){
			printf("Pout Malloc Fail\n");		
		}
		pout->A = B;
		pout->n =  i+1;
		pout->id = 1;
		pout->p = MAX_THREADS;
		pout->out = &P[i];
		pout->done = &done;
		pout->requests = &requests; 
		
		thr_id = pthread_create(&p_thread, NULL, minArray, pout);
		
		if(thr_id == EAGAIN) printf("NOT ENOUGH RESOURCES AVAILABLE FOR THREAD CREATE\n");
		if(thr_id == EINVAL) printf("VALUE GIVE BY ATTR IS INVALID\n");
		if(thr_id == EPERM) printf("CALLER DOES NOT HAVE PERMISSION TO CREATE THREADS\n");	

		data *sout = malloc(sizeof(data));
		if(sout==NULL){
			printf("Sout Malloc Fail\n");		
		}
		sout->A = B;
		sout->n =  n-i;
		sout->id = 0;
		sout->p = MAX_THREADS;
		sout->out = &S[i];
 		sout->done = NULL;
		pout->requests = &requests;

		minArray(sout);

		rc = pthread_mutex_lock(&a_mutex);
		if (rc) { /* an error has occurred */
			printf("pthread_mutex_lock\n");
			pthread_exit(NULL);
		}
		if(requests==0){
			rc = pthread_cond_wait(&done, &a_mutex);
		}
		pthread_mutex_unlock(&a_mutex);
		
	}
	rc = pthread_cond_destroy(&done);
	if (rc) { /* an error has occurred */
			printf("PTHEARD COND DESTROY ERROR\n");

	}
	rc = pthread_mutex_destroy(&a_mutex);
	if (rc) { /* an error has occurred */
			printf("PTHEARD MUTEX DESTROY ERROR\n");

	}
	free(B);

}
//int *A, int n, int id, int p, int *out
void* minArray(void* input){
	int j, min, m;
	pthread_cond_t *done;
	int *A, n, id, p,  *out, *requests;
	data *in = (data*)input;

	A = in->A;
	n = in->n;
	id = in->id;
	p = in->p;
	out = in->out;
	done = in->done;
	requests = in->requests;
	
	min = INT_MAX;
	m = ceil(log2(n));

	if(n==1){
		*out = A[0];
		if(done) pthread_exit(NULL);
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
	*out = min;
	free(input);
	if(done){
		int rc = pthread_cond_signal(done);
		*requests += 1;
		pthread_exit(NULL);
	}
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
