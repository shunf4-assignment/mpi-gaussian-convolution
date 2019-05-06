all : _rawconv _rawconvnew _oldfastgauss _mmap _newvalid _mmaptable _newfastgauss _newfastgauss_motion

.PHONY : clean

clean :
	$(RM) -f _*

_rawconv : rawconv.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_rawconvnew : rawconvnew.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_oldfastgauss : oldfastgauss.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_mmap : mmap.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_mmaptable : mmaptable.c
	mpicc -o$@ -O3 $(filter-out %.h, $^)

_newfastgauss : newfastgauss.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_newvalid : newvalid.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs opencv`

_newfastgauss_motion : newfastgauss_motion.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

