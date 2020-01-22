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

#include "personal.hpp"
#include "endian.hpp"
#include "personal_bwb2w2.h"
#include "personal_dppthgss.h"
#include "personal_lgpe.h"
#include "personal_rsfrlge.h"
#include "personal_smusum.h"
#include "personal_swsh.h"
#include "personal_xyoras.h"

namespace PersonalLGPE
{
    u8 baseHP(u16 species) { return personal_lgpe[species * 17 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_lgpe[species * 17 + 0x1]; }
    u8 baseDef(u16 species) { return personal_lgpe[species * 17 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_lgpe[species * 17 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_lgpe[species * 17 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_lgpe[species * 17 + 0x5]; }
    u8 type1(u16 species) { return personal_lgpe[species * 17 + 0x6]; }
    u8 type2(u16 species) { return personal_lgpe[species * 17 + 0x7]; }
    u8 gender(u16 species) { return personal_lgpe[species * 17 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_lgpe[species * 17 + 0x9]; }
    u8 expType(u16 species) { return personal_lgpe[species * 17 + 0xA]; }
    u8 ability(u16 species, u8 n) { return personal_lgpe[species * 17 + 0xB + n]; }
    u16 formStatIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_lgpe + species * 17 + 0xE); }
    u8 formCount(u16 species) { return personal_lgpe[species * 17 + 0x10]; }
}

namespace PersonalSMUSUM
{
    u8 baseHP(u16 species) { return personal_smusum[species * 17 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_smusum[species * 17 + 0x1]; }
    u8 baseDef(u16 species) { return personal_smusum[species * 17 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_smusum[species * 17 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_smusum[species * 17 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_smusum[species * 17 + 0x5]; }
    u8 type1(u16 species) { return personal_smusum[species * 17 + 0x6]; }
    u8 type2(u16 species) { return personal_smusum[species * 17 + 0x7]; }
    u8 gender(u16 species) { return personal_smusum[species * 17 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_smusum[species * 17 + 0x9]; }
    u8 expType(u16 species) { return personal_smusum[species * 17 + 0xA]; }
    u8 ability(u16 species, u8 n) { return personal_smusum[species * 17 + 0xB + n]; }
    u16 formStatIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_smusum + species * 17 + 0xE); }
    u8 formCount(u16 species) { return personal_smusum[species * 17 + 0x10]; }
}

namespace PersonalXYORAS
{
    u8 baseHP(u16 species) { return personal_xyoras[species * 17 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_xyoras[species * 17 + 0x1]; }
    u8 baseDef(u16 species) { return personal_xyoras[species * 17 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_xyoras[species * 17 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_xyoras[species * 17 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_xyoras[species * 17 + 0x5]; }
    u8 type1(u16 species) { return personal_xyoras[species * 17 + 0x6]; }
    u8 type2(u16 species) { return personal_xyoras[species * 17 + 0x7]; }
    u8 gender(u16 species) { return personal_xyoras[species * 17 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_xyoras[species * 17 + 0x9]; }
    u8 expType(u16 species) { return personal_xyoras[species * 17 + 0xA]; }
    u8 ability(u16 species, u8 n) { return personal_xyoras[species * 17 + 0xB + n]; }
    u16 formStatIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_xyoras + species * 17 + 0xE); }
    u8 formCount(u16 species) { return personal_xyoras[species * 17 + 0x10]; }
}

namespace PersonalBWB2W2
{
    u8 baseHP(u16 species) { return personal_bwb2w2[species * 17 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_bwb2w2[species * 17 + 0x1]; }
    u8 baseDef(u16 species) { return personal_bwb2w2[species * 17 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_bwb2w2[species * 17 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_bwb2w2[species * 17 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_bwb2w2[species * 17 + 0x5]; }
    u8 type1(u16 species) { return personal_bwb2w2[species * 17 + 0x6]; }
    u8 type2(u16 species) { return personal_bwb2w2[species * 17 + 0x7]; }
    u8 gender(u16 species) { return personal_bwb2w2[species * 17 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_bwb2w2[species * 17 + 0x9]; }
    u8 expType(u16 species) { return personal_bwb2w2[species * 17 + 0xA]; }
    u8 ability(u16 species, u8 n) { return personal_bwb2w2[species * 17 + 0xB + n]; }
    u16 formStatIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_bwb2w2 + species * 17 + 0xE); }
    u8 formCount(u16 species) { return personal_bwb2w2[species * 17 + 0x10]; }
}

namespace PersonalDPPtHGSS
{
    u8 baseHP(u16 species) { return personal_dppthgss[species * 16 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_dppthgss[species * 16 + 0x1]; }
    u8 baseDef(u16 species) { return personal_dppthgss[species * 16 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_dppthgss[species * 16 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_dppthgss[species * 16 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_dppthgss[species * 16 + 0x5]; }
    u8 type1(u16 species) { return personal_dppthgss[species * 16 + 0x6]; }
    u8 type2(u16 species) { return personal_dppthgss[species * 16 + 0x7]; }
    u8 gender(u16 species) { return personal_dppthgss[species * 16 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_dppthgss[species * 16 + 0x9]; }
    u8 expType(u16 species) { return personal_dppthgss[species * 16 + 0xA]; }
    u8 ability(u16 species, u8 n) { return personal_dppthgss[species * 16 + 0xB + n]; }
    u16 formStatIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_dppthgss + species * 16 + 0xD); }
    // Normalized to fit with other formCounts' return values
    u8 formCount(u16 species)
    {
        if (species == 201)
        {
            return 28;
        }
        else
        {
            u8 count = personal_dppthgss[species * 16 + 0xF];
            if (count == 0)
            {
                return 1;
            }
            return count;
        }
    }
}

namespace PersonalSWSH
{
    u8 baseHP(u16 species) { return personal_swsh[species * 36 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_swsh[species * 36 + 0x1]; }
    u8 baseDef(u16 species) { return personal_swsh[species * 36 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_swsh[species * 36 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_swsh[species * 36 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_swsh[species * 36 + 0x5]; }
    u8 type1(u16 species) { return personal_swsh[species * 36 + 0x6]; }
    u8 type2(u16 species) { return personal_swsh[species * 36 + 0x7]; }
    u8 gender(u16 species) { return personal_swsh[species * 36 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_swsh[species * 36 + 0x9]; }
    u8 expType(u16 species) { return personal_swsh[species * 36 + 0xA]; }
    u8 formCount(u16 species) { return personal_swsh[species * 36 + 0xB]; }
    u16 ability(u16 species, u8 n) { return Endian::convertTo<u16>((u8*)personal_swsh + species * 36 + 0xC + 2 * n); }
    u16 formStatIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_swsh + species * 36 + 0x12); }
    u16 pokedexIndex(u16 species) { return Endian::convertTo<u16>((u8*)personal_swsh + species * 36 + 0x14); }
    bool canLearnTR(u16 species, u8 trID) { return (personal_swsh[species * 36 + 0x16 + (trID >> 3)] & (1 << (trID & 7))) != 0 ? true : false; }
}

namespace PersonalRSFRLGE
{
    u8 baseHP(u16 species) { return personal_rsfrlge[species * 13 + 0x0]; }
    u8 baseAtk(u16 species) { return personal_rsfrlge[species * 13 + 0x1]; }
    u8 baseDef(u16 species) { return personal_rsfrlge[species * 13 + 0x2]; }
    u8 baseSpe(u16 species) { return personal_rsfrlge[species * 13 + 0x3]; }
    u8 baseSpa(u16 species) { return personal_rsfrlge[species * 13 + 0x4]; }
    u8 baseSpd(u16 species) { return personal_rsfrlge[species * 13 + 0x5]; }
    u8 type1(u16 species) { return personal_rsfrlge[species * 13 + 0x6]; }
    u8 type2(u16 species) { return personal_rsfrlge[species * 13 + 0x7]; }
    u8 gender(u16 species) { return personal_rsfrlge[species * 13 + 0x8]; }
    u8 baseFriendship(u16 species) { return personal_rsfrlge[species * 13 + 0x9]; }
    u8 expType(u16 species) { return personal_rsfrlge[species * 13 + 0xA]; }
    u8 ability(u16 species, u8 n) { return personal_rsfrlge[species * 13 + 0xB + n]; }
}
