#include <iostream>
#include <vector>
#include <fstream>
#include <thread>

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


class thread_raii {
    std::thread& t;
public:

    explicit thread_raii(std::thread& t_) : t(t_) { }

    ~thread_raii() {
        if(t.joinable()) {
            t.join();
        }
    }

    thread_raii(thread_raii const&)=delete;
    thread_raii& operator=(thread_raii const&)=delete;
};

vector<thread> threads;


uint8_t Key[8], IV[8];

static const uint8_t IP_Table[64] = {
        58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
};

static const uint8_t IPR_Table[64] = {
        40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25
};

static const uint8_t E_Table[48] = {
        32,1,2,3,4,5,4,5,6,7,8,9,
        8,9,10,11,12,13,12,13,14,15,16,17,
        16,17,18,19,20,21,20,21,22,23,24,25,
        24,25,26,27,28,29,28,29,30,31,32,1
};

static const uint8_t P_Table[32] = {
        16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,
        2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25
};

static const uint8_t PC1_Table[56] = {
        57,49,41,33,25,17,9,1,58,50,42,34,26,18,
        10,2,59,51,43,35,27,19,11,3,60,52,44,36,
        63,55,47,39,31,23,15,7,62,54,46,38,30,22,
        14,6,61,53,45,37,29,21,13,5,28,20,12,4
};

static const uint8_t PC2_Table[48] = {
        14,17,11,24,1,5,3,28,15,6,21,10,
        23,19,12,4,26,8,16,7,27,20,13,2,
        41,52,31,37,47,55,30,40,51,45,33,48,
        44,49,39,56,34,53,46,42,50,36,29,32
};

