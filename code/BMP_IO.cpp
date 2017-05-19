//
// Created by stybill on 16/05/2017.
//

#include "BMP_IO.h"
#include <stdio.h>
#include <stdint.h>
#include <vector>

bool BMP_IO::read_image(const std::string path, BMP_IO * filee){
    FILE* input_file;
    input_file = fopen(path, "rb");

    if (!input_file) {
        printf("Input Error!\n");
        return 0;
    }

    fread(&filee.File_data.bfType, sizeof(WORD), 1, input_file);
    fread(&filee.File_data.bfSize, sizeof(DWORD), 1, input_file);
    fread(&filee.File_data.bfReserved, sizeof(DWORD), 1, input_file);
    fread(&filee.File_data.bfOffBits, sizeof(DWORD), 1, input_file);

    if (File_data.bfType == 0x4D42) { //check if this is a RGB file or not
        fread(&filee.Info_data.biSize, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biWidth, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biHeight, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biPlanes, sizeof(WORD), 1, input_file);
        fread(&filee.Info_data.biBitCount, sizeof(WORD), 1, input_file);
        fread(&filee.Info_data.biCompression, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biSizeImage, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biXPelsPerMeter, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biYPelsPerMeter, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biClrUsed, sizeof(DWORD), 1, input_file);
        fread(&filee.Info_data.biClrImportant, sizeof(DWORD), 1, input_file);

        int offset = 0;
        int linelength = Info_data.biWidth * 3;
        offset = linelength % 4;
        if (offset > 0)
        {
            offset = 4 - offset;
        }
        BYTE pxVal;

        //read pixel
        for (int i = 0; i < filee.Info_data.biHeight; i++) {
            for (int j = 0; j < linelength; j++) {
                fread(&pxVal, filee(BYTE), 1, input_file);
                this.img_data.push_back(pxVal);
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
}

bool BMP_IO::write_image(const std::string path, BMP_IO * filee){
    FILE* output_file;
    output_file = fopen(path, "wb");
    if (!output_file) {
        printf("Output Error!\n");
        return 0;
    }

    //write heads
    fwrite(&filee.File_data.bfType, sizeof(WORD), 1, output_file);
    fwrite(&filee.File_data.bfSize, sizeof(DWORD), 1, output_file);
    fwrite(&filee.File_data.bfReserved, sizeof(DWORD), 1, output_file);
    fwrite(&filee.File_data.bfOffBits, sizeof(DWORD), 1, output_file);

    fwrite(&filee.Info_data.biSize, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biWidth, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biHeight, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biPlanes, sizeof(WORD), 1, output_file);
    fwrite(&filee.Info_data.biBitCount, sizeof(WORD), 1, output_file);
    fwrite(&filee.Info_data.biCompression, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biSizeImage, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biXPelsPerMeter, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biYPelsPerMeter, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biClrUsed, sizeof(DWORD), 1, output_file);
    fwrite(&filee.Info_data.biClrImportant, sizeof(DWORD), 1, output_file);

    int offset = 0;
    int linelength = filee.Info_data.biWidth * 3;
    offset = (3 * filee.Info_data.biWidth) % 4;
    if (offset > 0) {
        offset = 4 - offset;
    }
    //write pixel
    BYTE pxVal;
    auto iter = filee.img_data.begin();
    for (int i = 0; i < filee.Info_data.biHeight; i++) {
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
}