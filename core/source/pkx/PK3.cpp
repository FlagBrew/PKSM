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

#include "PK3.hpp"
#include "ValueConverter.hpp"
#include "endian.hpp"
#include "i18n.hpp"
#include "random.hpp"
#include "utils.hpp"
#include <algorithm>

u8 PK3::getUnownForm(u32 pid)
{
    u32 val = (pid & 0x3000000) >> 18 | (pid & 0x30000) >> 12 | (pid & 0x300) >> 6 | (pid & 0x3);
    return val % 28;
}

void PK3::shuffleArray(u8 sv)
{
    static const int blockLength = 12;
    u8 index                     = sv * 4;

    u8 cdata[length];
    std::copy(data, data + length, cdata);

    for (u8 block = 0; block < 4; block++)
    {
        u8 ofs = blockPosition(index + block);
        std::copy(cdata + 32 + blockLength * ofs, cdata + 32 + blockLength * ofs + blockLength, data + 32 + blockLength * block);
    }
}

void PK3::encrypt()
{
    if (!isEncrypted())
    {
        u8 sv = PID() % 24;
        refreshChecksum();
        shuffleArray(blockPositionInvert(sv));
        crypt();
    }
}

void PK3::decrypt()
{
    if (isEncrypted())
    {
        u8 sv = PID() % 24;
        crypt();
        shuffleArray(sv);
    }
}

void PK3::crypt(void)
{
    u32 seed    = Endian::convertTo<u32>(data) ^ Endian::convertTo<u32>(data + 4);
    auto xorkey = Endian::convertFrom<u32>(seed);

    for (int i = 32; i < 80; i++)
    {
        data[i] ^= xorkey[i % xorkey.size()];
    }
}

int PK3::swapBits(int value, int p1, int p2)
{
    int bit1 = (value >> p1) & 1;
    int bit2 = (value >> p2) & 1;
    int x    = bit1 ^ bit2;
    x        = (x << p1) | (x << p2);
    return value ^ x;
}

bool PK3::japanese(void) const
{
    return flagIsEgg() || language() == Language::JP;
}

u16 PK3::calcChecksum() const
{
    u16 chk = 0;
    for (int i = 0x20; i < 80; i += 2)
        chk += Endian::convertTo<u16>(data + i);
    return chk;
}

void PK3::refreshChecksum()
{
    checksum(calcChecksum());
}

bool PK3::isEncrypted() const
{
    return calcChecksum() != checksum();
}

PK3::PK3(u8* dt, bool party, bool directAccess) : PKX(dt, party ? 100 : 80, directAccess)
{
    if (isEncrypted())
    {
        decrypt();
    }
}

std::shared_ptr<PKX> PK3::clone(void) const
{
    // Can't use normal data constructor because of checksum encryption checks
    std::shared_ptr<PK3> ret = std::make_shared<PK3>(nullptr, length == 100);
    std::copy(data, data + length, ret->rawData());
    return ret;
}

Generation PK3::generation(void) const
{
    return Generation::THREE;
}

u32 PK3::encryptionConstant(void) const
{
    return PID();
}

void PK3::encryptionConstant(u32 v)
{
    (void)v;
}

u8 PK3::currentFriendship(void) const
{
    return otFriendship();
}
void PK3::currentFriendship(u8 v)
{
    otFriendship(v);
}

u8 PK3::currentHandler(void) const
{
    return 0;
}
void PK3::currentHandler(u8 v)
{
    (void)v;
}

u8 PK3::abilityNumber(void) const
{
    return 1 << ((PID() >> 16) & 1);
}
void PK3::abilityNumber(u8 v)
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

u32 PK3::PID(void) const
{
    return Endian::convertTo<u32>(data);
}
void PK3::PID(u32 v)
{
    Endian::convertFrom<u32>(data, v);
}

u16 PK3::TID(void) const
{
    return Endian::convertTo<u16>(data + 0x04);
}
void PK3::TID(u16 v)
{
    Endian::convertFrom<u16>(data + 0x04, v);
}

