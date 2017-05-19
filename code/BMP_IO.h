//
// Created by stybill on 16/05/2017.
//

#ifndef CODE_BMP_IO_H
#define CODE_BMP_IO_H

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef long     LONG;

//file header
typedef struct {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAP_FILE_HEADER;

//info header
typedef struct {
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BITMAP_INFO_HEADER;

//RGB map
typedef struct {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} BITMAP_RGBMAP;

class BMP_IO {
public:
    BITMAP_FILE_HEADER File_data;
    BITMAP_INFO_HEADER Info_data;
    BITMAP_RGBMAP RGB_map[256];
    std::vector<BYTE> img_data;

    bool read_image(const std::string path, BMP_IO * filee);
    bool write_image(const std::string path, BMP_IO * filee);
};

#endif //CODE_BMP_IO_H
