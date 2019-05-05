all : _mpi _gauss _mmap _newvalid _newgauss

_mpi : mpi.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_oldfastgauss : fastGaussBlur.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_mmap : mmap.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_newgauss : newgauss.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_newfastgauss : box.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_newvalid : newvalid.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs opencv`