u16 PK3::SID(void) const
{
    return Endian::convertTo<u16>(data + 0x06);
}
void PK3::SID(u16 v)
{
    Endian::convertFrom<u16>(data + 0x06, v);
}

std::string PK3::nickname(void) const
{
    return StringUtils::getString3(data, 0x08, 10, japanese());
}
void PK3::nickname(const std::string& v)
{
    StringUtils::setString3(data, v, 0x08, 10, japanese());
}

Language PK3::language(void) const
{
    return Language(data[0x12]);
}
void PK3::language(Language v)
{
    data[0x12] = u8(v);
}

bool PK3::flagIsBadEgg(void) const
{
    return (data[0x13] & 1) == 1;
}
void PK3::flagIsBadEgg(bool v)
{
    data[0x13] = (u8)((data[0x13] & ~2) | (v ? 1 : 0));
}

bool PK3::flagHasSpecies(void) const
{
    return (data[0x13] & 2) == 2;
}
void PK3::flagHasSpecies(bool v)
{
    data[0x13] = (u8)((data[0x13] & ~2) | (v ? 2 : 0));
}

bool PK3::flagIsEgg(void) const
{
    return (data[0x13] & 4) == 4;
}
void PK3::flagIsEgg(bool v)
{
    data[0x13] = (u8)((data[0x13] & ~4) | (v ? 4 : 0));
}

std::string PK3::otName(void) const
{
    return StringUtils::getString3(data, 0x14, 7, japanese());
}
void PK3::otName(const std::string& v)
{
    StringUtils::setString3(data, v, 0x14, 7, japanese());
}

u16 PK3::markValue(void) const
{
    return swapBits(data[0x1B], 1, 2);
}
void PK3::markValue(u16 v)
{
    data[0x1B] = swapBits(v, 1, 2);
}

u16 PK3::checksum(void) const
{
    return Endian::convertTo<u16>(data + 0x1C);
}
void PK3::checksum(u16 v)
{
    Endian::convertFrom<u16>(data + 0x1C, v);
}

u16 PK3::sanity(void) const
{
    return Endian::convertTo<u16>(data + 0x1E);
}
void PK3::sanity(u16 v)
{
    Endian::convertFrom<u16>(data + 0x1E, v);
}

u16 PK3::speciesID3(void) const
{
    return Endian::convertTo<u16>(data + 0x20);
}
void PK3::speciesID3(u16 v)
{
    Endian::convertFrom<u16>(data + 0x20, v);
}

u16 PK3::species(void) const
{
    return SpeciesConverter::g3ToNational(speciesID3());
}
void PK3::species(u16 v)
{
    speciesID3(SpeciesConverter::nationalToG3(v));
    flagHasSpecies(species() > 0);
}

u16 PK3::heldItem3(void) const
{
    return Endian::convertTo<u16>(data + 0x22);
}
void PK3::heldItem3(u16 v)
{
    Endian::convertFrom<u16>(data + 0x22, v);
}

u16 PK3::heldItem(void) const
{
    return ItemConverter::g3ToNational(heldItem3());
}
void PK3::heldItem(u16 v)
{
    heldItem3(ItemConverter::nationalToG3(v));
}

u32 PK3::experience(void) const
{
    return Endian::convertTo<u32>(data + 0x24);
}
void PK3::experience(u32 v)
{
    Endian::convertFrom<u16>(data + 0x24, v);
}

u8 PK3::PPUp(u8 move) const
{
    return (data[0x28] >> (move * 2) & 3);
}
void PK3::PPUp(u8 move, u8 v)
{
    data[0x28] = (u8)((data[0x28] & ~(3 << (move * 2))) | (v & 3) << (move * 2));
}

u8 PK3::otFriendship(void) const
{
    return data[0x29];
}
void PK3::otFriendship(u8 v)
{
    data[0x29] = v;
}

u16 PK3::move(u8 move) const
{
    return Endian::convertTo<u16>(data + 0x2C + move * 2);
}
void PK3::move(u8 move, u16 v)
{
    Endian::convertFrom<u16>(data + 0x2C + move * 2, v);
}

