#include <iostream>
#include <string>
#include <vector>
#include <time.h>

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

BITMAPHEADER imgHeader;
const int    BLOCK_SIZE = 3;


int                     checkSum(vector<vector<uint8_t>>);
void                    encode(vector<vector<uint8_t>>, vector<vector<uint8_t>>, vector<vector<uint8_t>>&,  int, int);
string                  readMessage(string);
void                    writeImage(vector<vector<uint8_t>>);
vector<vector<uint8_t>> blockAndKey(vector<vector<uint8_t>>, vector<vector<uint8_t>>);
vector<vector<uint8_t>> extractImgData(uint8_t*);
vector<vector<uint8_t>> readImage(char*);
vector<vector<uint8_t>> extractBlock(vector<vector<uint8_t>>, int);
string                  deStegano(vector<vector<uint8_t>>, vector<vector<uint8_t>>);
void                    stegano(vector<vector<uint8_t>>, vector<vector<uint8_t>>, string);


int main(int argc, char** argv) {

    vector<vector<uint8_t>> imgData = readImage("image/output.bmp");
    vector<vector<uint8_t>> key{{0, 1, 0},
                                {1, 1, 1},
                                {0, 1, 0}};
    string binaryMessage = readMessage(string("h"));
    
     
    cout << endl << deStegano(imgData, key) << endl;
    //*/




    /*
    for(int bitCount = 0, blockCount = -1; bitCount < binaryMessage.size(); ++bitCount) {
        vector<vector<uint8_t>> stegoBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
        int sumBlockAndKey;
        do {
            ++blockCount;
            stegoBlock     = extractBlock(imgData, blockCount);

            sumBlockAndKey = checkSum(blockAndKey(stegoBlock, key));      
        } while (sumBlockAndKey <= 0 || sumBlockAndKey >= checkSum(key));

        
        if  (sumBlockAndKey % 2 == binaryMessage[bitCount]) {       
            continue;
        }
        else {
            encode(stegoBlock, key, imgData, blockCount, sumBlockAndKey);
        }
    }
    writeImage(imgData);
    //*/


    return 0;
}


string readMessage(string message) {
   string binaryMessage(message.size() * 8, 0);
    for(int i = 0; i < message.size() * 8; ++i) {
            binaryMessage[i] = ((message[i/8] >> (7 - i%8)) & 1);
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
    fread(&imgHeader, sizeof(BITMAPHEADER), 1, img);

    int imgRowSize     = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;
    int imgDataSize    = imgRowSize * imgHeader.biHeight;

    uint8_t *imgDataWithPadding = (uint8_t*)malloc(imgDataSize);
    fseek(img, imgHeader.bfOffBits, SEEK_SET);
    fread(imgDataWithPadding, imgDataSize, 1, img);
    fclose(img);

    return extractImgData(imgDataWithPadding);
}

vector<vector<uint8_t>> extractImgData(uint8_t* imgDataWithPadding) {
    vector<vector<uint8_t>> extractedImgData(imgHeader.biHeight, vector<uint8_t>(imgHeader.biWidth, 0));

    int imgRowSize = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;

    for(int i = 0; i < imgHeader.biHeight; ++i) {
        for(int j = 0; j < imgHeader.biWidth; ++j) {
            extractedImgData[i][j] = ((imgDataWithPadding[imgRowSize * i + j/8] >> (7 - j%8)) & 1);
        }
    }
    return extractedImgData;
}

void writeImage(vector<vector<uint8_t>> imgData) {
    FILE *img = fopen("image/output.bmp", "wb");
    fwrite(&imgHeader, sizeof(BITMAPHEADER), 1, img);

    int imgRowSize  = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;
    int imgDataSize = imgRowSize * imgHeader.biHeight;
    uint8_t imgDataWithPadding[imgDataSize] = {};

    for (int i = 0; i < imgHeader.biHeight; ++i) {
        for (int j = 0; j < imgHeader.biWidth; ++j) {
 
            imgDataWithPadding[imgRowSize * i + j/8] |= ((imgData[i][j] * 128) >> (j%8)); 
        }
    }
  
    fseek(img, imgHeader.bfOffBits, SEEK_SET);
    fwrite(imgDataWithPadding, sizeof(uint8_t), imgDataSize, img);
    fclose(img);
    return;
}

vector<vector<uint8_t>> extractBlock(vector<vector<uint8_t>> imgData, int currentBlock) {
    vector<vector<uint8_t>> extractedBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            extractedBlock[i][j] = imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j];
        }
    }
    return extractedBlock;
}

