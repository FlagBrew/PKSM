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

#include "SavSUMO.hpp"

SavSUMO::SavSUMO(u8* dt)
{
    length = 0x6BE00;
    boxes = 32;
    
    data = new u8[length];
    std::copy(dt, dt + length, data);
}

u16 SavSUMO::check16(u8* buf, u32 blockID, u32 len) const
{
    if (blockID == 36)
    {
        u8 tmp[0x80] = {0};
        std::copy(tmp, tmp + 0x80, buf + 0x100);
    }

    u16 chk = ~0;
    for (u32 i = 0; i < len; i++)
    {
        chk = (crc16[(buf[i] ^ chk) & 0xFF] ^ chk >> 8);
    }
    return ~chk;
}

void SavSUMO::resign(void)
{
    const u8 blockCount = 37;
    u8* tmp = new u8[*std::max_element(chklen, chklen + blockCount)];
    const u32 csoff = 0x6BC1A;

    for (u8 i = 0; i < blockCount; i++)
    {
        std::copy(data + chkofs[i], data + chkofs[i] + chklen[i], tmp);
        *(u16*)(data + csoff + i*8) = check16(tmp, *(u16*)(data + csoff + i*8 - 2), chklen[i]);
    }

    delete[] tmp;

    const u32 checksumTableOffset = 0x6BC00;
    const u32 checksumTableLength = 0x140;
    const u32 memecryptoOffset = 0x6BB00;

    u8 currentSignature[0x80];
    std::copy(data + memecryptoOffset, data + memecryptoOffset + 0x80, currentSignature);

    u8 checksumTable[checksumTableLength];
    std::copy(data + checksumTableOffset, data + checksumTableOffset + checksumTableLength, checksumTable);

    u8 hash[SHA256_BLOCK_SIZE];
    sha256(hash, checksumTable, checksumTableLength);

    u8 decryptedSignature[0x80];
    reverseCrypt(currentSignature, decryptedSignature);
    std::copy(hash, hash + SHA256_BLOCK_SIZE, decryptedSignature);

    memecrypto_sign(decryptedSignature, currentSignature, 0x80);
    std::copy(currentSignature, currentSignature + 0x80, data + memecryptoOffset);
}

u16 SavSUMO::TID(void) const { return *(u16*)(data + 0x1200); }
void SavSUMO::TID(u16 v) { *(u16*)(data + 0x1200) = v; }

u16 SavSUMO::SID(void) const { return *(u16*)(data + 0x1202); }
void SavSUMO::SID(u16 v) { *(u16*)(data + 0x1202) = v; }

u8 SavSUMO::version(void) const { return data[0x1204]; }
void SavSUMO::version(u8 v) { data[0x1204] = v; }

u8 SavSUMO::gender(void) const { return data[0x1205]; }
void SavSUMO::gender(u8 v) { data[0x1205] = v; }

u8 SavSUMO::subRegion(void) const { return data[0x122E]; }
void SavSUMO::subRegion(u8 v) { data[0x122E] = v; }

u8 SavSUMO::country(void) const { return data[0x122F]; }
void SavSUMO::country(u8 v) { data[0x122F] = v; }

u8 SavSUMO::consoleRegion(void) const { return data[0x1234]; }
void SavSUMO::consoleRegion(u8 v) { data[0x1234] = v; }

u8 SavSUMO::language(void) const { return data[0x1235]; }
void SavSUMO::language(u8 v) { data[0x1235] = v; }

std::string SavSUMO::otName(void) const { return StringUtils::getString(data, 0x1238, 13); }
void SavSUMO::otName(const char* v) { return StringUtils::setString(data, v, 0x1238, 13); }

u32 SavSUMO::money(void) const { return *(u32*)(data + 0x4004); }
void SavSUMO::money(u32 v) { *(u32*)(data + 0x4004) = v > 9999999 ? 9999999 : v; }

u32 SavSUMO::BP(void) const { return *(u32*)(data + 0x411C); }
void SavSUMO::BP(u32 v) { *(u32*)(data + 0x411C) = v > 9999 ? 9999 : v; }

