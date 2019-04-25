#!/bin/bash
mpiexec -n 1 ./_mpi test.bmp output1.bmp
mpiexec -n 2 ./_mpi test.bmp output2.bmp
mpiexec -n 3 ./_mpi test.bmp output3.bmp
mpiexec -n 4 ./_mpi test.bmp output4.bmp
mpiexec -n 6 ./_mpi test.bmp output6.bmp
mpiexec -n 8 ./_mpi test.bmp output8.bmp
diff -s output1.bmp output2.bmp
diff -s output1.bmp output3.bmp
diff -s output1.bmp output4.bmp
diff -s output1.bmp output6.bmp
diff -s output1.bmp output8.bmp
