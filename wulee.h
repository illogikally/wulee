#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#ifndef _WU_LEE_
#define	_WU_LEE_ 

#pragma pack(1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    uint64_t ClrPallete;
} BITMAPHEADER;

void prints(char*);
void writeImg(uint8_t*);
uint8_t* readImg(char*);
uint8_t* binConvert(char*);
int sum(uint8_t*);
uint8_t* extract(uint8_t*, int);
uint8_t* bAndK(uint8_t*, uint8_t*);
void complement(uint8_t*, uint8_t*, int);
void encode(uint8_t*, uint8_t*, uint8_t*);
char* decode(uint8_t*, uint8_t*);
uint8_t* getKey(char*);

#endif