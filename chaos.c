#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

uint32_t rotl32(uint32_t value, uint32_t count) {
    const uint32_t mask = (CHAR_BIT*sizeof(value)-1);
    count &= mask;
    return (value<<count) | (value>>( (-count) & mask ));
}

uint32_t rotr32(uint32_t value, uint32_t count) {
    const uint32_t mask = (CHAR_BIT*sizeof(value)-1);
    count &= mask;
    return (value>>count) | (value<<( (-count) & mask ));
}

void hash(uint32_t *input, uint32_t len, uint32_t output[4])
{
    uint32_t x, y, z, u, X, Y, Z, U, A, B, C, D, RV1, RV2, RV3, RV4;
    uint32_t M = 0xffff;
    int i, offset;

    x = 0x0124fdce; y = 0x89ab57ea; z = 0xba89370a; u = 0xfedc45ef;
    A = 0x401ab257; B = 0xb7cd34e1; C = 0x76b3a27c; D = 0xf13c3adf;
    RV1 = 0xe12f23cd; RV2 = 0xc5ab6789; RV3 = 0xf1234567; RV4 = 0x9a8bc7ef;

    for (i=0; i < len; ++i) {
        offset = 4*i;
        X = input[offset + 0] ^ x;
        Y = input[offset + 1] ^ y;
        Z = input[offset + 2] ^ z;
        U = input[offset + 3] ^ u;
        
        /* compute chaos */
        x = (X & 0xffff)*(M-(Y>>16)) ^ rotl32(Z,1) ^ rotr32(U,1) ^ A;
        y = (Y & 0xffff)*(M-(Z>>16)) ^ rotl32(U,2) ^ rotr32(X,2) ^ B;
        z = (Z & 0xffff)*(M-(U>>16)) ^ rotl32(X,3) ^ rotr32(Y,3) ^ C;
        u = (U & 0xffff)*(M-(X>>16)) ^ rotl32(Y,4) ^ rotr32(Z,4) ^ D;
        
        RV1 ^= x; RV2 ^= y; RV3 ^= z; RV4 ^= u;
    }

    /* now run 4 more times */
    for (i=0; i < 4; ++i) {
        X = x; Y = y; Z = z; U = u;
        
        /* compute chaos */        
        
        x = (X & 0xffff)*(M-(Y>>16)) ^ rotl32(Z,1) ^ rotr32(U,1) ^ A;
        y = (Y & 0xffff)*(M-(Z>>16)) ^ rotl32(U,2) ^ rotr32(X,2) ^ B;
        z = (Z & 0xffff)*(M-(U>>16)) ^ rotl32(X,3) ^ rotr32(Y,3) ^ C;
        u = (U & 0xffff)*(M-(X>>16)) ^ rotl32(Y,4) ^ rotr32(Z,4) ^ D;
        
        RV1 ^= x; RV2 ^= y; RV3 ^= z; RV4 ^= u;
     }

     output[0] = RV1; output[1] = RV2; output[2] = RV3; output[3] = RV4;
}

int main(int argc, char **argv)
{
    uint32_t hash1[4];
    uint32_t hash2[4];

    const char *text1 = "hello";
    const char *text2 = "world";

    uint32_t *input1 = (uint32_t *) text1;
    uint32_t *input2 = (uint32_t *) text2;

    uint32_t len1 = strlen(text1);
    uint32_t len2 = strlen(text2);

    hash(input1, len1, hash1);
    printf("%s = %x%x%x%x\n", text1, hash1[0], hash1[1], hash1[2], hash1[3]);

    hash(input2, len2, hash2);
    printf("%s = %x%x%x%x\n", text2, hash2[0], hash2[1], hash2[2], hash2[3]);
    
    return 0;
}