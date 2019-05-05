#!/bin/bash -x

time mpiexec -n 4 ./_$1 timg.bmp output_$1.bmp
./newvalid timg.bmp output_$1.bmp

