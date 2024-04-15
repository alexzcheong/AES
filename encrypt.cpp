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

uint_fast32_t *makeRoundKey(uint_fast32_t *key) {
    substitute(key);
    rotate(key);
    mix(key);
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

uint_fast32_t *mix(uint_fast32_t *key) {
    //wtf is a hill cypher
    return nullptr;
}

uint_fast32_t *substitute(uint_fast32_t *key) {
    uint_fast8_t *byte = (uint_fast8_t *)key;//pragye
    for(int i = 0; i < sizeof(key) / sizeof(byte); i++){
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

uint_fast32_t *encrypt(const uint_fast32_t *key, uint_fast32_t *text) {
    uint_fast32_t copy[4];//do not change original key so we can send to recipient
    uint_fast32_t* next = copy;//for pointer reasons lol
    for(int i = 0; i < 4; i++){
        next[i] = key[i];
    }
    for (int i = 0; i < 11; i++){
        text = addRoundKey(next, text);
        next = makeRoundKey(next);
    }
    return text;
}
