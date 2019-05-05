all : _mpi _oldfastgauss _mmap _newvalid _newgauss _newfastgauss

.PHONY : clean

clean :
	$(RM) -f _*

_mpi : mpi.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_oldfastgauss : fastGaussBlur.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_mmap : mmap.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_newgauss : newgauss.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_newfastgauss : box.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_newvalid : newvalid.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs opencv`

