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
void                    stego(vector<vector<uint8_t>>, vector<vector<uint8_t>>, vector<vector<uint8_t>>&,  int, int);
string                  readMessage(string);
void                    writeImage(vector<vector<uint8_t>>);
vector<vector<uint8_t>> blockAndKey(vector<vector<uint8_t>>, vector<vector<uint8_t>>);
vector<vector<uint8_t>> extractImgData(uint8_t*);
vector<vector<uint8_t>> readImage(char*);
vector<vector<uint8_t>> extractBlock(vector<vector<uint8_t>>, int);
string                  deStego(vector<vector<uint8_t>>, vector<vector<uint8_t>>);


int main(int argc, char** argv) {

    vector<vector<uint8_t>> imgData = readImage("image/output.bmp");
    vector<vector<uint8_t>> key{{0, 1, 0},
                                {1, 1, 1},
                                {0, 1, 0}};
    string binaryMessage = readMessage(string("h"));
    
     
    string message = deStego(imgData, key);
    cout << endl<< "Message size: " << message.size() <<endl ;
    for (int i = 0; i < message.size(); ++i) {
        cout << (int)message[i] << " ";
    }
    cout << endl << "The string: " << message << endl;
    //*/




    /*
    int testCurrentBlock = 0;
    for(int bitCount = 0, blockCount = -1; bitCount < binaryMessage.size(); ++bitCount) {
        vector<vector<uint8_t>> stegoBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
        int sumBlockAndKey;
        do {
            ++blockCount;
            stegoBlock     = extractBlock(imgData, blockCount);
    //*/
            /*
            cout << "Stego block change: " << endl;
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                cout << endl;
                for (int j = 0; j < BLOCK_SIZE; ++j) {
                    cout << (int)stegoBlock[i][j];
                }
            }
            
            cout << endl << endl;
            //*/
    //        sumBlockAndKey = checkSum(blockAndKey(stegoBlock, key));
            /*
            cout << "Block and key: " << endl;
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                cout << endl;
                for (int j = 0; j < BLOCK_SIZE; ++j) {
                    cout << (int)blockAndKey(stegoBlock, key)[i][j];
                }
            }
            cout << endl << endl << "Sum block & key: " << sumBlockAndKey << endl;
            //*/
            
    //   } while (sumBlockAndKey <= 0 || sumBlockAndKey >= checkSum(key));

        /*
        cout << endl << "Data before change: " << endl;
        for (int i = 0; i < imgHeader.biHeight; ++i) {
            cout << endl;
            if (i && !(i%4)) cout << endl;
            for (int j = 0; j < imgHeader.biWidth; ++j) {
                if (!(j%4) && j) cout << " ";
                cout << (int)imgData[i][j];
            
            }
        }
        cout << endl << endl;
        */
        


    /*
        if  (sumBlockAndKey % 2 == binaryMessage[bitCount]) {
            cout << endl << "Block: " << blockCount << endl;
            cout << endl << "Sum \% 2 = b" << endl;
            continue;
            //cout << "Sum block&key mod 2: " << sumBlockAndKey % 2;
        }
        else {
            stego(stegoBlock, key, imgData, blockCount, sumBlockAndKey);
        }
    //*/



        /*/
        /*
        cout << endl << "Data after change: " << endl;
        for (int i = 0; i < imgHeader.biHeight; ++i) {
            cout << endl;
            if (i && !(i%4)) cout << endl;
            for (int j = 0; j < imgHeader.biWidth; ++j) {
                if (!(j%4) && j) cout << " ";
                cout << (int)imgData[i][j];
            
            }
        }
        //*/

    /*
    }
    writeImage(imgData);
    //*/


    

    cout << endl << endl;
    cout << endl << "Binary message: ";
    for (int i = 0; i < binaryMessage.size(); ++i) {
        cout << (int)binaryMessage[i] << " ";
    }

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

void stego(vector<vector<uint8_t>> block, vector<vector<uint8_t>> key, vector<vector<uint8_t>>& imgData, int currentBlock, int directiveNum) {

    if (directiveNum == 1) {
        for (int i = 0; i < BLOCK_SIZE; ++i) {
            for (int j = 0; j < BLOCK_SIZE; ++j) {
                if (block[i][j] == 0 && key[i][j] == 1) {
                    cout << endl << "Block: " << currentBlock << endl;
                    cout << "Current coordinate 1: " << BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i << " " << BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j << endl;                     
                    imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j] = 1;
                    i = BLOCK_SIZE;
                    j = BLOCK_SIZE;
                }
            }
        }
    }
    else if (directiveNum == checkSum(key) - 1) {
        for (int i = 0; i < BLOCK_SIZE; ++i) {
            for (int j = 0; j < BLOCK_SIZE; ++j) {
                if (block[i][j] == 1 && key[i][j] == 1) {
                    cout << endl << "Block: " << currentBlock << endl;
                    cout << "Current coordinate key - 1: " << BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i << " " << BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j << endl;         
                    imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j] = 0;
                    i = BLOCK_SIZE;
                    j = BLOCK_SIZE;
                }
            }
        }
    }
    else {
        int i = 0,
            j = 0;
        do {
            srand (time(NULL));
            i = rand() % BLOCK_SIZE;
            j = rand() % BLOCK_SIZE;
        } while(key[i][j] != 1);
        cout << endl << "Block: " << currentBlock << endl;
        cout << "Current coordinate other: " << BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i << " " << BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j << endl;              
        imgData[BLOCK_SIZE * (currentBlock / (imgHeader.biWidth / BLOCK_SIZE)) + i][BLOCK_SIZE * (currentBlock % (imgHeader.biWidth / BLOCK_SIZE)) + j] ^= 1;
    }        
    return;
}

