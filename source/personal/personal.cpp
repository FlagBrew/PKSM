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

#include "personal.hpp"

namespace PersonalSMUSUM
{
    u8 baseHP(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x0); }
    u8 baseAtk(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x1); }
    u8 baseDef(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x2); }
    u8 baseSpe(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x3); }
    u8 baseSpa(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x4); }
    u8 baseSpd(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x5); }
    u8 type1(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x6); }
    u8 type2(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x7); }
    u8 gender(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x8); }
    u8 baseFriendship(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x9); }
    u8 expType(u16 species) { return *(u8*)(personal_smusum + species*21 + 0xA); }
    u8 ability(u16 species, u8 n) { return *(u8*)(personal_smusum + species*21 + 0xB + n); }
    u16 formStatIndex(u16 species) { return *(u16*)(personal_smusum + species*21 + 0xE); }
    u16 formSprite(u16 species) { return *(u16*)(personal_smusum + species*21 + 0x10); }
    u8 formCount(u16 species) { return *(u8*)(personal_smusum + species*21 + 0x12); }
    u16 baseExp(u16 species) { return *(u16*)(personal_smusum + species*21 + 0x13); }
}

namespace PersonalXYORAS
{
    u8 baseHP(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x0); }
    u8 baseAtk(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x1); }
    u8 baseDef(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x2); }
    u8 baseSpe(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x3); }
    u8 baseSpa(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x4); }
    u8 baseSpd(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x5); }
    u8 type1(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x6); }
    u8 type2(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x7); }
    u8 gender(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x8); }
    u8 baseFriendship(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x9); }
    u8 expType(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0xA); }
    u8 ability(u16 species, u8 n) { return *(u8*)(personal_xyoras + species*21 + 0xB + n); }
    u16 formStatIndex(u16 species) { return *(u16*)(personal_xyoras + species*21 + 0xE); }
    u16 formSprite(u16 species) { return *(u16*)(personal_xyoras + species*21 + 0x10); }
    u8 formCount(u16 species) { return *(u8*)(personal_xyoras + species*21 + 0x12); }
    u16 baseExp(u16 species) { return *(u16*)(personal_xyoras + species*21 + 0x13); }
}

namespace PersonalBWB2W2
{
    u8 baseHP(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x0); }
    u8 baseAtk(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x1); }
    u8 baseDef(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x2); }
    u8 baseSpe(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x3); }
    u8 baseSpa(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x4); }
    u8 baseSpd(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x5); }
    u8 type1(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x6); }
    u8 type2(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x7); }
    u8 gender(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x8); }
    u8 baseFriendship(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x9); }
    u8 expType(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0xA); }
    u8 ability(u16 species, u8 n) { return *(u8*)(personal_bwb2w2 + species*21 + 0xB + n); }
    u16 formStatIndex(u16 species) { return *(u16*)(personal_bwb2w2 + species*21 + 0xE); }
    u16 formSprite(u16 species) { return *(u16*)(personal_bwb2w2 + species*21 + 0x10); }
    u8 formCount(u16 species) { return *(u8*)(personal_bwb2w2 + species*21 + 0x12); }
    u16 baseExp(u16 species) { return *(u16*)(personal_bwb2w2 + species*21 + 0x13); }
}

namespace PersonalDPPtHGSS
{
    u8 baseHP(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x0); }
    u8 baseAtk(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x1); }
    u8 baseDef(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x2); }
    u8 baseSpe(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x3); }
    u8 baseSpa(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x4); }
    u8 baseSpd(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x5); }
    u8 type1(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x6); }
    u8 type2(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x7); }
    u8 baseExp(u8 species) { return *(u8*)(personal_dppthgss + species*17 + 0x8); }
    u8 gender(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0x9); }
    u8 baseFriendship(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0xA); }
    u8 expType(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0xB); }
    u8 ability(u16 species, u8 n) { return *(u8*)(personal_dppthgss + species*17 + 0xC + n); }
    u8 formCount(u16 species) { return *(u8*)(personal_dppthgss + species*17 + 0xE); }
    u16 formStatIndex(u16 species) { return *(u16*)(personal_dppthgss + species*17 + 0xF); }
}