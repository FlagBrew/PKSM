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

#include "SavHGSS.hpp"
#include "PGT.hpp"

SavHGSS::SavHGSS(u8* dt)
{
    length = 0x80000;
    boxes = 18;

    data = new u8[length];
    std::copy(dt, dt + length, data);
    GBO();
    SBO();
}

void SavHGSS::GBO(void)
{
    int ofs = 0xF618;
    u8 temp[10], dummy[10];
    std::fill_n(dummy, 10, 0xFF);
    
    std::copy(data, data + 10, temp);
    if (!memcmp(temp, dummy, 10)) {
        gbo = 0x40000;
        return;
    }
    
    std::copy(data + 0x40000, data + 0x4000A, temp);
    if (!memcmp(temp, dummy, 10)) {
        gbo = 0;
        return;
    }

    u16 c1 = *(u16*)(data + ofs), c2 = *(u16*)(data + ofs + 0x40000);
    
    gbo = (c1 >= c2) ? 0 : 0x40000;
}

void SavHGSS::SBO(void)
{
    int ofs = 0x21A00;
    u8 temp[10], dummy[10];
    std::fill_n(dummy, 10, 0xFF);
    
    std::copy(data, data + 10, temp);
    if (!memcmp(temp, dummy, 10)) {
        sbo = 0x40000;
        return;
    }
    
    std::copy(data + 0x40000, data + 0x4000A, temp);
    if (!memcmp(temp, dummy, 10)) {
        sbo = 0;
        return;
    }

    u16 c1 = *(u16*)(data + ofs), c2 = *(u16*)(data + ofs + 0x40000);
    
    sbo = (c1 >= c2) ? 0 : 0x40000;
}

void SavHGSS::resign(void)
{
    u8* tmp = new u8[0x12300];
    u16 cs;
    // start, end, chkoffset
    int general[3] = {0x0, 0xF618, 0xF626};
    int storage[3] = {0xF700, 0x21A00, 0x21A0E};
    
    std::copy(data + gbo + general[0], data + gbo + general[1], tmp);
    cs = ccitt16(tmp, general[1] - general[0]);
    *(u16*)(data + gbo + general[2]) = cs;

    std::copy(data + sbo + storage[0], data + sbo + storage[1], tmp);
    cs = ccitt16(tmp, storage[1] - storage[0]);
    *(u16*)(data + sbo + storage[2]) = cs;

    delete[] tmp;
}

u16 SavHGSS::TID(void) const { return *(u16*)(data + gbo + 0x74); }
void SavHGSS::TID(u16 v) { *(u16*)(data + gbo + 0x74) = v; }

u16 SavHGSS::SID(void) const { return *(u16*)(data + gbo + 0x76); }
void SavHGSS::SID(u16 v) { *(u16*)(data + gbo + 0x76) = v; }

u8 SavHGSS::version(void) const { return 7; } // Always return HG's value: no way to check it, but needed for identification within Gen 4
void SavHGSS::version(u8 v) { (void)v; }

u8 SavHGSS::gender(void) const { return data[0x7C + gbo]; }
void SavHGSS::gender(u8 v) { data[0x7C + gbo] = v; }

u8 SavHGSS::subRegion(void) const { return 0; } // Todo?
void SavHGSS::subRegion(u8 v) { (void)v; }

u8 SavHGSS::country(void) const { return 0; } // Todo?
void SavHGSS::country(u8 v) { (void)v; }

u8 SavHGSS::consoleRegion(void) const { return 0; } // Return country?
void SavHGSS::consoleRegion(u8 v) { (void)v; }

u8 SavHGSS::language(void) const { return data[0x7D + gbo]; }
void SavHGSS::language(u8 v) { data[0x7D + gbo] = v; }

std::string SavHGSS::otName(void) const { return StringUtils::getString4(data, 0x64 + gbo, 8); }
void SavHGSS::otName(const char* v) { StringUtils::setString4(data, v, 0x64 + gbo, 8); }

u32 SavHGSS::money(void) const { return *(u32*)(data + gbo + 0x78); }
void SavHGSS::money(u32 v) { *(u32*)(data + gbo + 0x78) = v; }

u32 SavHGSS::BP(void) const { return *(u16*)(data + gbo + 0x84); } // Returns Coins @ Game Corner
void SavHGSS::BP(u32 v) { *(u16*)(data + gbo + 0x84) = v; }