u8 PK3::PP(u8 move) const
{
    return data[0x34 + move];
}
void PK3::PP(u8 move, u8 v)
{
    data[0x34 + move] = v;
}

u8 PK3::ev(Stat ev) const
{
    return data[0x38 + u8(ev)];
}
void PK3::ev(Stat ev, u8 v)
{
    data[0x38 + u8(ev)] = v;
}

u8 PK3::contest(u8 contest) const
{
    return data[0x3E + contest];
}
void PK3::contest(u8 contest, u8 v)
{
    data[0x3E + contest] = v;
}

u8 PK3::pkrs(void) const
{
    return data[0x44];
}
void PK3::pkrs(u8 v)
{
    data[0x44] = v;
}

u8 PK3::pkrsDays(void) const
{
    return data[0x44] & 0xF;
}
void PK3::pkrsDays(u8 v)
{
    data[0x44] = (u8)((data[0x44] & ~0xF) | v);
}

u8 PK3::pkrsStrain(void) const
{
    return data[0x44] >> 4;
}
void PK3::pkrsStrain(u8 v)
{
    data[0x44] = (u8)((data[0x44] & 0xF) | v << 4);
}

u16 PK3::metLocation(void) const
{
    return data[0x45];
}
void PK3::metLocation(u16 v)
{
    data[0x45] = (u8)v;
}

u8 PK3::metLevel(void) const
{
    return Endian::convertTo<u16>(data + 0x46) & 0x7F;
}
void PK3::metLevel(u8 v)
{
    Endian::convertFrom<u16>(data + 0x46, (u16)((Endian::convertTo<u16>(data + 0x46) & ~0x7F) | v));
}

u8 PK3::version(void) const
{
    return (Endian::convertTo<u16>(data + 0x46) >> 7) & 0xF;
}
void PK3::version(u8 v)
{
    Endian::convertFrom<u16>(data + 0x46, (u16)((Endian::convertTo<u16>(data + 0x46) & ~0x780) | ((v & 0xF) << 7)));
}

u8 PK3::ball(void) const
{
    return (Endian::convertTo<u16>(data + 0x46) >> 11) & 0xF;
}
void PK3::ball(u8 v)
{
    Endian::convertFrom<u16>(data + 0x46, (u16)((Endian::convertTo<u16>(data + 0x46) & ~0x7800) | ((v & 0xF) << 11)));
}

u8 PK3::otGender(void) const
{
    return (Endian::convertTo<u16>(data + 0x46) >> 15) & 1;
}
void PK3::otGender(u8 v)
{
    Endian::convertFrom<u16>(data + 0x46, (u16)((Endian::convertTo<u16>(data + 0x46) & ~(1 << 15)) | ((v & 1) << 15)));
}

u8 PK3::iv(Stat stat) const
{
    u32 buffer = Endian::convertTo<u32>(data + 0x48);
    return (u8)((buffer >> 5 * u8(stat)) & 0x1F);
}
void PK3::iv(Stat stat, u8 v)
{
    u32 buffer = Endian::convertTo<u32>(data + 0x48);
    buffer &= ~(0x1F << 5 * u8(stat));
    buffer |= v << (5 * u8(stat));
    Endian::convertFrom<u32>(data + 0x38, buffer);
}

bool PK3::egg() const
{
    return ((Endian::convertTo<u32>(data + 0x48) >> 30) & 1) == 1;
}
void PK3::egg(bool v)
{
    Endian::convertFrom<u32>(data + 0x48, (u32)((Endian::convertTo<u32>(data + 0x48) & ~0x40000000) | (u32)(v ? 0x40000000 : 0)));
    flagIsEgg(v);
    if (v)
    {
        nickname("タマゴ");
        language(Language::JP);
    }
}

