/*
* This file is part of PKSM
* Copyright (C) 2016 Bernardo Giordano
*
* Code is ported from original implementation of memecrypto, by SciresM and Kaphotics
*
* This software is provided 'as-is', 
* without any express or implied warranty. 
* In no event will the authors be held liable for any damages 
* arising from the use of this software.
*
* This code is subject to the following restrictions:
*
* 1) The origin of this software must not be misrepresented; 
* 2) You must not claim that you wrote the original software. 
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "memecrypto.h"
#include "aes.h"
#include "sha256.h"
#include "sha1.h"
#include "mini-gmp.h"
#include "rsa.h"

void sha256(u8 hash[], u8 data[], size_t len) {
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, len);
	sha256_final(&ctx,hash);
}

void sha1(u8 hash[], u8 data[], size_t len) {
	SHA1_CTX ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, data, len);
	sha1_final(&ctx, hash);
}

// TO DO
// RSADecrypt (only if I don't reach to find ad-hoc libraries to this)
// RSAEncrypt (only if I don't reach to find ad-hoc libraries to this)
// ReverseCrypt
// finish resign

void RSADecrypt(u8 data[], u8 output[]) {
    mpz_t M;  mpz_init(M);
    mpz_t C;  mpz_init(C);
    mpz_t DC;  mpz_init(DC);
	
    private_key ku;
    public_key kp;

    mpz_init(kp.n);
    mpz_init(kp.e); 
    mpz_init(ku.n); 
    mpz_init(ku.e); 
    mpz_init(ku.d);
	generate_keys(&ku, &kp);
	
	mpz_import(M, sizeof(&data), 1, sizeof(data[0]), 0, 0, data);
	block_decrypt(DC, C, ku);
	memcpy(output, (u8*)mpz_get_str(NULL, 16, DC), sizeof(&output));
}

void RSAEncrypt(u8 data[], u8 output[]) {
    mpz_t M;  mpz_init(M);
    mpz_t C;  mpz_init(C);
	
    private_key ku;
    public_key kp;

    mpz_init(kp.n);
    mpz_init(kp.e); 
    mpz_init(ku.n); 
    mpz_init(ku.e); 
    mpz_init(ku.d);
	generate_keys(&ku, &kp);
	mpz_import(M, sizeof(&data), 1, sizeof(data[0]), 0, 0, data);
	block_encrypt(C, M, kp);
	memcpy(output, (u8*)mpz_get_str(NULL, 16, C), sizeof(&output));
}

void Xor(u8 b1[], u8 b2[], u8 output[]) {
	int sz = sizeof(&b1);
	for (int i = 0; i < sz; i++) { // assuming b1 has the same size of b2
		output[i] = (b1[i] ^ b2[i]);
	}
}

void MemeCryptoAESEncrypt(u8 key[], u8 data[], u8 output[]) { // output has to be the same size of data
	int sz = sizeof(&data);
	u8 temp[0x10];
	u8 subkey[0x10];

	memset(temp, 0, 0x10); // maybe?
	memset(subkey, 0, 0x10); // maybe?
	
	for (int i = 0; i < sz / 0x10; i++) {
		u8 curblock[0x10];
		memcpy(&curblock[0], &data[i * 0x10], 0x10);
		Xor(temp, curblock, temp);
		AES128_ECB_encrypt(temp, key, temp);
		memcpy(&output[i * 0x10], temp, 0x10);
	}
	
	u8 temp1[0x10];
	memcpy(temp1, &output[0], 0x10);
	Xor(temp, temp1, temp);
	for (int ofs = 0; ofs < 0x10; ofs += 2) {
		u8 b1 = temp[ofs + 0];
		u8 b2 = temp[ofs + 1];
		
		subkey[ofs + 0] = (2 * b1 + (b2 >> 7));
		subkey[ofs + 1] = (2 * b2);
		if (ofs + 2 < 0x10)
			subkey[ofs + 1] += (temp[ofs + 2] >> 7);
	}
	if ((temp[0] & 0x80) != 0)
		subkey[0xF] ^= 0x87;

	memset(temp, 0, 0x10);
	for (int i = 0; i < sz / 0x10; i++) {
		u8 curblock[0x10];
		u8 temp2[0x10];
		u8 temp3[0x10];
		
		memcpy(curblock, &output[(sz / 0x10 - 1 - i) * 0x10], 0x10);
		Xor(curblock, subkey, temp2);
		AES128_ECB_encrypt(temp2, key, temp3);
		Xor(temp3, temp, temp3);
		memcpy(&output[(sz / 0x10 - 1 - i) * 0x10], temp3, 0x10);
		memcpy(temp, temp2, 0x10);	
	}
}

void MemeCryptoAESDecrypt(u8 key[], u8 data[], u8 output[]) { // output has to be the same size of data
	int sz = sizeof(&data);
	u8 temp[0x10];
	u8 subkey[0x10];

	memset(temp, 0, 0x10); // maybe?
	memset(subkey, 0, 0x10); // maybe?
	
	for (int i = 0; i < sz / 0x10; i++) {
		u8 curblock[0x10];
		memcpy(&curblock[0], &data[(sz / 0x10 - 1 - i) * 0x10], 0x10);
		Xor(temp, curblock, temp);
		AES128_ECB_decrypt(temp, key, temp);
		memcpy(&output[(sz / 0x10 - 1 - i) * 0x10], temp, 0x10);
	}
	
	memcpy(temp, &output[(sz / 0x10 - 1) * 0x10], 0x10);
	u8 temp1[0x10];
	memcpy(temp1, &output[0], 0x10);
	Xor(temp, temp1, temp);
	for (int ofs = 0; ofs < 0x10; ofs += 2) {
		u8 b1 = temp[ofs + 0];
		u8 b2 = temp[ofs + 1];
		
		subkey[ofs + 0] = (2 * b1 + (b2 >> 7));
		subkey[ofs + 1] = (2 * b2);
		if (ofs + 2 < 0x10)
			subkey[ofs + 1] += (temp[ofs + 2] >> 7);
	}
	if ((temp[0] & 0x80) != 0)
		subkey[0xF] ^= 0x87;
	
	for (int i = 0; i < sz / 0x10; i++) {
		u8 curblock[0x10];
		u8 temp[0x10];
		
		memcpy(curblock, &output[0x10 * i], 0x10);
		Xor(curblock, subkey, temp);
		memcpy(&output[0x10 * i], temp, 0x10);
	}

	memset(temp, 0, 0x10);
	for (int i = 0; i < sz / 0x10; i++) {
		u8 curblock[0x10];
		u8 temp2[0x10];

		memcpy(curblock, &output[i * 0x10], 0x10);
		AES128_ECB_decrypt(curblock, key, temp2);
		Xor(temp2, temp, temp2);
		memcpy(&output[i * 0x10], temp2, 0x10);
		memcpy(temp, curblock, 0x10);
	}
}

void resign(u8* mainbuf) {
	u8 outSav[0x6BE00];
	u8 CurSig[0x80];
	u8 ChecksumTable[0x140];
	u8 Hash[SHA256_BLOCK_SIZE];
	
	memcpy(&outSav[0], mainbuf, 0x6BE00);
	memcpy(&CurSig[0], &mainbuf[0x6BB00], 0x80);
	memcpy(&ChecksumTable[0], &mainbuf[0x6BC00], 0x140);
	sha256(Hash, ChecksumTable, 0x140);
	
	u8 PubKeyDer[] = "307C300D06092A864886F70D0101010500036B003068026100B61E192091F90A8F76A6EAAA9A3CE58C863F39AE253F037816F5975854E07A9A456601E7C94C29759FE155C064EDDFA111443F81EF1A428CF6CD32F9DAC9D48E94CFB3F690120E8E6B9111ADDAF11E7C96208C37C0143FF2BF3D7E831141A9730203010001";
	
	u8 keybuf[sizeof(PubKeyDer) + 0x20];
	memcpy(&keybuf[0], PubKeyDer, sizeof(PubKeyDer));
	memcpy(&keybuf[sizeof(PubKeyDer)], &Hash[0], 0x20);
	u8 temp[SHA1_BLOCK_SIZE];
	sha1(temp, keybuf, sizeof(keybuf));
	u8 key[0x10];
	memcpy(key, &temp[0], 0x10);
	
	// continue from 264;
}