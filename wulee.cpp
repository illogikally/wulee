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

vector<vector<uint8_t>> blockAndKey(vector<vector<uint8_t>>, vector<vector<uint8_t>>);
vector<vector<uint8_t>> extractImgData(uint8_t*);
vector<vector<uint8_t>> readImage(char*);
vector<vector<uint8_t>> extractBlock(vector<vector<uint8_t>>, int);
string                  readMessage(string);
string                  decode(vector<vector<uint8_t>>, vector<vector<uint8_t>>);
void                    encode(vector<vector<uint8_t>>, vector<vector<uint8_t>>, string);
void                    bitComplement(vector<vector<uint8_t>>, vector<vector<uint8_t>>, vector<vector<uint8_t>>&,  int, int);
void                    writeImage(vector<vector<uint8_t>>);
int                     checkSum(vector<vector<uint8_t>>);


int main(int argc, char** argv) {  
    vector<vector<uint8_t>> key{{0, 1, 0},
                                {1, 1, 1},
                                {0, 1, 0}};
    if (argc == 1) {
        cout << endl << "\n --ENCODE: *.exe [PATH] [MESSAGE]\n\n[PATH]\t\tCover image path.\n[MESSAGE]\tSecret message.\n\n --DECODE: *.exe [PATH]\n\n[PATH]\t\tStego image path." << endl;
        return 0;
    }
    else if (argc > 1) {
        vector<vector<uint8_t>> imgData  = readImage(argv[1]);
        if (imgHeader.biBitCount == 0) {
            cout << endl << "Can't open the file!" << endl;
            return 0;
        } else if(imgHeader.biBitCount != 1) {
            cout << endl << "The file is not a monochrome bitmap!" << endl;
            return 0;
        }
        if (argc == 2) {
            cout << endl << decode(imgData, key) << endl;
        }
        else if (argc == 3) {
            string binaryMessage = readMessage(string(argv[2]));
            if(binaryMessage.size() > (imgData.size() / BLOCK_SIZE) * (imgData.at(0).size() / BLOCK_SIZE)) {
                cout << endl << "The message is too large, please choose a higher resolution image!" << endl;
                return 0;
            }
            encode(imgData, key, binaryMessage);
        }
        else {
            cout << endl << "Too many arguments!" << endl;
            cout << endl << "\n --ENCODE: *.exe [PATH] [MESSAGE]\n\n[PATH]\t\tCover image path.\n[MESSAGE]\tSecret message.\n\n --DECODE: *.exe [PATH]\n\n[PATH]\t\tStego image path." << endl;
        }
    
    }
    return 0;
}