bool PK3::abilityBit(void) const
{
    return (Endian::convertTo<u32>(data + 0x48) >> 31) == 1;
}
void PK3::abilityBit(bool v)
{
    Endian::convertFrom<u32>(data + 0x48, (Endian::convertTo<u32>(data + 0x48) & 0x7FFFFFFF) | (v ? 1u << 31 : 0u));
}

bool PK3::ribbon(u8 cat, u8 rib) const
{
    return (data[0x4C + cat] & (1 << rib)) != 0;
}
void PK3::ribbon(u8 cat, u8 rib, u8 v)
{
    data[0x4C + cat] = (u8)((data[0x4C + cat] & ~(1 << rib)) | (v ? 1 << rib : 0));
}
u8 PK3::contestRibbonCount(u8 contest) const
{
    u16 rawVal = Endian::convertTo<u16>(data + 0x4C);
    return (rawVal >> (contest * 3)) & 7;
}

bool PK3::fatefulEncounter(void) const
{
    return (Endian::convertTo<u32>(data + 0x4C) >> 31) == 1;
}
void PK3::fatefulEncounter(bool v)
{
    Endian::convertFrom<u32>(data + 0x4C, (Endian::convertTo<u32>(data + 0x4C) & ~(1 << 31)) | (u32)(v ? 1 << 31 : 0));
}

int PK3::partyLevel() const
{
    if (length == 80)
    {
        return -1;
    }
    return data[0x54];
}
void PK3::partyLevel(u8 v)
{
    if (length == 100)
    {
        data[0x54] = v;
    }
}

int PK3::partyCurrHP(void) const
{
    if (length == 80)
    {
        return -1;
    }
    return Endian::convertTo<u16>(data + 0x56);
}
void PK3::partyCurrHP(u16 v)
{
    if (length == 100)
    {
        Endian::convertFrom<u16>(data + 0x56, v);
    }
}

int PK3::partyStat(Stat stat) const
{
    if (length == 80)
    {
        return -1;
    }
    return Endian::convertTo<u16>(data + 0x58 + u8(stat) * 2);
}
void PK3::partyStat(Stat stat, u16 v)
{
    if (length == 100)
    {
        Endian::convertFrom<u16>(data + 0x58 + u8(stat) * 2, v);
    }
}

