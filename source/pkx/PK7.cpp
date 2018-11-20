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

#include "PK7.hpp"

void PK7::shuffleArray(void)
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

void PK7::crypt(void)
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

PK7::PK7(u8* dt, bool ekx, bool party)
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

void PK7::decrypt(void)
{
    crypt();
    shuffleArray();
}

void PK7::encrypt(void)
{
    refreshChecksum();
    for (int i = 0; i < 11; i++)
    {
        shuffleArray();
    }
    crypt();
}

std::unique_ptr<PKX> PK7::clone(void) { return std::make_unique<PK7>(data); }

Generation PK7::generation(void) const { return Generation::SEVEN; }

u32 PK7::encryptionConstant(void) const { return *(u32*)(data); }
void PK7::encryptionConstant(u32 v) { *(u32*)(data) = v; }

u16 PK7::sanity(void) const { return *(u16*)(data + 0x04); }
void PK7::sanity(u16 v) { *(u16*)(data + 0x04) = v; }

u16 PK7::checksum(void) const { return *(u16*)(data + 0x06); }
void PK7::checksum(u16 v) { *(u16*)(data + 0x06) = v; }

u16 PK7::species(void) const { return *(u16*)(data + 0x08); }
void PK7::species(u16 v) { *(u16*)(data + 0x08) = v; }

u16 PK7::heldItem(void) const { return *(u16*)(data + 0x0A); }
void PK7::heldItem(u16 v) { *(u16*)(data + 0x0A) = v; }

u16 PK7::TID(void) const { return *(u16*)(data + 0x0C); }
void PK7::TID(u16 v) { *(u16*)(data + 0x0C) = v; }

u16 PK7::SID(void) const { return *(u16*)(data + 0x0E); }
void PK7::SID(u16 v) { *(u16*)(data + 0x0E) = v; }

u32 PK7::experience(void) const { return *(u32*)(data + 0x10); }
void PK7::experience(u32 v) { *(u32*)(data + 0x10) = v; }

u8 PK7::ability(void) const { return data[0x14]; }
void PK7::ability(u8 v)
{
    u16 tmpSpecies = formSpecies();
    u8 abilitynum;

    if (v == 0) abilitynum = 1;
    else if (v == 1) abilitynum = 2;
    else abilitynum = 4;

    abilityNumber(abilitynum);
    data[0x14] = PersonalSMUSUM::ability(tmpSpecies, v);    
}

u8 PK7::abilityNumber(void) const { return data[0x15]; }
void PK7::abilityNumber(u8 v) { data[0x15] = v; }

u16 PK7::markValue(void) const { return *(u16*)(data + 0x16); }
void PK7::markValue(u16 v) { *(u16*)(data + 0x16) = v; }

u32 PK7::PID(void) const { return *(u32*)(data + 0x18); }
void PK7::PID(u32 v) { *(u32*)(data + 0x18) = v; }

u8 PK7::nature(void) const { return data[0x1C]; }
void PK7::nature(u8 v) { data[0x1C] = v; }

bool PK7::fatefulEncounter(void) const { return (data[0x1D] & 1) == 1; }
void PK7::fatefulEncounter(bool v) { data[0x1D] = (u8)((data[0x1D] & ~0x01) | (v ? 1 : 0)); }

u8 PK7::gender(void) const { return (data[0x1D] >> 1) & 0x3; }
void PK7::gender(u8 v) { data[0x1D] = u8((data[0x1D] & ~0x06) | (v << 1)); }

u8 PK7::alternativeForm(void) const { return data[0x1D] >> 3; }
void PK7::alternativeForm(u8 v) { data[0x1D] = u8((data[0x1D] & 0x07) | (v << 3)); }

u8 PK7::ev(u8 ev) const { return data[0x1E + ev]; }
void PK7::ev(u8 ev, u8 v) { data[0x1E + ev] = v; }

u8 PK7::contest(u8 contest) const { return data[0x24 + contest]; }
void PK7::contest(u8 contest, u8 v) { data[0x24 + contest] = v; }

u8 PK7::pelagoEventStatus(void) const { return data[0x2A]; }
void PK7::pelagoEventStatus(u8 v) { data[0x2A] = v; }

u8 PK7::pkrs(void) const { return data[0x2B]; }
void PK7::pkrs(u8 v) { data[0x2B] = v; }

u8 PK7::pkrsDays(void) const { return data[0x2B] & 0xF; };
void PK7::pkrsDays(u8 v) { data[0x2B] = (u8)((data[0x2B] & ~0xF) | v); }

