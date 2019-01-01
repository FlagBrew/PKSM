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

#include "SavORAS.hpp"

SavORAS::SavORAS(u8* dt)
{
    length = 0x76000;
    boxes = 31;

    data = new u8[length];
    std::copy(dt, dt + length, data);
}

void SavORAS::resign(void)
{
    const u8 blockCount = 58;
    u8* tmp = new u8[*std::max_element(chklen, chklen + blockCount)];
    const u32 csoff = 0x75E1A;

    for (u8 i = 0; i < blockCount; i++)
    {
        std::copy(data + chkofs[i], data + chkofs[i] + chklen[i], tmp);
        *(u16*)(data + csoff + i*8) =  ccitt16(tmp, chklen[i]);
    }

    delete[] tmp;
}

u16 SavORAS::TID(void) const { return *(u16*)(data + 0x14000); }
void SavORAS::TID(u16 v) { *(u16*)(data + 0x14000) = v; }

u16 SavORAS::SID(void) const { return *(u16*)(data + 0x14002); }
void SavORAS::SID(u16 v) { *(u16*)(data + 0x14002) = v; }

u8 SavORAS::version(void) const { return data[0x14004]; }
void SavORAS::version(u8 v) { data[0x14004] = v; }

u8 SavORAS::gender(void) const { return data[0x14005]; }
void SavORAS::gender(u8 v) { data[0x14005] = v; }

u8 SavORAS::subRegion(void) const { return data[0x14026]; }
void SavORAS::subRegion(u8 v) { data[0x14026] = v; }

u8 SavORAS::country(void) const { return data[0x14027]; }
void SavORAS::country(u8 v) { data[0x14027] = v; }

u8 SavORAS::consoleRegion(void) const { return data[0x1402C]; }
void SavORAS::consoleRegion(u8 v) { data[0x1402C] = v; }

u8 SavORAS::language(void) const { return data[0x1402D]; }
void SavORAS::language(u8 v) { data[0x1402D] = v; }

std::string SavORAS::otName(void) const { return StringUtils::getString(data, 0x14048, 13); }
void SavORAS::otName(const char* v) { StringUtils::setString(data, v, 0x14048, 13); }

u32 SavORAS::money(void) const { return *(u32*)(data + 0x4208); }
void SavORAS::money(u32 v) { *(u32*)(data + 0x4208) = v; }

u32 SavORAS::BP(void) const { return *(u32*)(data + 0x4230); }
void SavORAS::BP(u32 v) { *(u32*)(data + 0x4230) = v; }

u16 SavORAS::playedHours(void) const { return *(u16*)(data + 0x1800); }
void SavORAS::playedHours(u16 v) { *(u16*)(data + 0x1800) = v; }

u8 SavORAS::playedMinutes(void) const { return *(u8*)(data + 0x1802); }
void SavORAS::playedMinutes(u8 v) { *(u8*)(data + 0x1802) = v; }

u8 SavORAS::playedSeconds(void) const { return *(u8*)(data + 0x1803); }
void SavORAS::playedSeconds(u8 v) { *(u8*)(data + 0x1803) = v; }

u8 SavORAS::currentBox(void) const { return data[0x483F]; }
void SavORAS::currentBox(u8 v) { data[0x483F] = v; }

u32 SavORAS::boxOffset(u8 box, u8 slot) const { return 0x33000 + 232*30*box + 232*slot; }

u32 SavORAS::partyOffset(u8 slot) const { return 0x14200 + 260*slot; }

std::unique_ptr<PKX> SavORAS::pkm(u8 slot) const
{
    u8 tmp[260];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 260, tmp);
    return std::make_unique<PK6>(tmp, true, true);
}

void SavORAS::pkm(PKX& pk, u8 slot)
{
    pk.encrypt();
    PK6* pk6 = (PK6*)&pk;
    std::copy(pk6->data, pk6->data + pk6->length, data + partyOffset(slot));
    pk.decrypt();
}

std::unique_ptr<PKX> SavORAS::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 tmp[232];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 232, tmp);
    return std::make_unique<PK6>(tmp, ekx);
}

