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

#include "WC7.hpp"
#include "personal.hpp"
#include "utils.hpp"

WC7::WC7(u8* dt, bool full)
{
    const u16 ofs = full ? 0x208 : 0;
    std::copy(dt + ofs, dt + ofs + length, data);
    for (int i = 0; i < 6; i++)
    {
        if (object(i) != 0)
        {
            numItems++;
        }
        else
            break;
    }
}

Generation WC7::generation(void) const
{
    return Generation::SEVEN;
}

u16 WC7::ID(void) const
{
    return *(u16*)(data);
}

std::string WC7::title(void) const
{
    return StringUtils::getString(data, 0x2, 36);
}

u32 WC7::rawDate(void) const
{
    return *(u32*)(data + 0x4C);
}
void WC7::rawDate(u32 v)
{
    *(u32*)(data + 0x4C) = v;
}

u32 WC7::year(void) const
{
    return rawDate() / 10000 + 2000;
}

u32 WC7::month(void) const
{
    return rawDate() % 10000 / 100;
}

u32 WC7::day(void) const
{
    return rawDate() % 100;
}

void WC7::year(u32 v)
{
    u32 newVal = v < 2000 ? v : v - 2000;
    newVal *= 10000;
    newVal += rawDate() % 10000;
    rawDate(newVal);
}

void WC7::month(u32 v)
{
    u32 newVal = rawDate() / 10000;
    newVal *= 100;
    newVal += v;
    newVal *= 100;
    newVal += rawDate() % 100;
    rawDate(newVal);
}

void WC7::day(u32 v)
{
    u32 newVal = rawDate() / 100;
    newVal *= 100;
    newVal += v;
    rawDate(newVal);
}

u8 WC7::type(void) const
{
    return *(u8*)(data + 0x51);
}

u8 WC7::flags(void) const
{
    return *(u8*)(data + 0x52);
}

bool WC7::multiObtainable(void) const
{
    return *(u8*)(data + 0x53) == 1;
}

bool WC7::BP(void) const
{
    return type() == 3;
}

bool WC7::bean(void) const
{
    return type() == 2;
}

bool WC7::item(void) const
{
    return type() == 1;
}

bool WC7::power(void) const
{
    return false;
}

u16 WC7::object(void) const
{
    return object(0);
}

u16 WC7::object(int index) const
{
    return *(u16*)(data + 0x68 + index * 4);
}

int WC7::items(void) const
{
    return numItems;
}

u16 WC7::objectQuantity(void) const
{
    return objectQuantity(0);
}

u16 WC7::objectQuantity(int index) const
{
    return *(u16*)(data + 0x6A + index * 4);
}

bool WC7::pokemon(void) const
{
    return type() == 0;
}

u8 WC7::PIDType(void) const
{
    return *(u8*)(data + 0xA3);
}

bool WC7::shiny(void) const
{
    return PIDType() == 2;
}

u16 WC7::TID(void) const
{
    return *(u16*)(data + 0x68);
}

u16 WC7::SID(void) const
{
    return *(u16*)(data + 0x6A);
}

u8 WC7::version(void) const
{
    return *(u8*)(data + 0x6C);
}

u32 WC7::encryptionConstant(void) const
{
    return *(u32*)(data + 0x70);
}

u8 WC7::ball(void) const
{
    return *(u8*)(data + 0x76);
}

u16 WC7::heldItem(void) const
{
    return *(u16*)(data + 0x78);
}

u16 WC7::move(u8 m) const
{
    return *(u16*)(data + 0x7A + m * 2);
}

u16 WC7::species(void) const
{
    return *(u16*)(data + 0x82);
}

u8 WC7::alternativeForm(void) const
{
    return *(u8*)(data + 0x84);
}

u8 WC7::language(void) const
{
    return *(u8*)(data + 0x85);
}

std::string WC7::nickname(void) const
{
    return *(u16*)(data + 0x86) != 0 ? StringUtils::getString(data, 0x86, 12) : "Pokemon Name";
} // Localization::speciesName(species()); }

u8 WC7::nature(void) const
{
    return *(u8*)(data + 0xA0);
}

u8 WC7::gender(void) const
{
    return *(u8*)(data + 0xA1);
}

u16 WC7::ability(void) const
{
    u8 abilitynum, type = abilityType();

    if (type == 2)
        abilitynum = 2;
    else if (type == 4)
        abilitynum = 2;
    else
        abilitynum = 0;

    return PersonalSMUSUM::ability(species(), abilitynum);
}

u8 WC7::abilityType(void) const
{
    return *(u8*)(data + 0xA2);
}

u8 WC7::metLevel(void) const
{
    return *(u8*)(data + 0xA8);
}

std::string WC7::otName(void) const
{
    return *(u16*)(data + 0xB6) != 0 ? StringUtils::getString(data, 0xB6, 12) : "Your OT Name";
}

u8 WC7::level(void) const
{
    return *(u8*)(data + 0xD0);
}

bool WC7::egg(void) const
{
    return *(u8*)(data + 0xD1) == 1;
}

u16 WC7::eggLocation(void) const
{
    return *(u16*)(data + 0xA4);
}

u16 WC7::metLocation(void) const
{
    return *(u16*)(data + 0xA6);
}

u8 WC7::contest(u8 index) const
{
    return *(u8*)(data + 0xA9 + index);
}

u8 WC7::iv(u8 index) const
{
    return *(u8*)(data + 0xAF + index);
}

u8 WC7::otGender(void) const
{
    return *(u8*)(data + 0xB5);
}

u16 WC7::additionalItem(void) const
{
    return *(u16*)(data + 0xD2);
}

u32 WC7::PID(void) const
{
    return *(u32*)(data + 0xD4);
}

u16 WC7::relearnMove(u8 index) const
{
    return *(u16*)(data + 0xD8 + index * 2);
}

u8 WC7::otIntensity(void) const
{
    return *(u8*)(data + 0xE0);
}

u8 WC7::otMemory(void) const
{
    return *(u8*)(data + 0xE1);
}

u16 WC7::otTextvar(void) const
{
    return *(u8*)(data + 0xE2);
}

u8 WC7::otFeeling(void) const
{
    return *(u8*)(data + 0xE4);
}

u8 WC7::ev(u8 index) const
{
    return *(u8*)(data + 0xE5 + index);
}

bool WC7::ribbon(u8 category, u8 index) const
{
    return (*(data + 0x74 + category) & (1 << index));
}

u8 WC7::cardLocation(void) const
{
    return *(u8*)(data + 0x50);
}

bool WC7::used(void) const
{
    return (flags() & 2) == 2;
}

bool WC7::oncePerDay(void) const
{
    return (flags() & 4) == 4;
}

u16 WC7::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form        = alternativeForm();
    u8 formcount   = PersonalSMUSUM::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies      = PersonalSMUSUM::formStatIndex(tmpSpecies);
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
