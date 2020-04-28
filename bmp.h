#ifndef BMP_H
#define BMP_H
#include <stdint.h>

// https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/
struct BMPHEAD1 {
    int16_t id;
};
struct BMPHEAD2  {
    int32_t filesize;
    int16_t reserved[2];
    int32_t headersize;

    int32_t infoSize;
    int32_t width;
    int32_t depth;
    int16_t biPlanes;
    int16_t bits;
    int32_t biCompression;
    int32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    int32_t biClrUsed;
    int32_t biClrImportant;
};

struct PIXELDATA {
    int8_t blueComponent;
    int8_t greenComponent;
    int8_t redComponent;
};

#endif