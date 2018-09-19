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
    u8 buf[232];
    std::copy(data + partyOffset(slot), data + partyOffset(slot) + 232, buf);
    return std::unique_ptr<PKX>(new PK7(buf, true));
}

std::unique_ptr<PKX> SavSUMO::pkm(u8 box, u8 slot, bool ekx) const
{
    u8 buf[232];
    std::copy(data + boxOffset(box, slot), data + boxOffset(box, slot) + 232, buf);
    return std::unique_ptr<PKX>(new PK7(buf, ekx));
}

void SavSUMO::pkm(PKX& pk, u8 box, u8 slot)
{
    // TODO: trade logic
    PK7* pk7 = (PK7*)&pk;
    std::copy(pk7->data, pk7->data + 232, data + boxOffset(box, slot));
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
    std::copy(wc7->data, wc7->data + 264, data + 0x65D00 + 264*pos);
    pos = (pos + 1) % 48;
}

std::string SavSUMO::boxName(u8 box) const
{
    return StringUtils::getString(data, 0x4800 + 0x22*box, 17);
}

u8 SavSUMO::partyCount(void) const { return data[partyOffset(0) + 6*260]; }

std::shared_ptr<PKX> SavSUMO::emptyPkm() const
{
    static std::shared_ptr<PKX> empty = std::shared_ptr<PKX>(new PK7);
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
    for (int i = 0; i < emptyGiftLocation() + 1; i++)
    {
        if (*(wonderCards + i * WC7::length + 0x51) == 0)
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC7::length, 0x2, 36), *(u16*)(wonderCards + i * WC7::length + 0x82), *(wonderCards + i * WC7::length + 0x84)});
        }
        else
        {
            ret.push_back({ StringUtils::getString(wonderCards + i * WC7::length, 0x2, 36), -1, -1 });
        }
    }
    return ret;
}