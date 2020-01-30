/*
 *   This file is part of PKSM-Core
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "wcx/WC8.hpp"
#include "personal/personal.hpp"
#include "utils/endian.hpp"
#include "utils/utils.hpp"

namespace
{
    int langIndex(Language lang)
    {
        if (lang > Language::UNUSED)
        {
            return u8(lang) - 2;
        }
        else if (lang == Language::UNUSED)
        {
            return 2 - 1; // English
        }
        else
        {
            return u8(lang) - 1;
        }
    }
}

WC8::WC8(u8* dt)
{
    std::copy(dt, dt + length, data);
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

Generation WC8::generation() const
{
    return Generation::EIGHT;
}

u16 WC8::ID() const
{
    return Endian::convertTo<u16>(data + 8);
}

std::string WC8::title() const
{
    return title(titleIndex());
}

std::string WC8::title(Language lang) const
{
    // TODO: use titleIndex
    return "Wonder Card";
}

Language WC8::titleIndex() const
{
    return Language(data[0x15]);
}

u8 WC8::type() const
{
    return data[0x11];
}

u8 WC8::flags() const
{
    return data[0x10];
}

bool WC8::multiObtainable() const
{
    return (flags() & 1) == 1;
}

bool WC8::BP(void) const
{
    return type() == 3;
}

bool WC8::item(void) const
{
    return type() == 2;
}

bool WC8::pokemon(void) const
{
    return type() == 1;
}

bool WC8::clothing(void) const
{
    return type() == 4;
}

u16 WC8::object(void) const
{
    return object(0);
}

u16 WC8::object(int index) const
{
    return Endian::convertTo<u16>(data + 0x20 + index * 4);
}

int WC8::items(void) const
{
    return numItems;
}

u16 WC8::objectQuantity(void) const
{
    return objectQuantity(0);
}

u16 WC8::objectQuantity(int index) const
{
    return Endian::convertTo<u16>(data + 0x22 + index * 4);
}

u8 WC8::PIDType(void) const
{
    return data[0x248];
}

bool WC8::shiny(void) const
{
    return PIDType() == 2 || PIDType() == 3;
}

u16 WC8::TID(void) const
{
    return Endian::convertTo<u16>(data + 0x20);
}

u16 WC8::SID(void) const
{
    return Endian::convertTo<u16>(data + 0x22);
}

u8 WC8::version(void) const
{
    return Endian::convertTo<u32>(data + 0x24);
}

u32 WC8::encryptionConstant(void) const
{
    return Endian::convertTo<u32>(data + 0x28);
}

u32 WC8::PID(void) const
{
    return Endian::convertTo<u32>(data + 0x2C);
}

u8 WC8::ball(void) const
{
    return data[0x22C];
}

u16 WC8::heldItem(void) const
{
    return Endian::convertTo<u16>(data + 0x22E);
}

u16 WC8::move(u8 m) const
{
    return Endian::convertTo<u16>(data + 0x230 + m * 2);
}

u16 WC8::relearnMove(u8 index) const
{
    return Endian::convertTo<u16>(data + 0x238 + index * 2);
}

u16 WC8::species(void) const
{
    return Endian::convertTo<u16>(data + 0x240);
}

u8 WC8::alternativeForm(void) const
{
    return data[0x242];
}

u8 WC8::gender(void) const
{
    return data[0x243];
}

u8 WC8::level(void) const
{
    return data[0x244];
}

bool WC8::egg(void) const
{
    return data[0x245] == 1;
}

u8 WC8::nature(void) const
{
    return data[0x246];
}

u8 WC8::abilityType(void) const
{
    return data[0x247];
}

u16 WC8::ability(void) const
{
    u8 abilitynum, type = abilityType();

    if (type == 2)
        abilitynum = 2;
    else if (type == 4)
        abilitynum = 2;
    else
        abilitynum = 0;

    return PersonalSWSH::ability(species(), abilitynum);
}

u8 WC8::metLevel(void) const
{
    return data[0x249];
}

u8 WC8::dynamaxLevel(void) const
{
    return data[0x24A];
}

bool WC8::canGigantamax() const
{
    return data[0x24B] != 0;
}

u8 WC8::ribbonValue(int index) const
{
    return data[0x24C + index];
}

u8 WC8::iv(Stat index) const
{
    return data[0x26C + u8(index)];
}

u8 WC8::otGender(void) const
{
    return data[0x272];
}

u8 WC8::ev(Stat index) const
{
    return data[0x273 + u8(index)];
}

u8 WC8::otIntensity(void) const
{
    return data[0x279];
}

u8 WC8::otMemory(void) const
{
    return data[0x27A];
}

u8 WC8::otFeeling(void) const
{
    return data[0x27B];
}

u16 WC8::otTextvar(void) const
{
    return Endian::convertTo<u16>(data + 0x27C);
}

u16 WC8::eggLocation(void) const
{
    return Endian::convertTo<u16>(data + 0x228);
}

u16 WC8::metLocation(void) const
{
    return Endian::convertTo<u16>(data + 0x22A);
}

bool WC8::ribbon(u8 category, u8 index) const
{
    return (*(data + 0x74 + category) & (1 << index));
}

bool WC8::nicknamed(Language lang) const
{
    return nickname(lang)[0] != 0;
}

std::string WC8::nickname(Language lang) const
{
    return StringUtils::getString(data, 0x30 + langIndex(lang) * 0x1C, 13);
}

std::string WC8::otName(Language lang) const
{
    return StringUtils::getString(data, 0x12C + langIndex(lang) * 0x1C, 13);
}

std::string WC8::nickname() const
{
    return nickname(Language::EN);
}

std::string WC8::otName() const
{
    return otName(Language::EN);
}

bool WC8::used() const
{
    return false;
}

u16 WC8::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form        = alternativeForm();
    u8 formcount   = PersonalSWSH::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies      = PersonalSWSH::formStatIndex(tmpSpecies);
        if (!tmpSpecies)
        {
            tmpSpecies = backSpecies;
        }
        else
        {
            tmpSpecies += form - 1;
        }
    }

    return tmpSpecies;
}
