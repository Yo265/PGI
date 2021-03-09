#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "bitmap.h"

void printBMPHeader(struct BITMAPINFO info){
    printf("ID field: %c%c \n", info.bfType & 0xFF, info.bfType >> 8);
    printf("Size of the BMP file: %u bytes\n", info.bfSize);
    printf("bfReserved1: %d \n", info.bfReserved1);
    printf("bfReserved2: %d \n", info.bfReserved2);
    printf("Offset where the pixel array (bitmap data) can be found: %u bytes \n", info.bfOffBits);
}

void printDIBHeader(struct BITMAPINFOHEADER info){
    printf("Number of bytes in the DIB header (from this point): = %u bytes\n", info.biSize);
    printf("Width of the bitmap in pixels: %d pixels\n", info.biWidth);
    printf("Height of the bitmap in pixels. Positive for bottom to top pixel order: %d pixels\n", info.biHeight);
    printf("Number of color planes being used: %d \n", info.biPlanes);
    printf("Number of bits per pixel: %hu bits \n", info.biBitCount);
    printf("Pixel array compression: %u \n", info.biCompression);
    printf("Size of the raw bitmap data (including padding): %u bytes \n", info.biSizeImage);
    printf("Print resolution of the image: %d pixel/metre horizontal %d pixel/metre vertical \n", info.biXPelsPerMeter, info.biYPelsPerMeter);
    printf("Number of colors in the palette: %u \n", info.biClrUsed);
    printf("Number of important colors: %u \n", info.biClrImportant);
}

int main() {
    struct BITMAPINFOHEADER inf;
    struct BITMAPINFO header;
    int bmp;
    char filename[] = "../Data/24bit bmp.bmp";
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

    long int width = inf.biWidth;
    long int height = inf.biHeight;
    unsigned long size;

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

    long int new_width = inf.biWidth + 30;
    long int new_height = inf.biHeight + 30;
    unsigned long new_size = new_width * 3 + new_width % 4;
    new_size *= new_height;

    unsigned char* new_buff = (unsigned char*)malloc(new_size);

    if(new_buff == NULL){
        perror("Buffer allocation error");
        exit(1);
    }

    for(int i = 0; i < new_height; ++i){
        for(int j = 0; j < new_width * 3; j += 3){
            if(i <= 15 || i >= new_height - 15 ){
                new_buff[i * (new_width * 3 + new_width % 4) + j] = 0;
                new_buff[i * (new_width * 3 + new_width % 4) + j + 1] = 255;
                new_buff[i * (new_width * 3 + new_width % 4) + j + 2] = 255;
            }else if((i >= 15 || i <= new_height * 3 - 15) && (j <= 15 * 3 || j >= new_width * 3 - 15 * 3)){
                new_buff[i * (new_width * 3 + new_width % 4) + j] = 0;
                new_buff[i * (new_width * 3 + new_width % 4) + j + 1] = 255;
                new_buff[i * (new_width * 3 + new_width % 4) + j + 2] = 255;
            }else{
                new_buff[i * (new_width * 3 + new_width % 4) + j] = buff[(i - 15) * (width * 3 + width % 4) + j - (15 * 3)];
                new_buff[i * (new_width * 3 + new_width % 4) + j + 1] = buff[(i - 15) * (width * 3 + width % 4) + j + 1 - (15 * 3)];
                new_buff[i * (new_width * 3 + new_width % 4) + j + 2] = buff[(i - 15) * (width * 3 + width % 4) + j + 2 - (15 * 3)];
            }
        }
    }

    struct BITMAPINFOHEADER new_inf = inf;
    struct BITMAPINFO new_header = header;

    new_inf.biSizeImage = new_size;
    new_inf.biHeight = new_height;
    new_inf.biWidth = new_width;
    new_header.bfSize = new_size + sizeof(new_header) + sizeof(new_inf);

    int new_bmp;
    char new_filename[] = "../Result/out.bmp";
    if ((new_bmp = open(new_filename,  O_CREAT | O_WRONLY,0666)) == -1)
        perror("open failed on output file");
    write(new_bmp, &new_header, sizeof(new_header));
    write(new_bmp, &new_inf, sizeof(new_inf));
    if(write(new_bmp, new_buff, new_size) != new_size){
        perror("Error while writing bitmap data");
    }
    close(new_bmp);
    return 0;
}
