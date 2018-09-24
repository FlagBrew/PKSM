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

#include "SavXY.hpp"

SavXY::SavXY(u8* dt)
{
    length = 0x65600;
    boxes = 31;

    data = new u8[length];
    std::copy(dt, dt + length, data);
}

void SavXY::resign(void)
{
    const u8 blockCount = 55;
    u8* tmp = new u8[*std::max_element(chklen, chklen + blockCount)];
    const u32 csoff = 0x6541A;

    for (u8 i = 0; i < blockCount; i++)
    {
        std::copy(data + chkofs[i], data + chkofs[i] + chklen[i], tmp);
        *(u16*)(data + csoff + i*8) = ccitt16(tmp, chklen[i]);
    }
}

u16 SavXY::TID(void) const { return *(u16*)(data + 0x14000); }
void SavXY::TID(u16 v) { *(u16*)(data + 0x14000) = v; }

u16 SavXY::SID(void) const { return *(u16*)(data + 0x14002); }
void SavXY::SID(u16 v) { *(u16*)(data + 0x14002) = v; }

u8 SavXY::version(void) const { return data[0x14004]; }
void SavXY::version(u8 v) { data[0x14004] = v; }

u8 SavXY::gender(void) const { return data[0x14005]; }
void SavXY::gender(u8 v) { data[0x14005] = v; }

u8 SavXY::subRegion(void) const { return data[0x14026]; }
void SavXY::subRegion(u8 v) { data[0x14026] = v; }

u8 SavXY::country(void) const { return data[0x14027]; }
void SavXY::country(u8 v) { data[0x14027] = v; }

u8 SavXY::consoleRegion(void) const { return data[0x1402C]; }
void SavXY::consoleRegion(u8 v) { data[0x1402C] = v; }

u8 SavXY::language(void) const { return data[0x1402D]; }
void SavXY::language(u8 v) { data[0x1402D] = v; }

std::string SavXY::otName(void) const { return StringUtils::getString(data, 0x14048, 13); }
void SavXY::otName(const char* v) { StringUtils::setString(data, v, 0x14048, 13); }

u32 SavXY::money(void) const { return *(u32*)(data + 0x4208); }
void SavXY::money(u32 v) { *(u32*)(data + 0x4208) = v; }

u32 SavXY::BP(void) const { return *(u32*)(data + 0x4230); }
void SavXY::BP(u32 v) { *(u32*)(data + 0x4230) = v; }

u16 SavXY::playedHours(void) const { return *(u16*)(data + 0x1800); }
void SavXY::playedHours(u16 v) { *(u16*)(data + 0x1800) = v; }

u8 SavXY::playedMinutes(void) const { return *(u8*)(data + 0x1802); }
void SavXY::playedMinutes(u8 v) { *(u8*)(data + 0x1802) = v; }

u8 SavXY::playedSeconds(void) const { return *(u8*)(data + 0x1803); }
void SavXY::playedSeconds(u8 v) { *(u8*)(data + 0x1803) = v; }

u8 SavXY::currentBox(void) const { return data[0x483F]; }
void SavXY::currentBox(u8 v) { data[0x483F] = v; }

u32 SavXY::boxOffset(u8 box, u8 slot) const { return 0x22600 + 232*30*box + 232*slot; }

u32 SavXY::partyOffset(u8 slot) const { return 0x14200 + 260*slot; }

std::unique_ptr<PKX> SavXY::pkm(u8 slot) const
{
    u8 tmp[232];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 232, tmp);
    return std::unique_ptr<PKX>(new PK6(tmp, true));
}
std::unique_ptr<PKX> SavXY::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 tmp[232];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 232, tmp);
    return std::unique_ptr<PKX>(new PK6(tmp, ekx));
}

void SavXY::pkm(PKX& pk, u8 box, u8 slot)
{
    PK6* pk6 = (PK6*)&pk;
    std::copy(pk6->data, pk6->data + 232, data + boxOffset(box, slot));
}

