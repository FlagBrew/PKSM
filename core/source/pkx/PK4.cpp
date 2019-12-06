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

#include "PK4.hpp"
#include "PK5.hpp"
#include "Sav.hpp"
#include "endian.hpp"
#include "utils.hpp"
#include <algorithm>

void PK4::shuffleArray(u8 sv)
{
    static constexpr int blockLength = 32;
    u8 index                         = sv * 4;

    u8 cdata[length];
    std::copy(data, data + length, cdata);

    for (u8 block = 0; block < 4; block++)
    {
        u8 ofs = blockPosition(index + block);
        std::copy(cdata + 8 + blockLength * ofs, cdata + 8 + blockLength * ofs + blockLength, data + 8 + blockLength * block);
    }
}

void PK4::crypt(void)
{
    u32 seed = checksum();

    for (int i = 0x08; i < 136; i += 2)
    {
        seed = seedStep(seed);
        data[i] ^= (seed >> 16);
        data[i + 1] ^= (seed >> 24);
    }

    seed = PID();
    for (u32 i = 136; i < length; i += 2)
    {
        seed = seedStep(seed);
        data[i] ^= (seed >> 16);
        data[i + 1] ^= (seed >> 24);
    }
}

PK4::PK4(u8* dt, bool ekx, bool party, bool direct) : directAccess(direct)
{
    length = party ? 236 : 136;
    if (directAccess)
    {
        data = dt;
    }
    else
    {
        data = new u8[length];
        std::copy(dt, dt + length, data);
    }

    if (ekx)
    {
        decrypt();
    }
}

std::shared_ptr<PKX> PK4::clone(void) const
{
    return std::make_shared<PK4>(const_cast<u8*>(data), false, length == 236);
}

Generation PK4::generation(void) const
{
    return Generation::FOUR;
}

u32 PK4::encryptionConstant(void) const
{
    return PID();
}
void PK4::encryptionConstant(u32 v)
{
    (void)v;
}

u8 PK4::currentFriendship(void) const
{
    return otFriendship();
}
void PK4::currentFriendship(u8 v)
{
    otFriendship(v);
}

u8 PK4::currentHandler(void) const
{
    return 0;
}
void PK4::currentHandler(u8 v)
{
    (void)v;
}

u8 PK4::abilityNumber(void) const
{
    return 1 << ((PID() >> 16) & 1);
}
void PK4::abilityNumber(u8 v)
{
    if (shiny())
    {
        do
        {
            PID(PKX::getRandomPID(species(), gender(), version(), nature(), alternativeForm(), v, PID(), generation()));
        } while (!shiny());
    }
    else
    {
        do
        {
            PID(PKX::getRandomPID(species(), gender(), version(), nature(), alternativeForm(), v, PID(), generation()));
        } while (shiny());
    }
}

u32 PK4::PID(void) const
{
    return Endian::convertTo<u32>(data);
}
void PK4::PID(u32 v)
{
    Endian::convertFrom<u32>(data, v);
}

u16 PK4::sanity(void) const
{
    return Endian::convertTo<u16>(data + 0x04);
}
void PK4::sanity(u16 v)
{
    Endian::convertFrom<u16>(data + 0x04, v);
}

u16 PK4::checksum(void) const
{
    return Endian::convertTo<u16>(data + 0x06);
}
void PK4::checksum(u16 v)
{
    Endian::convertFrom<u16>(data + 0x06, v);
}

u16 PK4::species(void) const
{
    return Endian::convertTo<u16>(data + 0x08);
}
void PK4::species(u16 v)
{
    Endian::convertFrom<u16>(data + 0x08, v);
}

u16 PK4::heldItem(void) const
{
    return Endian::convertTo<u16>(data + 0x0A);
}
void PK4::heldItem(u16 v)
{
    Endian::convertFrom<u16>(data + 0x0A, v);
}

u16 PK4::TID(void) const
{
    return Endian::convertTo<u16>(data + 0x0C);
}
void PK4::TID(u16 v)
{
    Endian::convertFrom<u16>(data + 0x0C, v);
}

u16 PK4::SID(void) const
{
    return Endian::convertTo<u16>(data + 0x0E);
}
void PK4::SID(u16 v)
{
    Endian::convertFrom<u16>(data + 0x0E, v);
}