static const uint8_t LOOP_Table[16] = {
        1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

static const uint8_t S_Box[8][4][16] = {
        // S1
        14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
        0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
        4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
        15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,
        //S2
        15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
        3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
        0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
        13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,
        //S3
        10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
        13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
        13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
        1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,
        //S4
        7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
        13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
        10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
        3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,
        //S5
        2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
        14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
        4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
        11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
        //S6
        12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
        10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
        9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
        4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,
        //S7
        4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
        13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
        1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
        6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,
        //S8
        13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
        1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
        7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
        2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
};

static bool SubKey[16][48];

void get_key();
void get_IV();

void Des_SetKey(const uint8_t Key[8]);

void F_func(bool In[32], const bool Ki[48]);

void S_func(bool Out[32], const bool In[48]);

void Transform(bool *Out, bool *In, const uint8_t *Table, int len);

void Xor(bool *InA, const bool *InB, int len);

void RotateL(bool *In, int len, int loop);

void ByteToBit(bool *Out, const uint8_t *In, int bits);
void BitToByte(uint8_t *Out, const bool *In, int bits);

bool encrypt(uint8_t BLOCK[8], uint8_t KEY[8]);
bool decrypt(uint8_t BLOCK[8], uint8_t KEY[8]);

void PlusOne(bool *In, int len);

void ECB_encrypt(BMP_IMG input);
void CBC_encrypt(BMP_IMG input);
void CFB_encrypt(BMP_IMG input);
void OFB_encrypt(BMP_IMG input, char * out_path);
void CTR_encrypt(BMP_IMG input, char * out_path);

void ECB_decrypt(BMP_IMG input);
void CBC_decrypt(BMP_IMG input);
void CFB_decrypt(BMP_IMG input);
void OFB_decrypt(BMP_IMG input, char * out_path);
void CTR_decrypt(BMP_IMG input, char * out_path);

void ECB_runtime(BMP_IMG input);
void CBC_runtime(BMP_IMG input);
void CFB_runtime(BMP_IMG input);
void OFB_runtime(BMP_IMG input);
void CTR_runtime(BMP_IMG input);

void get_key() {
    ifstream key_file("Key.txt");
    if (!key_file.is_open()) {
        cout << "Error opening key file";
        exit (1);
    }

    for (int i = 0; i < 8; ++i) {
        key_file >> Key[i];
    }

    key_file.close();
}

void get_IV() {
    ifstream key_file("IV.txt");
    if (!key_file.is_open()) {
        cout << "Error opening key file";
        exit (1);
    }

    for (int i = 0; i < 8; ++i) {
        key_file >> IV[i];
    }

    key_file.close();
}



void Des_SetKey(const uint8_t Key[8]) {
    static bool K[64], *KL = &K[0], *KR = &K[28];
    ByteToBit(K, Key, 64);
    Transform(K, K, PC1_Table, 56);

    for(int i=0; i<16; i++) {
        RotateL(KL, 28, LOOP_Table[i]);
        RotateL(KR, 28, LOOP_Table[i]);
        Transform(SubKey[i], K, PC2_Table, 48);
    }
}



void F_func(bool In[32], const bool Ki[48]) {
    static bool MR[48];
    Transform(MR, In, E_Table, 48);
    Xor(MR, Ki, 48);
    S_func(In, MR);
    Transform(In, In, P_Table, 32);
}

void S_func(bool Out[32], const bool In[48]) {
    for(uint8_t i=0,j,k; i<8; i++,In+=6,Out+=4) {
        j = (In[0]<<1) + In[5];
        k = (In[1]<<3) + (In[2]<<2) + (In[3]<<1) + In[4];
        ByteToBit(Out, &S_Box[i][j][k], 4);
    }
}

void Transform(bool *Out, bool *In, const uint8_t *Table, int len) {
    static bool Tmp[256];

    for(int i = 0; i < len; i++) {
        Tmp[i] = In[ Table[i] - 1 ];
    }

    memcpy(Out, Tmp, len);
}



void Xor(bool *InA, const bool *InB, int len) {
    for(int i = 0; i < len; i++) {
        InA[i] ^= InB[i];
    }
}

void RotateL(bool *In, int len, int loop) {
    static bool Tmp[256];
    memcpy(Tmp, In, loop);
    memcpy(In, In+loop, len-loop);
    memcpy(In+len-loop, Tmp, loop);
}

void ByteToBit(bool *Out, const uint8_t *In, int bits) {
    for(int i = 0; i < bits; i++) {
        Out[i] = (In[i/8] >> (i%8)) & 1;
    }
}

void BitToByte(uint8_t *Out, const bool *In, int bits) {
    memset(Out, 0, (bits+7)/8);

    for(int i = 0; i < bits; i++) {
        Out[i/8] |= In[i] << (i%8);
    }
}

bool encrypt(uint8_t BLOCK[8], uint8_t KEY[8]) {
    Des_SetKey(KEY);
    static bool M[64], Tmp[32], *Li = &M[0], *Ri = &M[32];
    ByteToBit(M, BLOCK, 64);
    Transform(M, M, IP_Table, 64);

    for(int i=0; i<16; i++) {
        memcpy(Tmp, Ri, 32);
        F_func(Ri, SubKey[i]);
        Xor(Ri, Li, 32);
        memcpy(Li, Tmp, 32);
    }

    Transform(M, M, IPR_Table, 64);
    BitToByte(BLOCK, M, 64);
    return true;
}

bool decrypt(uint8_t BLOCK[8], uint8_t KEY[8]) {
    Des_SetKey(KEY);
    static bool M[64], Tmp[32], *Li = &M[0], *Ri = &M[32];
    ByteToBit(M, BLOCK, 64);
    Transform(M, M, IP_Table, 64);

    for (int i = 15; i >= 0; i--) {
        memcpy(Tmp, Li, 32);
        F_func(Li, SubKey[i]);
        Xor(Li, Ri, 32);
        memcpy(Ri, Tmp, 32);
    }

    Transform(M, M, IPR_Table, 64);// 变换
    BitToByte(BLOCK, M, 64);
    return true;
}


void PlusOne(bool *In, int len) {
    if (len > 0) {
        if (In[len - 1] == 1) {
            In[len - 1] = 0;
            PlusOne(In, len - 1);
        } else {
            In[len - 1] = 1;
        }
    }
}

void ECB_encrypt(BMP_IMG input) {
    get_key();

    for (int i = 0; i < input.img_data.size(); i += 8) {
        encrypt(&input.img_data[i], Key);
    }

    input.write_image("ECBencrypted.bmp");
}

void CBC_encrypt(BMP_IMG input) {
    get_key();
    get_IV();

    bool tmp_1[64], tmp_2[64];

    for (int i = 0; i < input.img_data.size(); i += 8) {
        ByteToBit(tmp_1, &input.img_data[i], 64);

        if (i == 0) {
            ByteToBit(tmp_2, IV, 64);
        } else {
            ByteToBit(tmp_2, &input.img_data[i - 8], 64);
        }

        Xor(tmp_1, tmp_2, 64);

        BitToByte(&input.img_data[i], tmp_1, 64);

        encrypt(&input.img_data[i], Key);
    }

    input.write_image("CBCencrypted.bmp");
}

void CFB_encrypt(BMP_IMG input) {
    get_key();
    get_IV();

    bool tmp_1[64], tmp_2[64];
    uint8_t tmp_IV[8], tmp_data[8];

    for (int i = 0; i < input.img_data.size(); i += 8) {
        if (i == 0) {
            for (int j = 0; j < 8; ++j) {
                tmp_IV[j] = IV[j];
            }

            encrypt(tmp_IV, Key);
            ByteToBit(tmp_1, tmp_IV, 64);
        } else {
            for (int j = 0; j < 8; ++j) {
                tmp_data[j] = input.img_data[i - 8 + j];
            }

            encrypt(tmp_data, Key);
            ByteToBit(tmp_1, tmp_data, 64);
        }

        ByteToBit(tmp_2, &input.img_data[i], 64);

        Xor(tmp_1, tmp_2, 64);

        BitToByte(&input.img_data[i], tmp_1, 64);
    }

    input.write_image("CFBencrypted.bmp");
}

void OFB_encrypt(BMP_IMG input, char * out_path) {
    get_key();
    get_IV();

    bool tmp_1[64], tmp_2[64];
    uint8_t tmp_IV[8];

    for (int i = 0; i < 8; ++i) {
        tmp_IV[i] = IV[i];
    }

    BMP_IMG input_copy = input;

    for (int i = 0; i < input.img_data.size(); i += 8) {
        encrypt(tmp_IV, Key);

        for (int j = 0; j < 8; ++j) {
            input.img_data[i + j] = tmp_IV[j];
        }
    }

    for (int i = 0; i < input.img_data.size(); i += 8) {
        ByteToBit(tmp_1, &input.img_data[i], 64);
        ByteToBit(tmp_2, &input_copy.img_data[i], 64);
        Xor(tmp_1, tmp_2, 64);
        BitToByte(&input.img_data[i], tmp_1, 64);
    }

    input.write_image(out_path);
}


void CTR_encrypt(BMP_IMG input, char * out_path) {
    get_key();
    get_IV();

    bool tmp_1[64], tmp_2[64];
    uint8_t tmp_IV[8], tmp_IV_tmp[8];

    for (int i = 0; i < 8; ++i) {
        tmp_IV[i] = IV[i];
        tmp_IV_tmp[i] = IV[i];
    }

    for (int i = 0; i < input.img_data.size(); i += 8) {
        for (int i = 0; i < 8; ++i) {
            tmp_IV_tmp[i] = tmp_IV[i];
        }

        encrypt(tmp_IV_tmp, Key);
        ByteToBit(tmp_1, &input.img_data[i], 64);
        ByteToBit(tmp_2, tmp_IV_tmp, 64);

        Xor(tmp_1, tmp_2, 64);
        BitToByte(&input.img_data[i], tmp_1, 64);

        ByteToBit(tmp_1, tmp_IV, 64);
        PlusOne(tmp_1, 64);
        BitToByte(tmp_IV, tmp_1, 64);
    }

    input.write_image(out_path);
}


void ECB_decrypt(BMP_IMG input) {
    get_key();

    for (int i = 0; i < input.img_data.size(); i += 8) {
        decrypt(&input.img_data[i], Key);
    }

    input.write_image("ECBdecrypted.bmp");
}

void CBC_decrypt(BMP_IMG input) {
    get_key();
    get_IV();

    bool tmp_1[64], tmp_2[64];

    BMP_IMG input_copy = input;

    for (int i = 0; i < input.img_data.size(); i += 8) {
        decrypt(&input.img_data[i], Key);

        ByteToBit(tmp_1, &input.img_data[i], 64);

        if (i == 0) {
            ByteToBit(tmp_2, IV, 64);
        } else {
            ByteToBit(tmp_2, &input_copy.img_data[i - 8], 64);
        }

        Xor(tmp_1, tmp_2, 64);

        BitToByte(&input.img_data[i], tmp_1, 64);
    }

    input.write_image("CBCdecrypted.bmp");
}

void CFB_decrypt(BMP_IMG input) {
    get_key();
    get_IV();

    bool tmp_1[64], tmp_2[64];
    uint8_t tmp_IV[8], tmp_data[8], tmp_ciphertext[8];

    for (int i = 0; i < 8; ++i) {
        tmp_IV[i] = IV[i];
        tmp_ciphertext[i] = input.img_data[i];
    }

    for (int i = 0; i < input.img_data.size(); i += 8) {
        for (int j = 0; j < 8; ++j) {
            tmp_data[j] = tmp_ciphertext[j];
        }

        for (int j = 0; j < 8; ++j) {
            tmp_ciphertext[j] = input.img_data[i + j];
        }

        if (i == 0) {
            encrypt(tmp_IV, Key);
            ByteToBit(tmp_1, tmp_IV, 64);
        } else {
            encrypt(tmp_data, Key);
            ByteToBit(tmp_1, tmp_data, 64);
        }

        ByteToBit(tmp_2, tmp_ciphertext, 64);

        Xor(tmp_1, tmp_2, 64);

        BitToByte(&input.img_data[i], tmp_1, 64);
    }

    input.write_image("CFBdecrypted.bmp");
}

void OFB_decrypt(BMP_IMG input, char * out_path) {
    OFB_encrypt(input, out_path);
}

void CTR_decrypt(BMP_IMG input, char * out_path) {
    CTR_encrypt(input, out_path);
}


void ECB_runtime(BMP_IMG input) {
    clock_t start,end;
    start = clock();

    ECB_encrypt(input);

    char * path = "ECBencrypted.bmp";
    BMP_IMG IMAGE;
    if (IMAGE.read_image(path)) {
        printf("ECB reading success!\n");
    }

    ECB_decrypt(IMAGE);

    end = clock();
    printf("ECB Runtime: %lf ms\n",((double)((end - start) * 1000 / CLOCKS_PER_SEC)));
}

void CBC_runtime(BMP_IMG input) {
    clock_t start,end;
    start = clock();

    CBC_encrypt(input);

    char * path = "CBCencrypted.bmp";
    BMP_IMG IMAGE;
    if (IMAGE.read_image(path)) {
        printf("CBC reading success!\n");
    }

    CBC_decrypt(IMAGE);

    end = clock();
    printf("CBC Runtime: %lf ms\n",((double)((end - start) * 1000 / CLOCKS_PER_SEC)));
}

void CFB_runtime(BMP_IMG input) {
    clock_t start,end;
    start = clock();

    CFB_encrypt(input);

    char * path = "CFBencrypted.bmp";
    BMP_IMG IMAGE;
    if (IMAGE.read_image(path)) {
        printf("CFB reading success!\n");
    }

    CFB_decrypt(IMAGE);

    end = clock();
    printf("CFB Runtime: %lf ms\n",((double)((end - start) * 1000 / CLOCKS_PER_SEC)));
}

void OFB_runtime(BMP_IMG input) {
    clock_t start,end;
    start = clock();

    OFB_encrypt(input, "OFBencrypted.bmp");

    char * path = "OFBencrypted.bmp";
    BMP_IMG IMAGE;
    if (IMAGE.read_image(path)) {
        printf("OFB reading success!\n");
    }

    OFB_decrypt(IMAGE, "OFBdecrypted.bmp");

    end = clock();
    printf("OFB Runtime: %lf ms\n",((double)((end - start) * 1000 / CLOCKS_PER_SEC)));
}

void CTR_runtime(BMP_IMG input) {
    clock_t start,end;
    start = clock();



    char * path = "CTRencrypted.bmp";
    CTR_encrypt(input, path);
    BMP_IMG IMAGE;
    if (IMAGE.read_image(path)) {
        printf("CTR reading success!\n");
    }

    path = "CTRdecrypted.bmp";

    CTR_decrypt(IMAGE, path);

    end = clock();
    printf("CTR Runtime: %lf ms\n",((double)((end - start) * 1000 / CLOCKS_PER_SEC)));
}







int main() {
    BMP_IMG IMAGE;
    char * path_in = "test.bmp";
    if (IMAGE.read_image(path_in)) {
        printf("Reading success!\n");
    }

    ECB_runtime(IMAGE);

    CBC_runtime(IMAGE);

    CFB_runtime(IMAGE);

    OFB_runtime(IMAGE);

    CTR_runtime(IMAGE);

    char * path_out = "output.bmp";
    if (IMAGE.write_image(path_out)) {
        printf("Writing success!\n");
    }
    return 0;
}