void SavORAS::pkm(PKX& pk, u8 box, u8 slot)
{
    PK6* pk6 = (PK6*)&pk;
    std::copy(pk6->data, pk6->data + 232, data + boxOffset(box, slot));
}

void SavORAS::cryptBoxData(bool crypted)
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

int SavORAS::dexFormIndex(int species, int formct) const
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
        case 260: return 208; // 2 Swampert
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

void SavORAS::dex(PKX& pk)
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
    data[0x15400 + (bit * 7 + lang) / 8] |= (u8)(1 << ((bit * 7 + lang) % 8));

    // Set DexNav count (only if not encountered previously)
    if (*(u16*)(data + 0x15686 + (pk.species() - 1) * 2) == 0)
        *(u16*)(data + 0x15686 + (pk.species() - 1) * 2) = 1;

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

void SavORAS::mysteryGift(WCX& wc, int& pos)
{
    WC6* wc6 = (WC6*)&wc;
    *(u8*)(data + 0x1CC00 + wc6->ID()/8) |= 0x1 << (wc6->ID() % 8);
    std::copy(wc6->data, wc6->data + 264, data + 0x1CD00 + 264*pos);
    pos = (pos + 1) % 24;
}

std::string SavORAS::boxName(u8 box) const { return StringUtils::getString(data, 0x4400 + 0x22*box, 17); }

void SavORAS::boxName(u8 box, std::string name)
{
    StringUtils::setString(data, name.c_str(), 0x4400 + 0x22*box, 17);
}

u8 SavORAS::partyCount(void) const { return data[partyOffset(0) + 6*260]; }
void SavORAS::partyCount(u8 v) { data[partyOffset(0) + 6*260] = v; }

std::shared_ptr<PKX> SavORAS::emptyPkm() const
{
    static auto empty = std::make_shared<PK6>();
    return empty;
}

int SavORAS::emptyGiftLocation(void) const
{
    u8 t;
    bool empty;
    // 24 max wonder cards
    for (t = 0; t < 24; t++)
    {
        empty = true;
        for (u32 j = 0; j < WC6::length; j++)
        {
            if (*(data + 0x1CD00 + t * WC6::length + j) != 0)
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

std::vector<MysteryGift::giftData> SavORAS::currentGifts(void) const
{
    std::vector<MysteryGift::giftData> ret;
    u8* wonderCards = data + 0x1CD00;
    for (int i = 0; i < emptyGiftLocation(); i++)
    {
        if (*(wonderCards + i * WC6::length + 0x51) == 0)
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC6::length, 0x2, 36), "", *(u16*)(wonderCards + i * WC6::length + 0x82), *(wonderCards + i * WC6::length + 0x84)});
        }
        else
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC6::length, 0x2, 36), "", -1, -1 });
        }
    }
    return ret;
}

std::unique_ptr<WCX> SavORAS::mysteryGift(int pos) const
{
    return std::make_unique<WC6>(data + 0x1CD00 + pos * WC6::length);
}

void SavORAS::item(Item& item, Pouch pouch, u16 slot)
{
    Item6 inject = (Item6) item;
    auto write = inject.bytes();
    switch (pouch)
    {
        case NormalItem:
            std::copy(write.first, write.first + write.second, data + 0x400 + slot * 4);
            break;
        case KeyItem:
            std::copy(write.first, write.first + write.second, data + 0xA40 + slot * 4);
            break;
        case TM:
            std::copy(write.first, write.first + write.second, data + 0xBC0 + slot * 4);
            break;
        case Medicine:
            std::copy(write.first, write.first + write.second, data + 0xD70 + slot * 4);
            break;
        case Berry:
            std::copy(write.first, write.first + write.second, data + 0xE70 + slot * 4);
            break;
        default:
            return;
    }
}

std::unique_ptr<Item> SavORAS::item(Pouch pouch, u16 slot) const
{
    switch (pouch)
    {
        case NormalItem:
            return std::make_unique<Item6>(data + 0x400 + slot * 4);
        case KeyItem:
            return std::make_unique<Item6>(data + 0xA40 + slot * 4);
        case TM:
            return std::make_unique<Item6>(data + 0xBC0 + slot * 4);
        case Medicine:
            return std::make_unique<Item6>(data + 0xD70 + slot * 4);
        case Berry:
            return std::make_unique<Item6>(data + 0xE70 + slot * 4);
        default:
            return nullptr;
    }
}

