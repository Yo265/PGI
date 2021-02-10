#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "bitmap.h"

void printBMPHeader(struct BITMAPINFO info){
    printf("ID field: %c%c \n", info.bfType & 0xFF, info.bfType >> 8);
    printf("Size of the BMP file: %lu bytes\n", info.bfSize);
    printf("bfReserved1: %d \n", info.bfReserved1);
    printf("bfReserved2: %d \n", info.bfReserved2);
    printf("Offset where the pixel array (bitmap data) can be found: %lu bytes \n", info.bfOffBits);
}

void printDIBHeader(struct BITMAPINFOHEADER info){
    printf("Number of bytes in the DIB header (from this point): = %ld bytes\n", info.biSize);
    printf("Width of the bitmap in pixels: %lu pixels\n", info.biWidth);
    printf("Height of the bitmap in pixels. Positive for bottom to top pixel order: %ld pixels\n", info.biHeight);
    printf("Number of color planes being used: %d \n", info.biPlanes);
    printf("Number of bits per pixel: %hu bits \n", info.biBitCount);
    printf("Pixel array compression: %lu \n", info.biCompression);
    printf("Size of the raw bitmap data (including padding): %lu bytes \n", info.biSizeImage);
    printf("Print resolution of the image: %ld pixel/metre horizontal %ld pixel/metre vertical \n", info.biXPelsPerMeter, info.biYPelsPerMeter);
    printf("Number of colors in the palette: %lu \n", info.biClrUsed);
    printf("Number of important colors: %lu \n", info.biClrImportant);
}

int main() {
    struct BITMAPINFOHEADER inf;
    struct BITMAPINFO header;
    int bmp;
    char filename[] = ""; //Поменять путь
    if ((bmp = open(filename, O_RDONLY)) == -1){
        perror("Error while opening file");
        exit(1);
    }

    if(read(bmp, &header, sizeof(struct BITMAPINFO)) != sizeof(struct BITMAPINFO)){
        perror("Error while reading header from file");
        exit(1);
    }
    if(read(bmp, &inf, sizeof(struct BITMAPINFOHEADER)) != sizeof(struct BITMAPINFOHEADER)){
        perror("Error while reading info from file");
        exit(1);
    }

    printBMPHeader(header);
    printDIBHeader(inf);

    int width = inf.biWidth;
    int height = inf.biHeight;
    int size;

    if(inf.biSizeImage == 0){
        size = width * 3 + width % 4;
        size *= height;
    }else
        size = inf.biSizeImage;

    unsigned char* buff = (unsigned char*)malloc(size);

    if(buff == NULL){
        perror("Buffer allocation error");
        exit(1);
    }

    lseek(bmp, header.bfOffBits, SEEK_SET);
    if(read(bmp, buff, size) != size){
        perror("Possible error while reading pixel table");
        exit(1);
    }
    close(bmp);

    unsigned char* new_buff = (unsigned char*)malloc(size);

    for(int i = 0; i < height; ++i){
        for(int j = 0; j < width * 3; j += 3){
            unsigned char b = buff[i * (width * 3 + width % 4) + j];
            unsigned char g = buff[i * (width * 3 + width % 4) + j + 1];
            unsigned char r = buff[i * (width * 3 + width % 4) + j + 2];
            unsigned char new_color = (int)(b + g + r) / 3;
            new_buff[i * (width * 3 + width % 4) + j] = new_color;
            new_buff[i * (width * 3 + width % 4) + j + 1] = new_color;
            new_buff[i * (width * 3 + width % 4) + j + 2] = new_color;
        }
    }

    int new_bmp;
    char new_filename[] = "C:\\Users\\timur\\Desktop\\n.bmp";
    if ((new_bmp = open(new_filename,  O_CREAT | O_WRONLY)) == -1)
        perror("open failed on output file");
    write(new_bmp, &header, sizeof(header));
    write(new_bmp, &inf, sizeof(inf));
    if(write(new_bmp, new_buff, size) != size){
        perror("Error while writing bitmap data");
    }
    close(new_bmp);
    return 0;
}