vector<vector<uint8_t>> blockAndKey(vector<vector<uint8_t>> block1, vector<vector<uint8_t>> block2) {
    vector<vector<uint8_t>> block(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            block[i][j] = block1[i][j] & block2[i][j];
        }
    }
    return block;
}

void encode(vector<vector<uint8_t>> stegoBlock, vector<vector<uint8_t>> key, vector<vector<uint8_t>>& imgData, int currentBlock, int directiveNum) {

    srand(time(0));
    int col, row;

    if (directiveNum == 1) {
        do {
            col = rand() % BLOCK_SIZE;
            row = rand() % BLOCK_SIZE;
        } while(stegoBlock[col][row] != 0 || key[col][row] != 1);
        imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + col][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + row] = 1;
    }
    else if (directiveNum == checkSum(key) - 1) {
        do {
            col = rand() % BLOCK_SIZE;
            row = rand() % BLOCK_SIZE;
        } while(stegoBlock[col][row] != 1 || key[col][row] != 1);
        imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + col][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + row] = 0;
    }
    else {
        do {

            col = rand() % BLOCK_SIZE;
            row = rand() % BLOCK_SIZE;
        } while(key[col][row] != 1);
        imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + col][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + row] ^= 1;
    }        
    return;
}

string deStegano(vector<vector<uint8_t>> imgData, vector<vector<uint8_t>> key) {

    string binaryMessage;
    vector<vector<uint8_t>> stegoBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
    int sumBlockAndKey;
    int possibleNumOfBlock = (imgData.size() * imgData.at(0).size()) / (BLOCK_SIZE * BLOCK_SIZE);
    string message(possibleNumOfBlock, 0);

    for (int blockCount = 0; blockCount < possibleNumOfBlock; ) {
        do {
            stegoBlock     = extractBlock(imgData, blockCount);
            sumBlockAndKey = checkSum(blockAndKey(stegoBlock, key));
            ++blockCount;
        } while (sumBlockAndKey <= 0 || sumBlockAndKey >= checkSum(key));
        uint8_t ch =  sumBlockAndKey % 2;
        binaryMessage += ch;

        int binaryMesPos = binaryMessage.size() - 1;
        if(!(binaryMesPos%8) && binaryMesPos) {
            if(  ! ((int)message[binaryMesPos/8 - 1] == 32 || (int)message[binaryMesPos/8 - 1] == 33   || (int)message[binaryMesPos/8 - 1]  == 44  || (int)message[binaryMesPos/8 - 1]  == 46 
                || ((int)message[binaryMesPos/8 - 1] >= 48 && (int)message[binaryMesPos/8 - 1] <= 57)  || ((int)message[binaryMesPos/8 - 1] >= 65  && (int)message[binaryMesPos/8 - 1]  <= 90) 
                || ((int)message[binaryMesPos/8 - 1] >= 97 && (int)message[binaryMesPos/8 - 1] <= 122) || (int)message[binaryMesPos/8 - 1]  == 63)) {
                message.resize(binaryMesPos/8 - 1);
                break;
            }
        }
        message[binaryMesPos/8] |= ((binaryMessage[binaryMesPos] * 128) >> (binaryMesPos%8)); 
    }
    
    return message;

}