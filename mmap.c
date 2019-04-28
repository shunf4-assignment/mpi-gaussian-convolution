#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mpi/mpi.h>
#include <string.h>

#define BMP_MAGIC 19778

#define PIX(y,x) bmpdata + (y) * rowbytes + (x) * 3
#define OPIX(y,x) obmpdata + (y) * rowbytes + (x) * 3
#define RED +2
#define BLUE +0
#define GREEN +1

double ConvKernel[5][5] = {
	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881},
	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
	{0.0350727, 0.06831229, 0.08531173, 0.06831229, 0.0350727},
	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881}
};

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

void process_bmp(const char *filename, const char *output) {
    int fd = open(filename, O_RDWR, (mode_t)0600);
    int ofd = open(output, O_WRONLY | O_CREAT | O_TRUNC, (mode_t)0600);
    int res;
    bmp_header header;

    if (fd == -1 || ofd == -1) {
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

    if (header.biWidth < 0 || header.biBitCount != 24 || header.biCompression != 0 || header.biClrUsed != 0) {
        fprintf(stderr, "BMP format error\n");
        exit(EXIT_FAILURE);
    }

    unsigned char *map = mmap(NULL, header.bfSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    unsigned char *bmpdata = map + sizeof(char) + sizeof(bmp_header);

    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mmapping");
        exit(EXIT_FAILURE);
    }

    lseek(ofd, header.bfSize - 1, SEEK_SET);
    write(ofd, "", 1);
    unsigned char *omap = mmap(NULL, header.bfSize, PROT_READ | PROT_WRITE, MAP_SHARED, ofd, 0);
    if (omap == MAP_FAILED) {
        close(fd);
        perror("Error mmapping 2");
        exit(EXIT_FAILURE);
    }

    unsigned char *obmpdata = omap + sizeof(char) + sizeof(bmp_header);

    unsigned rowbytes = header.biWidth * 3 + header.biWidth % 4;

    int sz;
	int mr;
    int width = header.biWidth;
    int height = header.biHeight;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &mr);

    int start = (height + sz - 1) / sz * mr;
    int end = start + (height + sz - 1) / sz - 1;
    if (end >= height)
        end = height - 1;
    int size = (end - start + 1) * width;

    for (int y = start; y <= end; y++) {
        for (int x = 0; x < width; x++) {
            double red = 0;
            double green = 0;
            double blue = 0;
            for (int i = -2; i <= 2; ++i) {
                for (int j = -2; j <= 2; ++j) {
                    if (y + i >= 0 && y + i < height && x + j >= 0 && x + j < width) {
                        red += *(PIX(y + i, x + j) RED) * ConvKernel[i + 2][j + 2];
                        green += *(PIX(y + i, x + j) GREEN) * ConvKernel[i + 2][j + 2];
                        blue += *(PIX(y + i, x + j) BLUE) * ConvKernel[i + 2][j + 2];
                    }
                }
            }

            *(OPIX(y, x) RED) = (char)red;
            *(OPIX(y, x) GREEN) = (char)green;
            *(OPIX(y, x) BLUE) = (char)blue;
        }
    }

    if (mr != 0) {
        int send = 1;
        MPI_Send(&send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
		int recv;

        memcpy(omap, map, sizeof(char) + sizeof(bmp_header));

		for (int i = 1; i < sz; i++) {
			MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		
		if (msync(omap, header.bfSize, MS_SYNC) == -1)
        {
            perror("Could not sync the file to disk");
        }
	}

    if (munmap(map, header.bfSize) == -1)
    {
        close(fd);
        close(ofd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    if (munmap(omap, header.bfSize) == -1)
    {
        close(fd);
        close(ofd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    close(fd);
    close(ofd);
}

int main(int argc, const char *const *argv) {
    if (argc != 3) {
		printf("Usage: %s <input_bmp_file> <output_bmp_file>\n", argv[0]);
		return 0;
	}

    process_bmp(argv[1], argv[2]);
    return 0;
}