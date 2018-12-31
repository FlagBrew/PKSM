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

#include "WC6.hpp"

WC6::WC6(u8* dt, bool full)
{
	const u16 ofs = full ? 0x208 : 0;
	std::copy(dt + ofs, dt + ofs + length, data);
}

Generation WC6::generation(void) const { return Generation::SIX; }

u16 WC6::ID(void) const { return *(u16*)(data); }

std::string WC6::title(void) const { return StringUtils::getString(data, 0x2, 36); }

u32 WC6::rawDate(void) const { return *(u32*)(data + 0x4C); }
void WC6::rawDate(u32 v) { *(u32*)(data + 0x4C) = v; }

u32 WC6::year(void) const
{ 
	u32 year = rawDate() / 10000;
	return year < 2000 ? year + 2000 : year;
}

u32 WC6::month(void) const { return rawDate() % 10000 / 100; }

u32 WC6::day(void) const { return rawDate() % 100; }

u8 WC6::type(void) const { return *(u8*)(data + 0x51); }

u8 WC6::flags(void) const { return *(u8*)(data + 0x52); }

bool WC6::multiObtainable(void) const {	return *(u8*)(data + 0x53) == 1; }

bool WC6::BP(void) const { return false; } // WC6 only has Pokemon and Item cards

bool WC6::bean(void) const { return false; } // WC6 only has Pokemon and Item cards

bool WC6::item(void) const { return type() == 1; }

bool WC6::power(void) const { return false; }

u16 WC6::object(void) const { return *(u16*)(data + 0x68); }

u16 WC6::objectQuantity(void) const { return *(u16*)(data + 0x70); }

bool WC6::pokemon(void) const { return type() == 0; }

u8 WC6::PIDType(void) const { return *(u8*)(data + 0xA3); }

bool WC6::shiny(void) const { return PIDType() == 2; }

u16 WC6::TID(void) const { return *(u16*)(data + 0x68); }

u16 WC6::SID(void) const { return *(u16*)(data + 0x6A); }

u8 WC6::version(void) const { return *(u8*)(data + 0x6C); }

u32 WC6::encryptionConstant(void) const { return *(u32*)(data + 0x70); }

u8 WC6::ball(void) const { return *(u8*)(data + 0x76); }

u16 WC6::heldItem(void) const { return *(u16*)(data + 0x78); }

u16 WC6::move(u8 m) const { return *(u16*)(data + 0x7A + m*2); }

u16 WC6::species(void) const { return *(u16*)(data + 0x82); }

u8 WC6::alternativeForm(void) const { return *(u8*)(data + 0x84); }

u8 WC6::language(void) const { return *(u8*)(data + 0x85); }

std::string WC6::nickname(void) const { return *(u16*)(data + 0x86) != 0 ? StringUtils::getString(data, 0x86, 12) : "Pokemon Name"; } // Localization::speciesName(species()); }

u8 WC6::nature(void) const { return *(u8*)(data + 0xA0); }

u8 WC6::gender(void) const { return *(u8*)(data + 0xA1); }

u16 WC6::ability(void) const
{
	u8 abilitynum, type = abilityType();
	
	if (type == 2) abilitynum = 2;
	else if (type == 4) abilitynum = 2;
	else abilitynum = 0;
	
	return PersonalXYORAS::ability(species(), abilitynum);
}

u8 WC6::abilityType(void) const { return *(u8*)(data + 0xA2); }

u8 WC6::metLevel(void) const {  return *(u8*)(data + 0xA8); }

std::string WC6::otName(void) const { return *(u16*)(data + 0xB6) != 0 ? StringUtils::getString(data, 0xB6, 12) : "Your OT Name"; }

u8 WC6::level(void) const { return *(u8*)(data + 0xD0); }

bool WC6::egg(void) const { return *(u8*)(data + 0xD1) == 1; }

u16 WC6::eggLocation(void) const { return *(u16*)(data + 0xA4); }

u16 WC6::metLocation(void) const { return *(u16*)(data + 0xA6); }

u8 WC6::contest(u8 index) const { return *(u8*)(data + 0xA9 + index); }

u8 WC6::iv(u8 index) const { return *(u8*)(data + 0xAF + index); }

u8 WC6::otGender(void) const { return *(u8*)(data + 0xB5); }

u16 WC6::additionalItem(void) const { return *(u16*)(data + 0xD2); }

u32 WC6::PID(void) const { return *(u32*)(data + 0xD4); }

u16 WC6::relearnMove(u8 index) const { return *(u16*)(data + 0xD8 + index * 2); }

u8 WC6::otIntensity(void) const { return *(u8*)(data + 0xE0); }

u8 WC6::otMemory(void) const { return *(u8*)(data + 0xE1); }

u16 WC6::otTextvar(void) const { return *(u8*)(data + 0xE2); }

u8 WC6::otFeeling(void) const { return *(u8*)(data + 0xE4); }

u8 WC6::ev(u8 index) const { return *(u8*)(data + 0xE5 + index); }

bool WC6::ribbon(u8 category, u8 index) const { return (*(data + 0x74 + category) & (1 << index)); }

u8 WC6::cardLocation(void) const { return *(u8*)(data + 0x50); }

bool WC6::used(void) const { return (flags() & 2) == 2; }

bool WC6::oncePerDay(void) const { return (flags() & 4) == 4; }

u16 WC6::formSpecies(void) const
{
	u16 tmpSpecies = species();
    u8 form = alternativeForm();
    u8 formcount = PersonalXYORAS::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies = PersonalXYORAS::formStatIndex(tmpSpecies);
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