std::shared_ptr<PKX> PK3::convertToG4(Sav& save) const
{
    static constexpr std::array<std::array<u8, 18>, 7> trashBytes = {{
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x18, 0x20, 0x0D, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0xA1, 0x0C, 0x02, 0xE0, 0xFF},
        {0x74, 0x20, 0x0D, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0xA1, 0x0C, 0x02, 0xE0, 0xFF},
        {0x54, 0x20, 0x0D, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0xA1, 0x0C, 0x02, 0xE0, 0xFF},
        {0x74, 0x20, 0x0D, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0xA1, 0x0C, 0x02, 0xE0, 0xFF},
        {}, // Unused
        {0x74, 0x20, 0x0D, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0xA1, 0x0C, 0x02, 0xE0, 0xFF},
    }};

    time_t t              = time(NULL);
    struct tm* timeStruct = gmtime((const time_t*)&t);

    std::shared_ptr<PKX> pk4 = std::make_shared<PK4>();

    pk4->PID(PID());
    pk4->species(species());
    pk4->TID(TID());
    pk4->SID(SID());
    pk4->experience(egg() ? expTable(5, expType()) : experience());
    pk4->gender(gender());
    pk4->alternativeForm(alternativeForm());
    pk4->egg(false);
    pk4->otFriendship(70);
    pk4->markValue(markValue());
    pk4->language(language());
    pk4->ev(Stat::HP, ev(Stat::HP));
    pk4->ev(Stat::ATK, ev(Stat::ATK));
    pk4->ev(Stat::DEF, ev(Stat::DEF));
    pk4->ev(Stat::SPD, ev(Stat::SPD));
    pk4->ev(Stat::SPATK, ev(Stat::SPATK));
    pk4->ev(Stat::SPDEF, ev(Stat::SPDEF));
    pk4->contest(0, contest(0));
    pk4->contest(1, contest(1));
    pk4->contest(2, contest(2));
    pk4->contest(3, contest(3));
    pk4->contest(4, contest(4));
    pk4->contest(5, contest(5));
    pk4->move(0, move(0));
    pk4->move(1, move(1));
    pk4->move(2, move(2));
    pk4->move(3, move(3));
    pk4->PPUp(0, PPUp(0));
    pk4->PPUp(1, PPUp(1));
    pk4->PPUp(2, PPUp(2));
    pk4->PPUp(3, PPUp(3));
    pk4->iv(Stat::HP, iv(Stat::HP));
    pk4->iv(Stat::ATK, iv(Stat::ATK));
    pk4->iv(Stat::DEF, iv(Stat::DEF));
    pk4->iv(Stat::SPD, iv(Stat::SPD));
    pk4->iv(Stat::SPATK, iv(Stat::SPATK));
    pk4->iv(Stat::SPDEF, iv(Stat::SPDEF));
    pk4->ability(ability());
    pk4->version(version());
    pk4->ball(ball());
    pk4->pkrsStrain(pkrsStrain());
    pk4->pkrsDays(pkrsDays());
    pk4->otGender(otGender());
    pk4->metYear(timeStruct->tm_year - 100);
    pk4->metMonth(timeStruct->tm_mon + 1);
    pk4->metDay(timeStruct->tm_mday);
    pk4->metLevel(level());
    pk4->metLocation(0x37); // Pal Park
    pk4->fatefulEncounter(fatefulEncounter());

    for (u8 rib = 0; rib < 12; rib++)
    {
        pk4->ribbon(6 + (rib + 4) / 8, rib + 4, ribbon(1 + (rib + 7) / 8, rib + 7) ? 1 : 0);
    }

    // Contest ribbons
    for (u8 contest = 0; contest < 5; contest++)
    {
        u8 contestCount = contestRibbonCount(contest);
        if (contestCount > 0)
        {
            pk4->ribbon(4 + contest / 2, (contest % 2) * 4, 1);
        }
        if (contestCount > 1)
        {
            pk4->ribbon(4 + contest / 2, (contest % 2) * 4 + 1, 1);
        }
        if (contestCount > 2)
        {
            pk4->ribbon(4 + contest / 2, (contest % 2) * 4 + 2, 1);
        }
        if (contestCount > 3)
        {
            pk4->ribbon(4 + contest / 2, (contest % 2) * 4 + 3, 1);
        }
    }

    // Yay trash bytes
    if (u8(language()) - 1 < trashBytes.size())
    {
        auto& trash = trashBytes[u8(language()) - 1];
        std::copy(trash.begin(), trash.end(), pk4->rawData() + 0x48 + 4);
    }

    std::string name = i18n::species(language(), species());
    pk4->nickname(egg() ? StringUtils::toUpper(name) : nickname());
    pk4->nicknamed(!egg() && nicknamed());

    // Copy nickname trash into OT name
    std::copy(pk4->rawData() + 0x48, pk4->rawData() + 0x48 + 0x10, pk4->rawData() + 0x68);
    pk4->otName(otName());

    // I use 0 for invalid items
    pk4->heldItem(heldItem() == ItemConverter::G3_NOT_CONVERTIBLE ? 0 : heldItem());

    // Remove HM moves
    for (int i = 0; i < 4; i++)
    {
        static constexpr std::array<u16, 8> hms = {15, 19, 57, 70, 148, 249, 127, 291};
        if (std::find(hms.begin(), hms.end(), pk4->move(i)) != hms.end())
        {
            pk4->move(i, 0);
        }
        else
        {
            pk4->move(i, move(i));
        }
    }
    pk4->fixMoves();

    pk4->refreshChecksum();
    return pk4;
}

std::shared_ptr<PKX> PK3::convertToG5(Sav& save) const
{
    return convertToG4(save)->convertToG5(save);
}
std::shared_ptr<PKX> PK3::convertToG6(Sav& save) const
{
    return convertToG4(save)->convertToG5(save)->convertToG6(save);
}
std::shared_ptr<PKX> PK3::convertToG7(Sav& save) const
{
    return convertToG4(save)->convertToG5(save)->convertToG6(save)->convertToG7(save);
}

