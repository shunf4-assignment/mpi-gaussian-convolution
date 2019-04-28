#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BMP_MAGIC 19778

typedef struct _bmp_header
{
	unsigned int   bfSize;
	unsigned int   bfReserved;
	unsigned int   bfOffBits;
	
	unsigned int   biSize;
	int            biWidth;
	int            biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int   biCompression;
	unsigned int   biSizeImage;
	int            biXPelsPerMeter;
	int            biYPelsPerMeter;
	unsigned int   biClrUsed;
	unsigned int   biClrImportant;
} bmp_header;

typedef struct _bmp_pixel
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} bmp_pixel;

// This is faster than a function call
#define BMP_PIXEL(r,g,b) ((bmp_pixel){(b),(g),(r)})

void bmp_img_read(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    int res;
    bmp_header header;

    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    unsigned short magic;
    res = read(fd, &magic, sizeof(magic));
    if (res <= 0 || magic != BMP_MAGIC) {
        fprintf(stderr, "%hu\n", magic);
        perror("Error reading magic || magic incorrect");
        exit(EXIT_FAILURE);
    }

    res = read(fd, &header, sizeof(header));
    if (res < sizeof(header)) {
        perror("error");
        exit(EXIT_FAILURE);
    }

    printf("%u\n", header.bfSize);
    printf("%u\n", header.bfOffBits);
    printf("%u\n", header.biSize);
    printf("%d\n", header.biWidth);
    printf("%d\n", header.biHeight);
    printf("%hu\n", header.biPlanes);
    printf("%hu\n", header.biBitCount);
    printf("%u\n", header.biCompression);
    printf("%u\n", header.biSizeImage);
    printf("%d\n", header.biXPelsPerMeter);
    printf("%d\n", header.biYPelsPerMeter);
    printf("%u\n", header.biClrUsed);
    printf("%u\n", header.biClrImportant);

}

int main() {
    bmp_img_read("timg.bmp");
    return 0;
}