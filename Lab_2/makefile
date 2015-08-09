#*********************************************************
#IN4026 Lab B: Simple Merge
#Author: Christopher McGirr
#Student # 4415302
#
#This makefile contains all the commands to compile and run the various
#versions of the application. 
#
#	-make batch will run all the versions using N 1 to 48 and save the execution times to
#	to the folder "results/" under the file names given in the variable names below
#	-make all will run all the commands yielding the output in the "results/" folder
#*********************************************************** 

#Variables used
CC=gcc
SEQ = seq
OMP = par_omp
POS = par_posix
RESSEQ = results/batch_results_seq.txt
RESOMP = results/batch_results_omp.txt
RESPOS = results/batch_results_posix.txt
MAXTHREADS = 8
RUNS = 1000

all: seq omp posix batch

#compiles and runs the sequential version with n=32
seq: seq.c fileIO.c fileIO.h makefile
	$(CC) $(SEQ).c fileIO.c fileIO.h -o $(SEQ) -lm -fopenmp
	./$(SEQ) input_A.txt input_B.txt 8 32 C_ans.txt $(RUNS) 

#compiles and runs the OMP version with n=32
omp: par_omp.c
	$(CC) $(OMP).c fileIO.c fileIO.h -o $(OMP) -fopenmp -lm
	./$(OMP) input_A.txt input_B.txt 8 32 C_ans.txt $(RUNS) $(MAXTHREADS)

#compiles and runs the Pthread version with n=32
posix: par_posix.c
	$(CC) $(POS).c fileIO.c fileIO.h -o $(POS) -lpthread -lm -g
	./$(POS) input_A.txt input_B.txt 8 32 C_ans.txt $(RUNS) $(MAXTHREADS)

#Runs all the versions in a batch format running through N=1 to 48	
batch:
	@echo "Starting Sequential Batch Run of Varying Input Sizes 1-32"	
	@echo "RESULT for Sequential Batch:" > $(RESSEQ)
	@echo "Input N	 Execution Time	 Unit" > $(RESSEQ)
	@for n in $(shell seq 1 32); do\
		./$(SEQ) input_data/input_$$n.txt input_B.txt $$n 32 1 $(RUNS) >> $(RESSEQ); \
	done
	@echo "Finised Sequential Batch Run. Check RESULTS"
	@echo "Starting OMP Batch Run of Varying Input Sizes 1-32"	
	@echo "RESULT OMP Batch:" > $(RESOMP)
	@echo "Input N	 Execution Time	 Unit" > $(RESOMP)
	@for n in $(shell seq 1 32); do\
			./$(OMP) input_data/input_$$n.txt input_B.txt $$n 32 1 $(RUNS) $(MAXTHREADS) >> $(RESOMP); \
	done
	@echo "Finised OMP Batch Run. Check RESULTS"
	@echo "Starting POSIX Batch Run of Varying Input Sizes 1-32"	
	@echo "RESULT POSIX Batch:" > $(RESPOS)
	@echo "Input N	 Execution Time	 Unit" > $(RESPOS)
	@for n in $(shell seq 1 32); do\
		./$(POS) input_data/input_$$n.txt input_B.txt $$n 32 1 $(RUNS) $(MAXTHREADS) >> $(RESPOS); \
	done
	@echo "Finised POSIX Batch Run. Check RESULTS"

.PHONY: clean

#Cleans any results generate or binaries created
clean:
	rm results/results.txt results/results_omp.txt results/batch_results_seq.txt results/batch_results_omp.txt $(SEQ) $(OMP) $(POS) results/batch_results_posix.txt results/results_posix.txt o

