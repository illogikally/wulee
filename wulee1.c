#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

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

BITMAPHEADER imgHeader; 
const int BLOCK_SIZE = 3;
int imgWidth;
int imgHeight;

uint8_t *readImg(char *imgPath) {
	FILE *img = fopen(imgPath, "rb");

	if(img == NULL) {
		printf("Can't open the image, make sure the path is correct.");
	}

	fread(&imgHeader, sizeof(BITMAPHEADER), 1, img);
	if(imgHeader.biBitCount != 1) {
		printf("The given file is not a binary bitmap.");
	}
    imgHeight = imgHeader.biHeight;
    imgWidth  = imgHeader.biWidth;


	uint32_t rowSize = (imgWidth + 31) / 32 * 4;
	uint32_t imgDataPSize = rowSize * imgHeight;
	uint8_t *imgDataP = (uint8_t*)malloc(imgDataPSize); //imgData includes padding
    printf("%d %d %d %d %d", imgHeight, imgWidth, rowSize, imgDataPSize, sizeof(BITMAPHEADER));


	//fseek(img, imgHeader.bfOffBits, SEEK_SET);
	fread(imgDataP, imgDataPSize, 1, img);
    for(int i = 0; i < imgHeight; ++i) {
        printf("\n");
        for(int j = 0; j < rowSize*8; ++j) {
            printf("%d", (imgDataP[i*rowSize + j/8] >> ((7-j)%8) & 1));
        }
    }
	fclose(img);
    return imgDataP;
}

void writeImg(uint8_t *imgData) {
    FILE *img = fopen("output.bmp", "wb");

	uint32_t rowSize = (imgWidth + 31) / 32 * 4;
	uint32_t imgDataPSize = rowSize * imgHeight;
    uint8_t  *imgDataP = (uint8_t*)malloc(imgDataPSize);
    memset(imgDataP, 0, imgDataPSize);

    for(int i = 0; i < imgHeight; ++i) {
        for(int j = 0; j < imgWidth; ++j) {
            imgDataP[rowSize*i + j/8] |= (imgData[i*imgWidth + j]*128) >> (j%8);
        }
    }

    fwrite(&imgHeader, sizeof(BITMAPHEADER), 1, img);
	//fseek(img, imgHeader.bfOffBits, SEEK_SET);
    fwrite(imgData, sizeof(uint8_t), imgDataPSize, img);
    fclose(img);
    return;
}

int main(int argc, char** argv) {
    uint8_t *img = readImg("lena1.bmp");
    writeImg(img);


    //writeImg(readImg("lena.bmp"));
    



	return 0;
}
