#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#pragma pack(2)
typedef struct 
{
   u16 bfType;
   u32 bfSize;
   u16 bfReserved1;
   u16 bfReserved2;
   u32 bfOffBits;
   u32 biSize;
   u32 biWidth;
   u32 biHeight;
   u16 biPlanes;
   u16 biBitCount;
   u32 biCompression;
   u32 biSizeImage;
   u32 biXPelsPerMeter;
   u32 biYPelsPerMeter;
   u32 biClrUsed;
   u32 biClrImportant;
   u64 ClrPallete;
} BitmapHeader;

BitmapHeader header;
int          BLOCK_W, BLOCK_H, BLOCK_SIZE;
int          DATA_W, DATA_H, DATA_SIZE;
int          SUM_K;
u16          MESSAGE_LEN;

int  sum(u8 *);
void prints(char *);
void write_image(u8 *);
void complement(u8 **, u8 *);
void encode(u8 *, u8 *, u8 *);
char *decode(u8 *, u8 *);
u8   *read_image(char *);
u8   *string_to_binary(char *);
u8   **extract(u8 *, int);
u8   *b_and_k(u8 **, u8 *);
u8   *get_key(char *);

int main(int argc, u8 **argv) 
{
   if (argc == 3 || argc == 4) {
      u8 *data = read_image(argv[1]);
      u8 *key = get_key(argv[2]);

      if (argc == 3) { 
         prints(decode(data, key)); 
      } 
      else {
         u8 *message = string_to_binary(argv[3]);
         encode(data, key, message);
      }
   } 
   else {
      printf("Usage: \n\tPATH KEYSTRING [MESSAGE]\n \
            \n  - PATH: Path to cover/stego-image (Must be a monochrome bitmap) \
            \n  - KEYSTRING: Comma-separated width-suffix keystring \
                  (Sum of the key must be at least 3). Ex: \"3,000111101010\" \
            \n  - MESSAGE: Message to conceal\n");
   }
   return 0;
}

void prints(char *s) 
{
   for (int i = 0; i < strlen(s); i++) {
      printf("%c", s[i]);
   }
   printf("\n");
   return;
}

u8 *read_image(char *path) 
{
   FILE *img = fopen(path, "rb");
   if (img == NULL) {
      printf("Bitmap path error: Can't open the image, make sure the path is correct\n");
      exit(-1);
   }

   fread(&header, sizeof(BitmapHeader), 1, img);
   if (header.biBitCount != 1) {
      printf("Bitmap path error: The given file is not a monochrome bitmap\n");
      exit(-1);
   }

   int rowSize = (header.biWidth + 31) / 32 * 4;
   DATA_H      = header.biHeight;
   DATA_W      = rowSize * 8;
   DATA_SIZE   = DATA_H * DATA_W;
   u8 *data    = (u8 *) malloc(DATA_SIZE);

   u8 dataByte[DATA_SIZE/8];
   fread(dataByte, DATA_SIZE/8, 1, img);

   for (int i = 0; i < DATA_SIZE; i++) {
      data[i] = (dataByte[i/8] >> (7 - i%8)) & 1;
   }
   fclose(img);
   return data;
}

void write_image(u8 *data) 
{
   FILE *img = fopen("stego.bmp", "wb");
   u8 dataByte[DATA_SIZE/8]; 
   memset(dataByte, 0, DATA_SIZE/8);

   for (int i = 0; i < DATA_SIZE; i++) {
      dataByte[i/8] |= data[i] << (7 - i%8);
}

   fwrite(&header, sizeof(BitmapHeader), 1, img);
   fwrite(&dataByte, sizeof(u8), DATA_SIZE/8, img);
   fclose(img);
   return;
}

u8 *string_to_binary(char *messageString) 
{
   MESSAGE_LEN = strlen(messageString) * 8;
   u8 *message = (u8 *) malloc(MESSAGE_LEN);

   for (int i = 0; i < MESSAGE_LEN; i++) {
      message[i] = (messageString[i/8] >> (7 - i%8)) & 1;
   }
   return message;
}

int sum(u8 *block) 
{
   int sum = 0;
   for (int i = 0; i < BLOCK_SIZE; i++) {
      sum += block[i];
   }
   free(block);
   return sum;
}

u8 **extract(u8 *data, int blockIndex) 
{
   int maxBlockHor = DATA_W / BLOCK_W;
   int verPos      = BLOCK_H * (blockIndex / maxBlockHor);
   int horPos      = BLOCK_W * (blockIndex % maxBlockHor);

   u8 **block = (u8 **) malloc(BLOCK_H);
   for (int i = 0; i < BLOCK_H; i++) {
      block[i] = &data[(verPos + i)*DATA_W + horPos];
   }

   return block;
}

