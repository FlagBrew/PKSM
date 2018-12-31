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

#include "PK6.hpp"
#include "loader.hpp"
#include "random.hpp"

void PK6::shuffleArray(void)
{
    static const int blockLength = 56;
    u8 seed = (((encryptionConstant() & 0x3E000) >> 0xD) % 24);
    static int aloc[24] = { 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 };
    static int bloc[24] = { 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 };
    static int cloc[24] = { 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1 };
    static int dloc[24] = { 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0 };
    int ord[4] = {aloc[seed], bloc[seed], cloc[seed], dloc[seed]};

    u8 cdata[length];
    std::copy(data, data + length, cdata);

    for (u8 i = 0; i < 4; i++)
    {
        std::copy(
            cdata + blockLength * ord[i] + 8, 
            cdata + blockLength * ord[i] + blockLength + 8, 
            data + 8 + blockLength * i
        );
    }
}

void PK6::crypt(void)
{
    u32 seed = encryptionConstant();
    for (int i = 0x08; i < 232; i += 2)
    {
        u16 temp = *(u16*)(data + i);
        seed = seedStep(seed);
        temp ^= (seed >> 16);
        *(u16*)(data + i) = temp;
    }
    seed = encryptionConstant();
    for (u32 i = 232; i < length; i += 2)
    {
        u16 temp = *(u16*)(data + i);
        seed = seedStep(seed);
        temp ^= (seed >> 16);
        *(u16*)(data + i) = temp;
    }
}

PK6::PK6(u8* dt, bool ekx, bool party)
{
    length = party ? 260 : 232;
    data = new u8[length];
    std::fill_n(data, length, 0);
    
    std::copy(dt, dt + length, data);
    if (ekx)
    {
        decrypt();
    }
}

void PK6::decrypt(void)
{
    crypt();
    shuffleArray();
}

void PK6::encrypt(void)
{
    refreshChecksum();
    for (int i = 0; i < 11; i++)
    {
        shuffleArray();
    }
    crypt();
}

std::unique_ptr<PKX> PK6::clone(void) { return std::make_unique<PK6>(data); }

Generation PK6::generation(void) const { return Generation::SIX; }

u32 PK6::encryptionConstant(void) const { return *(u32*)(data); }
void PK6::encryptionConstant(u32 v) { *(u32*)(data) = v; }

u16 PK6::sanity(void) const { return *(u16*)(data + 0x04); }
void PK6::sanity(u16 v) { *(u16*)(data + 0x04) = v; }

u16 PK6::checksum(void) const { return *(u16*)(data + 0x06); }
void PK6::checksum(u16 v) { *(u16*)(data + 0x06) = v; }

u16 PK6::species(void) const { return *(u16*)(data + 0x08); }
void PK6::species(u16 v) { *(u16*)(data + 0x08) = v; }

u16 PK6::heldItem(void) const { return *(u16*)(data + 0x0A); }
void PK6::heldItem(u16 v) { *(u16*)(data + 0x0A) = v; }

u16 PK6::TID(void) const { return *(u16*)(data + 0x0C); }
void PK6::TID(u16 v) { *(u16*)(data + 0x0C) = v; }

u16 PK6::SID(void) const { return *(u16*)(data + 0x0E); }
void PK6::SID(u16 v) { *(u16*)(data + 0x0E) = v; }

u32 PK6::experience(void) const { return *(u32*)(data + 0x10); }
void PK6::experience(u32 v) { *(u32*)(data + 0x10) = v; }

u8 PK6::ability(void) const { return data[0x14]; }
void PK6::ability(u8 v) { data[0x14] = v; }

void PK6::setAbility(u8 v)
{
    u16 tmpSpecies = formSpecies();
    u8 abilitynum;

    if (v == 0) abilitynum = 1;
    else if (v == 1) abilitynum = 2;
    else abilitynum = 4;

    abilityNumber(abilitynum);
    data[0x14] = PersonalXYORAS::ability(tmpSpecies, v);
}

u8 PK6::abilityNumber(void) const { return data[0x15]; }
void PK6::abilityNumber(u8 v) { data[0x15] = v; }

u8 PK6::trainingBagHits(void) const { return data[0x16]; }
void PK6::trainingBagHits(u8 v) { data[0x16] = v; }

u8 PK6::trainingBag(void) const { return data[0x17]; }
void PK6::trainingBag(u8 v) { data[0x17] = v; }

