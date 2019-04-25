mpiexec -n 1 vs\MPI\x64\Debug\MPI.exe test.bmp output1.bmp
mpiexec -n 2 vs\MPI\x64\Debug\MPI.exe test.bmp output2.bmp
mpiexec -n 3 vs\MPI\x64\Debug\MPI.exe test.bmp output3.bmp
mpiexec -n 4 vs\MPI\x64\Debug\MPI.exe test.bmp output4.bmp
mpiexec -n 8 vs\MPI\x64\Debug\MPI.exe test.bmp output8.bmp
comp output1.bmp output2.bmp /M
comp output1.bmp output3.bmp /M
comp output1.bmp output4.bmp /M
comp output1.bmp output8.bmp /M
