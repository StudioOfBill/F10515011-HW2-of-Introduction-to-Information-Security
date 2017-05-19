#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "DES.h"

using namespace std;

#define length 1024

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;


//file header
typedef struct {
    WORD bfType;
    DWORD bfSize;
    DWORD bfReserved;
    DWORD bfOffBits;
} BITMAP_FILE_HEADER;

//info header
typedef struct {
    DWORD  biSize;
    DWORD  biWidth;
    DWORD  biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    DWORD  biXPelsPerMeter;
    DWORD  biYPelsPerMeter;
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

class BMP_IMG {
public:
    BITMAP_FILE_HEADER File_data;
    BITMAP_INFO_HEADER Info_data;
    BITMAP_RGBMAP RGB_map[256];
    vector<BYTE> img_data;

    bool read_image(const char * path){
        FILE* input_file;
        input_file = fopen(path, "rb+");

        if (!input_file) {
            printf("Input Error!\n");
            return 0;
        }

        fread(&File_data.bfType, sizeof(WORD), 1, input_file);
        fread(&File_data.bfSize, sizeof(DWORD), 1, input_file);
        fread(&File_data.bfReserved, sizeof(DWORD), 1, input_file);
        fread(&File_data.bfOffBits, sizeof(DWORD), 1, input_file);

        if (File_data.bfType == 0x4D42) { //check if this is a RGB file or not
            fread(&Info_data.biSize, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biWidth, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biHeight, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biPlanes, sizeof(WORD), 1, input_file);
            fread(&Info_data.biBitCount, sizeof(WORD), 1, input_file);
            fread(&Info_data.biCompression, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biSizeImage, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biXPelsPerMeter, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biYPelsPerMeter, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biClrUsed, sizeof(DWORD), 1, input_file);
            fread(&Info_data.biClrImportant, sizeof(DWORD), 1, input_file);

            int offset = 0;
            int linelength = Info_data.biWidth * 3;
            offset = linelength % 4;
            if (offset > 0)
            {
                offset = 4 - offset;
            }
            BYTE pxVal;

            //read pixel
            for (int i = 0; i < Info_data.biHeight; i++) {
                for (int j = 0; j < linelength; j++) {
                    fread(&pxVal, sizeof(BYTE), 1, input_file);
                    img_data.push_back(pxVal);
                }

                for (int k = 0; k < offset; k++) {
                    fread(&pxVal, sizeof(BYTE), 1, input_file);
                }
            }
        } else {
            return false;
        }

        fclose(input_file);
        return true;
    };

    bool write_image(const char * path){
        FILE* output_file;
        output_file = fopen(path, "wb");
        if (!output_file) {
            printf("Output Error!\n");
            return 0;
        }

        //write heads
        fwrite(&File_data.bfType, sizeof(WORD), 1, output_file);
        fwrite(&File_data.bfSize, sizeof(DWORD), 1, output_file);
        fwrite(&File_data.bfReserved, sizeof(DWORD), 1, output_file);
        fwrite(&File_data.bfOffBits, sizeof(DWORD), 1, output_file);

        fwrite(&Info_data.biSize, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biWidth, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biHeight, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biPlanes, sizeof(WORD), 1, output_file);
        fwrite(&Info_data.biBitCount, sizeof(WORD), 1, output_file);
        fwrite(&Info_data.biCompression, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biSizeImage, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biXPelsPerMeter, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biYPelsPerMeter, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biClrUsed, sizeof(DWORD), 1, output_file);
        fwrite(&Info_data.biClrImportant, sizeof(DWORD), 1, output_file);

        int offset = 0;
        int linelength = Info_data.biWidth * 3;
        offset = (3 * Info_data.biWidth) % 4;
        if (offset > 0) {
            offset = 4 - offset;
        }
        //write pixel
        BYTE pxVal;
        auto iter = img_data.begin();
        for (int i = 0; i < Info_data.biHeight; i++) {
            for (int j = 0; j < linelength; j++) {
                pxVal = *iter;
                fwrite(&pxVal, sizeof(BYTE), 1, output_file);
                iter += 1;
            }

            pxVal = 0;

            for (int k = 0; k < offset; k++) {
                fwrite(&pxVal, sizeof(BYTE), 1, output_file);
            }
        }
        fclose(output_file);
        return true;
    };
};

bool key[length];



int main() {
    BMP_IMG IMAGE;
    char * path_in = "test.bmp";
    if (IMAGE.read_image(path_in)) {
        printf("Reading success!\n");
    }


    uint8_t in[8], Key[7];

    for (int i = 0; i < 8; ++i) {
        in[i] = IMAGE.img_data[i];
    }

    for (int i = 0; i < 7; ++i) {
        Key[i] = 0xfb;
    }

    DES::encrypt(in, Key);

    for (int i = 0; i < 8; ++i) {
        printf("%2x\t", in[i]);
    }

    DES::decrypt(in, Key);

    for (int i = 0; i < 8; ++i) {
        printf("%2x\t", in[i]);
    }

    char * path_out = "output.bmp";
    if (IMAGE.write_image(path_out)) {
        printf("Writing success!\n");
    }
    return 0;
}