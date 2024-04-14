#include <iostream>
#include <inttypes.h>
#include <fstream>
#include "encrypt.h"
int main() {
    uint_fast32_t cipher[4] = {0};
    uint_fast8_t bytearr[4][4];
    uint_fast32_t round_keys[11] = {0};

    uint_fast32_t *head = splitCipher(cipher);
    for(int i = 0; i < 4; i++){
        printf("%" PRIu32 "\n", head[i]);
    }
    return 0;
}