u32 PK4::experience(void) const
{
    return Endian::convertTo<u32>(data + 0x10);
}
void PK4::experience(u32 v)
{
    Endian::convertFrom<u32>(data + 0x10, v);
}

u8 PK4::otFriendship(void) const
{
    return data[0x14];
}
void PK4::otFriendship(u8 v)
{
    data[0x14] = v;
}

u16 PK4::ability(void) const
{
    return data[0x15];
}
void PK4::ability(u16 v)
{
    data[0x15] = v;
}

void PK4::setAbility(u8 v)
{
    u8 abilitynum;

    if (v == 0)
        abilitynum = 1;
    else if (v == 1)
        abilitynum = 2;
    else
        abilitynum = 4;

    abilityNumber(abilitynum);
    ability(abilities(v));
}

u16 PK4::markValue(void) const
{
    return data[0x16];
}
void PK4::markValue(u16 v)
{
    data[0x16] = v;
}

Language PK4::language(void) const
{
    return Language(data[0x17]);
}
void PK4::language(Language v)
{
    data[0x17] = u8(v);
}

u8 PK4::ev(Stat ev) const
{
    return data[0x18 + u8(ev)];
}
void PK4::ev(Stat ev, u8 v)
{
    data[0x18 + u8(ev)] = v;
}

u8 PK4::contest(u8 contest) const
{
    return data[0x1E + contest];
}
void PK4::contest(u8 contest, u8 v)
{
    data[0x1E + contest] = v;
}

bool PK4::ribbon(u8 ribcat, u8 ribnum) const
{
    static constexpr u8 ribIndex[12] = {0x24, 0x25, 0x26, 0x27, 0x3C, 0x3D, 0x3E, 0x3F, 0x60, 0x61, 0x62, 0x63};
    return (data[ribIndex[ribcat]] & (1 << ribnum)) == 1 << ribnum;
}

void PK4::ribbon(u8 ribcat, u8 ribnum, u8 v)
{
    static constexpr u8 ribIndex[12] = {0x24, 0x25, 0x26, 0x27, 0x3C, 0x3D, 0x3E, 0x3F, 0x60, 0x61, 0x62, 0x63};
    data[ribIndex[ribcat]]           = (u8)((data[ribIndex[ribcat]] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0));
}

u16 PK4::move(u8 m) const
{
    return Endian::convertTo<u16>(data + 0x28 + m * 2);
}
void PK4::move(u8 m, u16 v)
{
    Endian::convertFrom<u16>(data + 0x28 + m * 2, v);
}

u16 PK4::relearnMove(u8 m) const
{
    return 0;
}
void PK4::relearnMove(u8 m, u16 v)
{
    // stubbed
}

u8 PK4::PP(u8 m) const
{
    return data[0x30 + m];
}
void PK4::PP(u8 m, u8 v)
{
    data[0x30 + m] = v;
}

u8 PK4::PPUp(u8 m) const
{
    return data[0x34 + m];
}
void PK4::PPUp(u8 m, u8 v)
{
    data[0x34 + m] = v;
}

u8 PK4::iv(Stat stat) const
{
    u32 buffer = Endian::convertTo<u32>(data + 0x38);
    return (u8)((buffer >> 5 * u8(stat)) & 0x1F);
}

void PK4::iv(Stat stat, u8 v)
{
    u32 buffer = Endian::convertTo<u32>(data + 0x38);
    buffer &= ~(0x1F << 5 * u8(stat));
    buffer |= v << (5 * u8(stat));
    Endian::convertFrom<u32>(data + 0x38, buffer);
}

bool PK4::egg(void) const
{
    return ((Endian::convertTo<u32>(data + 0x38) >> 30) & 0x1) == 1;
}
void PK4::egg(bool v)
{
    Endian::convertFrom<u32>(data + 0x38, (u32)((Endian::convertTo<u32>(data + 0x38) & ~0x40000000) | (u32)(v ? 0x40000000 : 0)));
}

bool PK4::nicknamed(void) const
{
    return ((Endian::convertTo<u32>(data + 0x38) >> 31) & 0x1) == 1;
}
void PK4::nicknamed(bool v)
{
    Endian::convertFrom<u32>(data + 0x38, (Endian::convertTo<u32>(data + 0x38) & 0x7FFFFFFF) | (v ? 0x80000000 : 0));
}