u32 PK6::PID(void) const { return *(u32*)(data + 0x18); }
void PK6::PID(u32 v) { *(u32*)(data + 0x18) = v; }

u8 PK6::nature(void) const { return data[0x1C]; }
void PK6::nature(u8 v) { data[0x1C] = v; }

bool PK6::fatefulEncounter(void) const { return (data[0x1D] & 1) == 1; }
void PK6::fatefulEncounter(bool v) { data[0x1D] = (u8)((data[0x1D] & ~0x01) | (v ? 1 : 0)); }

u8 PK6::gender(void) const { return (data[0x1D] >> 1) & 0x3; }
void PK6::gender(u8 v) { data[0x1D] = u8((data[0x1D] & ~0x06) | (v << 1)); }

u8 PK6::alternativeForm(void) const { return data[0x1D] >> 3; }
void PK6::alternativeForm(u8 v) { data[0x1D] = u8((data[0x1D] & 0x07) | (v << 3)); }

u8 PK6::ev(u8 ev) const { return data[0x1E + ev]; }
void PK6::ev(u8 ev, u8 v) { data[0x1E + ev] = v; }

u8 PK6::contest(u8 contest) const { return data[0x24 + contest]; }
void PK6::contest(u8 contest, u8 v) { data[0x24 + contest] = v; }

u16 PK6::markValue(void) const { return data[0x2A]; }
void PK6::markValue(u16 v) { data[0x2A] = v; }

u8 PK6::pkrs(void) const { return data[0x2B]; }
void PK6::pkrs(u8 v) { data[0x2B] = v; }

u8 PK6::pkrsDays(void) const { return data[0x2B] & 0xF; };
void PK6::pkrsDays(u8 v) { data[0x2B] = (u8)((data[0x2B] & ~0xF) | v); }

u8 PK6::pkrsStrain(void) const { return data[0x2B] >> 4; };
void PK6::pkrsStrain(u8 v) { data[0x2B] = (u8)((data[0x2B] & 0xF) | v << 4); }

bool PK6::ribbon(u8 ribcat, u8 ribnum) const { return (data[0x30 + ribcat] & (1 << ribnum)) == 1 << ribnum; }
void PK6::ribbon(u8 ribcat, u8 ribnum, u8 v) { data[0x30 + ribcat] = (u8)((data[0x30 + ribcat] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0)); }

u8 PK6::ribbonContestCount(void) const { return data[0x38]; }
void PK6::ribbonContestCount(u8 v) { data[0x38] = v; }

u8 PK6::ribbonBattleCount(void) const { return data[0x39]; }
void PK6::ribbonBattleCount(u8 v) { data[0x39] = v; }

std::string PK6::nickname(void) const { return StringUtils::getString(data, 0x40, 12); }
void PK6::nickname(const char* v) { StringUtils::setString(data, v, 0x40, 12); }

u16 PK6::move(u8 m) const { return *(u16*)(data + 0x5A + m*2); }
void PK6::move(u8 m, u16 v) { *(u16*)(data + 0x5A + m*2) = v; }

u8 PK6::PP(u8 m) const { return data[0x62 + m]; }
void PK6::PP(u8 m, u8 v) { data[0x62 + m] = v; }

u8 PK6::PPUp(u8 m) const { return data[0x66 + m]; }
void PK6::PPUp(u8 m, u8 v) { data[0x66 + m] = v; }

u16 PK6::relearnMove(u8 m) const { return *(u16*)(data + 0x6A + m*2); }
void PK6::relearnMove(u8 m, u16 v) { *(u16*)(data + 0x6A + m*2) = v; }

bool PK6::secretSuperTrainingUnlocked(void) const { return (data[0x72] & 1) == 1; }
void PK6::secretSuperTrainingUnlocked(bool v) { data[0x72] = (data[0x72] & ~1) | (v ? 1 : 0); }

bool PK6::secretSuperTrainingComplete(void) const { return (data[0x72] & 2) == 2; }
void PK6::secretSuperTrainingComplete(bool v) { data[0x72] = (data[0x72] & ~2) | (v ? 2 : 0);}

u8 PK6::iv(u8 stat) const
{
    u32 buffer = *(u32*)(data + 0x74);
    return (u8)((buffer >> 5*stat) & 0x1F);
}

void PK6::iv(u8 stat, u8 v)
{
    u32 buffer = *(u32*)(data + 0x74);
    buffer &= ~(0x1F << 5 * stat);
    buffer |= v << (5 * stat);
    *(u32*)(data + 0x74) = buffer;
}

