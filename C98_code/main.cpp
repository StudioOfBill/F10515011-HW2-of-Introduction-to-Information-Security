#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>

using namespace std;

#define length 1024

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

class BMP_IMG {
public:
    BITMAP_FILE_HEADER File_data;
    BITMAP_INFO_HEADER Info_data;
    BITMAP_RGBMAP RGB_map[256];
    vector<BYTE> img_data;

    bool read_image(const string path);
    bool write_image(const string path);
};

bool key[length];

bool BMP_IMG::write_image(const string path){
    FILE* output_file;
    output_file = fopen(path, "wb");
    if (!output_file) {
        printf("Output Error!\n");
        return 0;
    }

    //write heads
    fwrite(&File_data, sizeof(BITMAP_FILE_HEADER), 1, output_file);
    fwrite(&Info_data, sizeof(BITMAP_INFO_HEADER), 1, output_file);

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

bool BMP_IMG::read_image(const string path){
    FILE* input_file;
    input_file = fopen(path, "rb");

    if (!input_file) {
        printf("Input Error!\n");
        return 0;
    }

    fread(&File_data, sizeof(BITMAP_FILE_HEADER), 1, input_file);

    if (File_data.bfType == 0x4D42) { //check if this is a RGB file or not
        fread(&Info_data, sizeof(BITMAP_INFO_HEADER), 1, input_file);

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


int main() {
    string path = "test.bmp";
    BMP_IMG IMAGE;
    IMAGE.read_image(path);
    cout << IMAGE.img_data[0];
    path = "output.bmp";
    IMAGE.write_image(path);
    return 0;
}