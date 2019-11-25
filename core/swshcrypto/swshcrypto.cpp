/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#include "swshcrypto.hpp"
#include "endian.hpp"
#include "sha256.h"
#include <string.h>

#define XORPAD_LENGTH 127
#define HASH_PAD_LENGTH 64

// clang-format off
static constexpr u8 xorpad[XORPAD_LENGTH] = {
    0xA0, 0x92, 0xD1, 0x06, 0x07, 0xDB, 0x32, 0xA1, 0xAE, 0x01, 0xF5, 0xC5, 0x1E, 0x84, 0x4F, 0xE3,
    0x53, 0xCA, 0x37, 0xF4, 0xA7, 0xB0, 0x4D, 0xA0, 0x18, 0xB7, 0xC2, 0x97, 0xDA, 0x5F, 0x53, 0x2B,
    0x75, 0xFA, 0x48, 0x16, 0xF8, 0xD4, 0x8A, 0x6F, 0x61, 0x05, 0xF4, 0xE2, 0xFD, 0x04, 0xB5, 0xA3,
    0x0F, 0xFC, 0x44, 0x92, 0xCB, 0x32, 0xE6, 0x1B, 0xB9, 0xB1, 0x2E, 0x01, 0xB0, 0x56, 0x53, 0x36,
    0xD2, 0xD1, 0x50, 0x3D, 0xDE, 0x5B, 0x2E, 0x0E, 0x52, 0xFD, 0xDF, 0x2F, 0x7B, 0xCA, 0x63, 0x50,
    0xA4, 0x67, 0x5D, 0x23, 0x17, 0xC0, 0x52, 0xE1, 0xA6, 0x30, 0x7C, 0x2B, 0xB6, 0x70, 0x36, 0x5B,
    0x2A, 0x27, 0x69, 0x33, 0xF5, 0x63, 0x7B, 0x36, 0x3F, 0x26, 0x9B, 0xA3, 0xED, 0x7A, 0x53, 0x00,
    0xA4, 0x48, 0xB3, 0x50, 0x9E, 0x14, 0xA0, 0x52, 0xDE, 0x7E, 0x10, 0x2B, 0x1B, 0x77, 0x6E
};

static constexpr u8 hashBegin[HASH_PAD_LENGTH] = {
    0x9E, 0xC9, 0x9C, 0xD7, 0x0E, 0xD3, 0x3C, 0x44, 0xFB, 0x93, 0x03, 0xDC, 0xEB, 0x39, 0xB4, 0x2A,
    0x19, 0x47, 0xE9, 0x63, 0x4B, 0xA2, 0x33, 0x44, 0x16, 0xBF, 0x82, 0xA2, 0xBA, 0x63, 0x55, 0xB6,
    0x3D, 0x9D, 0xF2, 0x4B, 0x5F, 0x7B, 0x6A, 0xB2, 0x62, 0x1D, 0xC2, 0x1B, 0x68, 0xE5, 0xC8, 0xB5,
    0x3A, 0x05, 0x90, 0x00, 0xE8, 0xA8, 0x10, 0x3D, 0xE2, 0xEC, 0xF0, 0x0C, 0xB2, 0xED, 0x4F, 0x6D,
};

static constexpr u8 hashEnd[HASH_PAD_LENGTH] = {
    0xD6, 0xC0, 0x1C, 0x59, 0x8B, 0xC8, 0xB8, 0xCB, 0x46, 0xE1, 0x53, 0xFC, 0x82, 0x8C, 0x75, 0x75,
    0x13, 0xE0, 0x45, 0xDF, 0x32, 0x69, 0x3C, 0x75, 0xF0, 0x59, 0xF8, 0xD9, 0xA2, 0x5F, 0xB2, 0x17,
    0xE0, 0x80, 0x52, 0xDB, 0xEA, 0x89, 0x73, 0x99, 0x75, 0x79, 0xAF, 0xCB, 0x2E, 0x80, 0x07, 0xE6,
    0xF1, 0x26, 0xE0, 0x03, 0x0A, 0xE6, 0x6F, 0xF6, 0x41, 0xBF, 0x7E, 0x59, 0xC2, 0xAE, 0x55, 0xFD,
};
// clang-format on

static void applyStaticXorpad(std::shared_ptr<u8[]> data, size_t length)
{
    for (size_t i = 0; i < length - SHA256_BLOCK_SIZE; i++)
    {
        data[i] ^= xorpad[i % XORPAD_LENGTH];
    }
}

static void computeHash(u8* hash, u8* data, size_t length)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, hashBegin, HASH_PAD_LENGTH);
    sha256_update(&ctx, data, length);
    sha256_update(&ctx, hashEnd, HASH_PAD_LENGTH);
    sha256_final(&ctx, hash);
}

bool swshcrypto_hashValid(u8* data, size_t length)
{
    u8 hash[SHA256_BLOCK_SIZE];
    computeHash(hash, data, length - SHA256_BLOCK_SIZE);
    for (size_t i = 0; i < SHA256_BLOCK_SIZE; i++)
    {
        if (hash[i] != data[length - SHA256_BLOCK_SIZE + i])
        {
            return false;
        }
    }
    return true;
}