bool PK6::egg(void) const { return ((*(u32*)(data + 0x74) >> 30) & 0x1) == 1; }
void PK6::egg(bool v) { *(u32*)(data + 0x74) = (u32)((*(u32*)(data + 0x74) & ~0x40000000) | (u32)(v ? 0x40000000 : 0)); }

bool PK6::nicknamed(void) const { return ((*(u32*)(data + 0x74) >> 31) & 0x1) == 1; }
void PK6::nicknamed(bool v) { *(u32*)(data + 0x74) = (*(u32*)(data + 0x74) & 0x7FFFFFFF) | (v ? 0x80000000 : 0); }

std::string PK6::htName(void) const { return StringUtils::getString(data, 0x78, 12); }
void PK6::htName(const char* v) { StringUtils::setString(data, v, 0x78, 12); }

u8 PK6::htGender(void) const { return data[0x92]; }
void PK6::htGender(u8 v) { data[0x92] = v; }

u8 PK6::currentHandler(void) const { return data[0x93]; }
void PK6::currentHandler(u8 v) { data[0x93] = v; }

u8 PK6::geoRegion(u8 region) const { return data[0x94 + region*2]; }
void PK6::geoRegion(u8 region, u8 v) { data[0x94 + region*2] = v; }

u8 PK6::geoCountry(u8 country) const { return data[0x95 + country*2]; }
void PK6::geoCountry(u8 country, u8 v) { data[0x95 + country*2] = v; }

u8 PK6::htFriendship(void) const { return data[0xA2]; }
void PK6::htFriendship(u8 v) { data[0xA2] = v; }

u8 PK6::htAffection(void) const { return data[0xA3]; }
void PK6::htAffection(u8 v) { data[0xA3] = v; }

u8 PK6::htIntensity(void) const { return data[0xA4]; }
void PK6::htIntensity(u8 v) { data[0xA4] = v; }

u8 PK6::htMemory(void) const { return data[0xA5]; }
void PK6::htMemory(u8 v) { data[0xA5] =v; }

u8 PK6::htFeeling(void) const { return data[0xA6]; }
void PK6::htFeeling(u8 v) { data[0xA6] = v; }

u16 PK6::htTextVar(void) const { return *(u16*)(data + 0xA8); }
void PK6::htTextVar(u16 v) { *(u16*)(data + 0xA8) = v; }

u8 PK6::fullness(void) const { return data[0xAE]; }
void PK6::fullness(u8 v) { data[0xAE] = v; }

u8 PK6::enjoyment(void) const { return data[0xAF]; }
void PK6::enjoyment(u8 v) { data[0xAF] = v; }

std::string PK6::otName(void) const { return StringUtils::getString(data, 0xB0, 12); }
void PK6::otName(const char* v) { StringUtils::setString(data, v, 0xB0, 12); }

u8 PK6::otFriendship(void) const { return data[0xCA]; }
void PK6::otFriendship(u8 v) { data[0xCA] = v; }

u8 PK6::otAffection(void) const { return data[0xCB]; }
void PK6::otAffection(u8 v) { data[0xCB] = v; }

u8 PK6::otIntensity(void) const { return data[0xCC]; }
void PK6::otIntensity(u8 v) { data[0xCC] = v; }

u8 PK6::otMemory(void) const { return data[0xCD]; }
void PK6::otMemory(u8 v) { data[0xCD] = v; }

u16 PK6::otTextVar(void) const { return *(u16*)(data + 0xCE); }
void PK6::otTextVar(u16 v) { *(u16*)(data + 0xCE) = v; }

u8 PK6::otFeeling(void) const { return data[0xD0]; }
void PK6::otFeeling(u8 v) { data[0xD0] = v; }

u8 PK6::eggYear(void) const { return data[0xD1]; }
void PK6::eggYear(u8 v) { data[0xD1] = v; }

u8 PK6::eggMonth(void) const { return data[0xD2]; }
void PK6::eggMonth(u8 v) { data[0xD2] = v; }

u8 PK6::eggDay(void) const { return data[0xD3]; }
void PK6::eggDay(u8 v) { data[0xD3] = v; }

u8 PK6::metYear(void) const { return data[0xD4]; }
void PK6::metYear(u8 v) { data[0xD4] = v; }

u8 PK6::metMonth(void) const { return data[0xD5]; }
void PK6::metMonth(u8 v) { data[0xD5] = v; }

