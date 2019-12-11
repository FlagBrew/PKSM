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

#ifndef SWSH_CRYPTO_HPP
#define SWSH_CRYPTO_HPP

#include "coretypes.h"
#include <memory>
#include <vector>

class SCBlock
{
public:
    enum class SCBlockType : u8
    {
        None = 0,

        // All aliases of each other
        Common1 = 1,
        Common2 = 2,
        Common3 = 3,

        Data = 4,

        Array = 5,

        Single1  = 8,
        Single2  = 9,
        Single3  = 10,
        Single4  = 11,
        Single5  = 12,
        Single6  = 13,
        Single7  = 14,
        Single8  = 15,
        Single9  = 16,
        Single10 = 17,
    };
    static SCBlock decryptFromOffset(u8* data, size_t& offset);
    void encrypt();
    // Default state is decrypted; use this after encrypt() to get it back to editable data
    void decrypt();

    // Returns pointer to data at the beginning of the block's data region, skipping block identifying information
    u8* rawData() { return data + headerSize(type); }
    u8* rawData() const { return data + headerSize(type); }

private:
    SCBlock(u8* data) : data(data) {}

    u32 key() const;
    void key(u32 v);
    u32 dataLength;
    SCBlockType type;
    SCBlockType subtype;
    // Points to the beginning of the block data: *(u32*)(data) == key
    u8* data = nullptr;

    void cryptBytes(u8* data, size_t inputOffset, size_t start, size_t size);
    std::vector<u8> getKeyStream(size_t start, size_t size);
    size_t encryptedDataSize()
    {
        constexpr int baseSize = 4 + 1; // key + type
        switch (type)
        {
            case SCBlockType::Common1:
            case SCBlockType::Common2:
            case SCBlockType::Common3:
                return baseSize;
            case SCBlockType::Data:
                return baseSize + 4 + dataLength;
            case SCBlockType::Array:
                return baseSize + 5 + dataLength * arrayEntrySize(subtype);
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
                return baseSize + arrayEntrySize(type);
            default:
                //! CHECK WHY THIS HAPPENS
                std::abort();
        }
    }

    static void xorshiftAdvance(u32& key)
    {
        key ^= (key << 2);
        key ^= (key >> 15);
        key ^= (key << 13);
    }

    static u32 popcount(u64 key)
    {
        // https://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
        constexpr u64 m1 = 0x5555555555555555;
        constexpr u64 m2 = 0x3333333333333333;
        constexpr u64 m4 = 0x0f0f0f0f0f0f0f0f;
        // const u64 m8 = 0x00ff00ff00ff00ff;
        // const u64 m16 = 0x0000ffff0000ffff;
        // const u64 m32 = 0x00000000ffffffff;
        constexpr u64 h01 = 0x0101010101010101;
        key -= (key >> 1) & m1;
        key = (key & m2) + ((key >> 2) & m2);
        key = (key + (key >> 4)) & m4;
        return (u32)((key * h01) >> 56);
    }

    static size_t arrayEntrySize(SCBlockType type)
    {
        switch (type)
        {
            case SCBlockType::Common3:
            case SCBlockType::Single1:
            case SCBlockType::Single5:
                return 1;
            case SCBlockType::Single2:
            case SCBlockType::Single6:
                return 2;
            case SCBlockType::Single3:
            case SCBlockType::Single7:
            case SCBlockType::Single9:
                return 4;
            case SCBlockType::Single4:
            case SCBlockType::Single8:
            case SCBlockType::Single10:
                return 8;
            default:
                //! CHECK WHY THIS HAPPENS
                std::abort();
        }
    }

    static size_t headerSize(SCBlockType type)
    {
        switch (type)
        {
            case SCBlockType::Common1:
            case SCBlockType::Common2:
            case SCBlockType::Common3:
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
                return 5;
            case SCBlockType::Data:
                return 9;
            case SCBlockType::Array:
                return 10;
            default:
                std::abort(); //! CHECK WHY THIS HAPPENS
        }
    }
};

class SCBlockList : public std::vector<SCBlock>
{
public:
    static SCBlockList init(std::shared_ptr<u8[]> data, size_t length);
    void encrypt();
    void decrypt();

private:
    SCBlockList(std::shared_ptr<u8[]> data, size_t length) : associatedData(data), length(length) {}
    std::shared_ptr<u8[]> associatedData;
    size_t length;
};

bool swshcrypto_hashValid(u8* data, size_t length);

#endif
