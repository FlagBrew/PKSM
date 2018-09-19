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

#include "SavB2W2.hpp"

SavB2W2::SavB2W2(u8* dt)
{
    length = 0x26000;
    boxes = 24;
    
    data = new u8[length];
    std::copy(dt, dt + length, data);
}

void SavB2W2::resign(void)
{
    const u8 blockCount = 74;
    u8* tmp = new u8[*std::max_element(lengths, lengths + blockCount)];
    u16 cs;

    for (u8 i = 0; i < blockCount; i++)
    {
        std::copy(data + blockOfs[i], data + blockOfs[i] + lengths[i], tmp);
        cs = ccitt16(tmp, lengths[i]);
        *(u16*)(data + chkMirror[i]) = cs;
        *(u16*)(data + chkofs[i]) = cs;
    }
}

u16 SavB2W2::TID(void) const { return *(u16*)(data + 0x19414); }
void SavB2W2::TID(u16 v) { *(u16*)(data + 0x19414) = v; }

u16 SavB2W2::SID(void) const { return *(u16*)(data + 0x19416); }
void SavB2W2::SID(u16 v) { *(u16*)(data + 0x19416) = v; }

u8 SavB2W2::version(void) const { return data[0x1941F]; }
void SavB2W2::version(u8 v) { data[0x1941F] = v; }

u8 SavB2W2::gender(void) const { return data[0x19421]; }
void SavB2W2::gender(u8 v) { data[0x19421] = v; }

u8 SavB2W2::subRegion(void) const { return 0; } // Todo?
void SavB2W2::subRegion(u8 v) { (void)v; }

u8 SavB2W2::country(void) const { return 0; } // Todo?
void SavB2W2::country(u8 v) { (void)v; }

u8 SavB2W2::consoleRegion(void) const { return 0; } // return country?
void SavB2W2::consoleRegion(u8 v) { (void)v; }

u8 SavB2W2::language(void) const { return data[0x1941E]; }
void SavB2W2::language(u8 v) { data[0x1941E] = v; }

std::string SavB2W2::otName(void) const { return StringUtils::getTrimmedString(data, 0x19404, 8, (char*)"\uFFFF"); }
void SavB2W2::otName(const char* v) { StringUtils::setStringWithBytes(data, v, 0x19404, 8, (char*)"\uFFFF"); }

u32 SavB2W2::money(void) const { return *(u32*)(data + 0x21100); }
void SavB2W2::money(u32 v) { *(u32*)(data + 0x21100) = v; }

u32 SavB2W2::BP(void) const { return *(u32*)(data + 0x21B00); }
void SavB2W2::BP(u32 v) { *(u32*)(data + 0x21B00) = v; }

u16 SavB2W2::playedHours(void) const { return *(u16*)(data + 0x19424); }
void SavB2W2::playedHours(u16 v) { *(u16*)(data + 0x19424) = v; }

u8 SavB2W2::playedMinutes(void) const { return data[0x19426]; }
void SavB2W2::playedMinutes(u8 v) { data[0x19426] = v; }

u8 SavB2W2::playedSeconds(void) const { return data[0x19427]; }
void SavB2W2::playedSeconds(u8 v) { data[0x19427] = v; }

u8 SavB2W2::currentBox(void) const { return data[0]; }
void SavB2W2::currentBox(u8 v) { data[0] = v; }

u32 SavB2W2::boxOffset(u8 box, u8 slot) const { return 0x400 + 136*box*30 + 0x10*box + 136*slot ; }
u32 SavB2W2::partyOffset(u8 slot) const { return 0x18E08 + 220*slot; }

std::unique_ptr<PKX> SavB2W2::pkm(u8 slot) const
{
    u8 buf[136];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 136, buf);
    return std::unique_ptr<PKX>(new PK5(buf, true));
}

std::unique_ptr<PKX> SavB2W2::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 buf[136];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 136, buf);
    return std::unique_ptr<PKX>(new PK5(buf, ekx));
}

void SavB2W2::pkm(PKX& pk, u8 box, u8 slot)
{
    PK5* pk5 = (PK5*)&pk;
    std::copy(pk5->data, pk5->data + 136, data + boxOffset(box, slot));
}

void SavB2W2::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < boxes; box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::unique_ptr<PKX> pk5 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pk5->encrypt();
            }
            pkm(*pk5, box, slot);
        }
    }
}

int SavB2W2::dexFormIndex(int species, int formct) const
{
    if (formct < 1 || species < 0)
        return -1; // invalid
    switch (species)
    {
        case 646: return 72; // 3 Kyurem
        case 647: return 75; // 2 Keldeo
        case 642: return 77; // 2 Thundurus
        case 641: return 79; // 2 Tornadus
        case 645: return 81; // 2 Landorus
        case 201: return 0; // 28 Unown
        case 386: return 28; // 4 Deoxys
        case 492: return 32; // 2 Shaymin
        case 487: return 34; // 2 Giratina
        case 479: return 36; // 6 Rotom
        case 422: return 42; // 2 Shellos
        case 423: return 44; // 2 Gastrodon
        case 412: return 46; // 3 Burmy
        case 413: return 49; // 3 Wormadam
        case 351: return 52; // 4 Castform
        case 421: return 56; // 2 Cherrim
        case 585: return 58; // 4 Deerling
        case 586: return 62; // 4 Sawsbuck
        case 648: return 66; // 2 Meloetta
        case 555: return 68; // 2 Darmanitan
        case 550: return 70; // 2 Basculin
        default: return -1;
    }
}