u8 PK6::metDay(void) const { return data[0xD6]; }
void PK6::metDay(u8 v) { data[0xD6] = v; }

u16 PK6::eggLocation(void) const { return *(u16*)(data + 0xD8); }
void PK6::eggLocation(u16 v) { *(u16*)(data + 0xD8) = v; }

u16 PK6::metLocation(void) const { return *(u16*)(data + 0xDA); }
void PK6::metLocation(u16 v) { *(u16*)(data + 0xDA) = v;}

u8 PK6::ball(void) const { return data[0xDC]; }
void PK6::ball(u8 v) { data[0xDC] = v; }

u8 PK6::metLevel(void) const { return data[0xDD] & ~0x80; }
void PK6::metLevel(u8 v) { data[0xDD] = (data[0xDD] & 0x80) | v; }

u8 PK6::otGender(void) const { return data[0xDD] >> 7; }
void PK6::otGender(u8 v) { data[0xDD] = (data[0xDD] & ~0x80) | (v << 7); }

u8 PK6::encounterType(void) const { return data[0xDE]; }
void PK6::encounterType(u8 v) { data[0xDE] = v; }

u8 PK6::version(void) const { return data[0xDF]; }
void PK6::version(u8 v) { data[0xDF] = v; }

u8 PK6::country(void) const { return data[0xE0]; }
void PK6::country(u8 v) { data[0xE0] = v; }

u8 PK6::region(void) const { return data[0xE1]; }
void PK6::region(u8 v) { data[0xE1] = v; }

u8 PK6::consoleRegion(void) const { return data[0xE2]; }
void PK6::consoleRegion(u8 v) { data[0xE2] = v; }

u8 PK6::language(void) const { return data[0xE3]; }
void PK6::language(u8 v) { data[0xE3] = v; }

u8 PK6::currentFriendship(void) const { return currentHandler() == 0 ? otFriendship() : htFriendship(); }
void PK6::currentFriendship(u8 v) { if (currentHandler() == 0) otFriendship(v); else htFriendship(v); }

u8 PK6::oppositeFriendship(void) const { return currentHandler() == 1 ? otFriendship() : htFriendship(); }
void PK6::oppositeFriendship(u8 v) { if (currentHandler() == 1) otFriendship(v); else htFriendship(v); }

void PK6::refreshChecksum(void)
{
    u16 chk = 0;
    for (u8 i = 8; i < 232; i += 2)
    {
        chk += *(u16*)(data + i);
    }
    checksum(chk);
}

u8 PK6::hpType(void) const { return 15 * ((iv(0) & 1) + 2*(iv(1) & 1) + 4*(iv(2) & 1) + 8*(iv(3) & 1) + 16*(iv(4) & 1) + 32*(iv(5) & 1)) / 63; }
void PK6::hpType(u8 v)
{
    static constexpr u16 hpivs[16][6] = {
        { 1, 1, 0, 0, 0, 0 }, // Fighting
        { 0, 0, 0, 1, 0, 0 }, // Flying
        { 1, 1, 0, 1, 0, 0 }, // Poison
        { 1, 1, 1, 1, 0, 0 }, // Ground
        { 1, 1, 0, 0, 1, 0 }, // Rock
        { 1, 0, 0, 1, 1, 0 }, // Bug
        { 1, 0, 1, 1, 1, 0 }, // Ghost
        { 1, 1, 1, 1, 1, 0 }, // Steel
        { 1, 0, 1, 0, 0, 1 }, // Fire
        { 1, 0, 0, 1, 0, 1 }, // Water
        { 1, 0, 1, 1, 0, 1 }, // Grass
        { 1, 1, 1, 1, 0, 1 }, // Electric
        { 1, 0, 1, 0, 1, 1 }, // Psychic
        { 1, 0, 0, 1, 1, 1 }, // Ice
        { 1, 0, 1, 1, 1, 1 }, // Dragon
        { 1, 1, 1, 1, 1, 1 }, // Dark
    };

    for (u8 i = 0; i < 6; i++)
    {
        iv(i, (iv(i) & 0x1e) + hpivs[v][i]);
    }
}

u16 PK6::TSV(void) const { return (TID() ^ SID()) >> 4; }
u16 PK6::PSV(void) const { return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 4; }

u8 PK6::level(void) const
{
    u8 i = 1;
    u8 xpType = PersonalXYORAS::expType(species());
    while (experience() >= expTable(i, xpType) && ++i < 100);
    return i;
}

void PK6::level(u8 v)
{
    experience(expTable(v - 1, PersonalXYORAS::expType(species())));
}

