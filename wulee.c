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
} BitmapHeader;

BitmapHeader header; 
int BLOCK_WIDTH, BLOCK_HEIGHT; 
// int imgWidth;
int dataSizeByte;
int DATA_HEIGHT;
int DATA_WIDTH;
uint16_t MESSAGE_LEN; 

void prints(char*);
void write_image(uint8_t*);
uint8_t* read_image(char*);
uint8_t* string_to_binary(char*);
int sum(uint8_t*);
uint8_t* extract(uint8_t*, int);
uint8_t* b_and_k(uint8_t*, uint8_t*);
void complement(uint8_t*, uint8_t*, int);
void encode(uint8_t*, uint8_t*, uint8_t*);
char* decode(uint8_t*, uint8_t*);
uint8_t* get_key(char*);

int main(int argc, uint8_t **argv) {
	if (argc == 3 || argc == 4) {
		uint8_t *data = read_image(argv[1]);
		uint8_t *key = get_key(argv[2]);	
		if (key == NULL || data == NULL) 
			return -1;

		if (argc == 3) {
			prints(decode(data, key));
		}
		else {
			uint8_t *message = string_to_binary(argv[3]);
			encode(data, key, message);
		}
	}
	else {
		printf("Usage: [PATH] [KEYSTRING] [MESSAGE].\n \
			    \n  - [PATH]: Path to cover/stego-image. \
			    \n  - [KEYSTRING]: Comma-separated width-suffix keystring. Ex: \"3,000111101010\". \
			    \n  - [MESSAGE]: Message to conceal.\n"); 
	}
 	return 0;
}

void prints(char *s) {
	for (int i = 0; i < strlen(s); ++i) {
		printf("%c", s[i]);
	}
   printf("\n");
	return;
}

uint8_t* read_image(char *path) {
	FILE *img = fopen(path, "rb");
	if (img == NULL) {
		printf("Can't open the image, make sure the path is correct.\n");
		return NULL;
	}

	fread(&header, sizeof(BitmapHeader), 1, img);
	if (header.biBitCount != 1) {
		printf("The given file is not a monochrome bitmap.\n");
		return NULL;
	}

   DATA_HEIGHT = header.biHeight;
	int rowSize = (header.biWidth + 31) / 32 * 4;
	dataSizeByte = rowSize * DATA_HEIGHT;
	uint8_t dataByte[dataSizeByte];
	uint8_t *data = (uint8_t*) malloc(dataSizeByte * 8);

	fread(dataByte, dataSizeByte, 1, img);

	DATA_WIDTH = rowSize * 8;
	/* for (int i = 0; i < DATA_HEIGHT; ++i) { */
	/* 	for (int j = 0; j < DATA_WIDTH; ++j) { */
	/* 		data[i*DATA_WIDTH + j] = (dataRaw[i*rowSize + j/8] >> (7-j % 8)) & 1; */
	/* 	} */
	/* } */
   for (int i = 0; i < DATA_HEIGHT*DATA_WIDTH; ++i) {
      data[i] = (dataByte[i/8] >> (7 - i%8)) & 1;
   }
	fclose(img);
	return data;
}

void write_image(uint8_t *data) {
	FILE* img = fopen("output.bmp", "wb");
	
	uint8_t *dataByte = (uint8_t*) calloc(dataSizeByte, 1);
	// uint8_t dataByte[dataSizeByte] = {0};

	// for (int i = 0; i < DATA_HEIGHT; ++i) {
	// 	for (int j = 0; j < DATA_WIDTH; ++j) {
	// 		dataByte[i*rowSize + j/8] |= data[i*DATA_WIDTH + j] << (7-j % 8);
	// 	}
	// }

	for (int i = 0; i < DATA_WIDTH*DATA_HEIGHT; ++i) {
		dataByte[i/8] |= data[i] << (7 - i%8);
	}

	fwrite(&header, sizeof(BitmapHeader), 1, img);
	fwrite(dataByte, sizeof(uint8_t), dataSizeByte, img);
	fclose(img);
	free(dataByte);
	return;
}

uint8_t* string_to_binary(char *messageString) {
	MESSAGE_LEN = strlen(messageString) * 8;
	uint8_t *message = (uint8_t*) malloc(MESSAGE_LEN);

	for(int i = 0; i < MESSAGE_LEN; ++i) {
		message[i] = (messageString[i/8] >> (7 - i%8)) & 1;
	}
	return message;
}

int sum(uint8_t *block) {
	int sum = 0;
	// for(int i = 0; i < BLOCK_HEIGHT; ++i) {
	// 	for(int j = 0; j < BLOCK_WIDTH; ++j) {
	// 		sum += block[i*BLOCK_WIDTH + j] & 1;
	// 	}
	// }
	for (int i = 0; i < BLOCK_HEIGHT*BLOCK_WIDTH; ++i) {
		sum += block[i] & 1;
	}
	return sum;
}

uint8_t* extract(uint8_t *imgData, int blockIndex) {
	uint8_t *block = (uint8_t*) malloc(BLOCK_HEIGHT*BLOCK_WIDTH);
	for(int i = 0; i < BLOCK_HEIGHT; ++i) {
		for(int j = 0; j < BLOCK_WIDTH; ++j) {
         int maxBlockHor = DATA_WIDTH / BLOCK_WIDTH;
			int verPos = BLOCK_HEIGHT * (blockIndex / maxBlockHor) + i; 
			int horPos = BLOCK_WIDTH * (blockIndex % maxBlockHor) + j; 
			block[i*BLOCK_WIDTH + j] = imgData[verPos*DATA_WIDTH + horPos];
		}
	}
	return block;
}

