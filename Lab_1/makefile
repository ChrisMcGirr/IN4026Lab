CC=gcc
SEQ = seq
OMP = par_omp
POS = par_posix
RESSEQ = results/batch_results_seq.txt
RESOMP = results/batch_results_omp.txt
RESPOS = results/batch_results_posix.txt
MAXTHREADS = 8
RUNS = 10000

seq: seq.c
	$(CC) $(SEQ).c fileIO.c fileIO.h -o $(SEQ) -lm -fopenmp
	./$(SEQ) input.txt 32 P_ans.txt S_ans.txt $(RUNS) 

omp: par_omp.c
	$(CC) $(OMP).c fileIO.c fileIO.h -o $(OMP) -fopenmp -lm
	./$(OMP) input.txt 32 P_ans.txt S_ans.txt $(RUNS) $(MAXTHREADS)

posix: par_posix.c
	$(CC) $(POS).c fileIO.c fileIO.h -o $(POS) -lpthread -fopenmp -lm -g
	./$(POS) input.txt 32 P_ans.txt S_ans.txt $(RUNS) $(MAXTHREADS)
	
batch:
	@echo "Starting Sequential Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT for Sequential Batch:" > $(RESSEQ)
	@echo "Input N	 Execution Time	 Unit" > $(RESSEQ)
	@for n in $(shell seq 16 48); do\
		./$(SEQ) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) >> $(RESSEQ); \
	done
	@echo "Finised Sequential Batch Run. Check RESULTS"
	@echo "Starting OMP Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT OMP Batch:" > $(RESOMP)
	@echo "Input N	 Execution Time	 Unit" > $(RESOMP)
	@for n in $(shell seq 16 48); do\
		./$(OMP) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) $(MAXTHREADS) >> $(RESOMP); \
	done
	@echo "Finised OMP Batch Run. Check RESULTS"
	@echo "Starting POSIX Batch Run of Varying Input Sizes 16-48"	
	@echo "RESULT POSIX Batch:" > $(RESPOS)
	@echo "Input N	 Execution Time	 Unit" > $(RESPOS)
	@for n in $(shell seq 16 48); do\
		./$(POS) Input_Data/input_$$n.txt $$n 1 1 $(RUNS) $(MAXTHREADS) >> $(RESPOS); \
	done
	@echo "Finised POSIX Batch Run. Check RESULTS"

.PHONY: clean

clean:
	rm results/results.txt results/results_omp.txt results/batch_results_seq.txt results/batch_results_omp.txt $(SEQ) $(OMP) $(POS) results/batch_results_posix.txt results/results_posix.txt