bool PK6::shiny(void) const { return TSV() == PSV(); }
void PK6::shiny(bool v)
{
    if (v)
    {
        u16 buf = (PID() >> 16) ^ (TSV() << 4);
        *(u16*)(data + 0x18) = buf;
    }
    else
    {
        randomNumbers.seed(PID());
        PID(randomNumbers());
    }
}

u16 PK6::formSpecies(void) const
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

u16 PK6::stat(const u8 stat) const
{
    u16 tmpSpecies = formSpecies(), final;
    u8 mult = 10, basestat = 0;

    if (stat == 0) basestat = PersonalXYORAS::baseHP(tmpSpecies);
    else if (stat == 1) basestat = PersonalXYORAS::baseAtk(tmpSpecies);
    else if (stat == 2) basestat = PersonalXYORAS::baseDef(tmpSpecies);
    else if (stat == 3) basestat = PersonalXYORAS::baseSpe(tmpSpecies);
    else if (stat == 4) basestat = PersonalXYORAS::baseSpa(tmpSpecies);
    else if (stat == 5) basestat = PersonalXYORAS::baseSpd(tmpSpecies);

    if (stat == 0) 
        final = 10 + (2 * basestat + iv(stat) + ev(stat) / 4 + 100) * level() / 100;
    else
        final = 5 + (2 * basestat + iv(stat) + ev(stat) / 4) * level() / 100; 
    if (nature() / 5 + 1 == stat) mult++;
    if (nature() % 5 + 1 == stat) mult--;
    return final * mult / 10;
}

std::unique_ptr<PKX> PK6::next(void) const
{
    u8 dt[232];
    std::copy(data, data + 232, dt);

    // markvalue field moved, clear old gen 6 data
    dt[0x2A] = 0;

    // Bank Data clearing
    for (int i = 0x94; i < 0x9E; i++)
        dt[i] = 0; // Geolocations
    for (int i = 0xAA; i < 0xB0; i++)
        dt[i] = 0; // Amie fullness/enjoyment
    for (int i = 0xE4; i < 0xE8; i++)
        dt[i] = 0; // unused
    dt[0x72] &= 0xFC; // low 2 bits of super training
    dt[0xDE] = 0; // gen 4 encounter type

    PK7 *pk7 = new PK7(dt);
    
    pk7->markValue(markValue());

    switch (abilityNumber())
    {
        case 1:
        case 2:
        case 4:
            u8 index = abilityNumber() >> 1;
            if (PersonalXYORAS::ability(species(), index) == ability())
                pk7->ability(PersonalSMUSUM::ability(species(), index));
    }

    pk7->htMemory(4);
    pk7->htTextVar(0);
    pk7->htIntensity(1);
    pk7->htFeeling(randomNumbers() % 10);
    pk7->geoCountry(0, TitleLoader::save->country());
    pk7->geoRegion(0, TitleLoader::save->subRegion());

    pk7->refreshChecksum();
    return std::unique_ptr<PKX>(pk7);
}

