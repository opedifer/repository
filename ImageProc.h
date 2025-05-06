#pragma once

#include<stdio.h>
#include<math.h>
#include<windows.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define Y 5000
#define X 5000

extern WORD bfType;
extern DWORD bfSize;
extern WORD bfReserved1, bfReserved2;
extern DWORD bfOffBits;
extern DWORD biSize, biWidth, biHeight;
extern WORD biPlanes, biBitCount;
extern DWORD biCompression, biSizeImage,
biXPelsPerMeter, biYPelsPerMeter,
biClrUsed, biClrImportant;

extern FILE* fp;
extern int i, j, k;
extern int h, w;

extern unsigned char image_in[Y][X][3];
extern unsigned char image_out_1[Y][X][3];
extern unsigned char image_out[Y][X][3];
extern unsigned char gray[256];

extern int random[3];

void color_change();
int judge(int* a, int b);
void love(int i, int j);
void Proc(const char* input, const char* output);