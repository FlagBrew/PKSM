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

#include "Sav7.hpp"

u16 Sav7::check16(u8* buf, u32 blockID, u32 len) const
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

u16 Sav7::TID(void) const { return *(u16*)(data + TrainerCard); }
void Sav7::TID(u16 v) { *(u16*)(data + TrainerCard) = v; }

u16 Sav7::SID(void) const { return *(u16*)(data + TrainerCard + 2); }
void Sav7::SID(u16 v) { *(u16*)(data + TrainerCard + 2) = v; }

u8 Sav7::version(void) const { return data[TrainerCard + 4]; }
void Sav7::version(u8 v) { data[TrainerCard + 4] = v; }

u8 Sav7::gender(void) const { return data[TrainerCard + 5]; }
void Sav7::gender(u8 v) { data[TrainerCard + 5] = v; }

u8 Sav7::subRegion(void) const { return data[TrainerCard + 0x2E]; }
void Sav7::subRegion(u8 v) { data[TrainerCard + 0x2E] = v; }

u8 Sav7::country(void) const { return data[TrainerCard + 0x2F]; }
void Sav7::country(u8 v) { data[TrainerCard + 0x2F] = v; }

u8 Sav7::consoleRegion(void) const { return data[TrainerCard + 0x34]; }
void Sav7::consoleRegion(u8 v) { data[TrainerCard + 0x34] = v; }

u8 Sav7::language(void) const { return data[TrainerCard + 0x35]; }
void Sav7::language(u8 v) { data[TrainerCard + 0x35] = v; }

std::string Sav7::otName(void) const { return StringUtils::getString(data, TrainerCard + 0x38, 13); }
void Sav7::otName(const char* v) { return StringUtils::setString(data, v, TrainerCard + 0x38, 13); }

u32 Sav7::money(void) const { return *(u32*)(data + Misc + 0x4); }
void Sav7::money(u32 v) { *(u32*)(data + Misc + 0x4) = v > 9999999 ? 9999999 : v; }

u32 Sav7::BP(void) const { return *(u32*)(data + Misc + 0x11C); }
void Sav7::BP(u32 v) { *(u32*)(data + Misc + 0x11C) = v > 9999 ? 9999 : v; }

u16 Sav7::playedHours(void) const { return *(u16*)(data + PlayTime); }
void Sav7::playedHours(u16 v) { *(u16*)(data + PlayTime) = v; }

u8 Sav7::playedMinutes(void) const { return data[PlayTime + 2]; }
void Sav7::playedMinutes(u8 v) { data[PlayTime + 2] = v; }

u8 Sav7::playedSeconds(void) const { return data[PlayTime + 3]; }
void Sav7::playedSeconds(u8 v) { data[PlayTime + 3] = v; }

u8 Sav7::currentBox(void) const { return data[LastViewedBox]; }
void Sav7::currentBox(u8 v) { data[LastViewedBox] = v; }

u32 Sav7::boxOffset(u8 box, u8 slot) const { return Box + 232*30*box + 232*slot; }

u32 Sav7::partyOffset(u8 slot) const { return Party + 260*slot; }

std::shared_ptr<PKX> Sav7::pkm(u8 slot) const
{
    u8 buf[260];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 260, buf);
    return std::make_unique<PK7>(buf, true, true);
}

void Sav7::pkm(std::shared_ptr<PKX> pk, u8 slot)
{
    u8 buf[260] = {0};
    std::copy(pk->rawData(), pk->rawData() + pk->getLength(), buf);
    std::unique_ptr<PK7> pk7 = std::make_unique<PK7>(buf, false, true);

    if (pk->getLength() != 260)
    {
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
}

std::shared_ptr<PKX> Sav7::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 buf[232];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 232, buf);
    return std::make_unique<PK7>(buf, ekx);
}

void Sav7::pkm(std::shared_ptr<PKX> pk, u8 box, u8 slot)
{
    // TODO: trade logic
    std::copy(pk->rawData(), pk->rawData() + 232, data + boxOffset(box, slot));
}

void Sav7::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < boxes; box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::shared_ptr<PKX> pk7 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pk7->encrypt();
            }
            pkm(pk7, box, slot);
        }
    }
}

