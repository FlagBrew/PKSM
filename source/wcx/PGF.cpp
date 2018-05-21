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

#include "PGF.hpp"

PGF::PGF(u8* dt) { std::copy(dt, dt + length, data); }

u8 PGF::generation(void) const { return 5; }

u16 PGF::ID(void) const { return *(u16*)(data + 0xB0); }

std::string PGF::title(void) const { return StringUtils::getTrimmedString(data, 0x60, 37, (char*)"\uFFFF"); }

u8 PGF::type(void) const { return *(u8*)(data + 0xB3); }

u32 PGF::rawDate(void) const { return *(u32*)(data + 0xAC); }

void PGF::rawDate(u32 value) { *(u32*)(data + 0xAC) = value; }

u32 PGF::year(void) const { return (u32)(*(u16*)(data + 0xAE)); }

u32 PGF::month(void) const { return (u32)(*(u8*)(data + 0xAD)); }

u32 PGF::day(void) const { return (u32)(*(u8*)(data + 0xAC)); }

bool PGF::item(void) const { return type() == 2; }

u16 PGF::object(void) const { return *(u16*)(data); }

bool PGF::pokemon(void) const { return type() == 1; }

bool PGF::power(void) const { return type() == 3; }

bool PGF::bean(void) const { return false; }

bool PGF::BP(void) const { return false; }

u8 PGF::cardLocation(void) const { return *(u8*)(data + 0xB2); }

u8 PGF::flags(void) const { return *(u8*)(data + 0xB4); }

bool PGF::used(void) const { return (flags() >> 1) > 0; }

bool PGF::multiObtainable(void) const { return flags() == 0; }

u8 PGF::ball(void) const { return *(u8*)(data + 0x0E); }

u16 PGF::heldItem(void) const { return *(u16*)(data + 0x10); }

bool PGF::shiny(void) const { return PIDType() == 2; }

u8 PGF::PIDType(void) const { return *(u8*)(data + 0x37); }

u16 PGF::TID(void) const { return *(u16*)(data); }

u16 PGF::SID(void) const { return *(u16*)(data + 0x02); }

u16 PGF::move(u8 index) const { return *(u16*)(data + 0x12 + index * 2); }

u16 PGF::species(void) const { return *(u16*)(data + 0x1A); }

u8 PGF::gender(void) const { return *(u8*)(data + 0x35); }

std::string PGF::otName(void) const { return *((u16*)(data + 0x4A)) != 0xFFFF ? StringUtils::getTrimmedString(data, 0x4A, 8, (char*)"\uFFFF") : "Your OT Name"; }

u8 PGF::level(void) const { return *(u8*)(data + 0x5B); }

u32 PGF::PID(void) const { return *(u32*)(data + 0x08); }

bool PGF::ribbon(u8 category, u8 index) const { return *(data + 0x0C + category) & (1 << index); }

u8 PGF::alternativeForm(void) const { return *(u8*)(data + 0x1C); }

u8 PGF::language(void) const { return *(u8*)(data + 0x1D); }

std::string PGF::nickname(void) const { return *((u16*)(data + 0x1E)) != 0xFFFF ? StringUtils::getTrimmedString(data, 0x1E, 11, (char*)"\uFFFF") : "Pokemon Name"; }

u8 PGF::nature(void) const { return *(u8*)(data + 0x34); }

u8 PGF::abilityType(void) const { return *(u8*)(data + 0x36); }

u16 PGF::ability(void) const
{
    u8 abilitynum, type = abilityType();
	
	if (type == 2) abilitynum = 2;
	else if (type == 4) abilitynum = 2;
	else abilitynum = 0;

	return PersonalBWB2W2::ability(species(), abilitynum);
}

u16 PGF::eggLocation(void) const { return *(u16*)(data + 0x38); }

u16 PGF::metLocation(void) const { return *(u16*)(data + 0x3A); }

u8 PGF::metLevel(void) const { return *(u8*)(data + 0x3C); }

u8 PGF::contest(u8 index) const { return *(u8*)(data + 0x3D + index); }

u8 PGF::iv(u8 index) const { return *(u8*)(data + 0x43 + index); }

bool PGF::egg(void) const { return *(u8*)(data + 0x5C) == 1; }

u16 PGF::formSpecies(void) const
{
	u16 tmpSpecies = species();
    u8 form = alternativeForm();
    u8 formcount = PersonalBWB2W2::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies = PersonalBWB2W2::formStatIndex(tmpSpecies);
        if (!tmpSpecies)
        {
            tmpSpecies = backSpecies;
        }
        else if (form < formcount)
        {
            tmpSpecies += form - 1;
        }
    }

    return tmpSpecies;
}