u16 SavSUMO::playedHours(void) const { return *(u16*)(data + 0x40C00); }
void SavSUMO::playedHours(u16 v) { *(u16*)(data + 0x40C00) = v; }

u8 SavSUMO::playedMinutes(void) const { return data[0x40C02]; }
void SavSUMO::playedMinutes(u8 v) { data[0x40C02] = v; }

u8 SavSUMO::playedSeconds(void) const { return data[0x40C03]; }
void SavSUMO::playedSeconds(u8 v) { data[0x40C03] = v; }

u8 SavSUMO::currentBox(void) const { return data[0x4DE3]; }
void SavSUMO::currentBox(u8 v) { data[0x4DE3] = v; }

u32 SavSUMO::boxOffset(u8 box, u8 slot) const { return 0x4E00 + 232*30*box + 232*slot; }

u32 SavSUMO::partyOffset(u8 slot) const { return 0x1400 + 260*slot; }

std::unique_ptr<PKX> SavSUMO::pkm(u8 slot) const
{
    u8 buf[260];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 260, buf);
    return std::make_unique<PK7>(buf, true, true);
}

void SavSUMO::pkm(PKX& pk, u8 slot)
{
    PK7* pk7 = (PK7*)&pk;
    if (pk7->getLength() != 260)
    {
        u8 buf[260] = {0};
        std::copy(pk7->rawData(), pk7->rawData() + pk7->getLength(), buf);
        pk7 = new PK7(buf, false, true);
        for (int i = 0; i < 6; i++)
        {
            pk7->partyStat(i, pk7->stat(i));
        }
        pk7->partyLevel(pk7->level());
        pk7->partyCurrHP(pk7->stat(0));
    }
    pk7->encrypt();
    std::fill(data + partyOffset(slot), data + partyOffset(slot + 1), (u8)0);
    std::copy(pk7->rawData(), pk7->rawData() + pk7->getLength(), data + partyOffset(slot));
    pk7->decrypt();
    if (pk7 != &pk)
    {
        delete pk7;
    }
}

std::unique_ptr<PKX> SavSUMO::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 buf[232];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 232, buf);
    return std::make_unique<PK7>(buf, ekx);
}

void SavSUMO::pkm(PKX& pk, u8 box, u8 slot)
{
    // TODO: trade logic
    PK7* pk7 = (PK7*)&pk;
    std::copy(pk7->rawData(), pk7->rawData() + 232, data + boxOffset(box, slot));
}

void SavSUMO::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < boxes; box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::unique_ptr<PKX> pk7 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pk7->encrypt();
            }
            pkm(*pk7, box, slot);
        }
    }
}

int SavSUMO::dexFormIndex(int species, int formct, int start) const
{
    int formindex = start;
    int f = 0;
    for (u8 i = 0; i < 230; i += 2)
    {
        if (formtable[i] == species)
        {
            break;
        }
        f = formtable[i + 1];
        formindex += f - 1;
    }
    return f > formct ? -1 : formindex;
}

int SavSUMO::dexFormCount(int species) const
{
    for (u8 i = 0; i < 230; i+= 2)
    {
        if (formtable[i] == species)
        {
            return formtable[i + 1];
        }
    }
    return 0;
}

void SavSUMO::setDexFlags(int index, int gender, int shiny, int baseSpecies)
{
    const int brSize = 0x8C;
    int shift = gender | (shiny << 1);
    int off = 0x2AF0;
    int bd = index >> 3; 
    int bm = index & 7;
    int bd1 = baseSpecies >> 3;
    int bm1 = baseSpecies & 7;

    int brSeen = shift * brSize;
    data[off + brSeen + bd] |= (u8)(1 << bm);

    bool displayed = false;
    for (u8 i = 0; i < 4; i++)
    {
        int brDisplayed = (4 + i) * brSize;
        displayed |= (data[off + brDisplayed + bd1] & (u8)(1 << bm1)) != 0;
    }

    if (!displayed && baseSpecies != index)
    {
        for (u8 i = 0; i < 4; i++)
        {
            int brDisplayed = (4 + i) * brSize;
            displayed |= (data[off + brDisplayed + bd] & (u8)(1 << bm)) != 0;
        }
    }
    if (displayed)
        return;

    data[off + (4 + shift) * brSize + bd] |= (u8)(1 << bm);
}

