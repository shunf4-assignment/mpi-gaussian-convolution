#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"

int main(int argc, const char * const*argv) {
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
	
	for (size_t y = 0; y < bi.img_header.biHeight; y++) {
		for (size_t x = 0; x < bi.img_header.biWidth; x++) {
			bi.img_pixels[y][x].red = 255 - bi.img_pixels[y][x].red;
			bi.img_pixels[y][x].green = 255 - bi.img_pixels[y][x].green;
			bi.img_pixels[y][x].blue = 255 - bi.img_pixels[y][x].blue;
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

