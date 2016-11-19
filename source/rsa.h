#ifndef _RSA_H_
#define _RSA_H_

#include "mini-gmp.h"
#include <string.h>

#define RSA_BITS 768

#define RSA_BYTES (RSA_BITS/8)

void rsa_decrypt(unsigned char *input, unsigned char *output);
void rsa_encrypt(unsigned char *input, unsigned char *output);

#endif //_RSA_H_