uint8_t* b_and_k(uint8_t *b, uint8_t *k) {
	uint8_t *b_and_k= (uint8_t*) malloc(BLOCK_WIDTH*BLOCK_HEIGHT);

	// for(int i = 0; i < BLOCK_HEIGHT; ++i) {
	// 	for(int j = 0; j < BLOCK_WIDTH; ++j) {
	// 		int index = i*BLOCK_WIDTH + j;
	// 		b_and_k[index] = b[index] & k[index];
	// 	}
	// }

	for (int i = 0; i < BLOCK_HEIGHT*BLOCK_WIDTH; ++i) {
		b_and_k[i] = b[i] & k[i];
	}
	return b_and_k;
}

void complement(uint8_t *data, uint8_t *key, int blockIndex) {
	uint8_t *block = extract(data, blockIndex);
	int sumBAndK = sum(b_and_k(block, key));
	srand(time(0));
	int i, j, index;
	int sumK = sum(key);

	do {
		i = rand() % BLOCK_HEIGHT;
		j = rand() % BLOCK_WIDTH;
		index = i*BLOCK_WIDTH + j;
	} while(key[index] != 1 || block[index] != 0 && sumBAndK == 1 || block[index] != 1 && sumBAndK == sumK-1);	

   int maxBlockHor = DATA_WIDTH / BLOCK_WIDTH; 
	int verPos = BLOCK_HEIGHT * (blockIndex / maxBlockHor) + i; 
	int horPos = BLOCK_WIDTH * (blockIndex % maxBlockHor) + j; 
   data[verPos*DATA_WIDTH + horPos] ^= 1;
	return;
}

void encode(uint8_t *data, uint8_t *key, uint8_t *bMessage) {
	uint8_t message[MESSAGE_LEN + 16];
	for (int i = 0; i < 16; ++i) {
		message[i] = (MESSAGE_LEN >> (15 - i)) & 1;
	}
	memcpy(message+16, bMessage, MESSAGE_LEN);
	
	int maxBlock = DATA_HEIGHT/BLOCK_HEIGHT * DATA_WIDTH/BLOCK_WIDTH; 
	int sumK = sum(key);
	int sumBAndK;
	for(int bitIndex = 0, blockIndex = -1; bitIndex < sizeof(message); ++bitIndex) {
		uint8_t *block;
		do {
			++blockIndex;
			if (blockIndex >= maxBlock) {
				printf("The message is too long.\n");
				return;
			}
			block = extract(data, blockIndex);
			sumBAndK = sum(b_and_k(block, key));
		} while (sumBAndK <= 0 || sumBAndK >= sumK);

    	if (sumBAndK % 2 == message[bitIndex]) 
			continue;
		else 
			complement(data, key, blockIndex);
	}
    
	write_image(data);
	printf("The message has been successfully concealed.\n");
	return;
}

char* decode(uint8_t *data, uint8_t *key) {
	int maxBlock = DATA_WIDTH/BLOCK_WIDTH * DATA_HEIGHT/BLOCK_HEIGHT;
	uint8_t *block;
	int blockIndex = -1;
	int sumBAndK;

	uint16_t MESSAGE_LEN = 0;
	for (int i = 0; i < 16; ++i) {
		do {
			++blockIndex;
			block = extract(data, blockIndex);
			sumBAndK = sum(b_and_k(block, key));
		} while(sumBAndK <= 0 || sumBAndK >= sum(key));
		MESSAGE_LEN |= (sumBAndK % 2) << (15-i % 16);
	}
	
	if (MESSAGE_LEN > maxBlock) return "Wrong image.\n";

	char *message = (char*) calloc(MESSAGE_LEN/8, 1);
	for (int bitIndex = 0; bitIndex < MESSAGE_LEN; ++bitIndex) {
		do {
			++blockIndex;
			block = extract(data, blockIndex);
			sumBAndK = sum(b_and_k(block, key));
		} while (sumBAndK <= 0 || sumBAndK >= sum(key));
		message[bitIndex/8] |= (sumBAndK % 2) << (7 - bitIndex%8);
	}
	return message;
}

uint8_t* get_key(char *arg) {
	int cPos = 0;
	while (arg[cPos] != ',') {
		if (cPos >= strlen(arg)) {
			printf("Keystring is missing the comma.\n");
			return NULL;
		}
		++cPos; 	
	}

	for (int i = 0; i < cPos; ++i) {
		BLOCK_WIDTH += (arg[i] - '0') * pow(10, cPos-i-1);
	}

	int keyLen = strlen(arg) - cPos - 1; 
	if (BLOCK_WIDTH > keyLen || keyLen % BLOCK_WIDTH != 0) {
		printf("Invalid key width.\n");
		return NULL;
	}
	BLOCK_HEIGHT = keyLen / BLOCK_WIDTH;

	uint8_t *key = (uint8_t*) malloc(keyLen);
	for (int i = 0; i < keyLen; ++i) {
		key[i] = arg[cPos+i+1] - '0';
		if (key[i] != 0 && key[i] != 1) {
			printf("Key must only contain 0 and 1.\n");
			return NULL;
		}
	}

   if (sum(key) < 3) {
      printf("Key must have at least 1 bit 1.\n");
      return NULL;
   }
	return key;	
}


