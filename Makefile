all : _rawconvmultiio _rawconvscatter _oldfastgauss _mmap _newvalid _mmaptable _newfastgauss _newfastgauss_motion

.PHONY : clean

clean :
	$(RM) -f _*

_rawconvmultiio : rawconvmultiio.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_rawconvscatter : rawconvscatter.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_oldfastgauss : oldfastgauss.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_mmap : mmap.c
	mpicc -o$@ -O2 $(filter-out %.h, $^)

_mmaptable : mmaptable.c
	mpicc -o$@ -g $(filter-out %.h, $^)

_newfastgauss : newfastgauss.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

_newvalid : newvalid.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs opencv`

_newfastgauss_motion : newfastgauss_motion.c libbmp.c libbmp.h
	mpicc -o$@ -O2 $(filter-out %.h, $^) -lm

