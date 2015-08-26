#*********************************************************
#IN4026 Lab A: Prefix and Suffix Minima
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
RESSEQ = results/batch_results_seq
RESOMP = results/batch_results_omp
RESPOS = results/batch_results_posix
MAXTHREADS = 8
RUNS = 1

all: seq omp posix batch

#compiles and runs the sequential version with n=32
seq: seq.c fileIO.c
	$(CC) $(SEQ).c fileIO.c fileIO.h -o $(SEQ) -lm -fopenmp
	./$(SEQ) input.txt 32 P_ans.txt S_ans.txt $(RUNS) 

#compiles and runs the OMP version with n=32
omp: par_omp.c fileIO.c
	$(CC) $(OMP).c fileIO.c fileIO.h -o $(OMP) -fopenmp -lm
	./$(OMP) input.txt 32 P_ans.txt S_ans.txt $(RUNS) $(MAXTHREADS)

#compiles and runs the Pthread version with n=32
posix: par_posix.c fileIO.c
	$(CC) $(POS).c fileIO.c fileIO.h -o $(POS) -lpthread -fopenmp -lm -g
	./$(POS) input.txt 32 P_ans.txt S_ans.txt $(RUNS) $(MAXTHREADS)

#Runs all the versions in a batch format running through N=4096 to 262144	
batch:
	@echo "Starting Sequential Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT for Sequential Batch:" > $(RESSEQ)_1.txt
	@echo "Input N	 Execution Time	 Unit" > $(RESSEQ)_1.txt
	@for n in 4096 8192 16384 32768 65536 131072 262144; do\
		./$(SEQ) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) >> $(RESSEQ)_1.txt; \
	done
	@echo "Finised Sequential Batch Run. Check RESULTS"	
	@echo "Starting OMP Batch Run of Varying Input Sizes 16-48"	
	@for n in 1 2 4 8; do\
		echo "RESULT OMP Batch for" $$n > $(RESOMP)_$$n.txt ; \
		echo "Input N	 Execution Time	 Unit" > $(RESOMP)_$$n.txt ; \
	done
	@for n in 4096 8192 16384 32768 65536 131072 262144; do\
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 1 >> $(RESOMP)_1.txt; \
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 2 >> $(RESOMP)_2.txt; \
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 4 >> $(RESOMP)_4.txt; \
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 8 >> $(RESOMP)_8.txt; \
		#./$(OMP) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 16 >> $(RESOMP)_16.txt; \
	done
	@echo "Finised OMP Batch Run. Check RESULTS"	
	@echo "Starting POSIX Batch Run of Varying Input Sizes 16-48"	
	@for n in 1 2 4 8; do\
		echo "RESULT POSIX Batch for" $$n > $(RESPOS)_$$n.txt ; \
		echo "Input N	 Execution Time	 Unit" > $(RESPOS)_$$n.txt ; \
	done
	@for n in 4096 8192 16384 32768 65536 131072 262144; do\
		./$(POS) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 1 >> $(RESPOS)_1.txt; \
		./$(POS) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 2 >> $(RESPOS)_2.txt; \
		./$(POS) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 4 >> $(RESPOS)_4.txt; \
		./$(POS) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 8 >> $(RESPOS)_8.txt; \
		#./$(POS) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) 16 >> $(RESPOS)_16.txt; \
	done
	@echo "Finised POSIX Batch Run. Check RESULTS"

.PHONY: clean

#Cleans any results generate or binaries created
clean:
	rm results/* $(SEQ) $(OMP) $(POS) output/*