bool SavSUMO::sanitizeFormsToIterate(int species, int& fs, int& fe, int formIn) const
{
    switch (species)
    {
        case 351: // Castform
            fs = 0;
            fe = 3;
            return true;
        case 20:  // Raticate
        case 105: // Marowak
            fs = 0;
            fe = 1;
            return true;
        case 735: // Gumshoos
        case 758: // Salazzle
        case 754: // Lurantis
        case 738: // Vikavolt
        case 784: // Kommo-o
        case 752: // Araquanid
        case 777: // Togedemaru
        case 743: // Ribombee
        case 744: // Rockruff
            break;
        case 421: // Cherrim
        case 555: // Darmanitan
        case 648: // Meloetta
        case 746: // Wishiwashi
        case 778: // Mimikyu
            fs = 0;
            fe = 1;
            return true;
        case 774: // Minior
            if (formIn <= 6)
                break;
            else
            {
                int count = dexFormCount(species);
                fs = 0;
                fe = 0;
                return count < formIn;
            }
        case 718:
            if (formIn > 1)
                break;
            else
            {
                int count = dexFormCount(species);
                fs = 0;
                fe = 0;
                return count < formIn;
            }
    }

    fs = 0;
    fe = 0;
    return true;
}

void SavSUMO::dex(PKX& pk)
{
    int n = pk.species();
    int MaxSpeciesID = 802;
    int PokeDex = 0x2A00;
    int PokeDexLanguageFlags = PokeDex + 0x550;

    if (n == 0 || n > MaxSpeciesID || pk.egg())
        return;

    int bit = n - 1;
    int bd = bit >> 3;
    int bm = bit & 7;
    int gender = pk.gender() % 2;
    int shiny = pk.shiny() ? 1 : 0;
    if (n == 351)
        shiny = 0;
    int shift = gender | (shiny << 1);
    
    if (n == 327) // Spinda
    {
        if ((data[PokeDex + 0x84] & (1 << (shift + 4))) != 0)
        { // Already 2
            *(u32*)(data + PokeDex + 0x8E8 + shift*4) = pk.encryptionConstant();
            data[PokeDex + 0x84] |= (u8)(1 << shift);
        }
        else if ((data[PokeDex + 0x84] & (1 << shift)) == 0) 
        { // Not yet 1
            data[PokeDex + 0x84] |= (u8)(1 << shift); // 1
        }
    }

    int off = PokeDex + 0x08 + 0x80;
    data[off + bd] |= (u8)(1 << bm);

    int formstart = pk.alternativeForm();
    int formend = formstart;

    int fs = 0, fe = 0;
    if (sanitizeFormsToIterate(n, fs, fe, formstart))
    {
        formstart = fs;
        formend = fe;
    }

    for (int form = formstart; form <= formend; form++)
    {
        int bitIndex = bit;
        if (form > 0)
        {
            u8 fc = PersonalSMUSUM::formCount(n);
            if (fc > 1)
            { // actually has forms
                int f = dexFormIndex(n, fc, MaxSpeciesID - 1);
                if (f >= 0) // bit index valid
                    bitIndex = f + form;
            }
        }
        setDexFlags(bitIndex, gender, shiny, n - 1);
    }

    int lang = pk.language();
    const int langCount = 9;
    if (lang <= 10 && lang != 6 && lang != 0)
    {
        if (lang >= 7) lang--;
        lang--;
        if (lang < 0) lang = 1;
        int lbit = bit * langCount + lang;
        if (lbit >> 3 < 920)
            data[PokeDexLanguageFlags + (lbit >> 3)] |= (u8)(1 << (lbit & 7));
    }
}

void SavSUMO::mysteryGift(WCX& wc, int& pos)
{
    WC7* wc7 = (WC7*)&wc;
    *(u8*)(data + 0x65C00 + wc7->ID()/8) |= 0x1 << (wc7->ID() % 8);
    std::copy(wc7->rawData(), wc7->rawData() + 264, data + 0x65D00 + 264*pos);
    pos = (pos + 1) % 48;
}