u16 SavHGSS::playedHours(void) const { return *(u16*)(data + gbo + 0x86); }
void SavHGSS::playedHours(u16 v) { *(u16*)(data + gbo + 0x86) = v; }

u8 SavHGSS::playedMinutes(void) const { return data[gbo + 0x88]; }
void SavHGSS::playedMinutes(u8 v) { data[gbo + 0x88] = v; }

u8 SavHGSS::playedSeconds(void) const { return data[gbo + 0x89]; }
void SavHGSS::playedSeconds(u8 v) { data[gbo + 0x89] = v; }

u8 SavHGSS::currentBox(void) const { return data[boxOffset(18, 0)]; }
void SavHGSS::currentBox(u8 v) { data[boxOffset(18, 0)] = v; }

u32 SavHGSS::boxOffset(u8 box, u8 slot) const { return sbo + 0xF700 + 136*box*30 + box*0x10 + slot*136; }
u32 SavHGSS::partyOffset(u8 slot) const { return gbo + 0x98 + slot*236; }

std::unique_ptr<PKX> SavHGSS::pkm(u8 slot) const
{
    u8 buf[236];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 236, buf);
    return std::make_unique<PK4>(buf, true, true);
}

void SavHGSS::pkm(PKX& pk, u8 slot)
{
    pk.encrypt();
    PK4* pk4 = (PK4*)&pk;
    std::copy(pk4->data, pk4->data + pk4->length, data + partyOffset(slot));
    pk.decrypt();
}

std::unique_ptr<PKX> SavHGSS::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 buf[136];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 136, buf);
    return std::make_unique<PK4>(buf, ekx);
}

void SavHGSS::pkm(PKX& pk, u8 box, u8 slot)
{
    PK4* pk4 = (PK4*)&pk;
    std::copy(pk4->data, pk4->data + 136, data + boxOffset(box, slot));
}

void SavHGSS::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < boxes; box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::unique_ptr<PKX> pk4 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pk4->encrypt();
            }
            pkm(*pk4, box, slot);
        }
    }
}

void SavHGSS::mysteryGift(WCX& wc, int& pos)
{
    PGT* pgt = (PGT*)&wc;
    *(data + 0x9D3C + gbo + (2047 >> 3)) = 0x80;
    std::copy(pgt->data, pgt->data + PGT::length, data + 0x9E3C + gbo + pos * PGT::length);
    pos++;
}

std::string SavHGSS::boxName(u8 box) const { return StringUtils::getString4(data, boxOffset(18, 0) + 0x8 + box*0x28, 9); }

void SavHGSS::boxName(u8 box, std::string name)
{
    StringUtils::setString4(data, name, boxOffset(18, 0) + 0x8 + box * 0x28, 9);
}

u8 SavHGSS::partyCount(void) const { return data[gbo + 0x94 - 4]; }

