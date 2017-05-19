//
// Created by stybill on 18/05/2017.
//

#ifndef CODE_DES_H
#define CODE_DES_H

class DES {
public:
    bool encrypt(uint8_t BLOCK[8], uint8_t KEY[7]);
    bool decrypt(uint8_t BLOCK[8], uint8_t KEY[7]);
};


#endif //CODE_DES_H
