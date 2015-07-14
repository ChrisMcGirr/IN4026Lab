CC=gcc
SEQ = seq
OMP = par_openMP

seq: seq.c
	$(CC) $(SEQ).c -o $(SEQ)
	./$(SEQ) input.txt 32 P_ans.txt S_ans.txt

openmp: par_openMP.c
	$(CC) $(OMP).c -o $(OMP) -fopenmp
	./$(OMP) input.txt 32 P_ans.txt S_ans.txt
	
batch:
	@echo "Starting Sequential Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > batch_results_seq.txt
	@for n in $(shell seq 16 48); do\
		./$(SEQ) Input_Data/input_$$n.txt $$n 1 1 >> batch_results_seq.txt; \
	done
	@echo "Finised Batch Run. Check batch_results.txt"
	@echo "Starting OMP Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > batch_results_omp.txt
	@for n in $(shell seq 16 48); do\
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 >> batch_results_omp.txt; \
	done
	@echo "Finised Batch Run. Check batch_results.txt"

.PHONY: clean

clean:
	rm results.txt results_omp.txt batch_results_seq.txt batch_results_omp.txt $(SEQ) $(OMP) 