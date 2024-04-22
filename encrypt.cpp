//
// Created by me on 4/12/2024.
//
#include "encrypt.h"
#include <random>
#include <inttypes.h>
typedef std::mt19937 MyRNG;
uint32_t seed_val = 0x19EE448F;
MyRNG rng;
void initialize()
{
    rng.seed(seed_val);
}

//return for debugging
uint_fast32_t* splitCipher(uint_fast32_t* in) {
    uint_fast32_t* temp = in;
    uint_fast32_t ciphersplit[4] = {0};
    std::uniform_int_distribution<std::mt19937::result_type> gen(0,1);
    for(uint_fast32_t quarter : ciphersplit){
        for(int i = 0; i < 32; i++){
            int bit = gen(rng);
            quarter = quarter | bit; //last bit of quarter to rng bit
            quarter = (quarter << 1); // shift left by 1
        }
        *temp = quarter;
        temp++;
        //printf("checking: %" PRIu32 "\n", quarter);
    }
    return in;
}

uint_fast32_t *makeRoundKey(uint_fast32_t *key, int round) {
    uint_fast32_t temp[4];
    uint_fast32_t *copy = temp;//copy key for xor to generate new key later
    for(int i = 0; i < 4; i++){
        copy[i] = key[i];
    }
    substitute(key);
    rotate(key);
    rcon(key, round);
    key[0] = copy[0] ^ key[3];
    key[1] = key[0] ^ copy[1];
    key[2] = key[1] ^ copy[2];
    key[3] = key[2] ^ copy[3];
    return key;
}
uint_fast32_t *rotate(uint_fast32_t *key) {
    for(int i = 0; i < 4; i++){
        uint_fast32_t mask = num::MAX;
        mask &= key[i];
        key[i] = (key[i] << (8*i) ) | (mask >> (32 - (8*i) ) );//rotate each word left by i bytes
    }
    return key;
}

uint_fast32_t* rcon(uint_fast32_t *key, int round) {
    uint_fast8_t *byte = (uint_fast8_t *)key;//pragye
    for(int i = 0; i < 16; i++){
        byte[i] = byte[i] ^ num::roundTable[round - 1];//a constant derived from current round is XOR'd with the bytes in the word
    }
    return key;
}

uint_fast32_t *mix(uint_fast32_t *state) {
    //https://crypto.stackexchange.com/questions/2569/how-does-one-implement-the-inverse-of-aes-mixcolumns
    //row * column:   A.B = C
    uint_fast8_t *stateByte = (uint_fast8_t *)state;//pragye
    uint_fast8_t temparr[4][4] = {0};
    uint_fast8_t multarr[4] = {0};
    for(int i = 0; i < 4; i ++){
        for(int j = 0 ; j < 4; j++){
            for(int k = 0; k < 4; k++){
                //column indexes: (0, 4, 8, 12), (1, 5, 9, 13), (2, 6, 10, 14), (3, 7, 11, 15)
                multarr[k] = stateByte[(4*k) + j];
                uint_fast8_t mask = 0xff & multarr[k];
                multarr[k] =  (multarr[k] << (num::GFfield[j][k] == 0x2 || num::GFfield[j][k] == 0x3) ) ^ (mask * (num::GFfield[j][k] == 0x3));
                multarr[k] ^= (0x1b * ((mask & 0x80 ) > 0x00)); //xor by 0x1b if leftmost bit was 1 before multiplication
            }
            temparr[i][j] = multarr[0] ^ multarr[1] ^ multarr[2] ^ multarr[3];
        }
    }
    for(int i = 0; i < 4; i++){
        uint_fast32_t temp = 0x0, mask32 = 0x0;
        for(int j = 0; j < 4; j++){
            mask32 |= temp;
            temp |= temparr[i][j];
            temp = (temp << 8) | (mask32 >> 24);
        }
        state[i] = temp;
    }
    return state;
}

uint_fast32_t *substitute(uint_fast32_t *key) {

    uint_fast8_t *byte = (uint_fast8_t *)key;//pragye
    for(int i = 0; i < 16; i++){
        byte[i] = lookup(byte[i]);
    }
    return key;
}

uint_fast8_t lookup(uint_fast8_t &byte) {
    unsigned int frontNibble = 0xF0, rearNibble = 0x0F;
    frontNibble = (frontNibble & byte) >> 4;
    rearNibble = rearNibble & byte;
    return num::table[frontNibble][rearNibble];
}

uint_fast32_t *addRoundKey(const uint_fast32_t *key, uint_fast32_t *text) {
    for (int i = 0; i < 4; i++){
        text[i] = key[i] ^ text[i];
    }
    return text;
}

uint_fast32_t *processText(uint_fast32_t *state) {
    substitute(state);
    rotate(state);
    mix(state);
    return state;
}

uint_fast32_t *encrypt(const uint_fast32_t *key, uint_fast32_t *text) {
    uint_fast32_t copy[4];//do not change original key so we can send to recipient
    uint_fast32_t* next = copy;//for pointer reasons lol
    for(int i = 0; i < 4; i++){
        next[i] = key[i];//deep copy key
    }

    uint_fast32_t* roundKeys[11];//put key into list
    roundKeys[0] = copy;
    addRoundKey(copy, text);//0th round for AES 128
    for (int i = 1; i < 11; i++){
        roundKeys[i] = makeRoundKey(roundKeys[i - 1], i);//make key for current round from previous key
        processText(text);//do encryption things on text
        addRoundKey(roundKeys[i], text);//xor current round key with text
    }
    return text;
}