bool PK4::fatefulEncounter(void) const
{
    return (data[0x40] & 1) == 1;
}
void PK4::fatefulEncounter(bool v)
{
    data[0x40] = (u8)((data[0x40] & ~0x01) | (v ? 1 : 0));
}

u8 PK4::gender(void) const
{
    return (data[0x40] >> 1) & 0x3;
}
void PK4::gender(u8 g)
{
    data[0x40] = u8((data[0x40] & ~0x06) | (g << 1));
    if (shiny())
    {
        do
        {
            PID(PKX::getRandomPID(species(), g, version(), nature(), alternativeForm(), abilityNumber(), PID(), generation()));
        } while (!shiny());
    }
    else
    {
        do
        {
            PID(PKX::getRandomPID(species(), g, version(), nature(), alternativeForm(), abilityNumber(), PID(), generation()));
        } while (shiny());
    }
}

u16 PK4::alternativeForm(void) const
{
    return data[0x40] >> 3;
}
void PK4::alternativeForm(u16 v)
{
    data[0x40] = u8((data[0x40] & 0x07) | (v << 3));
}

u8 PK4::nature(void) const
{
    return PID() % 25;
}
void PK4::nature(u8 v)
{
    if (shiny())
    {
        do
        {
            PID(PKX::getRandomPID(species(), gender(), version(), v, alternativeForm(), abilityNumber(), PID(), generation()));
        } while (!shiny());
    }
    else
    {
        do
        {
            PID(PKX::getRandomPID(species(), gender(), version(), v, alternativeForm(), abilityNumber(), PID(), generation()));
        } while (shiny());
    }
}

u8 PK4::shinyLeaf(void) const
{
    return data[0x41];
}
void PK4::shinyLeaf(u8 v)
{
    data[0x41] = v;
}

std::string PK4::nickname(void) const
{
    return StringUtils::transString45(StringUtils::getString4(data, 0x48, 11));
}
void PK4::nickname(const std::string& v)
{
    StringUtils::setString4(data, StringUtils::transString45(v), 0x48, 11);
}

u8 PK4::version(void) const
{
    return data[0x5F];
}
void PK4::version(u8 v)
{
    data[0x5F] = v;
}

std::string PK4::otName(void) const
{
    return StringUtils::transString45(StringUtils::getString4(data, 0x68, 8));
}
void PK4::otName(const std::string& v)
{
    StringUtils::setString4(data, StringUtils::transString45(v), 0x68, 8);
}

u8 PK4::eggYear(void) const
{
    return data[0x78];
}
void PK4::eggYear(u8 v)
{
    data[0x78] = v;
}

u8 PK4::eggMonth(void) const
{
    return data[0x79];
}
void PK4::eggMonth(u8 v)
{
    data[0x79] = v;
}

u8 PK4::eggDay(void) const
{
    return data[0x7A];
}
void PK4::eggDay(u8 v)
{
    data[0x7A] = v;
}

u8 PK4::metYear(void) const
{
    return data[0x7B];
}
void PK4::metYear(u8 v)
{
    data[0x7B] = v;
}

u8 PK4::metMonth(void) const
{
    return data[0x7C];
}
void PK4::metMonth(u8 v)
{
    data[0x7C] = v;
}

u8 PK4::metDay(void) const
{
    return data[0x7D];
}
void PK4::metDay(u8 v)
{
    data[0x7D] = v;
}

u16 PK4::eggLocation(void) const
{
    u16 hgssLoc = Endian::convertTo<u16>(data + 0x44);
    if (hgssLoc != 0)
        return hgssLoc;
    return Endian::convertTo<u16>(data + 0x7E);
}
void PK4::eggLocation(u16 v)
{
    if (v == 0)
    {
        Endian::convertFrom<u16>(data + 0x44, v);
        Endian::convertFrom<u16>(data + 0x7E, v);
    }
    else if ((v < 2000 && v > 111) || (v < 3000 && v > 2010))
    {
        Endian::convertFrom<u16>(data + 0x44, v);
        Endian::convertFrom<u16>(data + 0x7E, 0xBBA);
    }
    else
    {
        // If this pokemon is from Platinum, HeartGold, or SoulSilver
        Endian::convertFrom<u16>(data + 0x44, (version() == 12 || version() == 7 || version() == 8) ? v : 0);
        Endian::convertFrom<u16>(data + 0x7E, v);
    }
}

