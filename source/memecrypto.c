#include "memecrypto.h"

#define PUBKEYDER_LEN 0x7E

void sha256(u8 hash[], u8 data[], size_t len) {
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, len);
	sha256_final(&ctx,hash);
}

static unsigned char pubkeyder[] = {
    0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05,
    0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A,
    0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C, 0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03,
    0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A, 0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29,
    0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1, 0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42,
    0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E, 0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E,
    0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C, 0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F,
    0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01
};

void xor(unsigned char *in, unsigned char *b, int len)
{
    for (int i = 0; i < len; i++)
        in[i] ^= b[i];
}

void memecrypto_aes_encrypt(unsigned char *buf, unsigned char * output, unsigned char *key)
{
    unsigned char temp[0x10];
    unsigned char temp2[0x10];
    unsigned char subkey[0x10];
    
    unsigned char block[0x10];
    
    for (int i = 0; i < 0x10; i++)
        temp[i] = 0;
    
    for (int i = 0; i < MEME_LEN/0x10; i++) // CBC
    {
        memcpy(block,  buf+i*0x10, 0x10);
        xor(block, temp, 0x10);
        AES128_ECB_encrypt(block, key, temp);
        memcpy(output+i*0x10, temp, 0x10);
    }
    
    // CMAC
    xor(temp, output, 0x10);
    for (int i = 0; i < 0x10; i += 2)
    {
        unsigned char b1 = temp[i];
        unsigned char b2 = temp[i+1];
        subkey[i] = (b1 << 1) | (b2 >> 7);
        subkey[i+1] = (b2 << 1);
        if (i + 2 < 0x10)
            subkey[i+1] |= temp[i+2] >> 7;
    }
    if (temp[0] & 0x80)
        subkey[0xF] ^= 0x87;
    
    for (int i = 0; i < 0x10; i++)
        temp[i] = 0;
    
    for (int i = 0; i < MEME_LEN/0x10; i++) // PBC
    {
        memcpy(block, output + ((MEME_LEN/0x10 - i - 1) * 0x10), 0x10);
        xor(block, subkey, 0x10);
        AES128_ECB_encrypt(block, key, temp2);
        xor(temp2, temp, 0x10);
        memcpy(output + ((MEME_LEN/0x10 - i - 1) * 0x10), temp2, 0x10);
        memcpy(temp, block, 0x10);
    }
}

void memecrypto_aes_decrypt(unsigned char *buf, unsigned char *output, unsigned char *key)
{
    unsigned char temp[0x10];
    unsigned char subkey[0x10];    
    unsigned char block[0x10];
    
    for (int i = 0; i < 0x10; i++)
        temp[i] = 0;

    for (int i = 0; i < MEME_LEN/0x10; i++) // PBC
    {
        memcpy(block, buf + ((MEME_LEN/0x10 - i - 1) * 0x10), 0x10);
        xor(block, temp, 0x10);
        AES128_ECB_decrypt(block, key, temp);
        memcpy(output + ((MEME_LEN/0x10 - i - 1) * 0x10), temp, 0x10);
    }
    
    memcpy(temp, output + ((MEME_LEN/0x10 - 1) * 0x10), 0x10);
    
    // CMAC
    xor(temp, output, 0x10);
    for (int i = 0; i < 0x10; i += 2)
    {
        unsigned char b1 = temp[i];
        unsigned char b2 = temp[i+1];
        subkey[i] = (b1 << 1) | (b2 >> 7);
        subkey[i+1] = (b2 << 1);
        if (i + 2 < 0x10)
            subkey[i+1] |= temp[i+2] >> 7;
    }
    if (temp[0] & 0x80)
        subkey[0xF] ^= 0x87;
    
    for (int i = 0; i < MEME_LEN/0x10; i++)
    {
        xor(output + i * 0x10, subkey, 0x10);
    }
    
    for (int i = 0; i < 0x10; i++)
        temp[i] = 0;

    for (int i = 0; i < MEME_LEN/0x10; i++) // CBC
    {
        memcpy(block, output+i*0x10, 0x10);
        AES128_ECB_decrypt(block, key, output + i*0x10);
        xor(output + i*0x10, temp, 0x10);
        memcpy(temp, block, 0x10);
    }
}

