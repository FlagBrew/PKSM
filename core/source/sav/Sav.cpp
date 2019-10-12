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

#include "Sav.hpp"
#include "PKX.hpp"
#include "SavB2W2.hpp"
#include "SavBW.hpp"
#include "SavDP.hpp"
#include "SavHGSS.hpp"
#include "SavLGPE.hpp"
#include "SavORAS.hpp"
#include "SavPT.hpp"
#include "SavSUMO.hpp"
#include "SavUSUM.hpp"
#include "SavXY.hpp"

Sav::~Sav()
{
    delete[] data;
}

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
    u16 chk1    = *(u16*)(dt + 0x24000 - 0x100 + 0x8C + 0xE);
    u16 actual1 = ccitt16(dt + 0x24000 - 0x100, 0x8C);
    if (chk1 == actual1)
    {
        return true;
    }
    u16 chk2    = *(u16*)(dt + 0x26000 - 0x100 + 0x94 + 0xE);
    u16 actual2 = ccitt16(dt + 0x26000 - 0x100, 0x94);
    if (chk2 == actual2)
    {
        return true;
    }

    // Check for block identifiers
    static constexpr size_t DP_OFFSET   = 0xC100;
    static constexpr size_t PT_OFFSET   = 0xCF2C;
    static constexpr size_t HGSS_OFFSET = 0xF628;
    if (validSequence(dt, DP_OFFSET))
        return true;
    if (validSequence(dt, PT_OFFSET))
        return true;
    if (validSequence(dt, HGSS_OFFSET))
        return true;

    // Check the other save
    if (validSequence(dt, DP_OFFSET + 0x40000))
        return true;
    if (validSequence(dt, PT_OFFSET + 0x40000))
        return true;
    if (validSequence(dt, HGSS_OFFSET + 0x40000))
        return true;
    return false;
}

std::unique_ptr<Sav> Sav::checkDSType(u8* dt)
{
    u16 chk1    = *(u16*)(dt + 0x24000 - 0x100 + 0x8C + 0xE);
    u16 actual1 = ccitt16(dt + 0x24000 - 0x100, 0x8C);
    if (chk1 == actual1)
    {
        return std::make_unique<SavBW>(dt);
    }
    u16 chk2    = *(u16*)(dt + 0x26000 - 0x100 + 0x94 + 0xE);
    u16 actual2 = ccitt16(dt + 0x26000 - 0x100, 0x94);
    if (chk2 == actual2)
    {
        return std::make_unique<SavB2W2>(dt);
    }

    // Check for block identifiers
    static constexpr size_t DP_OFFSET   = 0xC100;
    static constexpr size_t PT_OFFSET   = 0xCF2C;
    static constexpr size_t HGSS_OFFSET = 0xF628;
    if (validSequence(dt, DP_OFFSET))
        return std::make_unique<SavDP>(dt);
    if (validSequence(dt, PT_OFFSET))
        return std::make_unique<SavPT>(dt);
    if (validSequence(dt, HGSS_OFFSET))
        return std::make_unique<SavHGSS>(dt);

    // Check the other save
    if (validSequence(dt, DP_OFFSET + 0x40000))
        return std::make_unique<SavDP>(dt);
    if (validSequence(dt, PT_OFFSET + 0x40000))
        return std::make_unique<SavPT>(dt);
    if (validSequence(dt, HGSS_OFFSET + 0x40000))
        return std::make_unique<SavHGSS>(dt);
    return nullptr;
}

bool Sav::validSequence(u8* dt, size_t offset)
{
    static constexpr u32 DATE_INTERNATIONAL = 0x20060623;
    static constexpr u32 DATE_KOREAN        = 0x20070903;

    if (*(u32*)(dt + offset - 0xC) != (offset & 0xFFFF))
    {
        return false;
    }

    return *(u32*)(dt + offset - 0x8) == DATE_INTERNATIONAL || *(u32*)(dt + offset - 0x8) == DATE_KOREAN;
}

std::shared_ptr<PKX> Sav::transfer(std::shared_ptr<PKX> pk)
{
    std::shared_ptr<PKX> ret = pk;
    while (ret->generation() != generation())
    {
        Generation oldGen = pk->generation();
        if (ret->generation() > generation())
        {
            ret = ret->previous(*this);
        }
        else
        {
            ret = ret->next(*this);
        }
        if (ret->generation() == oldGen) // Untransferrable
        {
            ret = nullptr;
            break;
        }
    }
    return ret;
}

void Sav::fixParty()
{
    // Poor man's bubble sort-like thing
    int numPkm = 6;
    for (int i = 5; i > 0; i--)
    {
        auto checkPKM = pkm(i);
        if (checkPKM->species() == 0)
        {
            numPkm--;
            continue;
        }
        auto prevPKM = pkm(i - 1);
        if (checkPKM->species() != 0 && prevPKM->species() == 0)
        {
            pkm(checkPKM, i - 1);
            pkm(prevPKM, i);
            numPkm = 6;
            i      = 6; // reset loop
        }
    }
    partyCount(numPkm);
}

u32 Sav::displayTID() const
{
    switch (generation())
    {
        default:
            return TID();
        case Generation::SEVEN:
        case Generation::LGPE:
            return (u32)(SID() << 16 | TID()) % 1000000;
    }
}

u32 Sav::displaySID() const
{
    switch (generation())
    {
        default:
            return SID();
        case Generation::SEVEN:
        case Generation::LGPE:
            return (u32)(SID() << 16 | TID()) / 1000000;
    }
}
