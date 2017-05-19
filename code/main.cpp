#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "DES.h"
#include "BMP_IO.h"

using namespace std;

#define length 1024




int main() {
    BMP_IO IMAGE = BMP_IO();
    string path = "test.bmp";
    if (IMAGE.read_image(path, &IMAGE)) {
        printf("Reading success!\n");
    }


    uint8_t in[8], Key[7];

    DES DESS = DES();

    for (int i = 0; i < 8; ++i) {
        in[i] = IMAGE.img_data[i];
    }

    for (int i = 0; i < 7; ++i) {
        Key[i] = 0xfb;
    }

    DESS.encrypt(in, Key);

    for (int i = 0; i < 8; ++i) {
        printf("%2x\t", in[i]);
    }

    DESS.decrypt(in, Key);

    for (int i = 0; i < 8; ++i) {
        printf("%2x\t", in[i]);
    }

    path = "output.bmp";
    if (IMAGE.write_image(path, &IMAGE)) {
        printf("Writing success!\n");
    }
    return 0;
}