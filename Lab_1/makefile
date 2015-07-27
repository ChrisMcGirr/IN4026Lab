CC=gcc
SEQ = seq
OMP = par_omp
POS = par_posix

seq: seq.c
	$(CC) $(SEQ).c fileIO.c fileIO.h -o $(SEQ) -lm
	./$(SEQ) input.txt 32 P_ans.txt S_ans.txt

omp: par_omp.c
	$(CC) $(OMP).c fileIO.c fileIO.h -o $(OMP) -fopenmp -lm
	./$(OMP) input.txt 32 P_ans.txt S_ans.txt

posix: par_posix.c
	$(CC) $(POS).c fileIO.c fileIO.h -o $(POS) -lpthread -fopenmp -lm -g
	./$(POS) input.txt 32 P_ans.txt S_ans.txt
	
batch:
	@echo "Starting Sequential Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > results/batch_results_seq.txt
	@echo "Input N	 Execution Time	 Unit" > results/batch_results_omp.txt
	@for n in $(shell seq 16 48); do\
		./$(SEQ) Input_Data/input_$$n.txt $$n 1 1 >> results/batch_results_seq.txt; \
	done
	@echo "Finised Sequential Batch Run. Check RESULTS"
	@echo "Starting OMP Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > results/batch_results_omp.txt
	@echo "Input N	 Execution Time	 Unit" > results/batch_results_omp.txt
	@for n in $(shell seq 16 48); do\
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 >> results/batch_results_omp.txt; \
	done
	@echo "Finised OMP Batch Run. Check RESULTS"
	@echo "Starting OMP Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > results/batch_results_posix.txt
	@echo "Input N	 Execution Time	 Unit" > results/batch_results_posix.txt
	@for n in $(shell seq 16 48); do\
		./$(POS) Input_Data/input_$$n.txt $$n 1 1 >> results/batch_results_posix.txt; \
	done
	@echo "Finised POSIX Batch Run. Check RESULTS"

.PHONY: clean

clean:
	rm results/results.txt results/results_omp.txt results/batch_results_seq.txt results/batch_results_omp.txt $(SEQ) $(OMP) $(POS) results/batch_results_posix.txt results/results_posix.txt