void Sav7::setDexFlags(int index, int gender, int shiny, int baseSpecies)
{
    const int brSize = 0x8C;
    int shift = gender | (shiny << 1);
    int ofs = PokeDex + 0x08 + 0x80 + 0x68;
    int bd = index >> 3; 
    int bm = index & 7;
    int bd1 = baseSpecies >> 3;
    int bm1 = baseSpecies & 7;

    int brSeen = shift * brSize;
    data[ofs + brSeen + bd] |= (u8)(1 << bm);

    bool displayed = false;
    for (u8 i = 0; i < 4; i++)
    {
        int brDisplayed = (4 + i) * brSize;
        displayed |= (data[ofs + brDisplayed + bd1] & (u8)(1 << bm1)) != 0;
    }

    if (!displayed && baseSpecies != index)
    {
        for (u8 i = 0; i < 4; i++)
        {
            int brDisplayed = (4 + i) * brSize;
            displayed |= (data[ofs + brDisplayed + bd] & (u8)(1 << bm)) != 0;
        }
    }
    if (displayed)
        return;

    data[ofs + (4 + shift) * brSize + bd] |= (u8)(1 << bm);
}

bool Sav7::sanitizeFormsToIterate(int species, int& fs, int& fe, int formIn) const
{
    switch (species)
    {
        case 351: // Castform
            fs = 0;
            fe = 3;
            return true;

        case 421: // Cherrim
        case 555: // Darmanitan
        case 648: // Meloetta
        case 746: // Wishiwashi
        case 778: // Mimikyu
            // Alolans
        case 020: // Raticate
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

void Sav7::dex(std::shared_ptr<PKX> pk)
{
    int MaxSpeciesID = game == Game::SM ? 802 : 807;
    if (pk->species() == 0 || pk->species() > MaxSpeciesID || pk->egg())
        return;

    int bit = pk->species() - 1;
    int bd = bit >> 3;
    int bm = bit & 7;
    int gender = pk->gender() % 2;
    int shiny = pk->shiny() ? 1 : 0;
    if (pk->species() == 351)
        shiny = 0;
    int shift = gender | (shiny << 1);
    
    if (pk->species() == 327) // Spinda
    {
        if ((data[PokeDex + 0x84] & (1 << (shift + 4))) != 0)
        { // Already 2
            *(u32*)(data + PokeDex + 0x8E8 + shift*4) = pk->encryptionConstant();
            data[PokeDex + 0x84] |= (u8)(1 << shift);
        }
        else if ((data[PokeDex + 0x84] & (1 << shift)) == 0) 
        { // Not yet 1
            data[PokeDex + 0x84] |= (u8)(1 << shift); // 1
        }
    }

    int off = PokeDex + 0x08 + 0x80;
    data[off + bd] |= (u8)(1 << bm);

    int formstart = pk->alternativeForm();
    int formend = formstart;

    int fs = 0, fe = 0;
    if (sanitizeFormsToIterate(pk->species(), fs, fe, formstart))
    {
        formstart = fs;
        formend = fe;
    }

    for (int form = formstart; form <= formend; form++)
    {
        int bitIndex = bit;
        if (form > 0)
        {
            u8 fc = PersonalSMUSUM::formCount(pk->species());
            if (fc > 1)
            { // actually has forms
                int f = dexFormIndex(pk->species(), fc, MaxSpeciesID - 1);
                if (f >= 0) // bit index valid
                    bitIndex = f + form;
            }
        }
        setDexFlags(bitIndex, gender, shiny, pk->species() - 1);
    }

    int lang = pk->language();
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

void Sav7::mysteryGift(WCX& wc, int& pos)
{
    WC7* wc7 = (WC7*)&wc;
    *(u8*)(data + WondercardFlags + wc7->ID()/8) |= 0x1 << (wc7->ID() % 8);
    std::copy(wc7->rawData(), wc7->rawData() + 264, data + WondercardData + 264*pos);
    pos = (pos + 1) % 48;
}

std::string Sav7::boxName(u8 box) const
{
    return StringUtils::getString(data, PCLayout + 0x22*box, 17);
}

void Sav7::boxName(u8 box, std::string name)
{
    StringUtils::setString(data, name.c_str(), PCLayout + 0x22*box, 17);
}

u8 Sav7::partyCount(void) const { return data[Party + 6*260]; }
void Sav7::partyCount(u8 v) { data[Party + 6*260] = v; }

std::shared_ptr<PKX> Sav7::emptyPkm() const
{
    static auto empty = std::make_shared<PK7>();
    return empty;
}

int Sav7::emptyGiftLocation(void) const
{
    u8 t;
    bool empty;
    // 48 max wonder cards
    for (t = 0; t < 48; t++)
    {
        empty = true;
        for (u32 j = 0; j < WC7::length; j++)
        {
            if (*(data + WondercardData + t * WC7::length + j) != 0)
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

std::vector<MysteryGift::giftData> Sav7::currentGifts(void) const
{
    std::vector<MysteryGift::giftData> ret;
    u8* wonderCards = data + WondercardData;
    for (int i = 0; i < emptyGiftLocation(); i++)
    {
        if (*(wonderCards + i * WC7::length + 0x51) == 0)
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC7::length, 0x2, 36), "", *(u16*)(wonderCards + i * WC7::length + 0x82), *(wonderCards + i * WC7::length + 0x84), *(wonderCards + i * WC6::length + 0xA1)});
        }
        else
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC7::length, 0x2, 36), "", -1, -1, -1 });
        }
    }
    return ret;
}

