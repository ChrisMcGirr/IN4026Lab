CC=gcc
EXE=./seq

compile: seq.c
	$(CC) seq.c -o seq
run:
	$(EXE) input.txt 32 P_ans.txt S_ans.txt	
batch:
	@echo "Starting Batch Run of Varying Input Sizes 16-48"	
	@echo "" > batch_results.txt
	@for n in $(shell seq 16 48); do\
		$(EXE) Input_Data/input_$$n.txt $$n 0 0 >> batch_results.txt; \
	done
	@echo "Finised Batch Run. Check batch_results.txt"

.PHONY: clean

clean:
	rm results.txt seq
