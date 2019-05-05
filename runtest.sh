#!/bin/bash -x
if [ "x$1x" == "xx" ]; then
	echo "Usage: $0 <executable_file_without_underline>"
	exit 1;
fi

time mpiexec -n 4 ./_$1 timg.bmp output_$1.bmp
./newvalid timg.bmp output_$1.bmp