u16 PK4::metLocation(void) const
{
    u16 hgssLoc = Endian::convertTo<u16>(data + 0x46);
    if (hgssLoc != 0)
        return hgssLoc;
    return Endian::convertTo<u16>(data + 0x80);
}
void PK4::metLocation(u16 v)
{
    if (v == 0)
    {
        Endian::convertFrom<u16>(data + 0x46, v);
        Endian::convertFrom<u16>(data + 0x80, v);
    }
    else if ((v < 2000 && v > 111) || (v < 3000 && v > 2010))
    {
        Endian::convertFrom<u16>(data + 0x46, v);
        Endian::convertFrom<u16>(data + 0x80, 0xBBA);
    }
    else
    {
        // If this pokemon is from Platinum, HeartGold, or SoulSilver
        Endian::convertFrom<u16>(data + 0x46, (version() == 12 || version() == 7 || version() == 8) ? v : 0);
        Endian::convertFrom<u16>(data + 0x80, v);
    }
}

u8 PK4::pkrs(void) const
{
    return data[0x82];
}
void PK4::pkrs(u8 v)
{
    data[0x82] = v;
}

u8 PK4::pkrsDays(void) const
{
    return data[0x82] & 0xF;
};
void PK4::pkrsDays(u8 v)
{
    data[0x82] = (u8)((data[0x82] & ~0xF) | v);
}

u8 PK4::pkrsStrain(void) const
{
    return data[0x82] >> 4;
};
void PK4::pkrsStrain(u8 v)
{
    data[0x82] = (u8)((data[0x82] & 0xF) | v << 4);
}

u8 PK4::ball(void) const
{
    return data[0x83] > data[0x86] ? data[0x83] : data[0x86];
}
void PK4::ball(u8 v)
{
    data[0x83] = (u8)(v <= 0x10 ? v : 4);
    if (v > 0x10 || ((version() == 7 || version() == 8) && !fatefulEncounter()))
        data[0x86] = (u8)(v <= 0x18 ? v : 4);
    else
        data[0x86] = 0;
}

u8 PK4::metLevel(void) const
{
    return data[0x84] & ~0x80;
}
void PK4::metLevel(u8 v)
{
    data[0x84] = (data[0x84] & 0x80) | v;
}

u8 PK4::otGender(void) const
{
    return data[0x84] >> 7;
}
void PK4::otGender(u8 v)
{
    data[0x84] = (data[0x84] & ~0x80) | (v << 7);
}

u8 PK4::encounterType(void) const
{
    return data[0x85];
}
void PK4::encounterType(u8 v)
{
    data[0x85] = v;
}

u8 PK4::characteristic(void) const
{
    u8 maxIV = 0, pm6stat = 0, pm6 = PID() % 6;
    for (int i = 0; i < 6; i++)
        if (iv(Stat(i)) > maxIV)
            maxIV = iv(Stat(i));
    for (int i = 0; i < 6; i++)
    {
        pm6stat = (pm6 + i) % 6;
        if (iv(Stat(i)) == maxIV)
            break;
    }
    return pm6stat * 5 + maxIV % 5;
}

void PK4::refreshChecksum(void)
{
    u16 chk = 0;
    for (u8 i = 8; i < 136; i += 2)
    {
        chk += Endian::convertTo<u16>(data + i);
    }
    checksum(chk);
}

u8 PK4::hpType(void) const
{
    return 15 *
           ((iv(Stat::HP) & 1) + 2 * (iv(Stat::ATK) & 1) + 4 * (iv(Stat::DEF) & 1) + 8 * (iv(Stat::SPD) & 1) + 16 * (iv(Stat::SPATK) & 1) +
               32 * (iv(Stat::SPDEF) & 1)) /
           63;
}
void PK4::hpType(u8 v)
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

u16 PK4::TSV(void) const
{
    return (TID() ^ SID()) >> 3;
}
u16 PK4::PSV(void) const
{
    return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 3;
}

