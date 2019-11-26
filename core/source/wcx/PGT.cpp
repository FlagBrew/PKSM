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

#include "PGT.hpp"
#include "Language.hpp"
#include "PK4.hpp"
#include "endian.hpp"

PGT::PGT(u8* pgt, bool fromWC4)
{
    std::copy(pgt, pgt + length, data);
    u8 pk4Data[236];
    std::copy(pgt + 0x8, pgt + 0x8 + 236, pk4Data);
    pokemonData = new PK4(pk4Data, !fromWC4, true);
    if (type() == 7)
    {
        // Set visible manaphy data
        pokemonData->species(490);
        pokemonData->alternativeForm(0);
        pokemonData->egg(true);
        pokemonData->gender(2);
        pokemonData->level(1);
        pokemonData->ability(pokemonData->abilities(0));
        pokemonData->move(0, 294);
        pokemonData->move(1, 145);
        pokemonData->move(2, 346);
        pokemonData->move(3, 0);
        pokemonData->heldItem(0);
        pokemonData->fatefulEncounter(true);
        pokemonData->ball(4);
        pokemonData->version(10); // Diamond
        pokemonData->language(Language::EN);
        pokemonData->nickname("MANAPHY");
        pokemonData->nicknamed(false);
        pokemonData->eggLocation(1);
    }
    pokemonData->refreshChecksum();
    pokemonData->encrypt();
    std::copy(pokemonData->rawData(), pokemonData->rawData() + 236, data + 0x8);
    pokemonData->decrypt(); // encrypt Pokemon data if it isn't already
}

PGT::~PGT()
{
    delete pokemonData;
}

u16 PGT::ID(void) const
{
    return 0;
}

std::string PGT::title(void) const
{
    return "Wondercard";
}

bool PGT::power(void) const
{
    return false;
}

Generation PGT::generation(void) const
{
    return Generation::FOUR;
}

u8 PGT::type(void) const
{
    return data[0];
}

bool PGT::bean(void) const
{
    return false;
}

bool PGT::BP(void) const
{
    return false;
}

bool PGT::item(void) const
{
    return type() == 3 || type() == 8 || type() == 9 || type() == 10 || type() == 12;
}

// Pokemon, egg, or Manaphy egg
bool PGT::pokemon(void) const
{
    return type() == 1 || type() == 2 || type() == 7;
}

u16 PGT::object(void) const
{
    if (type() == 8)
    {
        return 454;
    }
    else if (type() == 9)
    {
        return 452;
    }
    else if (type() == 10)
    {
        return 455;
    }
    else if (type() == 12)
    {
        return 467;
    }
    return Endian::convertTo<u16>(data + 0x4);
}

u8 PGT::flags(void) const
{
    return data[3];
}

bool PGT::multiObtainable(void) const
{
    return false;
}

u32 PGT::year(void) const
{
    return pokemonData->egg() ? pokemonData->eggYear() : pokemonData->metYear();
}

u32 PGT::month(void) const
{
    return pokemonData->egg() ? pokemonData->eggMonth() : pokemonData->metMonth();
}

u32 PGT::day(void) const
{
    return pokemonData->egg() ? pokemonData->eggDay() : pokemonData->metDay();
}

void PGT::year(u32 v)
{
    v = v >= 2000 ? v - 2000 : v;
    pokemonData->egg() ? pokemonData->eggYear((u8)v) : pokemonData->metYear((u8)v);
    pokemonData->refreshChecksum();
    pokemonData->encrypt();
    std::copy(pokemonData->rawData(), pokemonData->rawData() + 236, data + 0x8); // Actually set the data
    pokemonData->decrypt();
}

void PGT::month(u32 v)
{
    pokemonData->egg() ? pokemonData->eggMonth((u8)v) : pokemonData->metMonth((u8)v);
    pokemonData->refreshChecksum();
    pokemonData->encrypt();
    std::copy(pokemonData->rawData(), pokemonData->rawData() + 236, data + 0x8); // Actually set the data
    pokemonData->decrypt();
}

void PGT::day(u32 v)
{
    pokemonData->egg() ? pokemonData->eggDay((u8)v) : pokemonData->metDay((u8)v);
    pokemonData->refreshChecksum();
    pokemonData->encrypt();
    std::copy(pokemonData->rawData(), pokemonData->rawData() + 236, data + 0x8); // Actually set the data
    pokemonData->decrypt();
}

u32 PGT::rawDate(void) const
{
    return (year() << 16) | (month() << 8) | day();
}

void PGT::rawDate(u32 v)
{
    pokemonData->egg() ? pokemonData->eggYear((v >> 16) & 0xFF) : pokemonData->metYear((v >> 16) & 0xFF);
    pokemonData->egg() ? pokemonData->eggMonth((v >> 8) & 0xFF) : pokemonData->metMonth((v >> 8) & 0xFF);
    pokemonData->egg() ? pokemonData->eggDay(v & 0xFF) : pokemonData->metDay(v & 0xFF);
    pokemonData->refreshChecksum();
    pokemonData->encrypt();
    std::copy(pokemonData->rawData(), pokemonData->rawData() + 236, data + 0x8); // Actually set the data
    pokemonData->decrypt();
}

u8 PGT::cardLocation(void) const
{
    return 0;
}

bool PGT::used(void) const
{
    return false;
}

u8 PGT::ball(void) const
{
    return pokemonData->ball();
}

u16 PGT::heldItem(void) const
{
    return pokemonData->heldItem();
}

bool PGT::shiny(void) const
{
    return pokemonData->shiny();
}

u8 PGT::PIDType(void) const
{
    return pokemonData->shiny() ? 2 : 0;
}

u16 PGT::TID(void) const
{
    return pokemonData->TID();
}

u16 PGT::SID(void) const
{
    return pokemonData->SID();
}

u16 PGT::move(u8 index) const
{
    return pokemonData->move(index);
}

u16 PGT::species(void) const
{
    return pokemonData->species();
}

u8 PGT::gender(void) const
{
    return pokemonData->gender();
}

std::string PGT::otName(void) const
{
    return !(flags() == 0 && type() != 1) ? pokemonData->otName() : "Your OT Name";
}

u8 PGT::level(void) const
{
    return pokemonData->level();
}

u32 PGT::PID(void) const
{
    return pokemonData->PID();
}

bool PGT::ribbon(u8 category, u8 index) const
{
    return pokemonData->ribbon(category, index);
}

u8 PGT::alternativeForm(void) const
{
    return pokemonData->alternativeForm();
}

Language PGT::language(void) const
{
    return pokemonData->language();
}

std::string PGT::nickname(void) const
{
    return pokemonData->nickname();
}

u8 PGT::nature(void) const
{
    return pokemonData->nature();
}

u8 PGT::abilityType(void) const
{
    return pokemonData->abilityNumber();
}

u16 PGT::ability(void) const
{
    return pokemonData->ability();
}

u16 PGT::eggLocation(void) const
{
    return pokemonData->eggLocation();
}

u16 PGT::metLocation(void) const
{
    return pokemonData->metLocation();
}

u8 PGT::metLevel(void) const
{
    return pokemonData->metLevel();
}

u8 PGT::contest(u8 index) const
{
    return pokemonData->contest(index);
}

u8 PGT::iv(Stat index) const
{
    return pokemonData->iv(index);
}

bool PGT::egg(void) const
{
    return pokemonData->egg();
}

u16 PGT::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form        = alternativeForm();
    u8 formcount   = PersonalDPPtHGSS::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies      = PersonalDPPtHGSS::formStatIndex(tmpSpecies);
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
