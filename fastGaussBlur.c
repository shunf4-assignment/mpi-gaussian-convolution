#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <memory.h>
#include "libbmp.h"

#define min(x,y) x<y?x:y
#define max(x,y) x<y?y:x

void boxBlurH(bmp_img *src, bmp_img *dest, int width, int height) {
	for (int i = 0; i < height; i++) {
		int sumr = src->img_pixels[i][0].red * 3;
		int sumg = src->img_pixels[i][0].green * 3;
		int sumb = src->img_pixels[i][0].blue * 3;
		for (int r = 0; r < width; r++) {
			sumr += src->img_pixels[i][min(r + 1, width - 1)].red - src->img_pixels[i][max(0, r - 2)].red;
			sumg += src->img_pixels[i][min(r + 1, width - 1)].green - src->img_pixels[i][max(0, r - 2)].green;
			sumb += src->img_pixels[i][min(r + 1, width - 1)].blue - src->img_pixels[i][max(0, r - 2)].blue;
			dest->img_pixels[i][r].red = round((double)sumr / 3);
			dest->img_pixels[i][r].green = round((double)sumg / 3);
			dest->img_pixels[i][r].blue = round((double)sumb / 3);
		}
	}
}

void boxBlurT(bmp_img * src, bmp_img * dest, int width, int height) {
	for (int i = 0; i < width; i++) {
		int sumr = src->img_pixels[0][i].red * 3;
		int sumg = src->img_pixels[0][i].green * 3;
		int sumb = src->img_pixels[0][i].blue * 3;
		for (int r = 0; r < height; r++) {
			sumr += src->img_pixels[min(r + 1, height - 1)][i].red - src->img_pixels[max(0, r - 2)][i].red;
			sumg += src->img_pixels[min(r + 1, height - 1)][i].green - src->img_pixels[max(0, r - 2)][i].green;
			sumb += src->img_pixels[min(r + 1, height - 1)][i].blue - src->img_pixels[max(0, r - 2)][i].blue;
			dest->img_pixels[r][i].red = round((double)sumr / 3);
			dest->img_pixels[r][i].green = round((double)sumg / 3);
			dest->img_pixels[r][i].blue = round((double)sumb / 3);
		}
	}
}

void boxBlur(bmp_img * src, bmp_img * dest, int width, int height) {
	boxBlurH(src, dest, width, height);
	boxBlurT(src, dest, width, height);
}

void gaussBlur(bmp_img * src, bmp_img * dest, int width, int height) {
	boxBlur(src, dest, width, height);
	printf("blur1 end\n");
	boxBlur(dest, src, width, height);
	printf("blur2 end\n");
	boxBlur(src, dest, width, height);
	printf("blur2 end\n");
}

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
	bmp_img bi2;
	enum bmp_error err2;
	err2 = bmp_img_read(&bi2, argv[1]);
	if (err2 != BMP_OK) {
		fprintf(stderr, "BMP Read Error!\n");
		return -1;
	}
	printf("read end\n");
	gaussBlur(&bi, &bi2, width, height);
	printf("blur end\n");
	err = bmp_img_write(&bi2, argv[2]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Write Error!\n");
		return -1;
	}
	bmp_img_free(&bi);
	bmp_img_free(&bi2);
	return 0;
}

