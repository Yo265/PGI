//
// Created by timur on 03.02.2021.
//

#ifndef LAB1_BITMAP_H
#define LAB1_BITMAP_H

#if defined(_WIN32)

#include <stdint.h>

typedef uint32_t u_int32_t;
typedef unsigned short u_short;

#endif
#pragma pack(push, 2)
struct BITMAPINFO{
    u_short bfType;
    u_int32_t bfSize;
    u_short bfReserved1;
    u_short bfReserved2;
    u_int32_t bfOffBits;
};
#pragma pack(pop)

#pragma pack(push, 2)
struct BITMAPINFOHEADER{
    u_int32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    u_short biPlanes;
    u_short biBitCount;
    u_int32_t biCompression;
    u_int32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    u_int32_t biClrUsed;
    u_int32_t biClrImportant;
};
#pragma pack(pop)

#endif //LAB1_BITMAP_H
