_test : test.c libbmp.c libbmp.h
	$(CC) -o$@ $(filter-out %.h, $^)

