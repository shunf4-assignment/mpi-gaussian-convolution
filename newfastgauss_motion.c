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
void motionBlurH1(bmp_img *src) {
	int width = 4096;
	int height = 2304;
	for (int h = 0; h < height; h++) {
		int sumr = src->img_pixels[h][0].red + src->img_pixels[h][1].red;
		int sumg = src->img_pixels[h][0].green + src->img_pixels[h][1].green;
		int sumb = src->img_pixels[h][0].blue + src->img_pixels[h][1].blue;
		red[h][0] = sumr;
		green[h][0] = sumg;
		blue[h][0] = sumb;
		sumr += src->img_pixels[h][2].red;
		sumg += src->img_pixels[h][2].green;
		sumb += src->img_pixels[h][2].blue;
		int w = 1;
		for (; w < width - 2; w++) {
			red[h][w] = sumr;
			green[h][w] = sumg;
			blue[h][w] = sumb;
			sumr += src->img_pixels[h][w + 2].red - src->img_pixels[h][w - 1].red;
			sumg += src->img_pixels[h][w + 2].green - src->img_pixels[h][w - 1].green;
			sumb += src->img_pixels[h][w + 2].blue - src->img_pixels[h][w - 1].blue;
		}
		red[h][w] = sumr;
		green[h][w] = sumg;
		blue[h][w] = sumb;
		sumr -= src->img_pixels[h][w - 1].red;
		sumg -= src->img_pixels[h][w - 1].green;
		sumb -= src->img_pixels[h][w - 1].blue;
		red[h][w + 1] = sumr;
		green[h][w + 1] = sumg;
		blue[h][w + 1] = sumb;
	}
}
void motionBlurT12() {
	int width = 4096;
	int height = 2304;
	for (int w = 0; w < width; w++) {
		int sumr = red[0][w] + red[1][w];
		int sumg = green[0][w] + green[1][w];
		int sumb = blue[0][w] + blue[1][w];
		red2[0][w] = sumr;
		green2[0][w] = sumg;
		blue2[0][w] = sumb;
		sumr += red[2][w];
		sumg += green[2][w];
		sumb += blue[2][w];
		int h = 1;
		for (; h < height - 2; h++) {
			red2[h][w] = sumr;
			green2[h][w] = sumg;
			blue2[h][w] = sumb;
			sumr += red[h + 2][w] - red[h - 1][w];
			sumg += green[h + 2][w] - green[h - 1][w];
			sumb += blue[h + 2][w] - blue[h - 1][w];
		}
		red2[h][w] = sumr;
		green2[h][w] = sumg;
		blue2[h][w] = sumb;
		sumr -= red[h - 1][w];
		sumg -= green[h - 1][w];
		sumb -= blue[h - 1][w];
		red2[h+1][w] = sumr;
		green2[h+1][w] = sumg;
		blue2[h+1][w] = sumb;
	}
}
void motionBlurH23() {
	int width = 4096;
	int height = 2304;
	for (int h = 0; h < height; h++) {
		int sumr = red2[h][0] + red2[h][1];
		int sumg = green2[h][0] + green2[h][1];
		int sumb = blue2[h][0] + blue2[h][1];
		red[h][0] = sumr;
		green[h][0] = sumg;
		blue[h][0] = sumb;
		sumr += red2[h][2];
		sumg += green2[h][2];
		sumb += blue2[h][2];
		int w = 1;
		for (; w < width - 2; w++) {
			red[h][w] = sumr;
			green[h][w] = sumg;
			blue[h][w] = sumb;
			sumr += red2[h][w + 2] - red2[h][w - 1];
			sumg += green2[h][w + 2] - green2[h][w - 1];
			sumb += blue2[h][w + 2] - blue2[h][w - 1];
		}
		red[h][w] = sumr;
		green[h][w] = sumg;
		blue[h][w] = sumb;
		sumr -= red2[h][w - 1];
		sumg -= green2[h][w - 1];
		sumb -= blue2[h][w - 1];
		red[h][w + 1] = sumr;
		green[h][w + 1] = sumg;
		blue[h][w + 1] = sumb;
	}
}
void motionBlurT3(bmp_img *tgt) {
	int width = 4096;
	int height = 2304;
	for (int w = 0; w < width; w++) {
		int sumr = red[0][w] + red[1][w];
		int sumg = green[0][w] + green[1][w];
		int sumb = blue[0][w] + blue[1][w];
		tgt->img_pixels[0][w].red = round((double)sumr / 729);
		tgt->img_pixels[0][w].green = round((double)sumg / 729);
		tgt->img_pixels[0][w].blue = round((double)sumb / 729);
		sumr += red[2][w];
		sumg += green[2][w];
		sumb += blue[2][w];
		int h = 1;
		for (; h < height - 2; h++) {
			tgt->img_pixels[h][w].red = round((double)sumr / 729);
			tgt->img_pixels[h][w].green = round((double)sumg / 729);
			tgt->img_pixels[h][w].blue = round((double)sumb / 729);
			sumr += red[h + 2][w] - red[h - 1][w];
			sumg += green[h + 2][w] - green[h - 1][w];
			sumb += blue[h + 2][w] - blue[h - 1][w];
		}
		tgt->img_pixels[h][w].red = round((double)sumr / 729);
		tgt->img_pixels[h][w].green = round((double)sumg / 729);
		tgt->img_pixels[h][w].blue = round((double)sumb / 729);
		sumr -= red[h - 1][w];
		sumg -= green[h - 1][w];
		sumb -= blue[h - 1][w];
		tgt->img_pixels[h+1][w].red = round((double)sumr / 729);
		tgt->img_pixels[h+1][w].green = round((double)sumg / 729);
		tgt->img_pixels[h+1][w].blue = round((double)sumb / 729);
	}
}
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
	
	motionBlurH1(&bi);
	motionBlurT12();
	motionBlurH23();
	motionBlurT12();
	motionBlurH23();
	motionBlurT3(&bi);
	
	err = bmp_img_write(&bi, argv[2]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Write Error!\n");
		return -1;
	}
	bmp_img_free(&bi);
	return 0;
}

