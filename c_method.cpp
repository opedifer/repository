#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include<math.h>
#include<windows.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define Y 5000
#define X 5000

WORD bfType;
DWORD bfSize;
WORD bfReserved1,bfReserved2;
DWORD bfOffBits;
DWORD biSize,biWidth,biHeight;
WORD biPlanes,biBitCount;
DWORD biCompression,biSizeImage,
      biXPelsPerMeter,biYPelsPerMeter,
	  biClrUsed,biClrImportant;

 
FILE *fp;
int i,j,k;
int h,w;

unsigned char image_in[Y][X][3];
unsigned char image_out_1[Y][X][3];
unsigned char image_out[Y][X][3];
unsigned char gray[256];

int random[3];
void color_change()
{
	random[0] = rand() % 255 + 1;
	random[1] = rand() % 255 + 1;
	random[2] = rand() % 255 + 1;
}

int judge(int* a, int b)
{
	int m;

	for (m = 0; a[m] != -1; m++)
	{
		if (b == a[m])
			return 0;
	}

	return 1;
}

void love(int i, int j)
{
	int m, n;

	int count = 0;

	int condition[500];

	int t;

	for (n = 0; image_out_1[i][j + n][0] == 255; n++)
	{
		for (t = 0; t <= 2; t++)
		{
			image_out_1[i][j + n][t] = random[t];
		}
	}

	for (n = 0; n <= 300; n++)
	{
		condition[n] = -1;
	}

	condition[0] = j; 

	for (m = i+1; m < h; m++)
	{
		count = 0;

		for (n = 0; condition[n] != -1; n++)
		{
			if (image_out_1[m][condition[n]][0] == 0)
			{	
				for (k = n; condition[k] != -1; k++)
				{
					condition[k] = condition[k+1];
				}

				n--;//每次交换完之后指针要往前一个
			}
		}

		for (n = 0; condition[n] != -1; n++)
		{
			count++;
		}

		if (count == 0)
			break;

		for (n = 0; condition[n] != -1; n++)
		{
			for (t = 0; t <= 2; t++)
			{
				image_out_1[m][condition[n]][t] = random[t];
			}
		}

		for (n = 0; condition[n] != -1; n++)
		{
			if (image_out_1[m][condition[n] - 1][0] == 255 && judge(condition, condition[n] - 1))
			{
				for (t = 0; t <= 2; t++)
				{
					image_out_1[m][condition[n] - 1][t] = random[t];
				}
				condition[count++] = condition[n] - 1;
			}

			if (image_out_1[m][condition[n] + 1][0] == 255 && judge(condition, condition[n] + 1))
			{
				for (t = 0; t <= 2; t++)
				{
					image_out_1[m][condition[n] + 1][t] = random[t];
				}
				condition[count++] = condition[n] + 1;
			}
		}
	}
}

