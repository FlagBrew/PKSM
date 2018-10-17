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

#include "PK4.hpp"

void PK4::shuffleArray(void)
{
    static const int blockLength = 32;
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

void PK4::crypt(void)
{
	u32 chk = checksum();

    for (int i = 0x08; i < 4 * 32 + 8; i += 2)
    {
        chk = seedStep(chk);
        data[i] ^= (chk >> 16);
        data[i+1] ^= (chk >> 24);
    }
}

PK4::PK4(u8* dt, bool ekx)
{
    length = 136;
    
    std::copy(dt, dt + length, data);
    if (ekx)
        decrypt();
}

void PK4::decrypt(void)
{
    crypt();
    shuffleArray();
}

void PK4::encrypt(void)
{
    refreshChecksum();
    for (int i = 0; i < 11; i++)
    {
        shuffleArray();
    }
    crypt();
}

std::unique_ptr<PKX> PK4::clone(void) { return std::make_unique<PK4>(data); }

u8 PK4::generation(void) const { return 4; }

u32 PK4::encryptionConstant(void) const { return PID(); }
void PK4::encryptionConstant(u32 v) { (void)v; }

u8 PK4::currentFriendship(void) const { return otFriendship(); }
void PK4::currentFriendship(u8 v) { otFriendship(v); }

u8 PK4::currentHandler(void) const { return 0; }
void PK4::currentHandler(u8 v) { (void)v; }

u8 PK4::abilityNumber(void) const { return 1 << ((PID() >> 16) & 1); }
void PK4::abilityNumber(u8 v) { (void)v; }

u32 PK4::PID(void) const { return *(u32*)(data); }
void PK4::PID(u32 v) { *(u32*)(data) = v; }

u16 PK4::sanity(void) const { return *(u16*)(data + 0x04); }
void PK4::sanity(u16 v) { *(u16*)(data + 0x04) = v; }

u16 PK4::checksum(void) const { return *(u16*)(data + 0x06); }
void PK4::checksum(u16 v) { *(u16*)(data + 0x06) = v; }

u16 PK4::species(void) const { return *(u16*)(data + 0x08); }
void PK4::species(u16 v) { *(u16*)(data + 0x08) = v; }

u16 PK4::heldItem(void) const { return *(u16*)(data + 0x0A); }
void PK4::heldItem(u16 v) { *(u16*)(data + 0x0A) = v; }

u16 PK4::TID(void) const { return *(u16*)(data + 0x0C); }
void PK4::TID(u16 v) { *(u16*)(data + 0x0C) = v; }

u16 PK4::SID(void) const { return *(u16*)(data + 0x0E); }
void PK4::SID(u16 v) { *(u16*)(data + 0x0E) = v; }

u32 PK4::experience(void) const { return *(u32*)(data + 0x10); }
void PK4::experience(u32 v) { *(u32*)(data + 0x10) = v; }

u8 PK4::otFriendship(void) const { return data[0x14]; }
void PK4::otFriendship(u8 v) { data[0x14] = v; }

u8 PK4::ability(void) const { return data[0x15]; }
void PK4::ability(u8 v) { data[0x15] = v; }

u16 PK4::markValue(void) const { return data[0x16]; }
void PK4::markValue(u16 v) { data[0x16] = v; }

u8 PK4::language(void) const { return data[0x17]; }
void PK4::language(u8 v) { data[0x17] = v; }

u8 PK4::ev(u8 ev) const { return data[0x18 + ev]; }
void PK4::ev(u8 ev, u8 v) { data[0x18 + ev] = v; }

u8 PK4::contest(u8 contest) const { return data[0x1E + contest]; }
void PK4::contest(u8 contest, u8 v) { data[0x1E + contest] = v; }

bool PK4::ribbon(u8 ribcat, u8 ribnum) const
{
    static u8 ribIndex[12] = { 0x24, 0x25, 0x26, 0x27, 0x3C, 0x3D, 0x3E, 0x3F, 0x60, 0x61, 0x62, 0x63 };
    return (data[ribIndex[ribcat]] & (1 << ribnum)) == 1 << ribnum;
}

void PK4::ribbon(u8 ribcat, u8 ribnum, u8 v)
{
    static u8 ribIndex[12] = { 0x24, 0x25, 0x26, 0x27, 0x3C, 0x3D, 0x3E, 0x3F, 0x60, 0x61, 0x62, 0x63 };
    data[ribIndex[ribcat]] = (u8)((data[ribIndex[ribcat]] & ~(1 << ribnum)) | (v ? 1 << ribnum : 0));
}

u16 PK4::move(u8 m) const { return *(u16*)(data + 0x28 + m*2); }
void PK4::move(u8 m, u16 v) { *(u16*)(data + 0x28 + m*2) = v; }

u8 PK4::PP(u8 m) const { return data[0x30 + m]; }
void PK4::PP(u8 m, u8 v) { data[0x30 + m] = v; }

u8 PK4::PPUp(u8 m) const { return data[0x34 + m]; }
void PK4::PPUp(u8 m, u8 v) { data[0x34 + m] = v; }

u8 PK4::iv(u8 stat) const
{
    u32 buffer = *(u32*)(data + 0x38);
    return (u8)((buffer >> 5*stat) & 0x1F);
}

void PK4::iv(u8 stat, u8 v)
{
    u32 buffer = *(u32*)(data + 0x38);
    u32 mask = 0xFFFFFFFF ^ 0x1F << 5*stat;
    buffer &= mask;
    buffer ^= ((v & 0x1F) << (5*stat));
    *(u32*)(data + 0x38) = buffer;
}

bool PK4::egg(void) const { return ((*(u32*)(data + 0x38) >> 30) & 0x1) == 1; }
void PK4::egg(bool v) { *(u32*)(data + 0x38) = (u32)((*(u32*)(data + 0x38) & ~0x40000000) | (u32)(v ? 0x40000000 : 0)); }

bool PK4::nicknamed(void) const { return ((*(u32*)(data + 0x38) >> 31) & 0x1) == 1; }
void PK4::nicknamed(bool v) { *(u32*)(data + 0x38) = (*(u32*)(data + 0x38) & 0x7FFFFFFF) | (v ? 0x80000000 : 0); }

bool PK4::fatefulEncounter(void) const { return (data[0x40] & 1) == 1; }
void PK4::fatefulEncounter(bool v) { data[0x40] = (u8)((data[0x40] & ~0x01) | (v ? 1 : 0)); }

u8 PK4::gender(void) const { return (data[0x40] >> 1) & 0x3; }
void PK4::gender(u8 g) { data[0x40] = u8((data[0x40] & ~0x06) | (g << 1)); }

u8 PK4::alternativeForm(void) const { return data[0x40] >> 3; }
void PK4::alternativeForm(u8 v) { data[0x40] = u8((data[0x40] & 0x07) | (v << 3)); }

u8 PK4::nature(void) const { return PID() % 25; }
void PK4::nature(u8) { }

u8 PK4::shinyLeaf(void) const { return *(u8*)(data + 0x41); }
void PK4::shinyLeaf(u8 v) { *(u8*)(data + 0x41) = v; }

std::string PK4::nickname(void) const { return StringUtils::getString4(data, 0x48, 11); }
void PK4::nickname(const char* v) { StringUtils::setString4(data, v, 0x48, 11); }

u8 PK4::version(void) const { return data[0x5F]; }
void PK4::version(u8 v) { data[0x5F] = v; }

std::string PK4::otName(void) const { return StringUtils::getString4(data, 0x68, 8); }
void PK4::otName(const char* v) { StringUtils::setString4(data, v, 0x68, 8); }

u8 PK4::eggYear(void) const { return data[0x78]; }
void PK4::eggYear(u8 v) { data[0x78] = v; }

u8 PK4::eggMonth(void) const { return data[0x79]; }
void PK4::eggMonth(u8 v) { data[0x79] = v;}

u8 PK4::eggDay(void) const { return data[0x7A]; }
void PK4::eggDay(u8 v) { data[0x7A] = v; }

u8 PK4::metYear(void) const { return data[0x7B]; }
void PK4::metYear(u8 v) { data[0x7B] = v; }

u8 PK4::metMonth(void) const { return data[0x7C]; }
void PK4::metMonth(u8 v) { data[0x7C] = v; }

u8 PK4::metDay(void) const { return data[0x7D]; }
void PK4::metDay(u8 v) { data[0x7D] = v; }

u16 PK4::eggLocation(void) const
{
    u16 hgssLoc = *(u16*)(data + 0x44);
    if (hgssLoc != 0)
        return hgssLoc;
    return *(u16*)(data + 0x7E);
}
void PK4::eggLocation(u16 v)
{
    if(v == 0)
    {
        *(u16*)(data + 0x44) = v;
        *(u16*)(data + 0x7E) = v;
    }
    else if ((v < 2000 && v > 111) || (v < 3000 && v > 2010))
    {
        *(u16*)(data + 0x44) = v;
        *(u16*)(data + 0x7E) = 0xBBA;
    }
    else
    {
        // If this pokemon is from Platinum, HeartGold, or SoulSilver
        *(u16*)(data + 0x44) = (version() == 12 || version() == 7 || version() == 8) ? v : 0;
        *(u16*)(data + 0x7E) = v;
    }
}

u16 PK4::metLocation(void) const
{
    u16 hgssLoc = *(u16*)(data + 0x46);
    if (hgssLoc != 0)
        return hgssLoc;
    return *(u16*)(data + 0x80);
}
void PK4::metLocation(u16 v)
{
    if(v == 0)
    {
        *(u16*)(data + 0x46) = v;
        *(u16*)(data + 0x80) = v;
    }
    else if ((v < 2000 && v > 111) || (v < 3000 && v > 2010))
    {
        *(u16*)(data + 0x46) = v;
        *(u16*)(data + 0x80) = 0xBBA;
    }
    else
    {
        // If this pokemon is from Platinum, HeartGold, or SoulSilver
        *(u16*)(data + 0x46) = (version() == 12 || version() == 7 || version() == 8) ? v : 0;
        *(u16*)(data + 0x80) = v;
    }
}

u8 PK4::pkrs(void) const { return data[0x82]; }
void PK4::pkrs(u8 v) { data[0x82] = v; }

u8 PK4::pkrsDays(void) const { return data[0x82] & 0xF; };
void PK4::pkrsDays(u8 v) { data[0x82] = (u8)((data[0x82] & ~0xF) | v); }

u8 PK4::pkrsStrain(void) const { return data[0x82] >> 4; };
void PK4::pkrsStrain(u8 v) { data[0x82] = (u8)((data[0x82] & 0xF) | v << 4); }

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

u8 PK4::metLevel(void) const { return data[0x84] & ~0x80; }
void PK4::metLevel(u8 v) { data[0x84] = (data[0x84] & 0x80) | v; }

u8 PK4::otGender(void) const { return data[0x84] >> 7; }
void PK4::otGender(u8 v) { data[0x84] = (data[0x84] & ~0x80) | (v << 7); }

u8 PK4::encounterType(void) const { return data[0x85]; }
void PK4::encounterType(u8 v) { data[0x85] = v; }

u8 PK4::characteristic(void) const
{
    u8 maxIV = 0, pm6stat = 0, pm6 = PID() % 6;
    for (int i = 0; i < 6; i++) if (iv(i) > maxIV) maxIV = iv(i);
    for (int i = 0; i < 6; i++)
    {
        pm6stat = (pm6 + i) % 6;
        if (iv(i) == maxIV) break;
    }
    return pm6stat * 5 + maxIV % 5;
}

void PK4::refreshChecksum(void)
{
    u16 chk = 0;
    for (u8 i = 8; i < length; i += 2)
    {
        chk += *(u16*)(data + i);
    }
    checksum(chk);
}

u8 PK4::hpType(void) const { return 15 * ((iv(0) & 1) + 2*(iv(1) & 1) + 4*(iv(2) & 1) + 8*(iv(3) & 1) + 16*(iv(4) & 1) + 32*(iv(5) & 1)) / 63; }
void PK4::hpType(u8 v)
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

u16 PK4::TSV(void) const { return (TID() ^ SID()) >> 3; }
u16 PK4::PSV(void) const { return ((PID() >> 16) ^ (PID() & 0xFFFF)) >> 3; }

u8 PK4::level(void) const
{
    u8 i = 1;
    u8 xpType = PersonalDPPtHGSS::expType(species());
    while (experience() >= expTable(i, xpType) && ++i < 100);
    return i;
}

void PK4::level(u8 v)
{
    experience(expTable(v - 1, PersonalDPPtHGSS::expType(species())));
}

bool PK4::shiny(void) const { return TSV() == PSV(); }
void PK4::shiny(bool v)
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

u16 PK4::formSpecies(void) const
{
    u16 tmpSpecies = species();
    u8 form = alternativeForm();
    u8 formcount = PersonalDPPtHGSS::formCount(tmpSpecies);

    if (form && form < formcount)
    {
        u16 backSpecies = tmpSpecies;
        tmpSpecies = PersonalDPPtHGSS::formStatIndex(tmpSpecies);
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

u16 PK4::stat(const u8 stat) const
{
    u16 tmpSpecies = formSpecies(), final;
    u8 mult = 10, basestat = 0;

    if (stat == 0) basestat = PersonalDPPtHGSS::baseHP(tmpSpecies);
    else if (stat == 1) basestat = PersonalDPPtHGSS::baseAtk(tmpSpecies);
    else if (stat == 2) basestat = PersonalDPPtHGSS::baseDef(tmpSpecies);
    else if (stat == 3) basestat = PersonalDPPtHGSS::baseSpe(tmpSpecies);
    else if (stat == 4) basestat = PersonalDPPtHGSS::baseSpa(tmpSpecies);
    else if (stat == 5) basestat = PersonalDPPtHGSS::baseSpd(tmpSpecies);

    if (stat == 0) 
        final = 10 + (2 * basestat + iv(stat) + ev(stat) / 4 + 100) * level() / 100;
    else
        final = 5 + (2 * basestat + iv(stat) + ev(stat) / 4) * level() / 100; 
    if (nature() / 5 + 1 == stat) mult++;
    if (nature() % 5 + 1 == stat) mult--;
    return final * mult / 10;
}

std::unique_ptr<PKX> PK4::next(void) const
{
    u8 dt[136];
    std::copy(data, data + 136, dt);

    // Clear HGSS data
    *(u16*)(dt + 0x86) = 0;

    // Clear PtHGSS met data
    *(u32*)(dt + 0x44) = 0;

    std::unique_ptr<PKX> pk5(new PK5(dt));

    time_t t = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *) &t);

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
                : 30001); // Pokétransfer (not Crown)

    pk5->ball(ball());

    pk5->nickname(nickname().c_str());
    pk5->otName(otName().c_str());

    // Check level
    pk5->metLevel(pk5->level());
    
    //Remove HM
    u16 moves[4] = { move(0), move(1), move(2), move(3) };

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