u8 PK7::pkrsStrain(void) const { return data[0x2B] >> 4; };
void PK7::pkrsStrain(u8 v) { data[0x2B] = (u8)((data[0x2B] & 0xF) | v << 4); }

bool PK7::ribbon(u8 ribcat, u8 ribnum) const { return (data[0x30 + ribcat] & (1 << ribnum)) == 1 << ribnum; }
void PK7::ribbon(u8 ribcat, u8 ribnum, u8 v) { data[0x30 + ribcat] = (u8)((data[0x30 + ribcat] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0)); }

std::string PK7::nickname(void) const { return StringUtils::getString(data, 0x40, 12); }
void PK7::nickname(const char* v) { StringUtils::setString(data, v, 0x40, 12); }

u16 PK7::move(u8 m) const { return *(u16*)(data + 0x5A + m*2); }
void PK7::move(u8 m, u16 v) { *(u16*)(data + 0x5A + m*2) = v; }

u8 PK7::PP(u8 m) const { return data[0x62 + m]; }
void PK7::PP(u8 m, u8 v) { data[0x62 + m] = v; }

u8 PK7::PPUp(u8 m) const { return data[0x66 + m]; }
void PK7::PPUp(u8 m, u8 v) { data[0x66 + m] = v; }

u16 PK7::relearnMove(u8 m) const { return *(u16*)(data + 0x6A + m*2); }
void PK7::relearnMove(u8 m, u16 v) { *(u16*)(data + 0x6A + m*2) = v; }

u8 PK7::iv(u8 stat) const
{
    u32 buffer = *(u32*)(data + 0x74);
    return (u8)((buffer >> 5*stat) & 0x1F);
}

void PK7::iv(u8 stat, u8 v)
{
    u32 buffer = *(u32*)(data + 0x74);
    u32 mask = 0xFFFFFFFF ^ 0x1F << 5*stat;
    buffer &= mask;
    buffer ^= ((v & 0x1F) << (5*stat));
    *(u32*)(data + 0x74) = buffer;
}

bool PK7::egg(void) const { return ((*(u32*)(data + 0x74) >> 30) & 0x1) == 1; }
void PK7::egg(bool v) { *(u32*)(data + 0x74) = (u32)((*(u32*)(data + 0x74) & ~0x40000000) | (u32)(v ? 0x40000000 : 0)); }

bool PK7::nicknamed(void) const { return ((*(u32*)(data + 0x74) >> 31) & 0x1) == 1; }
void PK7::nicknamed(bool v) { *(u32*)(data + 0x74) = (*(u32*)(data + 0x74) & 0x7FFFFFFF) | (v ? 0x80000000 : 0); }

std::string PK7::htName(void) const { return StringUtils::getString(data, 0x78, 12); }
void PK7::htName(const char* v) { StringUtils::setString(data, v, 0x78, 12); }

u8 PK7::htGender(void) const { return data[0x92]; }
void PK7::htGender(u8 v) { data[0x92] = v; }

u8 PK7::currentHandler(void) const { return data[0x93]; }
void PK7::currentHandler(u8 v) { data[0x93] = v; }

u8 PK7::geoRegion(u8 region) const { return data[0x94 + region*2]; }
void PK7::geoRegion(u8 region, u8 v) { data[0x94 + region*2] = v; }

u8 PK7::geoCountry(u8 country) const { return data[0x95 + country*2]; }
void PK7::geoCountry(u8 country, u8 v) { data[0x95 + country*2] = v; }

u8 PK7::htFriendship(void) const { return data[0xA2]; }
void PK7::htFriendship(u8 v) { data[0xA2] = v; }

u8 PK7::htAffection(void) const { return data[0xA3]; }
void PK7::htAffection(u8 v) { data[0xA3] = v; }

u8 PK7::htIntensity(void) const { return data[0xA4]; }
void PK7::htIntensity(u8 v) { data[0xA4] = v; }

u8 PK7::htMemory(void) const { return data[0xA5]; }
void PK7::htMemory(u8 v) { data[0xA5] = v; }

u8 PK7::htFeeling(void) const { return data[0xA6]; }
void PK7::htFeeling(u8 v) { data[0xA6] = v; }

u16 PK7::htTextVar(void) const { return *(u16*)(data + 0xA8); }
void PK7::htTextVar(u16 v) { *(u16*)(data + 0xA8) = v; }

u8 PK7::fullness(void) const { return data[0xAE]; }
void PK7::fullness(u8 v) { data[0xAE] = v; }

u8 PK7::enjoyment(void) const { return data[0xAF]; }
void PK7::enjoyment(u8 v) { data[0xAF] = v; }

