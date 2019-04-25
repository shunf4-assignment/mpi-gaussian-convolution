_mpi : mpi.c libbmp.c libbmp.h
	mpicc -o$@ $(filter-out %.h, $^)

