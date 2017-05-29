#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
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

void chaos_hash(uint32_t *input, uint32_t len, uint32_t output[4])
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

/**
 * Compute a suffix for the input such that the values of X, Y, Z, U
 * will match the supplied values in the last iteration.
 *
 * Controlling the values of X, Y, Z, U on the last iteration lets us
 * control the value of the hash.
 *
 * Put another way, for a given set of values _X, _Y, _Z, _U, we can compute
 * a suitable suffix for any input such that they all hash to the same
 * value.
 */
void compute_suffix(
    uint32_t _X, uint32_t _Y, uint32_t _Z, uint32_t _U,
    uint32_t *input, uint32_t len,
    uint32_t suffix[4])
{
    uint32_t x, y, z, u, X, Y, Z, U, A, B, C, D, RV1, RV2, RV3, RV4;
    uint32_t M = 0xffff;
    int i, offset;

    x = 0x0124fdce; y = 0x89ab57ea; z = 0xba89370a; u = 0xfedc45ef;
    A = 0x401ab257; B = 0xb7cd34e1; C = 0x76b3a27c; D = 0xf13c3adf;

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

        if (i == len) {
            suffix[0] = _X ^ x;
            suffix[1] = _Y ^ y;
            suffix[2] = _Z ^ z;
            suffix[3] = _U ^ u;
        }
    }
}

void hexprint(uint32_t *input, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        printf("%08x", input[i]);
    }
}

void print_hash(uint32_t *input, uint32_t len)
{
    uint32_t hash[4];
    chaos_hash(input, len, hash);
    printf("chaos_hash(");
    hexprint(input, len);
    printf(") = ");
    hexprint(hash, 4);
    printf("\n\n");
}

/**
 * Fill a buffer with random bytes.
 */
void random_fill(uint32_t *target, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        target[i] = rand();
    }
}

int main(int argc, char **argv)
{
    uint32_t input1[36];
    uint32_t input2[36];

    uint32_t len1 = sizeof(input1) / sizeof(uint32_t);
    uint32_t len2 = sizeof(input1) / sizeof(uint32_t);

    random_fill(input1, len1 - 4);
    random_fill(input2, len2 - 4);
    
    print_hash(input1, len1 - 4);
    print_hash(input2, len2 - 4);

    compute_suffix(0, 0, 0, 0, input1, len1 - 4, input1 + len1 - 4);    
    compute_suffix(0, 0, 0, 0, input2, len2 - 4, input2 + len2 - 4);

    print_hash(input1, len1);
    print_hash(input2, len2);
    
    return 0;
}