Result memecrypto_sign(unsigned char *input, unsigned char *output, int len)
{
    if (len < MEME_LEN)
        return 0;
    
    unsigned char memebuf[MEME_LEN];
    
    unsigned char hash[0x14];
    sha1_ctx sha1[1];
    
    memcpy(output, input, len - MEME_LEN);
    
    sha1_begin(sha1);
    sha1_hash(input, len - 8, sha1);
    sha1_end(hash, sha1);
    memcpy(input + len - 8, hash, 8); // Update SHA1 hash
    
    sha1_begin(sha1);
    sha1_hash(pubkeyder, PUBKEYDER_LEN, sha1);
    if (len > MEME_LEN)
        sha1_hash(input, len - MEME_LEN, sha1);
    sha1_end(hash, sha1); // Hash is now aes key
    
    memcpy(memebuf, input + (len - MEME_LEN), MEME_LEN);
    memecrypto_aes_encrypt(memebuf, memebuf, hash);
    memebuf[0x0] &= 0x7F;
    rsa_decrypt(memebuf, output + (len - MEME_LEN));
    
    return 1;
}

int memecrypto_verify(unsigned char *input, unsigned char *output, int len)
{
    if (len < MEME_LEN)
        return 0;
    
    unsigned char memebuf_1[MEME_LEN];
    unsigned char memebuf_2[MEME_LEN];
    unsigned char hash[0x14];
    sha1_ctx sha1[1];

    memcpy(output, input, len - MEME_LEN);
    
    sha1_begin(sha1);
    sha1_hash(pubkeyder, PUBKEYDER_LEN, sha1);
    if (len > MEME_LEN)
        sha1_hash(input, len - MEME_LEN, sha1);
    sha1_end(hash, sha1); // Hash is now aes key
    
    rsa_encrypt(input + (len - MEME_LEN), memebuf_1);
    memcpy(memebuf_2, memebuf_1, MEME_LEN);
    memebuf_2[0] |= 0x80;
    
    memecrypto_aes_decrypt(memebuf_1, memebuf_1, hash);
    memecrypto_aes_decrypt(memebuf_2, memebuf_2, hash);
    
    memcpy(output + (len - MEME_LEN), memebuf_1, MEME_LEN);
    sha1_begin(sha1);
    sha1_hash(output, len - 8, sha1);
    sha1_end(hash, sha1);
    
    if (!memcmp(hash, output + len - 8, 8))
        return 1;
    
    memcpy(output + (len - MEME_LEN), memebuf_2, MEME_LEN);
    sha1_begin(sha1);
    sha1_hash(output, len - 8, sha1);
    sha1_end(hash, sha1);
    
    if (!memcmp(hash, output + len - 8, 8))
        return 1;

    return 0;
}

void reverseCrypt(u8 input[], u8 output[]) {
	u8 enc[0x60];
	memcpy(enc, &input[0x20], 0x60);
	
	u8 keybuf[PUBKEYDER_LEN + 0x20];
	memcpy(keybuf, pubkeyder, PUBKEYDER_LEN);
	memcpy(&keybuf[PUBKEYDER_LEN], input, 0x20);
	
	u8 hash[0x14];
	u8 key[0x10];
	sha1(hash, keybuf, 0x20 + PUBKEYDER_LEN);
	memcpy(key, hash, 0x10);
	
	u8 RSA[RSA_BYTES];
	rsa_encrypt(enc, RSA);
	u8 dec[0x80];
	memecrypto_aes_decrypt(RSA, dec, key);

	u8 temp1[0x8];
	u8 temp2[0x8];
	sha1(hash, dec, 0x80);
	memcpy(temp1, hash, 0x8);
	memcpy(temp2, &dec[0x58], 0x8);
	if (!memcmp(temp1, temp2, 0x8)) {
		memcpy(output, dec, 0x80);
		return;
	}
	
	RSA[0] |= 0x80;
	memecrypto_aes_decrypt(RSA, dec, key);
	sha1(hash, dec, 0x80);
	memcpy(temp1, hash, 0x8);
	memcpy(temp2, &dec[0x58], 0x8);
	if (!memcmp(temp1, temp2, 0x8)) {
		memcpy(output, dec, 0x80);
		return;
	}
}

void resign(u8 *mainbuf) {
	u8 CurSig[0x80];
	memcpy(CurSig, &mainbuf[0x6BB00], 0x80);

	u8 ChecksumTable[0x140];
	memcpy(ChecksumTable, &mainbuf[0x6BC00], 0x140);
	
	u8 Hash[SHA256_BLOCK_SIZE];
	sha256(Hash, ChecksumTable, 0x140);
	
	u8 DecSig[0x80];
	reverseCrypt(CurSig, DecSig);
	memcpy(DecSig, Hash, SHA256_BLOCK_SIZE);
	
	memecrypto_sign(DecSig, CurSig, 0x80);
	memcpy(&mainbuf[0x6BB00], CurSig, 0x80);
}