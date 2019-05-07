# MPI Fast Gaussian Convolution

Gaussian convolution implemented with MPI as a class project.

There are 6 iterations

- `rawconvscatter.c`, the most primitive version. Source image is `MPI_Scatter`'ed by process 0 and target image is `MPI_Gather`'ed.

- `rawconvmultiio.c`, every process will directly read source image file once.

- `newfastgauss.c`, and

- `newfastgauss_motion.c` have enhancements in gaussian convolution algorithm. However, for the specific input image `timg.bmp`, these implementations' advantage is not explicit.

- `mmap.c`, input and output files are mapped into memory.

- `mmaptable.c`, input and output files are mapped into memory; Multiplications are substituted by looking up a table.

## Prerequisites

- OpenMPI

- OpenCV

## Test (use `timg.bmp` as input image)

```
make
./runtest.sh <executable_file_without_prefix_underline>
```
