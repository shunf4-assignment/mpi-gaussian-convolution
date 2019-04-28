all : _mpi _gauss _mmap

_mpi : mpi.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_gauss : fastGaussBlur.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_mmap : mmap.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)