std::string PK7::otName(void) const { return StringUtils::getString(data, 0xB0, 12); }
void PK7::otName(const char* v) { StringUtils::setString(data, v, 0xB0, 12); }

u8 PK7::otFriendship(void) const { return data[0xCA]; }
void PK7::otFriendship(u8 v) { data[0xCA] = v; }

u8 PK7::otAffection(void) const { return data[0xCB]; }
void PK7::otAffection(u8 v) { data[0xCB] = v; }

u8 PK7::otIntensity(void) const { return data[0xCC]; }
void PK7::otIntensity(u8 v) { data[0xCC] = v; }

u8 PK7::otMemory(void) const { return data[0xCD]; }
void PK7::otMemory(u8 v) { data[0xCD] = v; }

u16 PK7::otTextVar(void) const { return *(u16*)(data + 0xCE); }
void PK7::otTextVar(u16 v) { *(u16*)(data + 0xCE) = v; }

u8 PK7::otFeeling(void) const { return data[0xD0]; }
void PK7::otFeeling(u8 v) { data[0xD0] = v; }

u8 PK7::eggYear(void) const { return data[0xD1]; }
void PK7::eggYear(u8 v) { data[0xD1] = v; }

u8 PK7::eggMonth(void) const { return data[0xD2]; }
void PK7::eggMonth(u8 v) { data[0xD2] = v; }

u8 PK7::eggDay(void) const { return data[0xD3]; }
void PK7::eggDay(u8 v) { data[0xD3] = v; }

u8 PK7::metYear(void) const { return data[0xD4]; }
void PK7::metYear(u8 v) { data[0xD4] = v; }

u8 PK7::metMonth(void) const { return data[0xD5]; }
void PK7::metMonth(u8 v) { data[0xD5] = v; }

u8 PK7::metDay(void) const { return data[0xD6]; }
void PK7::metDay(u8 v) { data[0xD6] = v; }

u16 PK7::eggLocation(void) const { return *(u16*)(data + 0xD8); }
void PK7::eggLocation(u16 v) { *(u16*)(data + 0xD8) = v; }

u16 PK7::metLocation(void) const { return *(u16*)(data + 0xDA); }
void PK7::metLocation(u16 v) { *(u16*)(data + 0xDA) = v;}

u8 PK7::ball(void) const { return data[0xDC]; }
void PK7::ball(u8 v) { data[0xDC] = v; }

u8 PK7::metLevel(void) const { return data[0xDD] & ~0x80; }
void PK7::metLevel(u8 v) { data[0xDD] = (data[0xDD] & 0x80) | v; }

u8 PK7::otGender(void) const { return data[0xDD] >> 7; }
void PK7::otGender(u8 v) { data[0xDD] = (data[0xDD] & ~0x80) | (v << 7); }

bool PK7::hyperTrain(u8 num) const { return (data[0xDE] & (1 << num)) == 1 << num; }
void PK7::hyperTrain(u8 num, bool v) { data[0xDE] = (u8)((data[0xDE] & ~(1 << num)) | (v ? 1 << num : 0)); }

u8 PK7::version(void) const { return data[0xDF]; }
void PK7::version(u8 v) { data[0xDF] = v; }

u8 PK7::country(void) const { return data[0xE0]; }
void PK7::country(u8 v) { data[0xE0] = v; }

u8 PK7::region(void) const { return data[0xE1]; }
void PK7::region(u8 v) { data[0xE1] = v; }

u8 PK7::consoleRegion(void) const { return data[0xE2]; }
void PK7::consoleRegion(u8 v) { data[0xE2] = v; }

u8 PK7::language(void) const { return data[0xE3]; }
void PK7::language(u8 v) { data[0xE3] = v; }

u8 PK7::currentFriendship(void) const { return currentHandler() == 0 ? otFriendship() : htFriendship(); }
void PK7::currentFriendship(u8 v) { if (currentHandler() == 0) otFriendship(v); else htFriendship(v); }

u8 PK7::oppositeFriendship(void) const { return currentHandler() == 1 ? otFriendship() : htFriendship(); }
void PK7::oppositeFriendship(u8 v) { if (currentHandler() == 1) otFriendship(v); else htFriendship(v); }

void PK7::refreshChecksum(void)
{
    u16 chk = 0;
    for (u8 i = 8; i < 232; i += 2)
    {
        chk += *(u16*)(data + i);
    }
    checksum(chk);
}

u8 PK7::hpType(void) const { return 15 * ((iv(0) & 1) + 2*(iv(1) & 1) + 4*(iv(2) & 1) + 8*(iv(3) & 1) + 16*(iv(4) & 1) + 32*(iv(5) & 1)) / 63; }
void PK7::hpType(u8 v)
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
        iv((iv(i) & 0x1E) + hpivs[v][i], i);
    }
}