std::unique_ptr<PKX> PK6::previous(void) const
{
    u8 dt[232] = {0};
    PK5 *pk5 = new PK5(dt);

    pk5->species(species());
    pk5->TID(TID());
    pk5->SID(SID());
    pk5->experience(experience());
    pk5->PID(PID());
    pk5->ability(ability());

    pk5->markValue(markValue());
    pk5->language(language());
    
    for (int i = 0; i < 6; i++)
    {
        // EV Cap
        pk5->ev(i, ev(i) > 252 ? 252 : ev(i));
        pk5->iv(i, iv(i));
        pk5->contest(i, contest(i));
    }

    for (int i = 0; i < 4; i++)
    {
        pk5->move(i, move(i));
        pk5->PPUp(i, PPUp(i));
        pk5->PP(i, PP(i));
    }

    pk5->egg(egg());
    pk5->nicknamed(nicknamed());

    pk5->fatefulEncounter(fatefulEncounter());
    pk5->gender(gender());
    pk5->alternativeForm(alternativeForm());
    pk5->nature(nature());

    pk5->version(version());

    pk5->nickname(nickname().substr(0, 11).c_str());
    pk5->otName(otName().substr(0, 8).c_str());

    pk5->metYear(metYear());
    pk5->metMonth(metMonth());
    pk5->metDay(metDay());
    pk5->eggYear(eggYear());
    pk5->eggMonth(eggMonth());
    pk5->eggDay(eggDay());

    pk5->metLocation(metLocation());
    pk5->eggLocation(eggLocation());

    pk5->pkrsStrain(pkrsStrain());
    pk5->pkrsDays(pkrsDays());
    pk5->ball(ball());

    pk5->metLevel(metLevel());
    pk5->otGender(otGender());
    pk5->encounterType(encounterType());

    pk5->ribbon(6, 4, ribbon(0, 1)); // Hoenn Champion
    pk5->ribbon(0, 0, ribbon(0, 2)); // Sinnoh Champ
    pk5->ribbon(7, 0, ribbon(0, 7)); // Effort Ribbon

    pk5->ribbon(0, 7, ribbon(1, 0)); // Alert
    pk5->ribbon(1, 0, ribbon(1, 1)); // Shock
    pk5->ribbon(1, 1, ribbon(1, 2)); // Downcast
    pk5->ribbon(1, 2, ribbon(1, 3)); // Careless
    pk5->ribbon(1, 3, ribbon(1, 4)); // Relax
    pk5->ribbon(1, 4, ribbon(1, 5)); // Snooze
    pk5->ribbon(1, 5, ribbon(1, 6)); // Smile
    pk5->ribbon(1, 6, ribbon(1, 7)); // Gorgeous

    pk5->ribbon(1, 7, ribbon(2, 0)); // Royal
    pk5->ribbon(2, 0, ribbon(2, 1)); // Gorgeous Royal
    pk5->ribbon(6, 7, ribbon(2, 2)); // Artist
    pk5->ribbon(2, 1, ribbon(2, 3)); // Footprint
    pk5->ribbon(2, 2, ribbon(2, 4)); // Record
    pk5->ribbon(2, 4, ribbon(2, 5)); // Legend
    pk5->ribbon(7, 4, ribbon(2, 6)); // Country
    pk5->ribbon(7, 5, ribbon(2, 7)); // National

    pk5->ribbon(7, 6, ribbon(3, 0)); // Earth
    pk5->ribbon(7, 7, ribbon(3, 1)); // World
    pk5->ribbon(3, 2, ribbon(3, 2)); // Classic
    pk5->ribbon(3, 3, ribbon(3, 3)); // Premier
    pk5->ribbon(2, 3, ribbon(3, 4)); // Event
    pk5->ribbon(2, 6, ribbon(3, 5)); // Birthday
    pk5->ribbon(2, 7, ribbon(3, 6)); // Special
    pk5->ribbon(3, 0, ribbon(3, 7)); // Souvenir

    pk5->ribbon(3, 1, ribbon(4, 0)); // Wishing Ribbon
    pk5->ribbon(7, 1, ribbon(4, 1)); // Battle Champion
    pk5->ribbon(7, 2, ribbon(4, 2)); // Regional Champion
    pk5->ribbon(7, 3, ribbon(4, 3)); // National Champion
    pk5->ribbon(2, 5, ribbon(4, 4)); // World Champion

    pk5->otFriendship(PersonalBWB2W2::baseFriendship(pk5->species()));

    // Check if shiny pid needs to be modified
    u16 val = TID() ^ SID() ^ (PID() >> 16) ^ (PID() & 0xFFFF);
    if (shiny() && (val > 7) && (val < 16))
        pk5->PID(PID() ^ 0x80000000);

    for (int i = 0; i < 4; i++)
    {
        if (pk5->move(i) > TitleLoader::save->maxMove())
        {
            pk5->move(i, 0);
        }
    }

    pk5->refreshChecksum();
    return std::unique_ptr<PKX>(pk5);
}

int PK6::partyCurrHP(void) const
{
    if (length == 232)
    {
        return -1;
    }
    return *(u16*)(data + 0xF0);
}

void PK6::partyCurrHP(u16 v)
{
    if (length != 232)
    {
        *(u16*)(data + 0xF0) = v;
    }
}

int PK6::partyStat(const u8 stat) const
{
    if (length == 232)
    {
        return -1;
    }
    return *(u16*)(data + 0xF2 + stat*2);
}

void PK6::partyStat(const u8 stat, u16 v)
{
    if (length != 232)
    {
        *(u16*)(data + 0xF2 + stat*2) = v;
    }
}

int PK6::partyLevel() const
{
    if (length == 232)
    {
        return -1;
    }
    return *(data + 0xEC);
}

void PK6::partyLevel(u8 v)
{
    if (length != 232)
    {
        *(data + 0xEC) = v;
    }
}
