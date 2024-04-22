#include <iostream>
#include <inttypes.h>
#include <fstream>
#include "encrypt.h"
int main() {
    uint_fast32_t cipher[4] = {0};
    uint_fast32_t *head = splitCipher(cipher);
    uint_fast32_t *plaintext;
    uint_fast32_t plain[4];
    plaintext = plain;
    uint_fast8_t chararr[16];
    uint_fast8_t *pt = chararr;
    std::string text = "helloworldiambob";
    for(int i =0; i < 16; i++){
        chararr[i] = (uint_fast8_t)text[i];
        //printf("%" PRIx8, chararr[i]);
        if( (i + 1) % 4 == 0){
            //std::cout << std::endl;
            plain[i/4] = chararr[i] | (chararr[i - 1] << 8) | (chararr[i - 2] << 16) | (chararr[i-3] << 24);//correct order
        }
    }
    std::cout << std::endl;
    std::cout << "input check" << std::endl;
    for(int i =0; i < 16; i++){
        std::cout << (char)pt[i];
    }
    std::cout << std::endl;
    for(int i = 0; i < 4; i++){
        printf("%08" PRIx32 "\n", plaintext[i]);
    }
    return 0;
}