void SavHGSS::dex(PKX& pk)
{
    if (pk.species() == 0 || pk.species() > 493)
    {
        return;
    }

    static const int brSize = 0x40;
    int bit = pk.species() - 1;
    u8 mask = (u8)(1 << (bit & 7));
    int ofs = gbo + 0x12B8 + (bit >> 3) + 0x4;

    /* 4 BitRegions with 0x40*8 bits
    * Region 0: Caught (Captured/Owned) flags
    * Region 1: Seen flags
    * Region 2/3: Toggle for gender display
    * 4 possible states: 00, 01, 10, 11
    * 00 - 1Seen: Male Only
    * 01 - 2Seen: Male First, Female Second
    * 10 - 2Seen: Female First, Male Second
    * 11 - 1Seen: Female Only
    * (bit1 ^ bit2) + 1 = forms in dex
    * bit2 = male/female shown first toggle */

    // Set the species() Owned Flag
    data[ofs + brSize * 0] |= mask;

    // Check if already Seen
    if ((data[ofs + brSize * 1] & mask) == 0) // Not seen
    {
        data[ofs + brSize * 1] |= mask; // Set seen
        u8 gr = PersonalDPPtHGSS::gender(pk.species());
        switch (gr)
        {
            case 255: // Genderless
            case 0: // Male Only
                data[ofs + brSize * 2] &= mask;
                data[ofs + brSize * 3] &= mask;
                break;
            case 254: // Female Only
                data[ofs + brSize * 2] |= mask;
                data[ofs + brSize * 3] |= mask;
                break;
            default: // Male or Female
                bool m = (data[ofs + brSize * 2] & mask) != 0;
                bool f = (data[ofs + brSize * 3] & mask) != 0;
                if (m || f) // bit already set?
                    break;
                u8 gender = pk.gender() & 1;
                data[ofs + brSize * 2] &= ~mask; // unset
                data[ofs + brSize * 3] &= ~mask; // unset
                gender ^= 1; // Set OTHER gender seen bit so it appears second
                data[ofs + brSize * (2 + gender)] |= mask;
                break;
        }
    }

    int formOffset = gbo + 0x12B8 + 4 + brSize * 4 + 4;
    u8 forms[0x1C];
    bool validFormArray = getForms(forms, pk.species());
    u8 fc = PersonalDPPtHGSS::formCount(pk.species());
    if (validFormArray && fc > 0)
    {
        if (pk.species() == 201) // Unown
        {
            for (u8 i = 0; i < 0x1C; i++)
            {
                u8 val = data[formOffset + 4 + i];
                if (val == pk.alternativeForm())
                    break; // already set
                if (val != 0xFF)
                    continue; // keep searching

                data[formOffset + 4 + i] = (u8)pk.alternativeForm();
                break; // form now set
            }
        }
        else if (pk.species() == 172) // Pichu
        {
            u8 form = pk.alternativeForm() == 1 ? 2 : pk.gender();
            checkInsertForm(forms, fc, form);
            setForms(forms, pk.species());
        }
        else
        {
            checkInsertForm(forms, fc, pk.alternativeForm());
            setForms(forms, pk.species());
        }
    }

    // Set the Language
    int languageFlags = formOffset + 0x3C;
    int lang = pk.language() - 1;
    switch (lang) // invert ITA/GER
    {
        case 3:
            lang = 4;
            break;
        case 4:
            lang = 3;
            break;
    }
    if (lang > 5)
        lang = 0; // no KOR+
    lang = (lang < 0) ? 1 : lang; // default English
    data[languageFlags + pk.species()] |= (u8)(1 << lang);
}

bool SavHGSS::checkInsertForm(u8* forms, u8 fc, u8 formNum)
{
    for (int i = 0; i < fc; i++)
    {
        if (forms[i] == formNum)
        {
            return false;
        }
    }

    u8 dummy[fc];
    std::fill_n(dummy, fc, 0xFF);
    if (!memcmp(forms, dummy, fc))
    {
        forms[0] = formNum;
        return true;
    }

    // insert at first empty
    u8 index = 255;
    for (int i = 0; i < fc; i++)
    {
        if (forms[i] == 255)
        {
            index = i;
        }
    }

    if (index == 255)
    {
        return false;
    }

    forms[index] = formNum;
    return true;
}

bool SavHGSS::getForms(u8* forms, u16 species)
{
    static const u8 brSize = 0x40;
    if (species == 386)
    {
        u32 val = (u32)data[gbo + 0x12B8 + 0x4 + 1*brSize - 1] | data[gbo + 0x12B8 + 0x4 + 2*brSize - 1] << 8;
        getDexFormValues(forms, val, 4, 4);
        return true;
    }

    int formOffset = gbo + 0x12B8 + 4 + 4*brSize + 4;
    switch (species)
    {
        case 422: // Shellos
            getDexFormValues(forms, data[formOffset + 0], 1, 2);
            return true;
        case 423: // Gastrodon
            getDexFormValues(forms, data[formOffset + 1], 1, 2);
            return true;
        case 412: // Burmy
            getDexFormValues(forms, data[formOffset + 2], 2, 3);
            return true;
        case 413: // Wormadam
            getDexFormValues(forms, data[formOffset + 3], 2, 3);
            return true;
        case 201:
            int ofs = formOffset + 4;
            std::copy(data + ofs, data + ofs + 0x1C, forms);
            return true;
    }

    int languageFlags = formOffset + 0x3C;
    int formOffset2 = languageFlags + 0x1F4;
    switch (species)
    {
        case 479: // Rotom
            getDexFormValues(forms, *(u32*)(data + formOffset2), 3, 6);
            return true;
        case 492: // Shaymin
            getDexFormValues(forms, data[formOffset2 + 4], 1, 2);
            return true;
        case 487: // Giratina
            getDexFormValues(forms, data[formOffset2 + 5], 1, 2);
            return true;
        case 172:
            getDexFormValues(forms, data[formOffset2 + 6], 2, 3);
            return true;
    }

    return false;
}

