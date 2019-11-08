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
const int bSize;
int imgWidth;
int imgHeight;

uint8_t* readImg(char *imgPath) {
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
	uint32_t imgDataSize = rowSize * imgHeight;
	uint8_t *imgDataRaw = (uint8_t*)malloc(imgDataSize);
	uint8_t *imgData = (uint8_t*)malloc(imgDataSize * 8);


	fread(imgDataRaw, imgDataSize, 1, img);

	/* Convert image data to byte */
	uint32_t imgDataWidth = rowSize * 8;
	for(int i = 0; i < imgHeight; ++i) {
		for(int j = 0; j < imgDataWidth; ++j) {
			imgData[i*imgDataWidth + j] = ((imgDataRaw[i*rowSize + j/8] >> (7-j % 8)) & 1);
		}
	}
	
	//for(int i = 0; i < imgHeight; ++i) {
	//	printf("\n");
	//	for(int j = 0; j < imgDataWidth; ++j) {
	//		printf("%d", imgData[i*imgDataWidth + j]);
	//	}
	//}
	fclose(img);
	return imgData;
}

void writeImg(uint8_t *imgData) {
	FILE* img = fopen("output.bmp", "wb");
	
	uint32_t rowSize = (imgWidth + 31) / 32 * 4;
	uint32_t imgDataSize = rowSize * imgHeight;
	uint8_t *imgDataRaw = (uint8_t*)malloc(imgDataSize);
	memset(imgDataRaw, 0, imgDataSize);
	uint32_t imgDataWidth = rowSize * 8;

	/* Convert image data to bit */
	for(int i = 0; i < imgHeight; ++i) {
		for(int j = 0; j < imgDataWidth; ++j) {
			imgDataRaw[i*rowSize + j/8] |= (imgData[i*imgDataWidth + j]) << (j % 8);
		}
	}

	fwrite(&imgHeader, sizeof(BITMAPHEADER), 1, img);
	fwrite(imgDataRaw, sizeof(uint8_t), imgDataSize, img);
	fclose(img);
	free(imgDataRaw);
	return;
}

uint8_t* readMessage(uint8_t *message) {
	uint32_t bMSize = strlen(message) * 8;
	uint8_t *bMessage = (uint8_t*)malloc(bMSize);
	memset(bMessage, 0, bMSize);

	for(int i = 0; i < bMSize; ++i) {
		bMessage[i] = (message[i/8] >> (7-i % 8)) & 1;
	}
	return bMessage;
}

int main(uint32_t argc, uint8_t **argv) {


	return 0;
}

