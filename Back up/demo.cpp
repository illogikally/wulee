#include <iostream>
#include <string>
#include <bitset>


using namespace std;

    struct BitmapHeader
    {
    	char          m_type[2];
    	unsigned char m_size[4];
    	unsigned char m_reserved1[2];
    	unsigned char m_reserved2[2];
    	unsigned char m_data_offset[4];
    };

        struct BitmapInformation
    {
    	unsigned char m_size[4];
    	unsigned char m_width[4];
    	unsigned char m_height[4];
    	unsigned char m_planes[2];
    	unsigned char m_bit_depth[2];
    	unsigned char m_compression_type[16];
    	unsigned char m_color_used[4];
    	unsigned char m_color_important[4];
    };


void readImage(const char * _filePath, BitmapHeader & _header, BitmapInformation & _information, unsigned char * &_data)
    {
    	FILE* _imageFile = fopen(_filePath, "rb");
     
    	if (_imageFile == nullptr)
    	{
    		return;
    	}
     
    	fread(&_header, sizeof(BitmapHeader), sizeof(char), _imageFile);
    	fread(&_information, sizeof(BitmapInformation), sizeof(char), _imageFile);
     
    	int _width = *(int*)_information.m_width;
    	int _height = *(int*)_information.m_height;
    	int _bitdepth = *(int*)_information.m_bit_depth;
    	int data_offset = *(int*)_header.m_data_offset;
     
    	int _rowSize = (_bitdepth * _width + 31) / 32 * 4;
     
    	if (_data != nullptr)
    	{
    		delete[] _data;
            _data = nullptr;
    	}
    	_data = new unsigned char[_rowSize * _height];
     
    	fseek(_imageFile, data_offset, SEEK_SET);
    	fread(_data, _rowSize * _height, sizeof(char), _imageFile);
     
    	fclose(_imageFile);
    }

    void saveImage(const char * _filePath, BitmapHeader & _header, BitmapInformation & _information, const unsigned char * _data)
    {
    	int _width = *(int*)_information.m_width;
    	int _bitdepth = *(int*)_information.m_bit_depth;
    	int _offset = *(int*)_header.m_data_offset;
     
    	FILE * _imageFile = fopen(_filePath, "wb");
     
    	fwrite(&_header, sizeof(BitmapHeader), sizeof(char), _imageFile);
    	fwrite(&_information, sizeof(BitmapInformation), 1, _imageFile);
     
    	int _rowSize = (_bitdepth * _width + 31) / 32 * 4;
     
    	fseek(_imageFile, _offset, SEEK_SET);
    	fwrite(_data, _rowSize * _height, sizeof(char), _imageFile);
    	fclose(_imageFile);
    }


using namespace std;

int main() {
    BITMAPHEADER header;
    FILE* image = fopen("hehexd.bmp", "rb");
    fread(&header, sizeof(BITMAPHEADER), sizeof(uint8_t), image);

    int imageWidth      = *(uint32_t*) header.biWidth;
    int imageHeight     = *(uint32_t*) header.biHeight;
    int imageDataOffBits = *(uint32_t*) header.bfOffBits;
    int imageRowSize    = ((imageWidth + 31) / 32) * 4;
    int imageSize       = imageRowSize* imageHeight;


    uint8_t *data = (uint8_t*)malloc(imageSize);
    fseek(image,imageDataOffBits, SEEK_SET);
    fread(data, 1, imageSize, image);
    fclose(image);

    cout << imageDataOffBits << endl << imageSize << endl;

    for(int i = 0; i < imageSize; i += imageRowSize) {
      for(int offset = 0; offset < 32; ++offset) {
       cout << (((*(uint32_t*)&data[i] >> offset) & 1)) /*cout << 1*/ << " ";
      }
      cout << endl;
    }

    
    FILE* img = fopen("question.bmp", "wb");
    /*fwrite(&header, sizeof(BITMAPHEADER), 1, img);
    fseek(img, imageDataOffBits, SEEK_SET);
    fwrite(data, imageSize, 1, img);
    fclose(img);
    */



    

    


    return 0;
}