void SavB2W2::dex(PKX& pk)
{
    if (pk.species() == 0 )
        return;
    if (pk.species() > 649)
        return;

    const int brSize = 0x54;
    int bit = pk.species() - 1;
    int gender = pk.gender() % 2; // genderless -> male
    int shiny = pk.shiny() ? 1 : 0;
    int shift = shiny*2 + gender + 1;
    int shiftoff = shiny * brSize * 2 + gender * brSize + brSize;
    int ofs = 0x21400 + 0x8 + (bit >> 3);

    // Set the Species Owned Flag
    data[ofs + brSize*0] |= (u8)(1 << (bit % 8));

    // Set the [Species/Gender/Shiny] Seen Flag
    data[0x21400 + 0x8 + shiftoff + bit / 8] |= (u8)(1 << (bit&7));

    // Set the Display flag if none are set
    bool displayed = false;
    displayed |= (data[ofs + brSize*5] & (u8)(1 << (bit&7))) != 0;
    displayed |= (data[ofs + brSize*6] & (u8)(1 << (bit&7))) != 0;
    displayed |= (data[ofs + brSize*7] & (u8)(1 << (bit&7))) != 0;
    displayed |= (data[ofs + brSize*8] & (u8)(1 << (bit&7))) != 0;
    if (!displayed) // offset is already biased by brSize, reuse shiftoff but for the display flags.
        data[ofs + brSize*(shift + 4)] |= (u8)(1 << (bit&7));

    // Set the Language
    if (bit < 493) // shifted by 1, Gen5 species do not have international language bits
    {
        int lang = pk.language() - 1; if (lang > 5) lang--; // 0-6 language vals
        if (lang < 0) lang = 1;
        data[0x21728 + ((bit*7 + lang)>>3)] |= (u8)(1 << ((bit*7 + lang) & 7));
    }

    // Formes
    int fc = PersonalBWB2W2::formCount(pk.species());
    int f = dexFormIndex(pk.species(), fc);
    if (f < 0) return;

    int formLen = 0xB;
    int formDex = 0x21400 + 0x8 + brSize*9;
    bit = f + pk.alternativeForm();

    // Set Form Seen Flag
    data[formDex + formLen*shiny + (bit>>3)] |= (u8)(1 << (bit&7));

    // Set displayed Flag if necessary, check all flags
    for (int i = 0; i < fc; i++)
    {
        bit = f + i;
        if ((data[formDex + formLen*2 + (bit>>3)] & (u8)(1 << (bit&7))) != 0) // Nonshiny
            return; // already set
        if ((data[formDex + formLen*3 + (bit>>3)] & (u8)(1 << (bit&7))) != 0) // Shiny
            return; // already set
    }
    bit = f + pk.alternativeForm();
    data[formDex + formLen * (2 + shiny) + (bit>>3)] |= (u8)(1 << (bit&7));
}

void SavB2W2::mysteryGift(WCX& wc, int& pos)
{
    PGF* pgf = (PGF*)&wc;
    u32 seed = *(u32*)(data + 0x1D290);

    //decrypt
    for (int i = 0; i < 0xA90; i += 2)
    {
        seed = seed * 0x41C64E6D + 0x6073; // Replace with seedStep?
        *(u16*)(data + 0x1C800 + i) ^= (seed >> 16);
    }

    *(data + 0x1C800 - 0x100 + pgf->ID()) |= 0x1 << (pgf->ID() & 7);
    std::copy(pgf->data, pgf->data + 204, data + 0x1C800 + pos * 204);
    pos = (pos + 1) % 12;

    //encrypt
    seed = *(u32*)(data + 0x1D290);
    for (int i = 0; i < 0xA90; i += 2)
    {
        seed = seed * 0x41C64E6D + 0x6073; // Replace with seedStep?
        *(u16*)(data + 0x1C800 + i) ^= (seed >> 16);
    }
}

std::string SavB2W2::boxName(u8 box) const { return StringUtils::getTrimmedString(data, 0x28 * box + 4, 0x14, (char*)"\uFFFF"); }

u8 SavB2W2::partyCount(void) const { return data[0x18E04]; }

std::shared_ptr<PKX> SavB2W2::emptyPkm() const
{
    static std::shared_ptr<PKX> empty = std::shared_ptr<PKX>(new PK5);
    return empty;
}

int SavB2W2::emptyGiftLocation(void) const
{
    u8 t;
    bool empty;
    // 12 max wonder cards
    for (t = 0; t < 12; t++)
    {
        empty = true;
        for (u32 j = 0; j < PGF::length; j++)
        {
            if (*(data + 0x1C900 + t * PGF::length + j) != 0)
            {
                empty = false;
                break;
            }
        }

        if (empty)
        {
            break;
        }
    }

    return !empty ? 11 : t;
}

std::vector<MysteryGift::giftData> SavB2W2::currentGifts(void) const
{
    std::vector<MysteryGift::giftData> ret;
    u8* wonderCards = data + 0x1C900;
    for (int i = 0; i < emptyGiftLocation() + 1; i++)
    {
        if (*(wonderCards + i * PGF::length + 0xB3) == 1)
        {
            ret.push_back({ StringUtils::getTrimmedString(wonderCards + i * PGF::length, 0x60, 37, (char*)"\uFFFF"), *(u16*)(wonderCards + i * PGF::length + 0x1A), *(wonderCards + i * PGF::length + 0x1C)});
        }
        else
        {
            ret.push_back({ StringUtils::getTrimmedString(wonderCards + i * PGF::length, 0x60, 37, (char*)"\uFFFF"), -1, -1 });
        }
    }
    return ret;
}