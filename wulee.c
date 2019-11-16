#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

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
int bWidth, bHeight; 
int imgWidth;
int imgHeight;
int imgDataWidth;
uint16_t mSize; 

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

int main(int argc, uint8_t **argv) {
	if (argc >= 3 && argc <= 4) {
		uint8_t *imgData = readImg(argv[1]);
		uint8_t *key = getKey(argv[2]);	
		if(key == NULL || imgData == NULL) return -1;

		if(argc == 3) {
			prints(decode(imgData, key));
		}
		else if(argc == 4) {
			uint8_t *binMes = binConvert(argv[3]);
			encode(imgData, key, binMes);
		}
	}
	else {
		printf("Usage: [PATH] [KEYSTRING] [MESSAGE].\n \
			    \n  - [PATH]: Path to cover/stego-image. \
			    \n  - [KEYSTRING]: Comma-separated width-suffix keystring. Ex: \"3,000111101010\". \
			    \n  - [MESSAGE]: Message to conceal."); 
	}
 	return 0;
}

void prints(char *s) {
	for(int i = 0; i < strlen(s); ++i) {
		printf("%c", s[i]);
	}
	return;
}

uint8_t* readImg(char *imgPath) {
	FILE *img = fopen(imgPath, "rb");
	if(img == NULL) {
		printf("Can't open the image, make sure the path is correct.");
		return NULL;
	}

	fread(&imgHeader, sizeof(BITMAPHEADER), 1, img);
	if(imgHeader.biBitCount != 1) {
		printf("The given file is not a monochrome bitmap.");
		return NULL;
	}

    imgHeight = imgHeader.biHeight;
    imgWidth  = imgHeader.biWidth;
	int rowSize = (imgWidth + 31) / 32 * 4;
	int imgDataSize = rowSize * imgHeight;
	uint8_t imgDataRaw[imgDataSize];
	uint8_t *imgData = (uint8_t*)malloc(imgDataSize * 8);

	fread(imgDataRaw, imgDataSize, 1, img);

	imgDataWidth = rowSize * 8;
	for(int i = 0; i < imgHeight; ++i) {
		for(int j = 0; j < imgDataWidth; ++j) {
			imgData[i*imgDataWidth + j] = (imgDataRaw[i*rowSize + j/8] >> (7-j % 8)) & 1;
		}
	}
	fclose(img);
	return imgData;
}

void writeImg(uint8_t *imgData) {
	FILE* img = fopen("output.bmp", "wb");
	
	int rowSize = (imgWidth + 31) / 32 * 4;
	int imgDataSize = rowSize * imgHeight;
	uint8_t *imgDataRaw = (uint8_t*)calloc(imgDataSize, 1);

	for(int i = 0; i < imgHeight; ++i) {
		for(int j = 0; j < imgDataWidth; ++j) {
			imgDataRaw[i*rowSize + j/8] |= imgData[i*imgDataWidth + j] << (7-j % 8);
		}
	}
	fwrite(&imgHeader, sizeof(BITMAPHEADER), 1, img);
	fwrite(imgDataRaw, sizeof(uint8_t), imgDataSize, img);
	fclose(img);
	free(imgDataRaw);
	return;
}

uint8_t* binConvert(char *message) {
	mSize = strlen(message) * 8;
	uint8_t *bMessage = (uint8_t*)malloc(mSize);

	for(int i = 0; i < mSize; ++i) {
		bMessage[i] = (message[i/8] >> (7-i % 8)) & 1;
	}
	return bMessage;
}

int sum(uint8_t *block) {
	int sum = 0;
	for(int i = 0; i < bHeight; ++i) {
		for(int j = 0; j < bWidth; ++j) {
			sum += block[i*bWidth + j] & 1;
		}
	}
	return sum;
}

uint8_t* extract(uint8_t *imgData, int curBlock) {
	uint8_t *block = (uint8_t*)malloc(bHeight*bWidth);
	for(int i = 0; i < bHeight; ++i) {
		for(int j = 0; j < bWidth; ++j) {
            int pNBlockHor = imgDataWidth / bWidth;
			int verPos = bHeight * (curBlock / pNBlockHor) + i; 
			int horPos = bWidth * (curBlock % pNBlockHor) + j; 
			block[i*bWidth + j] = imgData[verPos*imgDataWidth + horPos];
		}
	}
	return block;
}