void SavHGSS::getDexFormValues(u8* forms, u32 v, u8 bitsPerForm, u8 readCt)
{
    u8 n1 = 0xFF >> (8 - bitsPerForm);
    for (int i = 0; i < readCt; i++)
    {
        u8 val = (u8)(v >> (i * bitsPerForm)) & n1;
        forms[i] = n1 == val && bitsPerForm > 1 ? 255 : val;
    }

    if (bitsPerForm == 1 && forms[0] == forms[1] && forms[0] == 1)
    {
        forms[0] = forms[1] = 255;
    }
}

void SavHGSS::setForms(u8* forms, u16 species)
{
    static const u8 brSize = 0x40;
    if (species == 386)
    {
        u32 newval = setDexFormValues(forms, 4, 4);
        data[gbo + 0x12B8 + 0x4 + 1*brSize - 1] = (u8) (newval & 0xFF);
        data[gbo + 0x12B8 + 0x4 + 2*brSize - 1] = (u8) ((newval >> 8) & 0xFF);
    }

    int formOffset = gbo + 0x12B8 + 4 + 4*brSize + 4;
    switch (species)
    {
        case 422: // Shellos
            data[formOffset + 0] = (u8)setDexFormValues(forms, 2, 1);
            return;
        case 423: // Gastrodon
            data[formOffset + 1] = (u8)setDexFormValues(forms, 2, 1);
            return;
        case 412: // Burmy
            data[formOffset + 2] = (u8)setDexFormValues(forms, 3, 2);
            return;
        case 413: // Wormadam
            data[formOffset + 3] = (u8)setDexFormValues(forms, 3, 2);
            return;
        case 201: // Unown
        {
            int ofs = formOffset + 4;
            // TODO: missing 0xFF handling
            std::copy(forms, forms + 0x1C, data + ofs);
            return;
        }
    }

    int languageFlags = formOffset + 0x3C;
    int formOffset2 = languageFlags + 0x1F4;
    switch (species)
    {
        case 479: // Rotom
        {
            u32 num = setDexFormValues(forms, 6, 3);
            u8* values = (u8*)&num;
            for (int i = formOffset2; i < formOffset2 + 6; i++)
            {
                data[i] = values[i - formOffset2];
            }
            return;
        }
        case 492: // Shaymin
        {
            data[formOffset2 + 4] = (u8)setDexFormValues(forms, 2, 1);
            return;
        }
        case 487: // Giratina
        {
            data[formOffset2 + 5] = (u8)setDexFormValues(forms, 2, 1);
            return;
        }
        case 172: // Pichu
        {
            data[formOffset2 + 6] = (u8)setDexFormValues(forms, 3, 2);
            return;
        }
    }
}

u32 SavHGSS::setDexFormValues(u8* forms, u8 fc, u8 bitsPerForm)
{
    int n1 = 0xFF >> (8 - bitsPerForm);
    u32 v = 0xFFFFFFFF << (fc*bitsPerForm);
    for (int i = 0; i < fc; i++)
    {
        int val = forms[i];
        if (val == 255)
            val = n1;

        v |= (u32)(val << (bitsPerForm*i));
    }
    return v;
}

std::shared_ptr<PKX> SavHGSS::emptyPkm() const
{
    static auto empty = std::make_shared<PK4>();
    return empty;
}

