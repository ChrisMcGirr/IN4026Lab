/******************************************************************
*	IN4026 Lab A: Prefix and Suffix Minima
*	Author: Christopher McGirr
*	Student # 4415302
*
*	File: fileIO.h
*	Description: contains all the functions needed for file
*	input and output like reading the arrays and writing the 
*	arrays to file. Also includes a file check function to see
*	if the results of the algorithm matchs the answer given in
*	a text file. 
*
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int read_input(int* A, int n, char* file);
int outputCheck(int *C,char* cfile, int n);
int write_output(int *S, int *R, int n, char *name);
int write_Array(int* A, int n);
void generateInputs(int *A, int n);
void generateArrays();