std::string SavSUMO::boxName(u8 box) const
{
    return StringUtils::getString(data, 0x4800 + 0x22*box, 17);
}

void SavSUMO::boxName(u8 box, std::string name)
{
    StringUtils::setString(data, name.c_str(), 0x4800 + 0x22*box, 17);
}

u8 SavSUMO::partyCount(void) const { return data[partyOffset(0) + 6*260]; }
void SavSUMO::partyCount(u8 v) { data[partyOffset(0) + 6*260] = v; }

std::shared_ptr<PKX> SavSUMO::emptyPkm() const
{
    static auto empty = std::make_shared<PK7>();
    return empty;
}

int SavSUMO::emptyGiftLocation(void) const
{
    u8 t;
    bool empty;
    // 48 max wonder cards
    for (t = 0; t < 48; t++)
    {
        empty = true;
        for (u32 j = 0; j < WC7::length; j++)
        {
            if (*(data + 0x65D00 + t * WC7::length + j) != 0)
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

    return !empty ? 47 : t;
}

std::vector<MysteryGift::giftData> SavSUMO::currentGifts(void) const
{
    std::vector<MysteryGift::giftData> ret;
    u8* wonderCards = data + 0x65D00;
    for (int i = 0; i < emptyGiftLocation(); i++)
    {
        if (*(wonderCards + i * WC7::length + 0x51) == 0)
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC7::length, 0x2, 36), "", *(u16*)(wonderCards + i * WC7::length + 0x82), *(wonderCards + i * WC7::length + 0x84)});
        }
        else
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC7::length, 0x2, 36), "", -1, -1 });
        }
    }
    return ret;
}

std::unique_ptr<WCX> SavSUMO::mysteryGift(int pos) const
{
    return std::make_unique<WC7>(data + 0x65D00 + pos * WC7::length);
}

void SavSUMO::item(Item& item, Pouch pouch, u16 slot)
{
    Item7 inject = (Item7) item;
    auto write = inject.bytes();
    switch (pouch)
    {
        case NormalItem:
            std::copy(write.first, write.first + write.second, data + 0x000 + slot * 4);
            break;
        case KeyItem:
            std::copy(write.first, write.first + write.second, data + 0x6B8 + slot * 4);
            break;
        case TM:
            std::copy(write.first, write.first + write.second, data + 0x998 + slot * 4);
            break;
        case Medicine:
            std::copy(write.first, write.first + write.second, data + 0xB48 + slot * 4);
            break;
        case Berry:
            std::copy(write.first, write.first + write.second, data + 0xC48 + slot * 4);
            break;
        case ZCrystals:
            std::copy(write.first, write.first + write.second, data + 0xD68 + slot * 4);
            break;
        default:
            return;
    }
}

std::unique_ptr<Item> SavSUMO::item(Pouch pouch, u16 slot) const
{
    switch (pouch)
    {
        case NormalItem:
            return std::make_unique<Item7>(data + 0x000 + slot * 4);
        case KeyItem:
            return std::make_unique<Item7>(data + 0x6B8 + slot * 4);
        case TM:
            return std::make_unique<Item7>(data + 0x998 + slot * 4);
        case Medicine:
            return std::make_unique<Item7>(data + 0xB48 + slot * 4);
        case Berry:
            return std::make_unique<Item7>(data + 0xC48 + slot * 4);
        case ZCrystals:
            return std::make_unique<Item7>(data + 0xD68 + slot * 4);
        default:
            return nullptr;
    }
}

std::vector<std::pair<Pouch, int>> SavSUMO::pouches(void) const
{
    return {
        { NormalItem, 430 },
        { KeyItem, 184 },
        { TM, 108 },
        { Medicine, 64 },
        { Berry, 72 },
        { ZCrystals, 30 }
    };
}

