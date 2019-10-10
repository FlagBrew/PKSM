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

#ifndef PERSONAL_HPP
#define PERSONAL_HPP

#include "coretypes.h"

namespace PersonalLGPE
{
    u8 baseHP(u16 species);
    u8 baseAtk(u16 species);
    u8 baseDef(u16 species);
    u8 baseSpe(u16 species);
    u8 baseSpa(u16 species);
    u8 baseSpd(u16 species);
    u8 type1(u16 species);
    u8 type2(u16 species);
    u8 gender(u16 species);
    u8 baseFriendship(u16 species);
    u8 expType(u16 species);
    u8 ability(u16 species, u8 n);
    u16 formStatIndex(u16 species);
    u8 formCount(u16 species);
}

namespace PersonalSMUSUM
{
    u8 baseHP(u16 species);
    u8 baseAtk(u16 species);
    u8 baseDef(u16 species);
    u8 baseSpe(u16 species);
    u8 baseSpa(u16 species);
    u8 baseSpd(u16 species);
    u8 type1(u16 species);
    u8 type2(u16 species);
    u8 gender(u16 species);
    u8 baseFriendship(u16 species);
    u8 expType(u16 species);
    u8 ability(u16 species, u8 n);
    u16 formStatIndex(u16 species);
    u8 formCount(u16 species);
}

namespace PersonalXYORAS
{
    u8 baseHP(u16 species);
    u8 baseAtk(u16 species);
    u8 baseDef(u16 species);
    u8 baseSpe(u16 species);
    u8 baseSpa(u16 species);
    u8 baseSpd(u16 species);
    u8 type1(u16 species);
    u8 type2(u16 species);
    u8 gender(u16 species);
    u8 baseFriendship(u16 species);
    u8 expType(u16 species);
    u8 ability(u16 species, u8 n);
    u16 formStatIndex(u16 species);
    u8 formCount(u16 species);
}

namespace PersonalBWB2W2
{
    u8 baseHP(u16 species);
    u8 baseAtk(u16 species);
    u8 baseDef(u16 species);
    u8 baseSpe(u16 species);
    u8 baseSpa(u16 species);
    u8 baseSpd(u16 species);
    u8 type1(u16 species);
    u8 type2(u16 species);
    u8 gender(u16 species);
    u8 baseFriendship(u16 species);
    u8 expType(u16 species);
    u8 ability(u16 species, u8 n);
    u16 formStatIndex(u16 species);
    u8 formCount(u16 species);
}

namespace PersonalDPPtHGSS
{
    u8 baseHP(u16 species);
    u8 baseAtk(u16 species);
    u8 baseDef(u16 species);
    u8 baseSpe(u16 species);
    u8 baseSpa(u16 species);
    u8 baseSpd(u16 species);
    u8 type1(u16 species);
    u8 type2(u16 species);
    u8 baseExp(u8 species);
    u8 gender(u16 species);
    u8 baseFriendship(u16 species);
    u8 expType(u16 species);
    u8 ability(u16 species, u8 n);
    u16 formStatIndex(u16 species);
    u8 formCount(u16 species);
}

#endif
