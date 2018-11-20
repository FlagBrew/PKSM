/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Sav.hpp"
#include "SavB2W2.hpp"
#include "SavBW.hpp"
#include "SavDP.hpp"
#include "SavHGSS.hpp"
#include "SavORAS.hpp"
#include "SavPT.hpp"
#include "SavSUMO.hpp"
#include "SavUSUM.hpp"
#include "SavXY.hpp"
#include "SavLGPE.hpp"

Sav::~Sav() { delete[] data; }

u16 Sav::ccitt16(const u8* buf, u32 len)
{
    u16 crc = 0xFFFF;
    for (u32 i = 0; i < len; i++)
    {
        crc ^= (u16)(buf[i] << 8);
        for (u32 j = 0; j < 0x8; j++)
        {
            if ((crc & 0x8000) > 0)
                crc = (u16)((crc << 1) ^ 0x1021);
            else
                crc <<= 1;
        }
    }
    return crc;
}

std::unique_ptr<Sav> Sav::getSave(u8* dt, size_t length)
{
    switch (length)
    {
        case 0x6CC00:
            return std::make_unique<SavUSUM>(dt);
        case 0x6BE00:
            return std::make_unique<SavSUMO>(dt);
        case 0x76000:
            return std::make_unique<SavORAS>(dt);
        case 0x65600:
            return std::make_unique<SavXY>(dt);
        case 0x80000:
            return checkDSType(dt);
        case 0xB8800:
        case 0x100000:
            return std::make_unique<SavLGPE>(dt);
        default:
            return std::unique_ptr<Sav>(nullptr);
    }
}

bool Sav::isValidDSSave(u8* dt)
{
    u16 chk1 = *(u16*)(dt + 0x24000 - 0x100 + 0x8C + 0xE);
    u16 actual1 = ccitt16(dt + 0x24000 - 0x100, 0x8C);
    if (chk1 == actual1)
    {
        return true;
    }
    u16 chk2 = *(u16*)(dt + 0x26000 - 0x100 + 0x94 + 0xE);
    u16 actual2 = ccitt16(dt + 0x26000 - 0x100, 0x94);
    if (chk2 == actual2)
    {
        return true;
    }

    if (*(u16*)(dt + 0xC0FE) == ccitt16(dt, 0xC0EC))
        return true;
    if (*(u16*)(dt + 0xCF2A) == ccitt16(dt, 0xCF18))
        return true;
    if (*(u16*)(dt + 0xF626) == ccitt16(dt, 0xF618))
        return true;

    // General Block Checksum is invalid, check for block identifiers
    u8 dpPattern[] = { 0x00, 0xC1, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 ptPattern[] = { 0x2C, 0xCF, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 hgssPattern[] = { 0x28, 0xF6, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    if (validSequence(dt, dpPattern))
        return true;
    if (validSequence(dt, ptPattern))
        return true;
    if (validSequence(dt, hgssPattern))
        return true;

    // Check the other save
    if (validSequence(dt, dpPattern, 0x40000))
        return true;
    if (validSequence(dt, ptPattern, 0x40000))
        return true;
    if (validSequence(dt, hgssPattern, 0x40000))
        return true;
    return false;
}

std::unique_ptr<Sav> Sav::checkDSType(u8* dt)
{
    u16 chk1 = *(u16*)(dt + 0x24000 - 0x100 + 0x8C + 0xE);
    u16 actual1 = ccitt16(dt + 0x24000 - 0x100, 0x8C);
    if (chk1 == actual1)
    {
        return std::make_unique<SavBW>(dt);
    }
    u16 chk2 = *(u16*)(dt + 0x26000 - 0x100 + 0x94 + 0xE);
    u16 actual2 = ccitt16(dt + 0x26000 - 0x100, 0x94);
    if (chk2 == actual2)
    {
        return std::make_unique<SavB2W2>(dt);
    }

    if (*(u16*)(dt + 0xC0FE) == ccitt16(dt, 0xC0EC))
        return std::make_unique<SavDP>(dt);
    if (*(u16*)(dt + 0xCF2A) == ccitt16(dt, 0xCF18))
        return std::make_unique<SavPT>(dt);
    if (*(u16*)(dt + 0xF626) == ccitt16(dt, 0xF618))
        return std::make_unique<SavHGSS>(dt);

    // General Block Checksum is invalid, check for block identifiers
    u8 dpPattern[] = { 0x00, 0xC1, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 ptPattern[] = { 0x2C, 0xCF, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    u8 hgssPattern[] = { 0x28, 0xF6, 0x00, 0x00, 0x23, 0x06, 0x06, 0x20, 0x00, 0x00 };
    if (validSequence(dt, dpPattern))
        return std::make_unique<SavDP>(dt);
    if (validSequence(dt, ptPattern))
        return std::make_unique<SavPT>(dt);
    if (validSequence(dt, hgssPattern))
        return std::make_unique<SavHGSS>(dt);

    // Check the other save
    if (validSequence(dt, dpPattern, 0x40000))
        return std::make_unique<SavDP>(dt);
    if (validSequence(dt, ptPattern, 0x40000))
        return std::make_unique<SavPT>(dt);
    if (validSequence(dt, hgssPattern, 0x40000))
        return std::make_unique<SavHGSS>(dt);
    return nullptr;
}

bool Sav::validSequence(u8* dt, u8* pattern, int shift)
{
    int ofs = *(u16*)(pattern) - 0xC + shift;
    for (int i = 0; i < 10; i++)
        if (dt[i + ofs] != pattern[i])
            return false;
    return true;
}