u8 PK3::level() const
{
    u8 i      = 1;
    u8 xpType = expType();
    while (experience() >= expTable(i, xpType) && ++i < 100)
        ;
    return i;
}
void PK3::level(u8 v)
{
    experience(expTable(v - 1, expType()));
}

u16 PK3::ability() const
{
    return abilityBit() ? abilities(1) : abilities(0);
}
void PK3::ability(u16 v)
{
    if (v == abilities(1))
    {
        abilityBit(true);
    }
    else if (v == abilities(0))
    {
        abilityBit(false);
    }
}
void PK3::setAbility(u8 num)
{
    if (num < 2)
    {
        ability(abilities(num));
    }
}

u8 PK3::nature() const
{
    return PID() % 25;
}
void PK3::nature(u8 v)
{
    PID(PKX::getRandomPID(species(), gender(), version(), nature(), alternativeForm(), abilityBit() ? 2 : 1, PID(), Generation::THREE));
}

u8 PK3::gender() const
{
    switch (genderType())
    {
        case 0:
            return 0;
        case 254:
            return 1;
        case 255:
            return 2;
        default:
            return (PID() & 0xFF) < genderType() ? 1 : 0;
    }
}
void PK3::gender(u8 v)
{
    PID(PKX::getRandomPID(species(), gender(), version(), nature(), alternativeForm(), abilityBit() ? 2 : 1, PID(), Generation::THREE));
}

u16 PK3::alternativeForm() const
{
    if (species() == 201)
    {
        return getUnownForm(PID());
    }
    return 0;
}
void PK3::alternativeForm(u16 v)
{
    if (species() == 201)
    {
        PID(PKX::getRandomPID(species(), gender(), version(), nature(), v, abilityBit() ? 2 : 1, PID(), Generation::THREE));
    }
}

u16 PK3::relearnMove(u8 move) const
{
    return 0;
}
void PK3::relearnMove(u8 move, u16 v) {}

bool PK3::nicknamed() const
{
    std::string target = i18n::species(language(), species());
    return nickname() != StringUtils::toUpper(target);
}
void PK3::nicknamed(bool v) {}

u8 PK3::metYear(void) const
{
    return 0;
}
void PK3::metYear(u8 v) {}

u8 PK3::metMonth(void) const
{
    return 0;
}
void PK3::metMonth(u8 v) {}

u8 PK3::metDay(void) const
{
    return 0;
}
void PK3::metDay(u8 v) {}

u8 PK3::eggYear(void) const
{
    return 0;
}
void PK3::eggYear(u8 v) {}

u8 PK3::eggMonth(void) const
{
    return 0;
}
void PK3::eggMonth(u8 v) {}

u8 PK3::eggDay(void) const
{
    return 0;
}
void PK3::eggDay(u8 v) {}

u16 PK3::eggLocation() const
{
    return 0;
}
void PK3::eggLocation(u16 v) {}

u8 PK3::hpType(void) const
{
    return 15 *
           ((iv(Stat::HP) & 1) + 2 * (iv(Stat::ATK) & 1) + 4 * (iv(Stat::DEF) & 1) + 8 * (iv(Stat::SPD) & 1) + 16 * (iv(Stat::SPATK) & 1) +
               32 * (iv(Stat::SPDEF) & 1)) /
           63;
}
void PK3::hpType(u8 v)
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

u16 PK3::TSV() const
{
    return (TID() ^ SID()) >> 3;
}

u16 PK3::PSV() const
{
    return (PID() >> 16 ^ (PID() & 0xFFFF)) >> 3;
}

bool PK3::shiny(void) const
{
    return TSV() == PSV();
}
void PK3::shiny(bool v)
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

u16 PK3::formSpecies() const
{
    return species();
}

u16 PK3::stat(Stat stat) const
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