// SCBlockList is associated to dataIn!
SCBlockList swshcrypto_decrypt(std::shared_ptr<u8[]> dataIn, size_t length)
{
    SCBlockList blocks{dataIn, length};
    applyStaticXorpad(dataIn, length);
    size_t offset = 0;
    while (offset < length - SHA256_BLOCK_SIZE)
    {
        blocks.push_back(SCBlock::decryptFromOffset(dataIn.get(), offset));
    }
    return blocks;
}

void swshcrypto_encrypt(SCBlockList& blocks)
{
    for (auto block : blocks)
    {
        block.encrypt();
    }

    applyStaticXorpad(blocks.associatedData, blocks.length);

    computeHash(blocks.associatedData.get() + blocks.length - SHA256_BLOCK_SIZE, blocks.associatedData.get(), blocks.length - SHA256_BLOCK_SIZE);
}

void SCBlock::cryptBytes(u8* data, size_t inputOffset, size_t start, size_t size)
{
    auto keyStream = getKeyStream(start, size);
    for (size_t i = 0; i < size; i++)
    {
        data[inputOffset + i] ^= keyStream[i];
    }
}

std::vector<u8> SCBlock::getKeyStream(size_t start, size_t size)
{
    std::vector<u8> ret = std::vector<u8>(size, 0);
    u32 key             = this->key;
    u32 popCount        = popcount(key);
    for (u32 i = 0; i < popCount; i++)
    {
        xorshiftAdvance(key);
    }

    size_t ofs = 0;
    while (ofs + 4 < start)
    {
        xorshiftAdvance(key);
        ofs += 4;
    }

    if (ofs < start)
    {
        int cur_size = std::min(size, 4 - (start - ofs));
        u8 leKeyData[4];
        Endian::convertFrom(leKeyData, key);
        std::copy(leKeyData + start - ofs, leKeyData + 4, ret.begin());
        ofs = cur_size;
        xorshiftAdvance(key);
    }

    while (ofs < size)
    {
        int cur_size = std::min(size - ofs, (size_t)4);
        u8 leKeyData[4];
        Endian::convertFrom(leKeyData, key);
        std::copy(leKeyData, leKeyData + cur_size, ret.begin() + ofs);
        ofs += cur_size;
        xorshiftAdvance(key);
    }

    return ret;
}

SCBlock SCBlock::decryptFromOffset(u8* data, size_t& offset)
{
    SCBlock block;
    block.key = Endian::convertTo<u32>(data + offset);
    offset += 4;

    block.cryptBytes(data, offset, 0, 1);

    block.type = SCBlockType(*(data + offset));

    switch (block.type)
    {
        case SCBlockType::Common1:
        case SCBlockType::Common2:
        case SCBlockType::Common3:
            // Block types A, B, and Common are empty; they have no extra data
            offset++;
            break;
        case SCBlockType::Data:
        {
            block.cryptBytes(data, offset, 1, 4);
            block.dataLength = Endian::convertTo<u32>(data + offset + 1);
            block.cryptBytes(data, offset, 5, block.dataLength);
            block.data = data + offset;
            offset += 5 + block.dataLength;
        }
        break;
        case SCBlockType::Array:
        {
            block.cryptBytes(data, offset, 1, 4);
            block.dataLength = Endian::convertTo<u32>(data + offset + 1);
            block.cryptBytes(data, offset, 5, 1);
            block.subtype = SCBlockType(*(data + offset + 5));
            switch (block.subtype)
            {
                case SCBlockType::Common3:
                    // An array of booleans
                    block.cryptBytes(data, offset, 6, block.dataLength);
                    offset += 6 + block.dataLength;
                    break;
                case SCBlockType::Single1:
                case SCBlockType::Single2:
                case SCBlockType::Single3:
                case SCBlockType::Single4:
                case SCBlockType::Single5:
                case SCBlockType::Single6:
                case SCBlockType::Single7:
                case SCBlockType::Single8:
                case SCBlockType::Single9:
                case SCBlockType::Single10:
                {
                    size_t entrySize = arrayEntrySize(block.subtype);
                    block.cryptBytes(data, offset, 6, block.dataLength * entrySize);
                    offset += 6 + (block.dataLength * entrySize);
                }
                break;
                default:
                    //! CHECK WHY THIS HAPPENED IF IT DOES
                    std::abort();
            }
        }
        break;
        case SCBlockType::Single1:
        case SCBlockType::Single2:
        case SCBlockType::Single3:
        case SCBlockType::Single4:
        case SCBlockType::Single5:
        case SCBlockType::Single6:
        case SCBlockType::Single7:
        case SCBlockType::Single8:
        case SCBlockType::Single9:
        case SCBlockType::Single10:
        {
            size_t entrySize = arrayEntrySize(block.type);
            block.cryptBytes(data, offset, 1, entrySize);
            offset += 1 + entrySize;
        }
        break;
        default:
            //! CHECK WHY THIS HAPPENED IF IT DOES
            std::abort();
    }

    return block;
}

void SCBlock::encrypt()
{
    Endian::convertFrom(data, key);
    data[4]       = u8(type);
    size_t outOfs = 5;

    if (type == SCBlockType::Data)
    {
        Endian::convertFrom(data + outOfs, dataLength);
        outOfs += 4;
    }
    else if (type == SCBlockType::Array)
    {
        Endian::convertFrom(data + outOfs, dataLength);
        data[outOfs + 4] = u8(subtype);
        outOfs += 5;
    }

    cryptBytes(data, 4, 0, encryptedDataSize() - 4);
}