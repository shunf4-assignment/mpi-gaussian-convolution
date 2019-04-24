#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"
double ConvKernel[5][5] = {
	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881},
	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
	{0.0350727, 0.06831229, 0.08531173, 0.06831229, 0.0350727},
	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881}
};
char newRed[2304][4096];
char newGreen[2304][4096];
char newBlue[2304][4096];

int main(int argc, const char *const *argv) {
	if (argc != 3) {
		printf("Usage: %s <input_bmp_file> <output_bmp_file\n", argv[0]);
		return 0;
	}
	bmp_img bi;
	enum bmp_error err;
	err = bmp_img_read(&bi, argv[1]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Read Error!\n");
		return -1;
	}

	printf("Width: %d, Height: %d\n", bi.img_header.biWidth, bi.img_header.biHeight);
	
	for (int y = 0; y < bi.img_header.biHeight; y++) {
		for (int x = 0; x < bi.img_header.biWidth; x++) {
			double red = 0;
			double green = 0;
			double blue = 0;
			for (int i = -2; i <= 2; ++i) {
				for (int j = -2; j <= 2; ++j) {
					if (y + i >= 0 && y + i < bi.img_header.biHeight && x + j >= 0 && x + j < bi.img_header.biWidth) {
						red += bi.img_pixels[y + i][x + j].red * ConvKernel[i + 2][j + 2];
						green += bi.img_pixels[y + i][x + j].green * ConvKernel[i + 2][j + 2];
						blue += bi.img_pixels[y + i][x + j].blue * ConvKernel[i + 2][j + 2];
					}					
				}
			}
			newRed[y][x] = (char)red;
			newGreen[y][x] = (char)green;
			newBlue[y][x] = (char)blue;
		}

	}
	for (int y = 0; y < bi.img_header.biHeight; y++) {
		for (int x = 0; x < bi.img_header.biWidth; x++) {
			bi.img_pixels[y][x].red = newRed[y][x];
			bi.img_pixels[y][x].green = newGreen[y][x];
			bi.img_pixels[y][x].blue = newBlue[y][x];
		}
	}
	err = bmp_img_write(&bi, argv[2]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Read Error!\n");
		return -1;
	}
	bmp_img_free(&bi);
	return 0;
}

