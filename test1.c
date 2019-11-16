#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int main(int argc, char** argv) {
    int a[] = {1, 2, 3, 4, 5, 6,
             7, 8, 9, 1, 2, 3,
             4, 5, 6, 7, 8, 9,
             1, 2, 3, 4, 5, 6};
    int w, h;
    w = h = 3;

    for(int i = 0; i < 2; ++i) {
        printf("\n");
        for(int j = 0; j < 2; ++j) {
            printf("%d",a[8+(i*2+j)]);
        }
    }
    return 0;
    


}