void SavXY::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < boxes; box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::unique_ptr<PKX> pk6 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pk6->encrypt();
            }
            pkm(*pk6, box, slot);
        }
    }
}

int SavXY::dexFormIndex(int species, int formct) const
{
    if (formct < 1 || species < 0)
        return -1; // invalid
    switch (species)
    {
        case 25: return 189; // 7 Pikachu
        case 720: return 196; // 2 Hoopa
        case 15: return 198; // 2 Beedrill
        case 18: return 200; // 2 Pidgeot
        case 80: return 202; // 2 Slowbro
        case 208: return 204; // 2 Steelix
        case 254: return 206; // 2 Sceptile
        case 360: return 208; // 2 Swampert
        case 302: return 210; // 2 Sableye
        case 319: return 212; // 2 Sharpedo
        case 323: return 214; // 2 Camerupt
        case 334: return 216; // 2 Altaria
        case 362: return 218; // 2 Glalie
        case 373: return 220; // 2 Salamence
        case 376: return 222; // 2 Metagross
        case 384: return 224; // 2 Rayquaza
        case 428: return 226; // 2 Lopunny
        case 475: return 228; // 2 Gallade
        case 531: return 230; // 2 Audino
        case 719: return 232; // 2 Diancie
        case 382: return 234; // 2 Kyogre
        case 383: return 236; // 2 Groudon
        case 493: return 238; // 18 Arceus
        case 649: return 256; // 5 Genesect
        case 676: return 261; // 10 Furfrou
        case 666: return 83; // 20 Vivillion
        case 669: return 103; // 5 Flabébé
        case 670: return 108; // 6 Floette
        case 671: return 114; // 5 Florges
        case 710: return 119; // 4 Pumpkaboo
        case 711: return 123; // 4 Gourgeist
        case 681: return 127; // 2 Aegislash
        case 716: return 129; // 2 Xerneas
        case 3: return 131; // 2 Venusaur
        case 6: return 133; // 3 Charizard
        case 9: return 136; // 2 Blastoise
        case 65: return 138; // 2 Alakazam
        case 94: return 140; // 2 Gengar
        case 115: return 142; // 2 Kangaskhan
        case 127: return 144; // 2 Pinsir
        case 130: return 146; // 2 Gyarados
        case 142: return 148; // 2 Aerodactyl
        case 150: return 150; // 3 Mewtwo
        case 181: return 153; // 2 Ampharos
        case 212: return 155; // 2 Scizor
        case 214: return 157; // 2 Heracros
        case 229: return 159; // 2 Houndoom
        case 248: return 161; // 2 Tyranitar
        case 257: return 163; // 2 Blaziken
        case 282: return 165; // 2 Gardevoir
        case 303: return 167; // 2 Mawile
        case 306: return 169; // 2 Aggron
        case 308: return 171; // 2 Medicham
        case 310: return 173; // 2 Manetric
        case 354: return 175; // 2 Banette
        case 359: return 177; // 2 Absol
        case 380: return 179; // 2 Latias
        case 381: return 181; // 2 Latios
        case 445: return 183; // 2 Garchomp
        case 448: return 185; // 2 Lucario
        case 460: return 187; // 2 Abomasnow
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

void SavXY::dex(PKX& pk)
{
    if (pk.species() == 0)
        return;
    if (pk.species() > 721)
        return;

    const int brSize = 0x60;
    int bit = pk.species() - 1;
    int lang = pk.language() - 1; if (lang > 5) lang--; // 0-6 language vals
    int gender = pk.gender() % 2; // genderless -> male
    int shiny = pk.shiny() ? 1 : 0;
    int shiftoff = brSize*(1 + gender + 2*shiny); // after the Owned region
    int bd = bit >> 3; // div8
    int bm = bit & 7; // mod8
    u8 mask = (u8)(1 << bm);
    int ofs = 0x15000 + 0x8 + bd;

    data[ofs + brSize * 0] |= mask;

    // Set the [Species/Gender/Shiny] Seen Flag
    data[ofs + shiftoff] |= mask;

    // Set the Display flag if none are set
    bool displayed = false;
    displayed |= (data[ofs + brSize * 5] & mask) != 0;
    displayed |= (data[ofs + brSize * 6] & mask) != 0;
    displayed |= (data[ofs + brSize * 7] & mask) != 0;
    displayed |= (data[ofs + brSize * 8] & mask) != 0;
    if (!displayed) // offset is already biased by brSize, reuse shiftoff but for the display flags.
        data[ofs + brSize * 4 + shiftoff] |= mask;

    // Set the Language
    if (lang < 0) lang = 1;
    data[0x153C8 + (bit * 7 + lang) / 8] |= (u8)(1 << ((bit * 7 + lang) % 8));

    // Set Form flags
    int fc = PersonalXYORAS::formCount(pk.species());
    int f = dexFormIndex(pk.species(), fc);
    if (f < 0) return;

    int formLen = 0x26;
    int formDex = 0x15000 + 0x8 + brSize*9;
    bit = f + pk.alternativeForm();

    // Set Form Seen Flag
    data[formDex + formLen*shiny + bit/8] |= (u8)(1 << (bit%8));

    // Set Displayed Flag if necessary, check all flags
    for (int i = 0; i < fc; i++)
    {
        bit = f + i;
        if ((data[formDex + formLen*2 + bit/8] & (u8) (1 << (bit%8))) != 0) // Nonshiny
            return; // already set
        if ((data[formDex + formLen*3 + bit/8] & (u8) (1 << (bit%8))) != 0) // Shiny
            return; // already set
    }
    bit = f + pk.alternativeForm();
    data[formDex + formLen * (2 + shiny) + bit / 8] |= (u8)(1 << (bit % 8));
}

void SavXY::mysteryGift(WCX& wc, int& pos)
{
    WC6* wc6 = (WC6*)&wc;
    *(u8*)(data + 0x1BC00 + wc6->ID()/8) |= 0x1 << (wc6->ID() % 8);
    std::copy(wc6->data, wc6->data + 264, data + 0x1BD00 + 264*pos);
    pos = (pos + 1) % 24;
}

std::string SavXY::boxName(u8 box) const { return StringUtils::getString(data, 0x4400 + 0x22*box, 17); }

void SavXY::boxName(u8 box, std::string name)
{
    StringUtils::setString(data, name.c_str(), 0x4400 + 0x22*box, 17);
}

u8 SavXY::partyCount(void) const { return data[partyOffset(0) + 6*260]; }

std::shared_ptr<PKX> SavXY::emptyPkm() const
{
    static std::shared_ptr<PKX> empty = std::shared_ptr<PKX>(new PK6);
    return empty;
}

int SavXY::emptyGiftLocation(void) const
{
    u8 t;
    bool empty;
    // 24 max wonder cards
    for (t = 0; t < 24; t++)
    {
        empty = true;
        for (u32 j = 0; j < WC6::length; j++)
        {
            if (*(data + 0x1BD00 + t * WC6::length + j) != 0)
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

    return !empty ? 23 : t;
}

std::vector<MysteryGift::giftData> SavXY::currentGifts(void) const
{
    std::vector<MysteryGift::giftData> ret;
    u8* wonderCards = data + 0x1BD00;
    for (int i = 0; i < emptyGiftLocation() + 1; i++)
    {
        if (*(wonderCards + i * WC6::length + 0x51) == 0)
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC6::length, 0x2, 36), *(u16*)(wonderCards + i * WC6::length + 0x82), *(wonderCards + i * WC6::length + 0x84)});
        }
        else
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC6::length, 0x2, 36), -1, -1 });
        }
    }
    return ret;
}