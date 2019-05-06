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
int kern_size_half = 5 / 2;
int main(int argc, const char *const *argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <input_bmp_file> <output_bmp_file>\n", argv[0]);
		return 0;
	}

	double stime = MPI_Wtime();

	int comm_sz;
	int my_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	double stime_aftermpiinit = MPI_Wtime();

	//进程通信发送 or 接收数据区
	unsigned char*img_list=NULL;	
	int send_size[2];

	//SPMD
	if (my_rank == 0)
	{
		//读取bmp文件
		bmp_img bi;
		enum bmp_error err;
		err = bmp_img_read(&bi, argv[1]);
		if (err != BMP_OK) {
			fprintf(stderr, "BMP Read Error!\n");
			return -1;
		}
		//获取图片大小
		int width = bi.img_header.biWidth, height = bi.img_header.biHeight;
		//将图片尺寸以广播形式发出
		int realheight = height;
		if (height%comm_sz != 0)
			height += comm_sz - height % comm_sz;
		send_size[0] = height;
		send_size[1] = width;
		MPI_Bcast(send_size, 2, MPI_INT, 0, MPI_COMM_WORLD);

		//将图片转化为一位数组
		int color_size = (height + comm_sz * kern_size_half * 2)*width;
		img_list = (unsigned char*)malloc(color_size * 3 * sizeof(char));
		int seg_size = (height + comm_sz - 1) / comm_sz;
		for (int i = 0; i < comm_sz; i++)
		{
			if (i == 0)
			{
				for (int j = 0; j < kern_size_half; j++)
				{
					for (int k = 0; k < width; k++)
					{
						img_list[3 * (j*width + k)] = 0;
						img_list[3 * (j*width + k) + 1] = 0;
						img_list[3 * (j*width + k) + 2] = 0;
					}
				}
				if (comm_sz == 1)
				{
					for (int j = kern_size_half; j < seg_size + kern_size_half; j++)
					{
						for (int k = 0; k < width; k++)
						{
							img_list[3 * (j*width + k)] = bi.img_pixels[j - kern_size_half][k].red;
							img_list[3 * (j*width + k) + 1] = bi.img_pixels[j - kern_size_half][k].green;
							img_list[3 * (j*width + k) + 2] = bi.img_pixels[j - kern_size_half][k].blue;
						}
					}
					for (int j = height + (i * 2 + 1) * kern_size_half; j < height + (i + 1) * 2 * kern_size_half; j++)
					{
						for (int k = 0; k < width; k++)
						{
							img_list[3 * (j*width + k)] = 0;
							img_list[3 * (j*width + k) + 1] = 0;
							img_list[3 * (j*width + k) + 2] = 0;
						}
					}
				}
				else
				{
					for (int j = kern_size_half; j < seg_size + kern_size_half * 2; j++)
					{
						for (int k = 0; k < width; k++)
						{
							img_list[3 * (j*width + k)] = bi.img_pixels[j - kern_size_half][k].red;
							img_list[3 * (j*width + k) + 1] = bi.img_pixels[j - kern_size_half][k].green;
							img_list[3 * (j*width + k) + 2] = bi.img_pixels[j - kern_size_half][k].blue;
						}
					}
				}
			}
			else if (i == comm_sz - 1)
			{
				for (int j = i * (seg_size + 2 * kern_size_half); j < realheight + i * 2 * kern_size_half + kern_size_half; j++)
				{
					for (int k = 0; k < width; k++)
					{
						img_list[3 * (j*width + k)] = bi.img_pixels[j - (2 * i + 1)*kern_size_half][k].red;
						img_list[3 * (j*width + k) + 1] = bi.img_pixels[j - (2 * i + 1)*kern_size_half][k].green;
						img_list[3 * (j*width + k) + 2] = bi.img_pixels[j - (2 * i + 1)*kern_size_half][k].blue;
					}
				}
				for (int j = realheight + (i * 2 + 1) * kern_size_half; j < height + (i + 1) * 2 * kern_size_half; j++)
				{
					for (int k = 0; k < width; k++)
					{
						img_list[3 * (j*width + k)] = 0;
						img_list[3 * (j*width + k) + 1] = 0;
						img_list[3 * (j*width + k) + 2] = 0;
					}
				}
			}
			else
			{
				for (int j = i * (seg_size + 2 * kern_size_half); j < (i + 1) * (seg_size + 2 * kern_size_half); j++)
				{
					for (int k = 0; k < width; k++)
					{
						img_list[3 * (j*width + k)] = bi.img_pixels[j - (2 * i + 1)*kern_size_half][k].red;
						img_list[3 * (j*width + k) + 1] = bi.img_pixels[j - (2 * i + 1)*kern_size_half][k].green;
						img_list[3 * (j*width + k) + 2] = bi.img_pixels[j - (2 * i + 1)*kern_size_half][k].blue;
					}
				}
			}
		}
		/*for (int i = 0; i < height + comm_sz * kern_size_half * 2; i++)
		{
			printf("%d: ", i);
			for (int j = 0; j < width; j++)
				printf("%d ", img_list[i*width + j]);
			printf("\n");
		}*/
		// 分配到各自进程的内容
		int start = (seg_size + kern_size_half*2) * my_rank;
		int end = start + seg_size + kern_size_half * 2 - 1;
		if (end >= height + comm_sz * kern_size_half * 2)
			end = height + comm_sz * kern_size_half * 2 - 1;
		int size = (end - start + 1)*width;
		unsigned char* local_img = (unsigned char*)malloc(size * 3 * sizeof(char));
		MPI_Scatter(img_list, size * 3, MPI_CHAR, local_img, size * 3, MPI_CHAR, 0, MPI_COMM_WORLD);
		// 记录结果
 		int real_size = (end - start + 1 - kern_size_half * 2)*width;
		unsigned char* local_img_temp = (unsigned char*)malloc(real_size * 3 * sizeof(char));
		for (int i = kern_size_half; i < (size / width)-kern_size_half; i++)
		{
			for (int j = 0; j < width; j++)
			{
				double red = 0;
				double green = 0;
				double blue = 0;
				for (int k = -2; k <= 2; k++)
				{
					for (int t = -2; t <= 2; t++)
					{
						if (t + j >= 0 && t + j < width)
						{
							red += local_img[3 * ((i + k)*width + t + j)] * ConvKernel[k + 2][t + 2];
							green += local_img[3 * ((i + k)*width + t + j) + 1] * ConvKernel[k + 2][t + 2];
							blue += local_img[3 * ((i + k)*width + t + j) + 2] * ConvKernel[k + 2][t + 2];
						}
					}
				}
				local_img_temp[3 * ((i - kern_size_half)*width + j)] = (char)(red +0.5);
				local_img_temp[3 * ((i - kern_size_half)*width + j) + 1] = (char)(green +0.5);
				local_img_temp[3 * ((i - kern_size_half)*width + j) + 2] = (char)(blue +0.5);
			}
		}

		unsigned char*main_list = (unsigned char*)malloc(height*width * 3 * sizeof(char));
		MPI_Gather(local_img_temp, real_size * 3, MPI_CHAR, main_list, real_size * 3, MPI_CHAR, 0, MPI_COMM_WORLD);
		for (int i = 0; i < realheight; i++)
		{
			for (int j = 0; j < width; j++)
			{
				bi.img_pixels[i][j].red = main_list[3 * (i*width + j)];
				bi.img_pixels[i][j].green = main_list[3 * (i*width + j) + 1];
				bi.img_pixels[i][j].blue = main_list[3 * (i*width + j) + 2];
			}
		}
		err = bmp_img_write(&bi, argv[2]);
		if (err != BMP_OK) {
			fprintf(stderr, "BMP Write Error!\n");
			return -1;
		}
		bmp_img_free(&bi);
		/*for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				printf("%d ", img[i][j]);
			printf("\n");
		}*/
	}
	else
	{
		MPI_Bcast(send_size, 2, MPI_INT, 0, MPI_COMM_WORLD);
		int height = send_size[0];
		int width = send_size[1];
		int seg_size = (height + comm_sz - 1) / comm_sz;

		// 分配到各自进程的内容
		int start = (seg_size + kern_size_half * 2) * my_rank;
		int end = start + seg_size + kern_size_half * 2 - 1;
		if (end >= height + comm_sz * kern_size_half * 2)
			end = height + comm_sz * kern_size_half * 2 - 1;
		int size = (end - start + 1)*width;
		unsigned char* local_img = (unsigned char*)malloc(size * 3 * sizeof(char));
		MPI_Scatter(img_list, size * 3, MPI_CHAR, local_img, size * 3, MPI_CHAR, 0, MPI_COMM_WORLD);
		// 记录结果
		int real_size = (end - start + 1 - kern_size_half * 2)*width;
		unsigned char* local_img_temp = (unsigned char*)malloc(real_size * 3 * sizeof(char));
		for (int i = kern_size_half; i < (size / width) - kern_size_half; i++)
		{
			for (int j = 0; j < width; j++)
			{
				double red = 0;
				double green = 0;
				double blue = 0;
				for (int k = -2; k <= 2; k++)
				{
					for (int t = -2; t <= 2; t++)
					{
						if (t + j >= 0 && t + j < width)
						{
							red += ConvKernel[k + 2][t + 2] * local_img[3 * ((i + k)*width + t + j)];
							green += ConvKernel[k + 2][t + 2] * local_img[3 * ((i + k)*width + t + j) + 1];
							blue += ConvKernel[k + 2][t + 2] * local_img[3 * ((i + k)*width + t + j) + 2];
						}
					}
				}
				local_img_temp[3 * ((i - kern_size_half)*width + j)] = (unsigned char)(red + 0.5);
				local_img_temp[3 * ((i - kern_size_half)*width + j) + 1] = (unsigned char)(green + 0.5);
				local_img_temp[3 * ((i - kern_size_half)*width + j) + 2] = (unsigned char)(blue + 0.5);
			}
		}

		unsigned char*main_list = (unsigned char*)malloc(height*width * 3 * sizeof(char));
		MPI_Gather(local_img_temp, real_size * 3, MPI_CHAR, main_list, real_size * 3, MPI_CHAR, 0, MPI_COMM_WORLD);
	}
	
	double etime = MPI_Wtime();
	printf("Process %d time: %lg\n", my_rank, etime - stime);
	printf("Process %d time (except MPI initialization): %lg\n", my_rank, etime - stime_aftermpiinit);
	MPI_Finalize();

	return 0;
}

