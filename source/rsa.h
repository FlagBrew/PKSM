#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "mini-gmp.h"

#define MODULUS_SIZE 768                   /* This is the number of bits we want in the modulus */
#define BLOCK_SIZE (MODULUS_SIZE/8)         /* This is the size of a block that gets en/decrypted at once */
#define BUFFER_SIZE ((MODULUS_SIZE/8) / 2)  /* This is the number of bytes in n and p */

typedef struct {
    mpz_t n; /* Modulus */
    mpz_t e; /* Public Exponent */
} public_key;

typedef struct {
    mpz_t n; /* Modulus */
    mpz_t e; /* Public Exponent */
    mpz_t d; /* Private Exponent */
} private_key;

void generate_keys(private_key* ku, public_key* kp);
void block_encrypt(mpz_t C, mpz_t M, public_key kp);
void block_decrypt(mpz_t M, mpz_t C, private_key ku);
