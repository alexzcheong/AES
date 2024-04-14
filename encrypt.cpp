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
            std::cout << bit;
            quarter = quarter | bit; //last bit of quarter to rng bit
            quarter = (quarter << 1) | (quarter >> 31); // rot left by 1
        }
        *temp = quarter;
        temp++;
        //printf("checking: %" PRIu32 "\n", quarter);
    }
    return in;
}