u8 PK4::level(void) const
{
    u8 i      = 1;
    u8 xpType = expType();
    while (experience() >= expTable(i, xpType) && ++i < 100)
        ;
    return i;
}

void PK4::level(u8 v)
{
    experience(expTable(v - 1, expType()));
}

bool PK4::shiny(void) const
{
    return TSV() == PSV();
}
void PK4::shiny(bool v)
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

u16 PK4::formSpecies(void) const
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

u16 PK4::stat(Stat stat) const
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
        calc = 10 + (2 * basestat + iv(stat) + ev(stat) / 4 + 100) * level() / 100;
    else
        calc = 5 + (2 * basestat + iv(stat) + ev(stat) / 4) * level() / 100;

    if (nature() / 5 + 1 == u8(stat))
        mult++;
    if (nature() % 5 + 1 == u8(stat))
        mult--;
    return calc * mult / 10;
}

int PK4::partyCurrHP(void) const
{
    if (length == 136)
    {
        return -1;
    }
    return Endian::convertTo<u16>(data + 0x8E);
}

void PK4::partyCurrHP(u16 v)
{
    if (length != 136)
    {
        Endian::convertFrom<u16>(data + 0x8E, v);
    }
}

int PK4::partyStat(Stat stat) const
{
    if (length == 136)
    {
        return -1;
    }
    return Endian::convertTo<u16>(data + 0x90 + u8(stat) * 2);
}

void PK4::partyStat(Stat stat, u16 v)
{
    if (length != 136)
    {
        Endian::convertFrom<u16>(data + 0x90 + u8(stat) * 2, v);
    }
}

int PK4::partyLevel() const
{
    if (length == 136)
    {
        return -1;
    }
    return *(data + 0x8C);
}

void PK4::partyLevel(u8 v)
{
    if (length != 136)
    {
        *(data + 0x8C) = v;
    }
}

std::shared_ptr<PKX> PK4::convertToG5(Sav& save) const
{
    std::shared_ptr<PK5> pk5 = std::make_shared<PK5>();
    std::copy(data, data + 136, pk5->rawData());

    // Clear HGSS data
    Endian::convertFrom<u16>(pk5->rawData() + 0x86, 0);

    // Clear PtHGSS met data
    Endian::convertFrom<u32>(pk5->rawData() + 0x44, 0);

    time_t t              = time(NULL);
    struct tm* timeStruct = gmtime((const time_t*)&t);

    pk5->otFriendship(70);
    pk5->metYear(timeStruct->tm_year - 100);
    pk5->metMonth(timeStruct->tm_mon + 1);
    pk5->metDay(timeStruct->tm_mday);

    // Force normal Arceus form
    if (pk5->species() == 493)
    {
        pk5->alternativeForm(0);
    }

    pk5->heldItem(0);

    pk5->nature(nature());

    // Check met location
    pk5->metLocation(pk5->gen4() && pk5->fatefulEncounter() && std::find(beasts, beasts + 4, pk5->species()) != beasts + 4
                         ? (pk5->species() == 251 ? 30010 : 30012) // Celebi : Beast
                         : 30001);                                 // Pokétransfer (not Crown)

    pk5->ball(ball());

    pk5->nickname(nickname());
    pk5->otName(otName());

    // Check level
    pk5->metLevel(pk5->level());

    // Remove HM
    u16 moves[4] = {move(0), move(1), move(2), move(3)};

    for (int i = 0; i < 4; i++)
    {
        if (std::find(banned, banned + 8, moves[i]) != banned + 8)
        {
            moves[i] = 0;
        }
        pk5->move(i, moves[i]);
    }
    pk5->fixMoves();

    pk5->refreshChecksum();
    return pk5;
}

std::shared_ptr<PKX> PK4::convertToG6(Sav& save) const
{
    if (auto pk5 = convertToG5(save))
    {
        return pk5->convertToG6(save);
    }
    return nullptr;
}

std::shared_ptr<PKX> PK4::convertToG7(Sav& save) const
{
    if (auto pk5 = convertToG5(save))
    {
        if (auto pk6 = pk5->convertToG6(save))
        {
            return pk6->convertToG7(save);
        }
    }
    return nullptr;
}