int SavHGSS::emptyGiftLocation(void) const
{
    u8 t;
    bool empty;
    u8* wondercards = data + 0x9E3C + gbo;
    for (t = 0; t < maxWondercards(); t++)
    {
        empty = true;
        for (u32 j = 0; j < PGT::length; j++)
        {
            if (*(wondercards + t * PGT::length + j) != 0)
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
    return !empty ? 7 : t;
}

std::vector<MysteryGift::giftData> SavHGSS::currentGifts(void) const
{
    std::vector<MysteryGift::giftData> ret;
    u8* wonderCards = data + 0x9E3C + gbo;
    for (int i = 0; i < emptyGiftLocation(); i++)
    {
        if (*(wonderCards + i * PGT::length) == 1 || *(wonderCards + i * PGT::length) == 2)
        {
            ret.push_back({ "Wonder Card", "", *(u16*)(wonderCards + i * PGT::length + 8 + 0x08), *(wonderCards + i * PGT::length + 8 + 0x40) >> 3});
        }
        else if (*(wonderCards + i * PGT::length) == 7)
        {
            ret.push_back({ "Wonder Card", "", 490, -1 });
        }
        else
        {
            ret.push_back({ "Wonder Card", "", -1, -1 });
        }
    }
    return ret;
}

std::unique_ptr<WCX> SavHGSS::mysteryGift(int pos) const
{
    return std::make_unique<PGT>(data + 0x9E3C + gbo + pos * PGT::length);
}

void SavHGSS::item(Item& item, Pouch pouch, u16 slot)
{
    Item4 inject = (Item4) item;
    auto write = inject.bytes();
    switch (pouch)
    {
        case NormalItem:
            std::copy(write.first, write.first + write.second, data + 0x644 + gbo + slot * 4);
            break;
        case KeyItem:
            std::copy(write.first, write.first + write.second, data + 0x8D8 + gbo + slot * 4);
            break;
        case TM:
            std::copy(write.first, write.first + write.second, data + 0x9A0 + gbo + slot * 4);
            break;
        case Mail:
            std::copy(write.first, write.first + write.second, data + 0xB34 + gbo + slot * 4);
            break;
        case Medicine:
            std::copy(write.first, write.first + write.second, data + 0xB64 + gbo + slot * 4);
            break;
        case Berry:
            std::copy(write.first, write.first + write.second, data + 0xC04 + gbo + slot * 4);
            break;
        case Ball:
            std::copy(write.first, write.first + write.second, data + 0xD04 + gbo + slot * 4);
            break;
        case Battle:
            std::copy(write.first, write.first + write.second, data + 0xD64 + gbo + slot * 4);
            break;
        default:
            return;
    }
}

std::unique_ptr<Item> SavHGSS::item(Pouch pouch, u16 slot) const
{
    switch (pouch)
    {
        case NormalItem:
            return std::make_unique<Item4>(data + 0x644 + gbo + slot * 4);
        case KeyItem:
            return std::make_unique<Item4>(data + 0x8D8 + gbo + slot * 4);
        case TM:
            return std::make_unique<Item4>(data + 0x9A0 + gbo + slot * 4);
        case Mail:
            return std::make_unique<Item4>(data + 0xB34 + gbo + slot * 4);
        case Medicine:
            return std::make_unique<Item4>(data + 0xB64 + gbo + slot * 4);
        case Berry:
            return std::make_unique<Item4>(data + 0xC04 + gbo + slot * 4);
        case Ball:
            return std::make_unique<Item4>(data + 0xD04 + gbo + slot * 4);
        case Battle:
            return std::make_unique<Item4>(data + 0xD64 + gbo + slot * 4);
        default:
            return nullptr;
    }
}

std::vector<std::pair<Pouch, int>> SavHGSS::pouches(void) const
{
    return {
        { NormalItem, 162 },
        { KeyItem, 38 },
        { TM, 100 },
        { Mail, 12 },
        { Medicine, 38 },
        { Berry, 64 },
        { Ball, 24 },
        { Battle, 13 }
    };
}

std::map<Pouch, std::vector<int>> SavHGSS::validItems() const
{
    return {
        { NormalItem, {
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 135, 136, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327
        }},
        { KeyItem, {
            434, 435, 437, 444, 445, 446, 447, 450, 456, 464, 465, 466, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 501, 502, 503, 504, 532, 533, 534, 535, 536
        }},
        { TM, {
            328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427
        }},
        { Mail, {
            137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148
        }},
        { Medicine, {
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54
        }},
        { Berry, {
            149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212
        }},
        { Ball, {
            1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 492, 493, 494, 495, 496, 497, 498, 499, 500
        }},
        { Battle, {
            55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67
        }}
    };
}

std::string SavHGSS::pouchName(Pouch pouch) const
{
    switch (pouch)
    {
        case NormalItem:
            return i18n::localize("ITEMS");
        case KeyItem:
            return i18n::localize("KEY_ITEMS");
        case TM:
            return i18n::localize("TMHM");
        case Mail:
            return i18n::localize("MAIL");
        case Medicine:
            return i18n::localize("MEDICINE");
        case Berry:
            return i18n::localize("BERRIES");
        case Ball:
            return i18n::localize("BALLS");
        case Battle:
            return i18n::localize("BATTLE_ITEMS");
        default:
            return "";
    }
}