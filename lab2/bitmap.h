//
// Created by timur on 03.02.2021.
//

#ifndef LAB2_BITMAP_H
#define LAB2_BITMAP_H

#pragma pack(push, 2)
struct BITMAPINFO{
    unsigned short int bfType;
    unsigned long int bfSize;
    unsigned short int bfReserved1;
    unsigned short int bfReserved2;
    unsigned long int bfOffBits;
};
#pragma pack(pop)

#pragma pack(push, 2)
struct BITMAPINFOHEADER{
    unsigned long int biSize;
    long int biWidth;
    long int biHeight;
    unsigned short int biPlanes;
    unsigned short int biBitCount;
    unsigned long int biCompression;
    unsigned long int biSizeImage;
    long int biXPelsPerMeter;
    long int biYPelsPerMeter;
    unsigned long int biClrUsed;
    unsigned long int biClrImportant;
};
#pragma pack(pop)

#endif //LAB2_BITMAP_H