std::vector<std::pair<Pouch, int>> SavORAS::pouches(void) const
{
    return {
        { NormalItem, 305 },
        { KeyItem, 47 },
        { TM, 107 },
        { Medicine, 54 },
        { Berry, 67 }
    };
}

std::map<Pouch, std::vector<int>> SavORAS::validItems() const
{
    return {
        { NormalItem, {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            16, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
            67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
            80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
            93, 94, 99, 100, 101, 102, 103, 104, 105, 106, 107,
            108, 109, 110, 112, 116, 117, 118, 119, 135, 136,
            213, 214, 215, 217, 218, 219, 220, 221, 222, 223,
            224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
            234, 235, 236, 237, 238, 239, 240, 241, 242, 243,
            244, 245, 246, 247, 248, 249, 250, 251, 252, 253,
            254, 255, 256, 257, 258, 259, 260, 261, 262, 263,
            264, 265, 266, 267, 268, 269, 270, 271, 272, 273,
            274, 275, 276, 277, 278, 279, 280, 281, 282, 283,
            284, 285, 286, 287, 288, 289, 290, 291, 292, 293,
            294, 295, 296, 297, 298, 299, 300, 301, 302, 303,
            304, 305, 306, 307, 308, 309, 310, 311, 312, 313,
            314, 315, 316, 317, 318, 319, 320, 321, 322, 323,
            324, 325, 326, 327, 492, 493, 494, 495, 496, 497,
            498, 499, 500, 534, 535, 537, 538, 539, 540, 541,
            542, 543, 544, 545, 546, 547, 548, 549, 550, 551,
            552, 553, 554, 555, 556, 557, 558, 559, 560, 561,
            562, 563, 564, 571, 572, 573, 576, 577, 580, 581,
            582, 583, 584, 585, 586, 587, 588, 589, 590, 639,
            640, 644, 646, 647, 648, 649, 650, 652, 653, 654,
            655, 656, 657, 658, 659, 660, 661, 662, 663, 664,
            665, 666, 667, 668, 669, 670, 671, 672, 673, 674,
            675, 676, 677, 678, 679, 680, 681, 682, 683, 684,
            685, 699, 704, 710, 711, 715, 752, 753, 754, 755,
            756, 757, 758, 759, 760, 761, 762, 763, 764, 767,
            768, 769, 770
        }},
        { KeyItem, {
            216, 431, 442, 445, 446, 447, 450, 457, 465, 466,
            471, 474, 503, 628, 629, 631, 632, 638, 641, 642,
            643, 689, 695, 696, 697, 698, 700, 701, 702, 703,
            705, 712, 713, 714, 718, 719, 720, 721, 722, 724,
            725, 726, 727, 728, 729, 730, 731, 732, 733, 734,
            735, 736, 738, 739, 740, 741, 742, 743, 744, 751,
            765, 771, 772, 774, 775
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
            418, 419, 618, 619, 620, 690, 691, 692, 693, 694,
            420, 421, 422, 423, 424, 425, 737
        }},
        { Medicine, {
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
            29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
            53, 54, 65, 66, 67, 134, 504, 565, 566, 567,
            568, 569, 570, 571, 591, 645, 708, 709
        }},
        { Berry, {
            149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
            159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
            169, 170, 171, 172, 173, 174, 175, 176, 177, 178,
            179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
            189, 190, 191, 192, 193, 194, 195, 196, 197, 198,
            199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
            209, 210, 211, 212, 686, 687, 688
        }}
    };
}

std::string SavORAS::pouchName(Pouch pouch) const
{
    switch (pouch)
    {
        case NormalItem:
            return i18n::localize("ITEMS");
        case KeyItem:
            return i18n::localize("KEY_ITEMS");
        case TM:
            return i18n::localize("TMHM");
        case Medicine:
            return i18n::localize("MEDICINE");
        case Berry:
            return i18n::localize("BERRIES");
        default:
            return "";
    }
}