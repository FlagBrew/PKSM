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

#include "PK8.hpp"
#include "utils.hpp"

void PK8::shuffleArray(u8 sv)
{
    static constexpr int blockLength = 0x50;
    u8 index                         = sv * 4;

    u8 cdata[length];
    std::copy(data, data + length, cdata);

    for (u8 block = 0; block < 4; block++)
    {
        u8 ofs = blockPosition(index + block);
        std::copy(cdata + 8 + blockLength * ofs, cdata + 8 + blockLength * ofs + blockLength, data + 8 + blockLength * block);
    }
}

void PK8::crypt(void)
{
    u32 seed = encryptionConstant();
    for (int i = 0x08; i < 0x148; i += 2)
    {
        u16 temp = *(u16*)(data + i);
        seed     = seedStep(seed);
        temp ^= (seed >> 16);
        *(u16*)(data + i) = temp;
    }
    seed = encryptionConstant();
    for (u32 i = 0x148; i < length; i += 2)
    {
        u16 temp = *(u16*)(data + i);
        seed     = seedStep(seed);
        temp ^= (seed >> 16);
        *(u16*)(data + i) = temp;
    }
}

PK8::PK8(u8* dt, bool ekx, bool party, bool direct) : directAccess(direct)
{
    length = party ? 0x158 : 0x148;

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

std::shared_ptr<PKX> PK8::clone(void) const
{
    return std::make_shared<PK8>(const_cast<u8*>(data), false, length == 0x158);
}

Generation PK8::generation(void) const
{
    return Generation::EIGHT;
}

u32 PK8::encryptionConstant(void) const
{
    return *(u32*)(data);
}
void PK8::encryptionConstant(u32 v)
{
    *(u32*)(data) = v;
}

u16 PK8::sanity(void) const
{
    return *(u16*)(data + 0x04);
}
void PK8::sanity(u16 v)
{
    *(u16*)(data + 0x04) = v;
}

u16 PK8::checksum(void) const
{
    return *(u16*)(data + 0x06);
}
void PK8::checksum(u16 v)
{
    *(u16*)(data + 0x06) = v;
}

u16 PK8::species(void) const
{
    return *(u16*)(data + 0x08);
}
void PK8::species(u16 v)
{
    *(u16*)(data + 0x08) = v;
}

u16 PK8::heldItem(void) const
{
    return *(u16*)(data + 0x0A);
}
void PK8::heldItem(u16 v)
{
    *(u16*)(data + 0x0A) = v;
}

u16 PK8::TID(void) const
{
    return *(u16*)(data + 0x0C);
}
void PK8::TID(u16 v)
{
    *(u16*)(data + 0x0C) = v;
}

u16 PK8::SID(void) const
{
    return *(u16*)(data + 0x0E);
}
void PK8::SID(u16 v)
{
    *(u16*)(data + 0x0E) = v;
}

u32 PK8::experience(void) const
{
    return *(u32*)(data + 0x10);
}
void PK8::experience(u32 v)
{
    *(u32*)(data + 0x10) = v;
}

u16 PK8::ability(void) const
{
    return *(u16*)(data + 0x14);
}
void PK8::ability(u16 v)
{
    *(u16*)(data + 0x14) = v;
}

void PK8::setAbility(u8 v)
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

u8 PK8::abilityNumber(void) const
{
    return (data[0x16] & 0x7) == 1;
}
void PK8::abilityNumber(u8 v)
{
    data[0x16] = (data[0x16] & ~7) | (v & 7);
}

bool PK8::favorite(void) const
{
    return (data[0x16] & 8) == 1;
}
void PK8::favorite(bool v) const
{
    data[0x16] = (data[0x16] & ~8) | (v ? 8 : 0);
}

bool PK8::canGiga(void) const
{
    return (data[0x16] & 16) == 1;
}
void PK8::canGiga(bool v) const
{
    data[0x16] = (data[0x16] & ~16) | (v ? 16 : 0);
}

u16 PK8::markValue(void) const
{
    return *(u16*)(data + 0x18);
}
void PK8::markValue(u16 v)
{
    *(u16*)(data + 0x18) = v;
}

u32 PK8::PID(void) const
{
    return *(u32*)(data + 0x1C);
}
void PK8::PID(u32 v)
{
    *(u32*)(data + 0x1C) = v;
}

u8 PK8::origNature(void) const
{
    return data[0x20];
}
void PK8::origNature(u8 v)
{
    data[0x20] = v;
}

u8 PK8::nature(void) const
{
    return data[0x21];
}
void PK8::nature(u8 v)
{
    data[0x21] = v;
}

bool PK8::fatefulEncounter(void) const
{
    return (data[0x22] & 1) == 1;
}
void PK8::fatefulEncounter(bool v)
{
    data[0x22] = (u8)((data[0x22] & ~0x01) | (v ? 1 : 0));
}

/*
bool PK8::data22flag2(void) const
{
    return (data[0x22] & 2) == 1;
}
void PK8::data22flag2(bool v)
{
    data[0x22] = (data[0x22] & ~2) | (v ? 2 : 0);
}
*/

u8 PK8::gender(void) const
{
    return (data[0x22] >> 2) & 0x3;
}
void PK8::gender(u8 v)
{
    data[0x22] = (data[0x22] & ~12) | ((v & 3) << 2);
}

u16 PK8::alternativeForm(void) const
{
    return *(u16*)(data + 0x24);
}
void PK8::alternativeForm(u16 v)
{
    *(u16*)(data + 0x24) = v;
}

u8 PK8::ev(Stat ev) const
{
    return data[0x26 + u8(ev)];
}
void PK8::ev(Stat ev, u8 v)
{
    data[0x26 + u8(ev)] = v;
}

u8 PK8::contest(u8 contest) const
{
    return data[0x2C + contest];
}
void PK8::contest(u8 contest, u8 v)
{
    data[0x2C + contest] = v;
}

u8 PK8::pkrs(void) const
{
    return data[0x32];
}
void PK8::pkrs(u8 v)
{
    data[0x32] = v;
}

u8 PK8::pkrsDays(void) const
{
    return data[0x32] & 0xF;
}
void PK8::pkrsDays(u8 v)
{
    data[0x32] = (data[0x32] & ~0xF) | (v & 0xF);
}

u8 PK8::pkrsStrain(void) const
{
    return data[0x32] >> 4;
}
void PK8::pkrsStrain(u8 v)
{
    data[0x32] = (data[0x2B] & 0xF) | (v << 4);
}

bool PK8::ribbon(u8 ribcat, u8 ribnum) const
{
    if (ribcat > 7)
    {
        ribcat += 2;
    }
    return (data[0x34 + ribcat] & (1 << ribnum)) == 1 << ribnum;
}
void PK8::ribbon(u8 ribcat, u8 ribnum, u8 v)
{
    if (ribcat > 7)
    {
        ribcat += 2;
    }
    data[0x34 + ribcat] = (u8)((data[0x34 + ribcat] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0));
}

u8 PK8::ribbonCountContest(void) const
{
    return data[0x3C];
}
void PK8::ribbonCountContest(u8 v)
{
    data[0x3C] = v;
    if (v)
    {
        // Bitflag for having at least one
        ribbon(4, 5, 1);
    }
    else
    {
        // Bitflag for having at least one
        ribbon(4, 5, 0);
    }
}

u8 PK8::ribbonCountBattle(void) const
{
    return data[0x3D];
}
void PK8::ribbonCountBattle(u8 v)
{
    data[0x3D] = v;
    if (v)
    {
        // Bitflag for having at least one
        ribbon(4, 6, 1);
    }
    else
    {
        // Bitflag for having at least one
        ribbon(4, 6, 0);
    }
}

u8 PK8::height(void) const
{
    return data[0x50];
}
void PK8::height(u8 v)
{
    data[0x50] = v;
}

u8 PK8::weight(void) const
{
    return data[0x51];
}
void PK8::weight(u8 v)
{
    data[0x51] = v;
}

std::string PK8::nickname(void) const
{
    return StringUtils::transString67(StringUtils::getString(data, 0x58, 13));
}
void PK8::nickname(const std::string& v)
{
    StringUtils::setString(data, StringUtils::transString67(v), 0x58, 13);
}

u16 PK8::move(u8 m) const
{
    return *(u16*)(data + 0x72 + m * 2);
}
void PK8::move(u8 m, u16 v)
{
    *(u16*)(data + 0x72 + m * 2) = v;
}

u8 PK8::PP(u8 m) const
{
    return data[0x7A + m];
}
void PK8::PP(u8 m, u8 v)
{
    data[0x7A + m] = v;
}

u8 PK8::PPUp(u8 m) const
{
    return data[0x7E + m];
}
void PK8::PPUp(u8 m, u8 v)
{
    data[0x7E + m] = v;
}

u16 PK8::relearnMove(u8 m) const
{
    return *(u16*)(data + 0x82 + m * 2);
}
void PK8::relearnMove(u8 m, u16 v)
{
    *(u16*)(data + 0x82 + m * 2) = v;
}

int PK8::partyCurrHP(void) const
{
    return *(u16*)(data + 0x8A);
}
void PK8::partyCurrHP(u16 v)
{
    *(u16*)(data + 0x8A) = v;
}

u8 PK8::iv(Stat stat) const
{
    u32 buffer = *(u32*)(data + 0x8C);
    return (u8)((buffer >> 5 * u8(stat)) & 0x1F);
}
void PK8::iv(Stat stat, u8 v)
{
    u32 buffer = *(u32*)(data + 0x8C);
    buffer &= ~(0x1F << 5 * u8(stat));
    buffer |= v << (5 * u8(stat));
    *(u32*)(data + 0x8C) = buffer;
}

bool PK8::egg(void) const
{
    return ((*(u32*)(data + 0x8C) >> 30) & 0x1) == 1;
}
void PK8::egg(bool v)
{
    *(u32*)(data + 0x8C) = (u32)((*(u32*)(data + 0x8C) & ~0x40000000) | (u32)(v ? 0x40000000 : 0));
}

bool PK8::nicknamed(void) const
{
    return ((*(u32*)(data + 0x8C) >> 31) & 0x1) == 1;
}
void PK8::nicknamed(bool v)
{
    *(u32*)(data + 0x8C) = (*(u32*)(data + 0x8C) & 0x7FFFFFFF) | (v ? 0x80000000 : 0);
}

u8 PK8::dynamaxLevel(void) const
{
    return data[0x90];
}
void PK8::dynamaxLevel(u8 v)
{
    data[0x90] = v;
}

std::string PK8::htName(void) const
{
    return StringUtils::transString67(StringUtils::getString(data, 0xA8, 13));
}
void PK8::htName(const std::string& v)
{
    StringUtils::setString(data, StringUtils::transString67(v), 0xA8, 13);
}

u8 PK8::htGender(void) const
{
    return data[0xC2];
}
void PK8::htGender(u8 v)
{
    data[0xC2] = v;
}

u8 PK8::currentHandler(void) const
{
    return data[0xC4];
}
void PK8::currentHandler(u8 v)
{
    data[0xC4] = v;
}

u16 PK8::htID(void) const
{
    return *(u16*)(data + 0xC6);
}
void PK8::htID(u16 v)
{
    *(u16*)(data + 0xC6) = v;
}

u8 PK8::htFriendship(void) const
{
    return data[0xC8];
}
void PK8::htFriendship(u8 v)
{
    data[0xC8] = v;
}

u8 PK8::htIntensity(void) const
{
    return data[0xC9];
}
void PK8::htIntensity(u8 v)
{
    data[0xC9] = v;
}

u8 PK8::htMemory(void) const
{
    return data[0xCA];
}
void PK8::htMemory(u8 v)
{
    data[0xCA] = v;
}

u8 PK8::htFeeling(void) const
{
    return data[0xCB];
}
void PK8::htFeeling(u8 v)
{
    data[0xCB] = v;
}

u16 PK8::htTextVar(void) const
{
    return *(u16*)(data + 0xCC);
}
void PK8::htTextVar(u16 v)
{
    *(u16*)(data + 0xCC) = v;
}

u8 PK8::fullness(void) const
{
    return data[0xDC];
}
void PK8::fullness(u8 v)
{
    data[0xDC] = v;
}

u8 PK8::enjoyment(void) const
{
    return data[0xDD];
}
void PK8::enjoyment(u8 v)
{
    data[0xDD] = v;
}

u8 PK8::version(void) const
{
    return data[0xDE];
}
void PK8::version(u8 v)
{
    data[0xDE] = v;
}

u8 PK8::country(void) const
{
    return data[0xDF];
}
void PK8::country(u8 v)
{
    data[0xDF] = v;
}

u8 PK8::region(void) const
{
    return data[0xE0];
}
void PK8::region(u8 v)
{
    data[0xE0] = v;
}

u8 PK8::consoleRegion(void) const
{
    return data[0xE1];
}
void PK8::consoleRegion(u8 v)
{
    data[0xE1] = v;
}

Language PK8::language(void) const
{
    return Language(data[0xE2]);
}
void PK8::language(Language v)
{
    data[0xE2] = u8(v);
}

u32 PK8::formDuration(void) const
{
    return *(u32*)(data + 0xE4);
}
void PK8::formDuration(u32 v)
{
    *(u32*)(data + 0xE4) = v;
}

s8 PK8::favRibbon(void) const
{
    return data[0xE8];
}
void PK8::favRibbon(s8 v)
{
    data[0xE8] = v;
}

std::string PK8::otName(void) const
{
    return StringUtils::transString67(StringUtils::getString(data, 0xF8, 13));
}
void PK8::otName(const std::string& v)
{
    StringUtils::setString(data, StringUtils::transString67(v), 0xF8, 13);
}

u8 PK8::otFriendship(void) const
{
    return data[0x112];
}
void PK8::otFriendship(u8 v)
{
    data[0x112] = v;
}

u8 PK8::otIntensity(void) const
{
    return data[0x113];
}
void PK8::otIntensity(u8 v)
{
    data[0x113] = v;
}

u8 PK8::otMemory(void) const
{
    return data[0x114];
}
void PK8::otMemory(u8 v)
{
    data[0x114] = v;
}

u16 PK8::otTextVar(void) const
{
    return *(u16*)(data + 0x116);
}
void PK8::otTextVar(u16 v)
{
    *(u16*)(data + 0x116) = v;
}

u8 PK8::otFeeling(void) const
{
    return data[0x118];
}
void PK8::otFeeling(u8 v)
{
    data[0x118] = v;
}

u8 PK8::eggYear(void) const
{
    return data[0x119];
}
void PK8::eggYear(u8 v)
{
    data[0x119] = v;
}

u8 PK8::eggMonth(void) const
{
    return data[0x11A];
}
void PK8::eggMonth(u8 v)
{
    data[0x11A] = v;
}

u8 PK8::eggDay(void) const
{
    return data[0x11B];
}
void PK8::eggDay(u8 v)
{
    data[0x11B] = v;
}

u8 PK8::metYear(void) const
{
    return data[0x11C];
}
void PK8::metYear(u8 v)
{
    data[0x11C] = v;
}

u8 PK8::metMonth(void) const
{
    return data[0x11D];
}
void PK8::metMonth(u8 v)
{
    data[0x11D] = v;
}

u8 PK8::metDay(void) const
{
    return data[0x11E];
}
void PK8::metDay(u8 v)
{
    data[0x11E] = v;
}

u16 PK8::eggLocation(void) const
{
    return *(u16*)(data + 0x120);
}
void PK8::eggLocation(u16 v)
{
    *(u16*)(data + 0x120) = v;
}

u16 PK8::metLocation(void) const
{
    return *(u16*)(data + 0x122);
}
void PK8::metLocation(u16 v)
{
    *(u16*)(data + 0x122) = v;
}

u8 PK8::ball(void) const
{
    return data[0x124];
}
void PK8::ball(u8 v)
{
    data[0x124] = v;
}

u8 PK8::metLevel(void) const
{
    return data[0x125] & ~0x80;
}
void PK8::metLevel(u8 v)
{
    data[0x125] = (data[0x125] & 0x80) | v;
}

u8 PK8::otGender(void) const
{
    return data[0x125] >> 7;
}
void PK8::otGender(u8 v)
{
    data[0x125] = (data[0x125] & ~0x80) | (v << 7);
}

bool PK8::hyperTrain(u8 num) const
{
    return (data[0x126] & (1 << num)) == 1 << num;
}
void PK8::hyperTrain(u8 num, bool v)
{
    data[0x126] = (u8)((data[0x126] & ~(1 << num)) | (v ? 1 << num : 0));
}

bool PK8::moveRecordFlag(u8 index) const
{
    return (*(data + (index >> 3)) & (index & 7)) == 1;
}
void PK8::moveRecordFlag(u8 index, bool v)
{
    *(data + (index >> 3)) = (*(data + (index >> 3)) & ~(index & 7)) | ((v ? 1 : 0) << (index & 7));
}

int PK8::partyStat(Stat stat) const
{
    if (length == 0x158)
    {
        return -1;
    }
    return *(u16*)(data + 0x14A + u8(stat) * 2);
}

void PK8::partyStat(Stat stat, u16 v)
{
    if (length == 0x158)
    {
        *(u16*)(data + 0x14A + u8(stat) * 2) = v;
    }
}

int PK8::partyLevel() const
{
    if (length == 0x158)
    {
        return -1;
    }
    return *(data + 0x148);
}

void PK8::partyLevel(u8 v)
{
    if (length == 0x158)
    {
        *(data + 0x148) = v;
    }
}

u16 PK8::dynamaxType(void) const
{
    return *(u16*)(data + 0x156);
}
void PK8::dynamaxType(u16 v)
{
    *(u16*)(data + 0x156) = v;
}

u8 PK8::currentFriendship(void) const
{
    return currentHandler() == 0 ? otFriendship() : htFriendship();
}
void PK8::currentFriendship(u8 v)
{
    if (currentHandler() == 0)
        otFriendship(v);
    else
        htFriendship(v);
}

u8 PK8::oppositeFriendship(void) const
{
    return currentHandler() == 1 ? otFriendship() : htFriendship();
}
void PK8::oppositeFriendship(u8 v)
{
    if (currentHandler() == 1)
        otFriendship(v);
    else
        htFriendship(v);
}

void PK8::refreshChecksum(void)
{
    u16 chk = 0;
    for (int i = 8; i < 0x148; i += 2)
    {
        chk += *(u16*)(data + i);
    }
    checksum(chk);
}

u8 PK8::hpType(void) const
{
    return 15 *
           ((iv(Stat::HP) & 1) + 2 * (iv(Stat::ATK) & 1) + 4 * (iv(Stat::DEF) & 1) + 8 * (iv(Stat::SPD) & 1) + 16 * (iv(Stat::SPATK) & 1) +
               32 * (iv(Stat::SPDEF) & 1)) /
           63;
}
void PK8::hpType(u8 v)
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

u16 PK8::TSV(void) const
{
    return (TID() ^ SID()) >> 4;
}
u16 PK8::PSV(void) const
{
    return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 4;
}

u8 PK8::level(void) const
{
    u8 i      = 1;
    u8 xpType = expType();
    while (experience() >= expTable(i, xpType) && ++i < 100)
        ;
    return i;
}

void PK8::level(u8 v)
{
    experience(expTable(v - 1, expType()));
}

bool PK8::shiny(void) const
{
    return TSV() == PSV();
}
void PK8::shiny(bool v)
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

u16 PK8::formSpecies(void) const
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
        else if (form < formcount)
        {
            tmpSpecies += form - 1;
        }
    }

    return tmpSpecies;
}

u16 PK8::stat(Stat stat) const
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
    return calc * mult / 10;
}