uint8_t* bAndK(uint8_t *b, uint8_t *k) {
	uint8_t *bAndK = (uint8_t*)malloc(bWidth*bHeight);

	for(int i = 0; i < bHeight; ++i) {
		for(int j = 0; j < bWidth; ++j) {
			int curPos = i*bWidth + j;
			bAndK[curPos] = b[curPos] & k[curPos];
		}
	}
	return bAndK;
}

void complement(uint8_t *imgData, uint8_t *key, int curBlock) {
	uint8_t *block = extract(imgData, curBlock);
	int sumBAndK = sum(bAndK(block, key));
	srand(time(0));
	int i, j;
	int sumK = sum(key);
	int curPos;

	do {
		i = rand() % bHeight;
		j = rand() % bWidth;
		curPos = i*bWidth + j;
	} while(key[curPos] != 1 || block[curPos] != 0 && sumBAndK == 1 || block[curPos] != 1 && sumBAndK == sumK-1);	
    int pNBlockHor = imgDataWidth / bWidth; 
	int verPos = bHeight * (curBlock / pNBlockHor) + i; 
	int horPos = bWidth * (curBlock % pNBlockHor) + j; 
    imgData[verPos*imgDataWidth + horPos] ^= 1;
	return;
}

void encode(uint8_t *imgData, uint8_t *key, uint8_t *bMessage) {
	uint8_t message[mSize + 16];
	for(int i = 0; i < 16; ++i) {
		message[i] = (mSize >> (15-i % 16)) & 1;
	}
	memcpy(message+16, bMessage, mSize);
	
	int pNBlock = imgHeight/bHeight * imgDataWidth/bWidth; 
	int sumK = sum(key);
	int sumBAndK;
	for(int curBit = 0, curBlock = -1; curBit < sizeof(message); ++curBit) {
		uint8_t *block;
		do {
			++curBlock;
			if(curBlock >= pNBlock) {
				printf("The message is too long.");
				return;
			}
			block = extract(imgData, curBlock);
			sumBAndK = sum(bAndK(block, key));
		} while(sumBAndK <= 0 || sumBAndK >= sumK);

    	if(sumBAndK % 2 == message[curBit]) continue;
		else complement(imgData, key, curBlock);
	}
    
	writeImg(imgData);
	printf("Concealing message is succeeded.");
	return;
}

char* decode(uint8_t *imgData, uint8_t *key) {
	int pNBlock = imgDataWidth/bWidth * imgHeight/bHeight;
	uint8_t *block;
	int curBlock = -1;
	int sumBAndK;

	uint16_t mSize = 0;
	for(int i = 0; i < 16; ++i) {
		do {
			++curBlock;
			block = extract(imgData, curBlock);
			sumBAndK = sum(bAndK(block, key));
		} while(sumBAndK <= 0 || sumBAndK >= sum(key));
		mSize |= (sumBAndK % 2) << (15-i % 16);
	}
	
	if(mSize > pNBlock) return "Wrong image.";

	char *message = (char*)calloc(mSize/8, 1);
	for(int curBit = 0; curBit < mSize; ++curBit) {
		do {
			++curBlock;
			block = extract(imgData, curBlock);
			sumBAndK = sum(bAndK(block, key));
		} while(sumBAndK <= 0 || sumBAndK >= sum(key));
		message[curBit/8] |= (sumBAndK % 2) << (7-curBit % 8);
	}
	return message;
}

uint8_t* getKey(char *arg) {
	int cPos = 0;
	while(arg[cPos] != ',') {
		if(cPos >= strlen(arg)) {
			printf("Keystring is missing comma.");
			return NULL;
		}
		++cPos; 	
	}

	for(int i = 0; i < cPos; ++i) {
		bWidth += (arg[i] - '0') * pow(10, cPos-i-1);
	}

	int keyLen = strlen(arg)-cPos-1; 
	if(bWidth > keyLen || keyLen % bWidth != 0) {
		printf("Invalid key width.");
		return NULL;
	}
	bHeight = keyLen / bWidth;

	uint8_t *key = (uint8_t*)malloc(keyLen);
	for(int i = 0; i < keyLen; ++i) {
		key[i] = arg[cPos+i+1] - '0';
		if(key[i] != 0 && key[i] != 1) {
			printf("Key must only contain 0 and 1.");
			return NULL;
		}
	}

    if(sum(key) < 3) {
        printf("Key must have at least 3 bits 1.");
        return NULL;
    }
	return key;	
}