string readMessage(string message) {
    string binaryMessage(message.size() * 8, 0);
    for (int i = 0; i < message.size() * 8; ++i) {
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
    if (img == NULL) {
        imgHeader.biBitCount = 0;
        vector<vector<uint8_t>> error;
        return error;
    }
    fread(&imgHeader, sizeof(BITMAPHEADER), 1, img);
    if(imgHeader.biBitCount != 1) {
        vector<vector<uint8_t>> error;
        return error;
    }
    int     imgRowSize             = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;
    int     imgDataWithPaddingSize = imgRowSize * imgHeader.biHeight;
    uint8_t *imgDataWithPadding    = (uint8_t*)malloc(imgDataWithPaddingSize);
    fseek(img, imgHeader.bfOffBits, SEEK_SET);
    fread(imgDataWithPadding, imgDataWithPaddingSize, 1, img);
    fclose(img);
    return extractImgData(imgDataWithPadding);
}

vector<vector<uint8_t>> extractImgData(uint8_t* imgDataWithPadding) {
    vector<vector<uint8_t>> extractedImgData(imgHeader.biHeight, vector<uint8_t>(imgHeader.biWidth, 0));
    int imgRowSize = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;
    for (int i = 0; i < imgHeader.biHeight; ++i) {
        for (int j = 0; j < imgHeader.biWidth; ++j) {
            extractedImgData[i][j] = ((imgDataWithPadding[imgRowSize * i + j/8] >> (7 - j%8)) & 1);
        }
    }
    return extractedImgData;
}

void writeImage(vector<vector<uint8_t>> imgData) {
    FILE *img = fopen("output.bmp", "wb");
    fwrite(&imgHeader, sizeof(BITMAPHEADER), 1, img);
    int       imgRowSize                                 = ((imgHeader.biBitCount * imgHeader.biWidth + 31) / 32) * 4;
    const int imgDataWithPaddingSize                     = imgRowSize * imgHeader.biHeight;
    uint8_t   imgDataWithPadding[imgDataWithPaddingSize] = {};
    for (int i = 0; i < imgHeader.biHeight; ++i) {
        for (int j = 0; j < imgHeader.biWidth; ++j) {
            imgDataWithPadding[imgRowSize * i + j/8] |= ((imgData[i][j] * 128) >> (j%8)); 
        }
    }
    fseek(img, imgHeader.bfOffBits, SEEK_SET);
    fwrite(imgDataWithPadding, sizeof(uint8_t), imgDataWithPaddingSize, img);
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

void bitComplement(vector<vector<uint8_t>> stegoBlock, vector<vector<uint8_t>> key, vector<vector<uint8_t>>& imgData, int currentBlock, int sumBlockAndKey) {
    srand(time(0));
    int col, row;
    if (sumBlockAndKey == 1) {
        do {
            col = rand() % BLOCK_SIZE;
            row = rand() % BLOCK_SIZE;
        } while (stegoBlock[col][row] != 0 || key[col][row] != 1);
    }
    else if (sumBlockAndKey == checkSum(key) - 1) {
        do {
            col = rand() % BLOCK_SIZE;
            row = rand() % BLOCK_SIZE;
        } while (stegoBlock[col][row] != 1 || key[col][row] != 1);
    }
    else {
        do {
            col = rand() % BLOCK_SIZE;
            row = rand() % BLOCK_SIZE;
        } while (key[col][row] != 1);
        
    } 
    imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + col][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + row] ^= 1;       
    return;
}

string decode(vector<vector<uint8_t>> imgData, vector<vector<uint8_t>> key) {
    string                  binaryMessage;
    int                     sumBlockAndKey;
    int                     possibleNumOfBlocks = (imgData.size() / BLOCK_SIZE) * (imgData.at(0).size() / BLOCK_SIZE);
    vector<vector<uint8_t>> stegoBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
    string                  message(possibleNumOfBlocks/8, 0);
    for (int blockCount = -1; blockCount < possibleNumOfBlocks; ) {
        do {
            ++blockCount;
            stegoBlock     = extractBlock(imgData, blockCount);
            sumBlockAndKey = checkSum(blockAndKey(stegoBlock, key));
            
        } while ( sumBlockAndKey  <= 0 || sumBlockAndKey >= checkSum(key));
        uint8_t ch =  sumBlockAndKey % 2;
        binaryMessage += ch;
        int binaryMesPos = binaryMessage.size() - 1;
        if(!(binaryMesPos%8) && binaryMesPos) {
            if(  ! (message[binaryMesPos/8 - 1] == 32 || message[binaryMesPos/8 - 1] == 33   || message[binaryMesPos/8 - 1]  == 44  || message[binaryMesPos/8 - 1]  == 46
                ||  message[binaryMesPos/8 - 1] == 39 || message[binaryMesPos/8 - 1] == 40   || message[binaryMesPos/8 - 1]  == 41  || message[binaryMesPos/8 - 1]  == 34
                || (message[binaryMesPos/8 - 1] >= 48 && message[binaryMesPos/8 - 1] <= 57)  || (message[binaryMesPos/8 - 1] >= 65  && message[binaryMesPos/8 - 1]  <= 90) 
                || (message[binaryMesPos/8 - 1] >= 97 && message[binaryMesPos/8 - 1] <= 122) || message[binaryMesPos/8 - 1]  == 63)) 
                {
                message.resize(binaryMesPos/8 - 1);
                break;
            }
        }
        message[binaryMesPos/8] |= ((binaryMessage[binaryMesPos] * 128) >> (binaryMesPos%8)); 
    }    
    return message;
}

void encode(vector<vector<uint8_t>> imgData, vector<vector<uint8_t>> key, string binaryMessage) {
    for (int bitCount = 0, blockCount = -1; bitCount < binaryMessage.size(); ++bitCount) {
        vector<vector<uint8_t>> stegoBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
        int                     sumBlockAndKey = 0;
        do {
            ++blockCount;
            if (blockCount >= (imgHeader.biHeight / BLOCK_SIZE) * (imgHeader.biWidth / BLOCK_SIZE)) {
            cout << endl << "The message can't be fully concealed. Please choose a higher resolution image!" << endl;
            return;
            }            
            stegoBlock     = extractBlock(imgData, blockCount);
            sumBlockAndKey = checkSum(blockAndKey(stegoBlock, key));      
        } while (sumBlockAndKey <= 0 || sumBlockAndKey >= checkSum(key));
        if  (sumBlockAndKey % 2 == binaryMessage[bitCount]) {   
            continue;
        }
        else {
            bitComplement(stegoBlock, key, imgData, blockCount, sumBlockAndKey);
        }
    }
    writeImage(imgData);
    cout << endl << "The message has been successfully concealed!" << endl;
    return;
}