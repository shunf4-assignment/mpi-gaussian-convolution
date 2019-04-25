#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"
#include <mpi.h>
#define CORE_SIZE 5
// const double ConvKernel[CORE_SIZE][CORE_SIZE] = {
// 	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881},
// 	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
// 	{0.0350727, 0.06831229, 0.08531173, 0.06831229, 0.0350727},
// 	{0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402},
// 	{0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881}
// };
const double ConvKernel[CORE_SIZE*CORE_SIZE] = {
	0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881,
	0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402,
	0.0350727, 0.06831229, 0.08531173, 0.06831229, 0.0350727,
	0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402,
	0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881
};
char newRed[2304][4096];
char newGreen[2304][4096];
char newBlue[2304][4096];
/***************************************************************************
  函数名称：conv
  功    能：卷积运算
  输入参数：像素块，卷积核，卷积核大小
  返 回 值：修改后的像素值
  说    明：
***************************************************************************/
char conv(char*pixel,const double*core)
{
	double rslt=0;
	for(int i=0;i<CORE_SIZE;i++)
	{
		for (int j=0;j<CORE_SIZE;j++)
		{
			rslt+=pixel[i*CORE_SIZE+j]*core[i*CORE_SIZE+j];
		}
	}
	return (char)rslt;
}
/***************************************************************************
  函数名称：box2list
  功    能：将二维数组转换为一维数组
  输入参数：图像，像素位置
  返 回 值：一维数组
  说    明：
***************************************************************************/
char** box2list(bmp_img img,int x,int y)
{
	char**p=(char**)malloc(3*sizeof(char*));
	for(int i=0;i<3;i++)
		p[i]=(char*)malloc(CORE_SIZE*CORE_SIZE*sizeof(char));
	for(int t=0;t<3;t++)
	{
		for(int i=-(CORE_SIZE/2);i<CORE_SIZE/2+1;i++)
		{
			for(int j=-(CORE_SIZE/2);j<CORE_SIZE/2+1;j++)
			{
				if (y + i >= 0 && y + i < img.img_header.biHeight && x + j >= 0 && x + j < img.img_header.biWidth)
				{
					if(t==0)
						p[t][(i+CORE_SIZE/2)*CORE_SIZE+j+CORE_SIZE/2]=img.img_pixels[y+i][x+j].red;
					else if(t==1)
						p[t][(i+CORE_SIZE/2)*CORE_SIZE+j+CORE_SIZE/2]=img.img_pixels[y+i][x+j].green;
					else
						p[t][(i+CORE_SIZE/2)*CORE_SIZE+j+CORE_SIZE/2]=img.img_pixels[y+i][x+j].blue;
				}
				else
					p[t][(i+CORE_SIZE/2)*CORE_SIZE+j+CORE_SIZE/2]=0;
			}
		}
	}
	return p;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: %s <input_bmp_file> <output_bmp_file>\n", argv[0]);
		return 0;
	}
	/***bmp图片读取***/
	bmp_img bi;
	enum bmp_error err;
	err = bmp_img_read(&bi, argv[1]);
	if (err != BMP_OK) {
		fprintf(stderr, "BMP Read Error!\n");
		return -1;
	}
	printf("Width: %d, Height: %d\n", bi.img_header.biWidth, bi.img_header.biHeight);
	/***MPI初始化***/
	int comm_sz;
	int my_rank;
	int desk_rank=0;
	double starttime,endtime;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int rank_len=bi.img_header.biHeight*bi.img_header.biWidth/comm_sz;
	// SPMD编程
	if(my_rank==desk_rank)//desk_process
	{
		starttime = MPI_Wtime();
		for(int i=0;i<comm_sz;i++)
		{
			if(i==0)
			{
				for(int j=i*rank_len;j<(i+1)*rank_len;j++)
				{
					char**p_list=box2list(bi,j%bi.img_header.biWidth,j/bi.img_header.biWidth);
					for(int t=0;t<3;t++)
					{
						t=conv(p_list[i],ConvKernel);
						if(i==0)
							newRed[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth]=t;
						else if(i==1)
							newGreen[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth]=t;
						else
							newBlue[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth]=t;
					}
				}
			}
			else if(i!=comm_sz-1)
			{
				for(int j=i*rank_len;j<(i+1)*rank_len;j++)
				{
					char**p_list=box2list(bi,j%bi.img_header.biWidth,j/bi.img_header.biWidth);
					for(int t=0;t<3;t++)
						MPI_Send(p_list[t],CORE_SIZE*CORE_SIZE,MPI_CHAR,i,0,MPI_COMM_WORLD);
				}
			}
			else
			{
				for(int j=i*rank_len;j<bi.img_header.biHeight*bi.img_header.biWidth;j++)
				{
					char**p_list=box2list(bi,j%bi.img_header.biWidth,j/bi.img_header.biWidth);
					for(int t=0;t<3;t++)
						MPI_Send(p_list[t],CORE_SIZE*CORE_SIZE,MPI_CHAR,i,0,MPI_COMM_WORLD);
				}
			}
		}
	}
	else
	{
		starttime = MPI_Wtime();
		char rgb_list[3][CORE_SIZE*CORE_SIZE];
		for(int j=0;j<rank_len;j++)
		{
			for(int i=0;i<3;i++)
			{
				MPI_Recv(rgb_list[i],CORE_SIZE*CORE_SIZE,MPI_CHAR,desk_rank,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				char t=conv(rgb_list[i],ConvKernel);
				if(i==0)
					newRed[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth]=t;
				else if(i==1)
					newGreen[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth]=t;
				else
					newBlue[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth]=t;
			}
		}
		int finish=1;
		MPI_Send(&finish,1,MPI_INT,0,0,MPI_COMM_WORLD);
	}
	if(my_rank==desk_rank)
	{
		for(int i=1;i<comm_sz;i++)
		{
			int finishsig;
			MPI_Recv(&finishsig,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
		endtime=MPI_Wtime();
		for(int j=0;j<rank_len;j++)
		{
			bi.img_pixels[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth].red=newRed[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth];
			bi.img_pixels[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth].green=newGreen[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth];
			bi.img_pixels[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth].blue=newBlue[(my_rank*rank_len+j)/bi.img_header.biWidth][(my_rank*rank_len+j)%bi.img_header.biWidth];
		}
		err = bmp_img_write(&bi, argv[2]);
		if (err != BMP_OK) {
			fprintf(stderr, "BMP Read Error!\n");
			return -1;
		}
	}
	bmp_img_free(&bi);
	if(my_rank==desk_rank)
		printf("Total time: %f\n",endtime-starttime);
	else
		printf("Time for the core No.%d: %f\n",my_rank,endtime-starttime);
	MPI_Finalize();
	return 0;
}