u16 PK7::TSV(void) const { return (TID() ^ SID()) >> 4; }
u16 PK7::PSV(void) const { return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 4; }

u8 PK7::level(void) const
{
    u8 i = 1;
    u8 xpType = PersonalSMUSUM::expType(species());
    while (experience() >= expTable(i, xpType) && ++i < 100);
    return i;
}

void PK7::level(u8 v)
{
    experience(expTable(v - 1, PersonalSMUSUM::expType(species())));
}

bool PK7::shiny(void) const { return TSV() == PSV(); }
void PK7::shiny(bool v)
{
    if (v)
    {
        u16 buf = (PID() >> 16) ^ (TSV() << 4);
        *(u16*)(data + 0x18) = buf;
    }
    else
    {
        srand(PID());
        PID(rand());
    }
}

u16 PK7::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form = alternativeForm();
    u8 formcount = PersonalSMUSUM::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies = PersonalSMUSUM::formStatIndex(tmpSpecies);
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

u16 PK7::stat(const u8 stat) const
{
    u16 tmpSpecies = formSpecies(), final;
    u8 mult = 10, basestat = 0;

    if (stat == 0) basestat = PersonalSMUSUM::baseHP(tmpSpecies);
    else if (stat == 1) basestat = PersonalSMUSUM::baseAtk(tmpSpecies);
    else if (stat == 2) basestat = PersonalSMUSUM::baseDef(tmpSpecies);
    else if (stat == 3) basestat = PersonalSMUSUM::baseSpe(tmpSpecies);
    else if (stat == 4) basestat = PersonalSMUSUM::baseSpa(tmpSpecies);
    else if (stat == 5) basestat = PersonalSMUSUM::baseSpd(tmpSpecies);

    if (stat == 0) 
        final = 10 + ((2 * basestat) + ((((data[0xDE] >> hyperTrainLookup[stat]) & 1) == 1) ? 31 : iv(stat)) + ev(stat) / 4 + 100) * level() / 100;
    else
        final = 5 + (2 * basestat + ((((data[0xDE] >> hyperTrainLookup[stat]) & 1) == 1) ? 31 : iv(stat)) + ev(stat) / 4) * level() / 100; 
    if (nature() / 5 + 1 == stat) mult++;
    if (nature() % 5 + 1 == stat) mult--;
    return final * mult / 10;
}

std::unique_ptr<PKX> PK7::previous(void) const
{
    u8 dt[232];
    std::copy(data, data + 232, dt);

    // markvalue field moved, clear old gen 7 data
    *(u16*)(dt + 0x16) = 0;

    PK6 *pk6 = new PK6(dt);
    
    pk6->markValue(markValue());

    switch (abilityNumber())
    {
        case 1:
        case 2:
        case 4:
            u8 index = abilityNumber() >> 1;
            if (PersonalSMUSUM::ability(species(), index) == ability())
                pk6->ability(PersonalXYORAS::ability(species(), index));
    }

    // TODO
    pk6->htMemory(4);
    pk6->htTextVar(0);
    pk6->htIntensity(1);
    pk6->htFeeling(rand() % 10);
    //pk6->geoCountry
    //pk6->geoRegion

    // check illegal moves ???

    pk6->refreshChecksum();
    return std::unique_ptr<PKX>(pk6);
}

std::unique_ptr<PKX> PK7::next() const
{
    if (species() > 151) // Stop any non-gen-one PKM from going through. That could be bad.
    {
        return nullptr;
    }
    u8 dt[260];
    std::copy(data, data + length, dt);
}

int PK7::partyCurrHP(void) const
{
    if (length == 232)
    {
        return -1;
    }
    return *(u16*)(data + 0xF0);
}

void PK7::partyCurrHP(u16 v)
{
    if (length != 232)
    {
        *(u16*)(data + 0xF0) = v;
    }
}

int PK7::partyStat(const u8 stat) const
{
    if (length == 232)
    {
        return -1;
    }
    return *(u16*)(data + 0xF2 + stat*2);
}

void PK7::partyStat(const u8 stat, u16 v)
{
    if (length != 232)
    {
        *(u16*)(data + 0xF2 + stat*2) = v;
    }
}

int PK7::partyLevel() const
{
    if (length == 232)
    {
        return -1;
    }
    return *(data + 0xEC);
}

void PK7::partyLevel(u8 v)
{
    if (length != 232)
    {
        *(data + 0xEC) = v;
    }
}
