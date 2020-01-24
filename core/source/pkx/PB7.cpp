/*
 *   This file is part of PKSM
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

#include "pkx/PB7.hpp"
#include "utils/endian.hpp"
#include "utils/utils.hpp"

void PB7::shuffleArray(u8 sv)
{
    static constexpr int blockLength = 56;
    u8 index                         = sv * 4;

    u8 cdata[length];
    std::copy(data, data + length, cdata);

    for (u8 block = 0; block < 4; block++)
    {
        u8 ofs = blockPosition(index + block);
        std::copy(cdata + 8 + blockLength * ofs, cdata + 8 + blockLength * ofs + blockLength, data + 8 + blockLength * block);
    }
}

void PB7::crypt(void)
{
    u32 seed = encryptionConstant();
    for (int i = 0x08; i < 232; i += 2)
    {
        u16 temp = Endian::convertTo<u16>(data + i);
        seed     = seedStep(seed);
        temp ^= (seed >> 16);
        Endian::convertFrom<u16>(data + i, temp);
    }
    seed = encryptionConstant();
    for (u32 i = 232; i < length; i += 2)
    {
        u16 temp = Endian::convertTo<u16>(data + i);
        seed     = seedStep(seed);
        temp ^= (seed >> 16);
        Endian::convertFrom<u16>(data + i, temp);
    }
}

bool PB7::isEncrypted() const
{
    return Endian::convertTo<u16>(data + 0xC8) != 0 && Endian::convertTo<u16>(data + 0x58) != 0;
}

PB7::PB7(u8* dt, bool direct) : PKX(dt, 260, direct)
{
    if (isEncrypted())
    {
        decrypt();
    }
}

std::shared_ptr<PKX> PB7::clone(void) const
{
    return std::make_shared<PB7>(const_cast<u8*>(data));
}

Generation PB7::generation(void) const
{
    return Generation::LGPE;
}

u32 PB7::encryptionConstant(void) const
{
    return Endian::convertTo<u32>(data);
}
void PB7::encryptionConstant(u32 v)
{
    Endian::convertFrom<u32>(data, v);
}

u16 PB7::sanity(void) const
{
    return Endian::convertTo<u16>(data + 0x04);
}
void PB7::sanity(u16 v)
{
    Endian::convertFrom<u16>(data + 0x04, v);
}

u16 PB7::checksum(void) const
{
    return Endian::convertTo<u16>(data + 0x06);
}
void PB7::checksum(u16 v)
{
    Endian::convertFrom<u16>(data + 0x06, v);
}

u16 PB7::species(void) const
{
    return Endian::convertTo<u16>(data + 0x08);
}
void PB7::species(u16 v)
{
    Endian::convertFrom<u16>(data + 0x08, v);
}

u16 PB7::heldItem(void) const
{
    return Endian::convertTo<u16>(data + 0x0A);
}
void PB7::heldItem(u16 v)
{
    Endian::convertFrom<u16>(data + 0x0A, v);
}

u16 PB7::TID(void) const
{
    return Endian::convertTo<u16>(data + 0x0C);
}
void PB7::TID(u16 v)
{
    Endian::convertFrom<u16>(data + 0x0C, v);
}

u16 PB7::SID(void) const
{
    return Endian::convertTo<u16>(data + 0x0E);
}
void PB7::SID(u16 v)
{
    Endian::convertFrom<u16>(data + 0x0E, v);
}

u32 PB7::experience(void) const
{
    return Endian::convertTo<u32>(data + 0x10);
}
void PB7::experience(u32 v)
{
    Endian::convertFrom<u32>(data + 0x10, v);
}

u16 PB7::ability(void) const
{
    return data[0x14];
}
void PB7::ability(u16 v)
{
    data[0x14] = v;
}

void PB7::setAbility(u8 v)
{
    u8 abilitynum;

    if (v == 0)
        abilitynum = 1;
    else if (v == 1)
        abilitynum = 2;
    else
        abilitynum = 4;

    abilityNumber(abilitynum);
    data[0x14] = abilities(v);
}

u8 PB7::abilityNumber(void) const
{
    return data[0x15];
}
void PB7::abilityNumber(u8 v)
{
    data[0x15] = v;
}

u16 PB7::markValue(void) const
{
    return Endian::convertTo<u16>(data + 0x16);
}
void PB7::markValue(u16 v)
{
    Endian::convertFrom<u16>(data + 0x16, v);
}

u32 PB7::PID(void) const
{
    return Endian::convertTo<u32>(data + 0x18);
}
void PB7::PID(u32 v)
{
    Endian::convertFrom<u32>(data + 0x18, v);
}

u8 PB7::nature(void) const
{
    return data[0x1C];
}
void PB7::nature(u8 v)
{
    data[0x1C] = v;
}

bool PB7::fatefulEncounter(void) const
{
    return (data[0x1D] & 1) == 1;
}
void PB7::fatefulEncounter(bool v)
{
    data[0x1D] = (u8)((data[0x1D] & ~0x01) | (v ? 1 : 0));
}

u8 PB7::gender(void) const
{
    return (data[0x1D] >> 1) & 0x3;
}
void PB7::gender(u8 v)
{
    data[0x1D] = u8((data[0x1D] & ~0x06) | (v << 1));
}

u16 PB7::alternativeForm(void) const
{
    return data[0x1D] >> 3;
}
void PB7::alternativeForm(u16 v)
{
    data[0x1D] = u8((data[0x1D] & 0x07) | (v << 3));
}

u8 PB7::ev(Stat ev) const
{
    return data[0x1E + u8(ev)];
}
void PB7::ev(Stat ev, u8 v)
{
    data[0x1E + u8(ev)] = v;
}

u8 PB7::awakened(Stat stat) const
{
    return data[0x24 + u8(stat)];
}
void PB7::awakened(Stat stat, u8 v)
{
    data[0x24 + u8(stat)] = v;
}

u8 PB7::pelagoEventStatus(void) const
{
    return data[0x2A];
}
void PB7::pelagoEventStatus(u8 v)
{
    data[0x2A] = v;
}

u8 PB7::pkrs(void) const
{
    return data[0x2B];
}
void PB7::pkrs(u8 v)
{
    data[0x2B] = v;
}

u8 PB7::pkrsDays(void) const
{
    return data[0x2B] & 0xF;
};
void PB7::pkrsDays(u8 v)
{
    data[0x2B] = (u8)((data[0x2B] & ~0xF) | v);
}

u8 PB7::pkrsStrain(void) const
{
    return data[0x2B] >> 4;
};
void PB7::pkrsStrain(u8 v)
{
    data[0x2B] = (u8)((data[0x2B] & 0xF) | v << 4);
}

bool PB7::ribbon(u8 ribcat, u8 ribnum) const
{
    return (data[0x30 + ribcat] & (1 << ribnum)) == 1 << ribnum;
}
void PB7::ribbon(u8 ribcat, u8 ribnum, u8 v)
{
    data[0x30 + ribcat] = (u8)((data[0x30 + ribcat] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0));
}

std::string PB7::nickname(void) const
{
    return StringUtils::getString(data, 0x40, 12);
}
void PB7::nickname(const std::string& v)
{
    StringUtils::setString(data, v, 0x40, 12);
}

u16 PB7::move(u8 m) const
{
    return Endian::convertTo<u16>(data + 0x5A + m * 2);
}
void PB7::move(u8 m, u16 v)
{
    Endian::convertFrom<u16>(data + 0x5A + m * 2, v);
}

u8 PB7::PP(u8 m) const
{
    return data[0x62 + m];
}
void PB7::PP(u8 m, u8 v)
{
    data[0x62 + m] = v;
}

u8 PB7::PPUp(u8 m) const
{
    return data[0x66 + m];
}
void PB7::PPUp(u8 m, u8 v)
{
    data[0x66 + m] = v;
}

u16 PB7::relearnMove(u8 m) const
{
    return Endian::convertTo<u16>(data + 0x6A + m * 2);
}
void PB7::relearnMove(u8 m, u16 v)
{
    Endian::convertFrom<u16>(data + 0x6A + m * 2, v);
}

u8 PB7::iv(Stat stat) const
{
    u32 buffer = Endian::convertTo<u32>(data + 0x74);
    return (u8)((buffer >> 5 * u8(stat)) & 0x1F);
}

void PB7::iv(Stat stat, u8 v)
{
    u32 buffer = Endian::convertTo<u32>(data + 0x74);
    buffer &= ~(0x1F << 5 * u8(stat));
    buffer |= v << (5 * u8(stat));
    Endian::convertFrom<u32>(data + 0x74, buffer);
}

bool PB7::egg(void) const
{
    return ((Endian::convertTo<u32>(data + 0x74) >> 30) & 0x1) == 1;
}
void PB7::egg(bool v)
{
    Endian::convertFrom<u32>(data + 0x74, (Endian::convertTo<u32>(data + 0x74) & ~0x40000000) | (u32)(v ? 0x40000000 : 0));
}

bool PB7::nicknamed(void) const
{
    return ((Endian::convertTo<u32>(data + 0x74) >> 31) & 0x1) == 1;
}
void PB7::nicknamed(bool v)
{
    Endian::convertFrom<u32>(data + 0x74, (Endian::convertTo<u32>(data + 0x74) & 0x7FFFFFFF) | (v ? 0x80000000 : 0));
}

std::string PB7::htName(void) const
{
    return StringUtils::getString(data, 0x78, 12);
}
void PB7::htName(const std::string& v)
{
    StringUtils::setString(data, v, 0x78, 12);
}

u8 PB7::htGender(void) const
{
    return data[0x92];
}
void PB7::htGender(u8 v)
{
    data[0x92] = v;
}

u8 PB7::currentHandler(void) const
{
    return data[0x93];
}
void PB7::currentHandler(u8 v)
{
    data[0x93] = v;
}

u8 PB7::geoRegion(u8 region) const
{
    return data[0x94 + region * 2];
}
void PB7::geoRegion(u8 region, u8 v)
{
    data[0x94 + region * 2] = v;
}

u8 PB7::geoCountry(u8 country) const
{
    return data[0x95 + country * 2];
}
void PB7::geoCountry(u8 country, u8 v)
{
    data[0x95 + country * 2] = v;
}

u8 PB7::htFriendship(void) const
{
    return data[0xA2];
}
void PB7::htFriendship(u8 v)
{
    data[0xA2] = v;
}

u8 PB7::htAffection(void) const
{
    return data[0xA3];
}
void PB7::htAffection(u8 v)
{
    data[0xA3] = v;
}

u8 PB7::htIntensity(void) const
{
    return data[0xA4];
}
void PB7::htIntensity(u8 v)
{
    data[0xA4] = v;
}

u8 PB7::htMemory(void) const
{
    return data[0xA5];
}
void PB7::htMemory(u8 v)
{
    data[0xA5] = v;
}

u8 PB7::htFeeling(void) const
{
    return data[0xA6];
}
void PB7::htFeeling(u8 v)
{
    data[0xA6] = v;
}

u16 PB7::htTextVar(void) const
{
    return Endian::convertTo<u16>(data + 0xA8);
}
void PB7::htTextVar(u16 v)
{
    Endian::convertFrom<u16>(data + 0xA8, v);
}

u8 PB7::fullness(void) const
{
    return data[0xAE];
}
void PB7::fullness(u8 v)
{
    data[0xAE] = v;
}

u8 PB7::enjoyment(void) const
{
    return data[0xAF];
}
void PB7::enjoyment(u8 v)
{
    data[0xAF] = v;
}

std::string PB7::otName(void) const
{
    return StringUtils::getString(data, 0xB0, 12);
}
void PB7::otName(const std::string& v)
{
    StringUtils::setString(data, v, 0xB0, 12);
}

u8 PB7::otFriendship(void) const
{
    return data[0xCA];
}
void PB7::otFriendship(u8 v)
{
    data[0xCA] = v;
}

u8 PB7::otAffection(void) const
{
    return data[0xCB];
}
void PB7::otAffection(u8 v)
{
    data[0xCB] = v;
}

u8 PB7::otIntensity(void) const
{
    return data[0xCC];
}
void PB7::otIntensity(u8 v)
{
    data[0xCC] = v;
}

u8 PB7::otMemory(void) const
{
    return data[0xCD];
}
void PB7::otMemory(u8 v)
{
    data[0xCD] = v;
}

u16 PB7::otTextVar(void) const
{
    return Endian::convertTo<u16>(data + 0xCE);
}
void PB7::otTextVar(u16 v)
{
    Endian::convertFrom<u16>(data + 0xCE, v);
}

u8 PB7::otFeeling(void) const
{
    return data[0xD0];
}
void PB7::otFeeling(u8 v)
{
    data[0xD0] = v;
}

u8 PB7::eggYear(void) const
{
    return data[0xD1];
}
void PB7::eggYear(u8 v)
{
    data[0xD1] = v;
}

u8 PB7::eggMonth(void) const
{
    return data[0xD2];
}
void PB7::eggMonth(u8 v)
{
    data[0xD2] = v;
}

u8 PB7::eggDay(void) const
{
    return data[0xD3];
}
void PB7::eggDay(u8 v)
{
    data[0xD3] = v;
}

u8 PB7::metYear(void) const
{
    return data[0xD4];
}
void PB7::metYear(u8 v)
{
    data[0xD4] = v;
}

u8 PB7::metMonth(void) const
{
    return data[0xD5];
}
void PB7::metMonth(u8 v)
{
    data[0xD5] = v;
}

u8 PB7::metDay(void) const
{
    return data[0xD6];
}
void PB7::metDay(u8 v)
{
    data[0xD6] = v;
}

u16 PB7::eggLocation(void) const
{
    return Endian::convertTo<u16>(data + 0xD8);
}
void PB7::eggLocation(u16 v)
{
    Endian::convertFrom<u16>(data + 0xD8, v);
}

u16 PB7::metLocation(void) const
{
    return Endian::convertTo<u16>(data + 0xDA);
}
void PB7::metLocation(u16 v)
{
    Endian::convertFrom<u16>(data + 0xDA, v);
}

u8 PB7::ball(void) const
{
    return data[0xDC];
}
void PB7::ball(u8 v)
{
    data[0xDC] = v;
}

u8 PB7::metLevel(void) const
{
    return data[0xDD] & ~0x80;
}
void PB7::metLevel(u8 v)
{
    data[0xDD] = (data[0xDD] & 0x80) | v;
}

u8 PB7::otGender(void) const
{
    return data[0xDD] >> 7;
}
void PB7::otGender(u8 v)
{
    data[0xDD] = (data[0xDD] & ~0x80) | (v << 7);
}

bool PB7::hyperTrain(u8 num) const
{
    return (data[0xDE] & (1 << num)) == 1 << num;
}
void PB7::hyperTrain(u8 num, bool v)
{
    data[0xDE] = (u8)((data[0xDE] & ~(1 << num)) | (v ? 1 << num : 0));
}

u8 PB7::version(void) const
{
    return data[0xDF];
}
void PB7::version(u8 v)
{
    data[0xDF] = v;
}

u8 PB7::country(void) const
{
    return data[0xE0];
}
void PB7::country(u8 v)
{
    data[0xE0] = v;
}

u8 PB7::region(void) const
{
    return data[0xE1];
}
void PB7::region(u8 v)
{
    data[0xE1] = v;
}

u8 PB7::consoleRegion(void) const
{
    return data[0xE2];
}
void PB7::consoleRegion(u8 v)
{
    data[0xE2] = v;
}

Language PB7::language(void) const
{
    return Language(data[0xE3]);
}
void PB7::language(Language v)
{
    data[0xE3] = u8(v);
}

u8 PB7::currentFriendship(void) const
{
    return currentHandler() == 0 ? otFriendship() : htFriendship();
}
void PB7::currentFriendship(u8 v)
{
    if (currentHandler() == 0)
        otFriendship(v);
    else
        htFriendship(v);
}

u8 PB7::oppositeFriendship(void) const
{
    return currentHandler() == 1 ? otFriendship() : htFriendship();
}
void PB7::oppositeFriendship(u8 v)
{
    if (currentHandler() == 1)
        otFriendship(v);
    else
        htFriendship(v);
}

void PB7::refreshChecksum(void)
{
    u16 chk = 0;
    for (u8 i = 8; i < 232; i += 2)
    {
        chk += Endian::convertTo<u16>(data + i);
    }
    checksum(chk);
}

u8 PB7::hpType(void) const
{
    return 15 *
           ((iv(Stat::HP) & 1) + 2 * (iv(Stat::ATK) & 1) + 4 * (iv(Stat::DEF) & 1) + 8 * (iv(Stat::SPD) & 1) + 16 * (iv(Stat::SPATK) & 1) +
               32 * (iv(Stat::SPDEF) & 1)) /
           63;
}
void PB7::hpType(u8 v)
{
    static constexpr u16 hpivs[16][6] = {
        {1, 1, 0, 0, 0, 0}, // Fighting
        {0, 0, 0, 1, 0, 0}, // Flying
        {1, 1, 0, 1, 0, 0}, // Poison
        {1, 1, 1, 1, 0, 0}, // Ground
        {1, 1, 0, 0, 1, 0}, // Rock
        {1, 0, 0, 1, 1, 0}, // Bug
        {1, 0, 1, 1, 1, 0}, // Ghost
        {1, 1, 1, 1, 1, 0}, // Steel
        {1, 0, 1, 0, 0, 1}, // Fire
        {1, 0, 0, 1, 0, 1}, // Water
        {1, 0, 1, 1, 0, 1}, // Grass
        {1, 1, 1, 1, 0, 1}, // Electric
        {1, 0, 1, 0, 1, 1}, // Psychic
        {1, 0, 0, 1, 1, 1}, // Ice
        {1, 0, 1, 1, 1, 1}, // Dragon
        {1, 1, 1, 1, 1, 1}, // Dark
    };

    for (u8 i = 0; i < 6; i++)
    {
        iv(Stat(i), (iv(Stat(i)) & 0x1E) + hpivs[v][i]);
    }
}

u16 PB7::TSV(void) const
{
    return (TID() ^ SID()) >> 4;
}
u16 PB7::PSV(void) const
{
    return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 4;
}

u8 PB7::level(void) const
{
    u8 i      = 1;
    u8 xpType = expType();
    while (experience() >= expTable(i, xpType) && ++i < 100)
        ;
    return i;
}

void PB7::level(u8 v)
{
    experience(expTable(v - 1, expType()));
}

bool PB7::shiny(void) const
{
    return TSV() == PSV();
}
void PB7::shiny(bool v)
{
    if (v)
    {
        while (!shiny())
        {
            PID(PKX::getRandomPID(species(), gender(), version(), nature(), alternativeForm(), abilityNumber(), PID(), generation()));
        }
    }
    else
    {
        while (shiny())
        {
            PID(PKX::getRandomPID(species(), gender(), version(), nature(), alternativeForm(), abilityNumber(), PID(), generation()));
        }
    }
}

u16 PB7::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form        = alternativeForm();
    u8 formcount   = PersonalLGPE::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies      = PersonalLGPE::formStatIndex(tmpSpecies);
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

u16 PB7::stat(Stat stat) const
{
    u16 calc;
    u8 mult = 10, basestat = 0;

    switch (stat)
    {
        case Stat::HP:
            basestat = baseHP();
            break;
        case Stat::ATK:
            basestat = baseAtk();
            break;
        case Stat::DEF:
            basestat = baseDef();
            break;
        case Stat::SPD:
            basestat = baseSpe();
            break;
        case Stat::SPATK:
            basestat = baseSpa();
            break;
        case Stat::SPDEF:
            basestat = baseSpd();
            break;
    }

    if (stat == Stat::HP)
        calc = 10 + ((2 * basestat) + ((((data[0xDE] >> hyperTrainLookup[u8(stat)]) & 1) == 1) ? 31 : iv(stat)) + ev(stat) / 4 + 100) * level() / 100;
    else
        calc = 5 + (2 * basestat + ((((data[0xDE] >> hyperTrainLookup[u8(stat)]) & 1) == 1) ? 31 : iv(stat)) + ev(stat) / 4) * level() / 100;
    if (nature() / 5 + 1 == u8(stat))
        mult++;
    if (nature() % 5 + 1 == u8(stat))
        mult--;
    return calc * mult / 10 + awakened(stat);
}

int PB7::partyCurrHP(void) const
{
    if (length == 232)
    {
        return -1;
    }
    return Endian::convertTo<u16>(data + 0xF0);
}

void PB7::partyCurrHP(u16 v)
{
    if (length != 232)
    {
        Endian::convertFrom<u16>(data + 0xF0, v);
    }
}

int PB7::partyStat(Stat stat) const
{
    if (length == 232)
    {
        return -1;
    }
    return Endian::convertTo<u16>(data + 0xF2 + u8(stat) * 2);
}

void PB7::partyStat(Stat stat, u16 v)
{
    if (length != 232)
    {
        Endian::convertFrom<u16>(data + 0xF2 + u8(stat) * 2, v);
    }
}

int PB7::partyLevel() const
{
    if (length == 232)
    {
        return -1;
    }
    return *(data + 0xEC);
}

void PB7::partyLevel(u8 v)
{
    if (length != 232)
    {
        *(data + 0xEC) = v;
    }
}

u16 PB7::partyCP() const
{
    return Endian::convertTo<u16>(data + 0xFE);
}

void PB7::partyCP(u16 v)
{
    Endian::convertFrom<u16>(data + 0xFE, v);
}

u16 PB7::CP() const
{
    int base  = stat(Stat::HP) + 10 + level(); // HP
    int mult  = ((currentFriendship() / 255.0f / 10.0f) + 1.0f) * 100.0f;
    int awake = awakened(Stat::HP);

    for (int i = 1; i < 6; i++)
    {
        base += stat(Stat(i)) * mult / 100;
        awake += awakened(Stat(i));
    }

    base = (u16)((float)(base * 6 * level()) / 100.0f);

    double modifier = level() / 100.0 + 2.0;
    awake           = (u16)modifier * awake;
    return std::min(10000, base + awake);
}

u8 PB7::height(void) const
{
    return data[0x3A];
}

void PB7::height(u8 v)
{
    data[0x3A] = v;
}

u8 PB7::weight(void) const
{
    return data[0x3B];
}

void PB7::weight(u8 v)
{
    data[0x3B] = v;
}
