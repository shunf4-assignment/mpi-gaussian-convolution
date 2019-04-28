#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "libbmp.h"

double ConvKernel[5][5] = {
	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881},
	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
	{0.0350727, 0.06831229, 0.08531173, 0.06831229, 0.0350727},
	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881}
};

int main(int argc, const char *const *argv) {
	if (argc != 3) {
		printf("Usage: %s <input_bmp_file> <output_bmp_file>\n", argv[0]);
		return 0;
	}
	bmp_img bi;
	enum bmp_error err;
	err = bmp_img_read(&bi, argv[1]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Read Error!\n");
		return -1;
	}
	int width = bi.img_header.biWidth, height = bi.img_header.biHeight;
	int sz;
	int mr;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &mr);
	double startTime = MPI_Wtime();
	if (mr != 0) {
		double stime = MPI_Wtime();
		int start = (height + sz - 1) / sz * mr;
		int end = start + (height + sz - 1) / sz - 1;
		if (end >= height)
			end = height - 1;
		int size = (end - start + 1) * width;
		char *buf = malloc(3 * size * sizeof(char));
		for (int y = start; y <= end; y++) {
			for (int x = 0; x < width; x++) {
				double red = 0;
				double green = 0;
				double blue = 0;
				for (int i = -2; i <= 2; ++i) {
					for (int j = -2; j <= 2; ++j) {
						if (y + i >= 0 && y + i < height && x + j >= 0 && x + j < width) {
							red += bi.img_pixels[y + i][x + j].red * ConvKernel[i + 2][j + 2];
							green += bi.img_pixels[y + i][x + j].green * ConvKernel[i + 2][j + 2];
							blue += bi.img_pixels[y + i][x + j].blue * ConvKernel[i + 2][j + 2];
						}
					}
				}

				buf[(y - start) * width + x] = (char)(red + 0.5);
				buf[(y - start) * width + x + size] = (char)(green + 0.5);
				buf[(y - start) * width + x + size + size] = (char)(blue + 0.5);
			}
		}
		double etime = MPI_Wtime();
		printf("process%d time: %lg\n", mr, etime - stime);
		MPI_Send(buf, 3 * size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	else {
		double stime = MPI_Wtime();
		int start = (height + sz - 1) / sz * mr;
		int end = start + (height + sz - 1) / sz - 1;
		if (end >= height)
			end = height - 1;
		int size = (end - start + 1) * width;
		char *buf = malloc(3 * size * sizeof(char));
		for (int y = start; y <= end; y++) {
			for (int x = 0; x < width; x++) {
				double red = 0;
				double green = 0;
				double blue = 0;
				for (int i = -2; i <= 2; ++i) {
					for (int j = -2; j <= 2; ++j) {
						if (y + i >= 0 && y + i < height && x + j >= 0 && x + j < width) {
							red += bi.img_pixels[y + i][x + j].red * ConvKernel[i + 2][j + 2];
							green += bi.img_pixels[y + i][x + j].green * ConvKernel[i + 2][j + 2];
							blue += bi.img_pixels[y + i][x + j].blue * ConvKernel[i + 2][j + 2];
						}
					}
				}
				buf[(y - start) * width + x] = (char)(red + 0.5);
				buf[(y - start) * width + x + size] = (char)(green + 0.5);
				buf[(y - start) * width + x + size + size] = (char)(blue + 0.5);
			}
		}
		double etime = MPI_Wtime();
		printf("process0 time: %lg\n", etime - stime);
		for (int y = start; y <= end; y++) {
			for (int x = 0; x < width; x++) {
				bi.img_pixels[y][x].red = buf[(y - start) * width + x];
				bi.img_pixels[y][x].green = buf[(y - start) * width + x + size];
				bi.img_pixels[y][x].blue = buf[(y - start) * width + x + size + size];
			}
		}
		for (int i = 1; i < sz; i++) {
			int start = (height + sz - 1) / sz * i;
			int end = start + (height + sz - 1) / sz - 1;
			if (end >= height)
				end = height - 1;
			int size = (end - start + 1) * width;
			MPI_Recv(buf, 3*size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int y = start; y <= end; y++) {
				for (int x = 0; x < width; x++) {
					bi.img_pixels[y][x].red = buf[(y - start) * width + x];
					bi.img_pixels[y][x].green = buf[(y - start) * width + x + size];
					bi.img_pixels[y][x].blue = buf[(y - start) * width + x + size + size];
				}
			}
		}
		double endTime = MPI_Wtime();
		printf("total time: %lg\n", endTime - startTime);

		err = bmp_img_write(&bi, argv[2]);
		if (err != BMP_OK) {
			fprintf(stderr, "BMP Write Error!\n");
			return -1;
		}
	}
	
	bmp_img_free(&bi);
	MPI_Finalize();
	return 0;
}

