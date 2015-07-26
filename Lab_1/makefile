CC=gcc
SEQ = seq
OMP = par_omp
POS = par_posix

seq: seq.c
	$(CC) $(SEQ).c -o $(SEQ) -lm
	./$(SEQ) input.txt 32 P_ans.txt S_ans.txt

omp: par_omp.c
	$(CC) $(OMP).c -o $(OMP) -fopenmp -lm
	./$(OMP) input.txt 32 P_ans.txt S_ans.txt

posix: par_posix.c
	$(CC) $(POS).c -o $(POS) -lpthread -fopenmp -lm -g

	./$(POS) input.txt 32 P_ans.txt S_ans.txt
	
batch:
	@echo "Starting Sequential Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > results/batch_results_seq.txt
	@echo "Input N	 Execution Time	 Unit" > results/batch_results_omp.txt
	@for n in $(shell seq 16 48); do\
		./$(SEQ) Input_Data/input_$$n.txt $$n 1 1 >> results/batch_results_seq.txt; \
	done
	@echo "Finised Batch Run. Check batch_results.txt"
	@echo "Starting OMP Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT:" > results/batch_results_omp.txt
	@echo "Input N	 Execution Time	 Unit" > results/batch_results_omp.txt
	@for n in $(shell seq 16 48); do\
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 >> results/batch_results_omp.txt; \
	done
	@echo "Finised Batch Run. Check batch_results.txt"

.PHONY: clean

clean:
	rm results/results.txt results/results_omp.txt results/batch_results_seq.txt results/batch_results_omp.txt $(SEQ) $(OMP) 
