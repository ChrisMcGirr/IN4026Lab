#include "fileIO.h"

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