u8 *b_and_k(u8 **b, u8 *k) 
{
   u8 *bAndK = (u8 *) malloc(BLOCK_SIZE);
   for (int i = 0; i < BLOCK_H; i++) {
      for (int j = 0; j < BLOCK_W; j++) {
         bAndK[i*BLOCK_W + j] = b[i][j] & k[i*BLOCK_W + j];
      }
   }
   return bAndK;
}

void complement(u8 **block, u8 *key) 
{
   int sumBAndK = sum(b_and_k(block, key));
   srand(time(0));
   int i, j;

   do {
      i = rand() % BLOCK_H;
      j = rand() % BLOCK_W;
   } while (key[i*BLOCK_W + j] != 1 
            || block[i][j] != 0 && sumBAndK == 1 
            || block[i][j] != 1 && sumBAndK == SUM_K-1);

   block[i][j] ^= 1;
   free(block);
   return;
}

void encode(u8 *data, u8 *key, u8 *bMessage) 
{
   u8 message[MESSAGE_LEN+16];
   for (int i = 0; i < 16; i++) {
      message[i] = (MESSAGE_LEN >> (15 - i)) & 1;
   }
   memcpy(message+16, bMessage, MESSAGE_LEN);

   int maxBlock = DATA_H/BLOCK_H * DATA_W/BLOCK_W;
   int sumBAndK;
   for (int bitIndex = 0, blockIndex = -1; bitIndex < sizeof(message); bitIndex++) {
      u8 **block;
      do {
         blockIndex++;
         if (blockIndex >= maxBlock) {
            printf("Encode failed: The message is too long\n");
            return;
         }
         block = extract(data, blockIndex);
         sumBAndK = sum(b_and_k(block, key));
      } while (sumBAndK <= 0 || sumBAndK >= SUM_K);

      if (sumBAndK % 2 == message[bitIndex])
         continue;
      else
         complement(block, key);
   }

   write_image(data);
   printf("The message has been successfully concealed\n");
   return;
}

char *decode(u8 *data, u8 *key) 
{
   int maxBlock   = DATA_W/BLOCK_W * DATA_H/BLOCK_H;
   int blockIndex = -1;
   u8  **block;
   int sumBAndK;

   u16 MESSAGE_LEN = 0;
   for (int i = 0; i < 16; i++) {
      do {
         blockIndex++;
         block = extract(data, blockIndex);
         sumBAndK = sum(b_and_k(block, key));
      } while (sumBAndK <= 0 || sumBAndK >= SUM_K);
      MESSAGE_LEN |= (sumBAndK % 2) << (15 - i%16);
   }

   if (MESSAGE_LEN > maxBlock)
      return "Decode failed: Message not found\n";

   char *message = (char *) calloc(MESSAGE_LEN/8, 1);
   for (int bitIndex = 0; bitIndex < MESSAGE_LEN; bitIndex++) {
      do {
         blockIndex++;
         if (blockIndex >= maxBlock) {
            return message;
         }
         block = extract(data, blockIndex);
         sumBAndK = sum(b_and_k(block, key));
      } while (sumBAndK <= 0 || sumBAndK >= SUM_K);
      message[bitIndex/8] |= (sumBAndK % 2) << (7 - bitIndex%8);
   }
   return message;
}

u8 *get_key(char *arg) 
{
   int cPos = 0;
   while (arg[cPos] != ',') {
      if (cPos >= strlen(arg)) {
         printf("Keystring error: Keystring is missing the comma\n");
         exit(-2);
      }
      cPos++;
   }

   for (int i = 0; i < cPos; i++) {
      BLOCK_W += (arg[i] - '0') * pow(10, cPos-i-1);
   }

   int keyLen = strlen(arg) - cPos - 1;
   if (BLOCK_W > keyLen || keyLen % BLOCK_W != 0) {
      printf("Keystring error: Invalid key width\n");
      exit(-2);
   }
   BLOCK_H    = keyLen / BLOCK_W;
   BLOCK_SIZE = BLOCK_H * BLOCK_W;

   u8 *key = (u8 *) malloc(keyLen);
   for (int i = 0; i < keyLen; i++) {
      key[i] = arg[cPos+i+1] - '0';
      if (key[i] != 0 && key[i] != 1) {
         printf("Keystring error: Key must only contain 0 and 1\n");
         exit(-2);
      }
      SUM_K += key[i];
   }

   if (SUM_K < 3) {
      printf("Keystring error: Sum of the key must be at least 3\n");
      exit(-2);
   }

   return key;
}


