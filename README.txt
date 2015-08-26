/*******************************************************************************
IN4026 Lab Assignments A,B,C
Name: Christopher McGirr
Student ID: 4415302
Email: c.a.mcgirr-1@student.tudelft.nl

-------------------
Directory Stucture:
-------------------

Each folder, Lab_1 (A), Lab_2 (B), Lab_3 (C) contains three main files.

	The seq.c	Source code of sequential version
	The par_omp.c	Source code of OMP version
	The par_posix	Source code of PThread version

The tests are done from the Makefile where the main commands are

	make seq	Compiles and runs single test case for correctness
			Output of the program is given in the output/seq folder

	make omp	Compiles and runs single test case for correctness
			Output of the program is given in the output/omp folder

	make posix	Compiles and runs single test case for correctness
			Output of the program is given in the output/posix folder

	make batch	Runs all the versions with large input size
			of N. Number of threads and Number are runs
			are passed to the application through the
			command line and given as a variable in the
			makefile

	The results of the batch command are given in the results 
	folder where X is the number of threads used, e.g. the 
	batch_results_*_X.txt. For example, batch_results_omp_8.txt
	are the execution times for varying input sizes of 4096 to 
	262144 with thread number set to 8.

Note:

I am sorry, but I did not follow the template code that was given on
blackboard. So the code may be a little harder to follow, but it should
be readable. If there are any issues please let me know.

******************************************************************************/
