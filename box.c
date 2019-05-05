#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <mpi.h>
#include <memory.h>
#include <time.h>
#include "libbmp.h"

int red[2304][4096];
int green[2304][4096];
int blue[2304][4096];
int red2[2304][4096];
int green2[2304][4096];
int blue2[2304][4096];

void boxBlur1(bmp_img *src) {
	int width = src->img_header.biWidth;
	int height = src->img_header.biHeight;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			int sumr = 0;
			int sumg = 0;
			int sumb = 0;
			for (int i = h - 1; i <= h + 1; i++) {
				for (int j = w - 1; j <= w + 1; j++) {
					if (i >= 0 && j >= 0 && i < height && j < width) {
						sumr += src->img_pixels[i][j].red;
						sumg += src->img_pixels[i][j].green;
						sumb += src->img_pixels[i][j].blue;
					}
				}
			}
			red[h][w] = sumr;
			green[h][w] = sumg;
			blue[h][w] = sumb;
		}
	}
}
void boxBlur2() {
	int width = 4096;
	int height = 2304;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			int sumr = 0;
			int sumg = 0;
			int sumb = 0;
			for (int i = h - 1; i <= h + 1; i++) {
				for (int j = w - 1; j <= w + 1; j++) {
					if (i >= 0 && j >= 0 && i < height && j < width) {
						sumr += red[i][j];
						sumg += green[i][j];
						sumb += blue[i][j];
					}
				}
			}
			red2[h][w] = sumr;
			green2[h][w] = sumg;
			blue2[h][w] = sumb;
		}
	}
}
void boxBlur3(bmp_img *tgt) {
	int width = 4096;
	int height = 2304;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			int sumr = 0;
			int sumg = 0;
			int sumb = 0;
			for (int i = h - 1; i <= h + 1; i++) {
				for (int j = w - 1; j <= w + 1; j++) {
					if (i >= 0 && j >= 0 && i < height && j < width) {
						sumr += red2[i][j];
						sumg += green2[i][j];
						sumb += blue2[i][j];
					}
				}
			}
			tgt->img_pixels[h][w].red = round((double)sumr / 729);
			tgt->img_pixels[h][w].green = round((double)sumg / 729);
			tgt->img_pixels[h][w].blue = round((double)sumb / 729);
		}
	}
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
	boxBlur1(&bi);
	boxBlur2();
	boxBlur3(&bi);
	err = bmp_img_write(&bi, argv[2]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Write Error!\n");
		return -1;
	}
	bmp_img_free(&bi); 
	return 0;
}

