#ifndef _WU_LEE
#define _WU_LEE


BITMAPHEADER imgHeader;
const int BLOCK_SIZE = 4;

using namespace std;

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


int                     checkSum(vector<vector<uint8_t>>);
void                    stego(uint8_t*);
uint8_t*                readMessage(string);
void                    writeImage(vector<vector<uint8_t>>);
vector<vector<uint8_t>> blockAndBlock(vector<vector<uint8_t>>, vector<vector<uint8_t>>);
vector<vector<uint8_t>> extractImageData(uint8_t*);
vector<vector<uint8_t>> readImage(char*);
vector<vector<uint8_t>> extractBlock(vector<vector<uint8_t>>, int);


uint8_t* readMessage(string message) {
  uint8_t *binaryMessage = (uint8_t*)malloc(message.size() * 8);
  for(int i = 0; i < message.size() * 8;) {
    for(int shiftNum = 7; shiftNum >= 0; --shiftNum) {
      binaryMessage[i++] = ((message[i/8] >> shiftNum) & 1);
    }
  }
  return binaryMessage;
}

int checkSum(vector<vector<uint8_t>> block) {
  int sum = 0;
  for (int i = 0; i < BLOCK_SIZE; ++i) {
    for (int j = 0; j < BLOCK_SIZE; ++j) {
      sum += (block[i][j] & 1);
    }
  }
  return sum;
}

vector<vector<uint8_t>> readImage(char* imgPath) {
  FILE* img = fopen(imgPath, "rb");
  fread(imgHeader, sizeof(BITMAPHEADER), 1, img);

  int imgRowSize     = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;
  int imgDataSize    = imgRowSize * imgHeader.biHeight;

  uint8_t *imgDataWithPadding = (uint8_t*)malloc(imgDataSize);
  fseek(img, imgHeader.bfOffBits, SEEK_SET);
  fread(imgDataWithPadding, imgDataSize, 1, img);
  fclose(img);

  return extractImageData(imgDataWithPadding);
}

#endif