string deStego(vector<vector<uint8_t>> imgData, vector<vector<uint8_t>> key) {

    string binaryMessage;
    vector<vector<uint8_t>> stegoBlock(BLOCK_SIZE, vector<uint8_t>(BLOCK_SIZE, 0));
    int sumBlockAndKey;
    int possibleNumOfBlock = (imgData.size() * imgData.at(0).size()) / (BLOCK_SIZE * BLOCK_SIZE);
    for (int blockCount = -1; blockCount < possibleNumOfBlock; ) {
        cout << blockCount<< endl;
        do {
            ++blockCount;
            stegoBlock     = extractBlock(imgData, blockCount);
            sumBlockAndKey = checkSum(blockAndKey(stegoBlock, key));
        } while (sumBlockAndKey <= 0 || sumBlockAndKey >= checkSum(key));
        uint8_t ch =  sumBlockAndKey % 2;
        binaryMessage += ch; 
    }

    cout << "Binary message size: " << binaryMessage.size() << endl;
    for(int i = 0; i < binaryMessage.size(); ++i) {
        cout << (int)binaryMessage[i] << " ";
    }
    cout << endl  << endl;
    string message(binaryMessage.size() / 8, 0);
    for(int i = 0; i < binaryMessage.size(); ++i) {
        if(!(i%8) && i) {
            if(  ! ((int)message[i/8 - 1] == 32 || (int)message[i/8 - 1] == 33   || (int)message[i/8 - 1]  == 44  || (int)message[i/8 - 1]  == 46 
                || ((int)message[i/8 - 1] >= 48 && (int)message[i/8 - 1] <= 57)  || ((int)message[i/8 - 1] >= 65  && (int)message[i/8 - 1]  <= 90) 
                || ((int)message[i/8 - 1] >= 97 && (int)message[i/8 - 1] <= 122) || (int)message[i/8 - 1]  == 63)) {
                message.resize(i/8 - 1);
                break;
            }
        }
        message[i/8] |= ((binaryMessage[i] * 128) >> (i%8)); 
            
    }
    cout << "?" << endl;
    return message;

}