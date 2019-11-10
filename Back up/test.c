#include <stdio.h>
#include <string.h>
#include <malloc.h>

unsigned char *read_bmp(char *fname,int* _w, int* _h)
{
    unsigned char head[54];
    FILE *f = fopen(fname,"rb");

    // BMP header is 54 bytes
    fread(head, 1, 54, f);

    int w = head[18];
    int h = head[22];

    // lines are aligned on 4-byte boundary
    int lineSize = ((w  + 31) / 32)  * 4;
    int fileSize = lineSize * h;
    printf("%d %d \n", w, h);


    unsigned char *img = (unsigned char*)malloc(w * h), *data = (unsigned char*)malloc(fileSize);

    // skip the header
    fseek(f,54,SEEK_SET);

    // skip palette - two rgb quads, 8 bytes
    fseek(f, 8, SEEK_CUR);

    // read data
    fread(data,1,fileSize,f);

    // decode bits
    int i, j, k, rev_j;
    for(j = 0, rev_j = h - 1; j < h ; j++, rev_j--) {
        for(i = 0 ; i < w / 8; i++) {
            int fpos = j * lineSize + i, pos = rev_j * w + i * 8;
            for(k = 0 ; k < 8 ; k++)
                img[pos + (7 - k)] = (data[fpos] >> k ) & 1;
        }
    }

    free(data);
    *_w = w; *_h = h;
    return img;
}

int main()
{
    int w, h, i, j;
    unsigned char* img = read_bmp("input.bmp", &w, &h);

    for(j = 0 ; j < h ; j++)
    {
        for(i = 0 ; i < w ; i++)
            printf("%c ", img[j * w + i] ? '0' : '1' );

        printf("\n");
    }

    return 0;
}