std::map<Pouch, std::vector<int>> SavSUMO::validItems() const
{
    return {
        { NormalItem, {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
            80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
            92, 93, 94, 99, 100, 101, 102, 103, 104, 105,
            106, 107, 108, 109, 110, 111, 112, 116, 117, 118,
            119, 135, 136, 137, 213, 214, 215, 217, 218, 219,
            220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
            230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
            240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
            250, 251, 252, 253, 254, 255, 256, 257, 258, 259,
            260, 261, 262, 263, 264, 265, 266, 267, 268, 269,
            270, 271, 272, 273, 274, 275, 276, 277, 278, 279,
            280, 281, 282, 283, 284, 285, 286, 287, 288, 289,
            290, 291, 292, 293, 294, 295, 296, 297, 298, 299,
            300, 301, 302, 303, 304, 305, 306, 307, 308, 309,
            310, 311, 312, 313, 314, 315, 316, 317, 318, 319,
            320, 321, 322, 323, 324, 325, 326, 327, 492, 493,
            494, 495, 496, 497, 498, 499, 534, 535, 537, 538,
            539, 540, 541, 542, 543, 544, 545, 546, 547, 548,
            549, 550, 551, 552, 553, 554, 555, 556, 557, 558,
            559, 560, 561, 562, 563, 564, 571, 572, 573, 576,
            577, 580, 581, 582, 583, 584, 585, 586, 587, 588,
            589, 590, 639, 640, 644, 646, 647, 648, 649, 650,
            656, 657, 658, 659, 660, 661, 662, 663, 664, 665,
            666, 667, 668, 669, 670, 671, 672, 673, 674, 675,
            676, 677, 678, 679, 680, 681, 682, 683, 684, 685,
            699, 704, 710, 711, 715, 752, 753, 754, 755, 756,
            757, 758, 759, 760, 761, 762, 763, 764, 767, 768,
            769, 770, 795, 796, 844, 846, 849, 851, 853, 854,
            855, 856, 879, 880, 881, 882, 883, 884, 904, 905,
            906, 907, 908, 909, 910, 911, 912, 913, 914, 915,
            916, 917, 918, 919, 920
        }},
        { KeyItem, {
            216, 465, 466, 628, 629, 631, 632, 638, 705, 706,
            765, 773, 797, 841, 842, 843, 845, 847, 850, 857,
            858, 860
        }},
        { TM, {
            328, 329, 330, 331, 332, 333, 334, 335, 336, 337,
            338, 339, 340, 341, 342, 343, 344, 345, 346, 347,
            348, 349, 350, 351, 352, 353, 354, 355, 356, 357,
            358, 359, 360, 361, 362, 363, 364, 365, 366, 367,
            368, 369, 370, 371, 372, 373, 374, 375, 376, 377,
            378, 379, 380, 381, 382, 383, 384, 385, 386, 387,
            388, 389, 390, 391, 392, 393, 394, 395, 396, 397,
            398, 399, 400, 401, 402, 403, 404, 405, 406, 407,
            408, 409, 410, 411, 412, 413, 414, 415, 416, 417,
            418, 419, 618, 619, 620, 690, 691, 692, 693, 694
        }},
        { Medicine, {
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
            43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 65,
            66, 67, 134, 504, 565, 566, 567, 568, 569, 570, 591,
            645, 708, 709, 852
        }},
        { Berry, {
            149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
            159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
            169, 170, 171, 172, 173, 174, 175, 176, 177, 178,
            179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
            189, 190, 191, 192, 193, 194, 195, 196, 197, 198,
            199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
            209, 210, 211, 212, 686, 687, 688
        }},
        { ZCrystals, {
            807, 808, 809, 810, 811, 812, 813, 814, 815, 816,
            817, 818, 819, 820, 821, 822, 823, 824, 825, 826,
            827, 828, 829, 830, 831, 832, 833, 834, 835
        }}
    };
}

std::string SavSUMO::pouchName(Pouch pouch) const
{
    switch (pouch)
    {
        case NormalItem:
            return i18n::localize("ITEMS");
        case KeyItem:
            return i18n::localize("KEY_ITEMS");
        case TM:
            return i18n::localize("TMS");
        case Medicine:
            return i18n::localize("MEDICINE");
        case Berry:
            return i18n::localize("BERRIES");
        case ZCrystals:
            return i18n::localize("ZCRYSTALS");
        default:
            return "";
    }
}
