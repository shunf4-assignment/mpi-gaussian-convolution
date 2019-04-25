_mpi : mpi.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

