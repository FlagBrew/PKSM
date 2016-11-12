#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "memecrypto.h"
#include "aes.h"
#include "sha256.h"
#include "sha1.h"

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
// MemeCryptoAESEncrypt
// MemeCryptoAESDecrypt
// RSADecrypt (only if I don't reach to find ad-hoc libraries to this)
// RSAEncrypt (only if I don't reach to find ad-hoc libraries to this)
// ReverseCrypt

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
	
	// continue from 252;
}