void Proc(const char* input, const char* output)
{
		srand(time(NULL));

		char file_name[20];

		strcpy(file_name, input);
		fp=fopen(file_name,"rb");

		fread(&bfType,sizeof(bfType),1,fp);
		fread(&bfSize,sizeof(bfSize),1,fp);
		fread(&bfReserved1,sizeof(bfReserved1),1,fp);
		fread(&bfReserved2,sizeof(bfReserved2),1,fp);
		fread(&bfOffBits,sizeof(bfOffBits),1,fp);

		fread(&biSize,sizeof(biSize),1,fp);
		fread(&biWidth,sizeof(biWidth),1,fp);
		fread(&biHeight,sizeof(biHeight),1,fp);
		fread(&biPlanes,sizeof(biPlanes),1,fp);
		fread(&biBitCount,sizeof(biBitCount),1,fp);
		fread(&biCompression,sizeof(biCompression),1,fp);
		fread(&biSizeImage,sizeof(biSizeImage),1,fp);
		fread(&biXPelsPerMeter,sizeof(biXPelsPerMeter),1,fp);
		fread(&biYPelsPerMeter,sizeof(biYPelsPerMeter),1,fp);
		fread(&biClrUsed,sizeof(biClrUsed),1,fp);
		fread(&biClrImportant,sizeof(biClrImportant),1,fp);


		for(i=0;i<(int)biHeight;i++)
		{
			for(j=0;j<(int)biWidth;j++)
			{
				for(k=0;k<3;k++)
				{
					fread(&image_in[i][j][k],1,1,fp);
				}
			}
		}
		fclose(fp);
		printf("输入图像完成，数据如下\n");

		h=biHeight;
		w=biWidth;
		printf("输入图像的宽[%d]\n输入图像的长[%d]\n",h,w);

		printf("bfType:%d\n",bfType);
		printf("bfSize:%d\n",bfSize);
		printf("bfReserved1:%d\n",bfReserved1);
		printf("bfReserved2:%d\n",bfReserved2);
		printf("bfOffBits:%d\n",bfOffBits);
		printf("biSize:%d\n",biSize);
		printf("biWidth:%d\n",biWidth);
		printf("biHeight:%d\n",biHeight);
		printf("biPlanes:%d\n",biPlanes);
		printf("biBitCount:%d\n",biBitCount);
		printf("biCompression:%d\n",biCompression);
		printf("biSizeImage:%d\n",biSizeImage);
		printf("biXPelsPerMeter:%d\n",biXPelsPerMeter);
		printf("biYPelsPerMeter:%d\n",biYPelsPerMeter);
		printf("biClrUsed:%d\n",biClrUsed);
		printf("biClrImportant:%d\n",biClrImportant);

/////////////////////////////////////////////////////////////////////////////////////////

		int a,b,c,d,e;
		//a=0;
		a=512;

		for(i=0;i<h;i++)
		{
			for(j=0;j<w;j++)
			{
				if (image_in[i][j][0] != 0 || image_in[i][j][1] != 0 || image_in[i][j][2] != 0)
				{
						image_out_1[i][j][0] = 255;
						image_out_1[i][j][1] = 0;
						image_out_1[i][j][2] = 0;
				}
			}
		}

///把边界处理掉
		for (j = 0; j <= 511; j++)
		{
			for (k = 0; k <= 2; k++)
			{
				image_out_1[0][j][k] = 0;
				image_out_1[511][j][k] = 0;
			}
		}

		for (i = 0; i <= 511; i++)
		{
			for (k = 0; k <= 2; k++)
			{
				image_out_1[i][0][k] = 0;
				image_out_1[i][511][k] = 0;
			}
		}
///
/////////////////////////////////////////////////////////////////////////////////////////
		int switch1 = 0;
		int color = 50;

		while (1)
		{
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					if (image_out_1[i][j][0] == 255)
					{
						color_change();

						switch1 = 1;
						love(i, j);
						
						break;
					}
				}
			}

			if (switch1 == 0)
				break;
			else
				switch1 = 0;
		}


/////////////////////////////////////////////////////////////////////////////////////////
		int o1, o2, o3;
		
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				if (image_out_1[i][j][0] != 0 && image_out_1[i][j+1][0] != 0 && image_out_1[i][j][0] != image_out_1[i][j+1][0])
				{
					for (o1 = 0; o1 < h; o1++)
					{
						for (o2 = 0; o2 < w; o2++)
						{
							if (image_out_1[o1][o2][0] == image_out_1[i][j + 1][0])
							{
								for (o3 = 0; o3 <= 2; o3++)
								{
									image_out_1[o1][o2][o3] = image_out_1[i][j][o3];
								}
							}
						}
					}
				}
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////

	char file_name2[20];
		
		strcpy(file_name2, output);
		fp=fopen(file_name2,"wb");


		fwrite(&bfType,sizeof(bfType),1,fp);
		fwrite(&bfSize,sizeof(bfSize),1,fp);
		fwrite(&bfReserved1,sizeof(bfReserved1),1,fp);
		fwrite(&bfReserved2,sizeof(bfReserved2),1,fp);
		fwrite(&bfOffBits,sizeof(bfOffBits),1,fp);


		fwrite(&biSize,sizeof(biSize),1,fp);
		fwrite(&biWidth,sizeof(biWidth),1,fp);
		fwrite(&biHeight,sizeof(biHeight),1,fp);
		fwrite(&biPlanes,sizeof(biPlanes),1,fp);
		fwrite(&biBitCount,sizeof(biBitCount),1,fp);


		fwrite(&biCompression,sizeof(biSize),1,fp);
		fwrite(&biSizeImage,sizeof(biSizeImage),1,fp);
		fwrite(&biXPelsPerMeter,sizeof(biXPelsPerMeter),1,fp);
		fwrite(&biYPelsPerMeter,sizeof(biYPelsPerMeter),1,fp);
		fwrite(&biClrUsed,sizeof(biClrUsed),1,fp);
		fwrite(&biClrImportant,sizeof(biClrImportant),1,fp);

		for (i=0;i<(int)biHeight;i++)
		{
			for(j=0;j<(int)biWidth;j++)
			{
				for(k=0;k<3;k++)
				{
					fwrite(&image_out_1[i][j][k],1,1,fp);
				}
			}
		}
		fclose(fp);

		printf("输出图像完成，数据如下\n");
		printf("输出图像的宽[%d]\n输出图像的长[%d]\n",h,w);
}