std::unique_ptr<WCX> Sav7::mysteryGift(int pos) const
{
    return std::make_unique<WC7>(data + WondercardData + pos * WC7::length);
}

void Sav7::item(Item& item, Pouch pouch, u16 slot)
{
    Item7 inject = (Item7) item;
    auto write = inject.bytes();
    switch (pouch)
    {
        case NormalItem:
            std::copy(write.first, write.first + write.second, data + PouchHeldItem + slot * 4);
            break;
        case KeyItem:
            std::copy(write.first, write.first + write.second, data + PouchKeyItem + slot * 4);
            break;
        case TM:
            std::copy(write.first, write.first + write.second, data + PouchTMHM + slot * 4);
            break;
        case Medicine:
            std::copy(write.first, write.first + write.second, data + PouchMedicine + slot * 4);
            break;
        case Berry:
            std::copy(write.first, write.first + write.second, data + PouchBerry + slot * 4);
            break;
        case ZCrystals:
            std::copy(write.first, write.first + write.second, data + PouchZCrystals + slot * 4);
            break;
        case Battle:
            std::copy(write.first, write.first + write.second, data + BattleItems + slot * 4);
            break;
        default:
            return;
    }
}

std::unique_ptr<Item> Sav7::item(Pouch pouch, u16 slot) const
{
    switch (pouch)
    {
        case NormalItem:
            return std::make_unique<Item7>(data + PouchHeldItem + slot * 4);
        case KeyItem:
            return std::make_unique<Item7>(data + PouchKeyItem + slot * 4);
        case TM:
            return std::make_unique<Item7>(data + PouchTMHM + slot * 4);
        case Medicine:
            return std::make_unique<Item7>(data + PouchMedicine + slot * 4);
        case Berry:
            return std::make_unique<Item7>(data + PouchBerry + slot * 4);
        case ZCrystals:
            return std::make_unique<Item7>(data + PouchZCrystals + slot * 4);
        case Battle:
            return std::make_unique<Item7>(data + BattleItems + slot * 4);
        default:
            return nullptr;
    }
}

std::vector<std::pair<Pouch, int>> Sav7::pouches(void) const
{
    std::vector<std::pair<Pouch, int>> pouches = 
    {
        { NormalItem, game == Game::SM ? 430 : 427 },
        { KeyItem, game == Game::SM ? 184 : 198 },
        { TM, 108 },
        { Medicine, game == Game::SM ? 64 : 60 },
        { Berry, game == Game::SM ? 72 : 67 },
        { ZCrystals, game == Game::SM ? 30 : 35 }
    };

    if (game == Game::USUM)
        pouches.push_back({ Battle, 11 });

    return pouches;
}

std::string Sav7::pouchName(Pouch pouch) const
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
        case Battle:
            return i18n::localize("ROTOM_POWERS");
        default:
            return "";
    }
}
