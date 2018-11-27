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

#include "SavLGPE.hpp"
#include "PB7.hpp"
#include "gui.hpp"
#include "FSStream.hpp"
#include "archive.hpp"
#include "WB7.hpp"

SavLGPE::SavLGPE(u8* dt)
{
    length = 0x100000;
    boxes = 34; // Ish

    data = new u8[length];
    std::copy(dt, dt + length, data);
}

SavLGPE::~SavLGPE() {}

u32 SavLGPE::boxOffset(u8 box, u8 slot) const
{
    return 0x5C00 + box * 30 * 260 + slot * 260;
}

u16 SavLGPE::partyBoxSlot(u8 slot) const
{
    return *(u16*)(data + 0x5A00 + slot * 2);
}

void SavLGPE::partyBoxSlot(u8 slot, u16 v)
{
    *(u16*)(data + 0x5A00 + slot * 2) = v;
}

u32 SavLGPE::partyOffset(u8 slot) const
{
    u16 boxSlot = partyBoxSlot(slot);
    if (boxSlot == 1001)
    {
        return 0;
    }
    return boxOffset(boxSlot / 30, boxSlot % 30);
}

u16 SavLGPE::boxedPkm() const
{
    return *(u16*)(data + 0x5A00 + 14);
}

void SavLGPE::boxedPkm(u16 v)
{
    *(u16*)(data + 0x5A00 + 14) = v;
}

u16 SavLGPE::followPkm() const
{
    return *(u16*)(data + 0x5A00 + 12);
}

void SavLGPE::followPkm(u16 v)
{
    *(u16*)(data + 0x5A00 + 12) = v;
}

u8 SavLGPE::partyCount() const
{
    u16 ret = 0;
    for (int i = 0; i < 6; i++)
    {
        if (partyBoxSlot(i) != 1001)
        {
            ret++;
        }
    }
    return ret;
}

void SavLGPE::fixParty()
{
    for (int i = 0; i < 5; i++)
    {
        if (partyBoxSlot(i) == 1001)
        {
            partyBoxSlot(i, partyBoxSlot(i + 1));
            partyBoxSlot(i + 1, 1001);
        }
    }
}

static bool isPKM(u8* pkmData)
{
    if (*(u32*)(pkmData) == 0 && *(u16*)(pkmData + 8) == 0)
    {
        return false;
    }
    return true;
}

void SavLGPE::compressBox()
{
    u16 emptyIndex = 1001;
    u8 emptyData[260] = {0};
    for (u16 i = 0; i < 1000; i++)
    {
        u32 offset = boxOffset(i / 30, i % 30);
        if (emptyIndex == 1001 && !isPKM(data + offset))
        {
            emptyIndex = i;
        }
        else if (emptyIndex != 1001)
        {
            if (isPKM(data + offset))
            {
                u32 emptyOffset = boxOffset(emptyIndex / 30, emptyIndex % 30);
                // Swap the two slots
                std::copy(data + emptyOffset, data + emptyOffset + 260, emptyData);
                std::copy(data + offset, data + offset + 260, data + emptyOffset);
                std::copy(emptyData, emptyData + 260, data + offset);
                for (int j = 0; j < partyCount(); j++)
                {
                    if (partyBoxSlot(j) == i)
                    {
                        partyBoxSlot(j, emptyIndex);
                    }
                }
                if (followPkm() == i)
                {
                    followPkm(emptyIndex);
                }
                emptyIndex++;
            }
        }
    }
}

u16 SavLGPE::check16(u8* buf, u32 blockID, u32 len) const
{
    u16 chk = 0;
    for (u32 i = 0; i < len; i++)
    {
        chk = (crc16[(buf[i] ^ chk) & 0xFF] ^ chk >> 8);
    }
    return chk;
}

void SavLGPE::resign()
{
    const u8 blockCount = 21;
    u8* tmp = new u8[*std::max_element(chklen, chklen + blockCount)];
    const u32 csoff = 0xB861A;

    for (u8 i = 0; i < blockCount; i++)
    {
        std::copy(data + chkofs[i], data + chkofs[i] + chklen[i], tmp);
        *(u16*)(data + csoff + i*8) = check16(tmp, *(u16*)(data + csoff + i*8 - 2), chklen[i]);
    }

    delete[] tmp;
}

u16 SavLGPE::TID() const
{
    return *(u16*)(data + 0x1000);
}

void SavLGPE::TID(u16 v)
{
    *(u16*)(data + 0x1000) = v;
}

u16 SavLGPE::SID() const
{
    return *(u16*)(data + 0x1002);
}

void SavLGPE::SID(u16 v)
{
    *(u16*)(data + 0x1002) = v;
}

u8 SavLGPE::version() const
{
    return *(data + 0x1004);
}

void SavLGPE::version(u8 v)
{
    *(data + 0x1004) = v;
}

u8 SavLGPE::gender() const
{
    return *(data + 0x1005);
}

void SavLGPE::gender(u8 v)
{
    *(data + 0x1005) = v;
}

u8 SavLGPE::language() const
{
    return *(data + 0x1035);
}

void SavLGPE::language(u8 v)
{
    *(data + 0x1035) = v;
}

std::string SavLGPE::otName() const
{
    // TODO StringUtils GetString7b
    return "";
}

void SavLGPE::otName(const char* v)
{
    (void) v;
}

u32 SavLGPE::money() const
{
    return *(u32*)(data + 0x4C04);
}

void SavLGPE::money(u32 v)
{
    *(u32*)(data + 0x4C04) = v;
}


u16 SavLGPE::playedHours(void) const
{
    return *(u16*)(data + 0x45400);
}

void SavLGPE::playedHours(u16 v)
{
    *(u16*)(data + 0x45400) = v;
}

u8 SavLGPE::playedMinutes(void) const
{
    return *(data + 0x45402);
}

void SavLGPE::playedMinutes(u8 v)
{
    *(data + 0x45402) = v;
}

u8 SavLGPE::playedSeconds(void) const
{
    return *(data + 0x45403);
}

void SavLGPE::playedSeconds(u8 v)
{
    *(data + 0x45403) = v;
}
    
std::unique_ptr<PKX> SavLGPE::pkm(u8 slot) const
{
    u32 off = partyOffset(slot);
    if (off != 0)
    {
        return std::make_unique<PB7>(data + off);
    }
    else
    {
        return emptyPkm()->clone();
    }
}

std::unique_ptr<PKX> SavLGPE::pkm(u8 box, u8 slot, bool ekx) const
{
    return std::make_unique<PB7>(data + boxOffset(box, slot), ekx);
}

void SavLGPE::pkm(PKX& pk, u8 box, u8 slot)
{
    PB7* pb7 = (PB7*)&pk;
    std::copy(pb7->data, pb7->data + pb7->length, data + boxOffset(box, slot));
}

void SavLGPE::pkm(PKX& pk, u8 slot)
{
    PB7* pb7 = (PB7*)&pk;
    u32 off = partyOffset(slot);
    u16 newSlot = partyBoxSlot(slot);
    if (off == 0)
    {
        for (int i = 999; i >= 0; i--)
        {
            if (!isPKM(data + 0x5C00 + i * 260))
            {
                off = boxOffset(i / 30, i % 30);
                newSlot = i;
                break;
            }
        }
        if (off == 0)
        {
            Gui::warn("No empty slot found");
            return;
        }
    }

    std::copy(pb7->data, pb7->data + pb7->length, data + off);
    partyBoxSlot(slot, newSlot);
}

std::shared_ptr<PKX> SavLGPE::emptyPkm() const
{
    static auto empty = std::make_shared<PB7>();
    return empty;
}

std::string SavLGPE::boxName(u8 box) const
{
    return "Box " + std::to_string((int)box + 1);
}

void SavLGPE::boxName(u8 box, std::string name)
{
    (void) box, (void) name;
}

int SavLGPE::dexFormCount(int species) const
{
    for (int i = 0; i < 62; i += 2)
    {
        if (formtable[i] == species)
        {
            return formtable[i + 1];
        }
        else if (formtable[i] > species)
        {
            return 0;
        }
    }
    return 0;
}

int SavLGPE::dexFormIndex(int species, int formct, int start) const
{
    int formindex = start;
    int f = 0;
    for (int i = 0; i < 62; i += 2)
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

bool SavLGPE::sanitizeFormsToIterate(int species, int& fs, int& fe, int formIn) const
{
    switch (species)
    {
        case 20: // Raticate
        case 105: // Marowak
            fs = 0;
            fe = 1;
            return true;
        default:
            int count = dexFormCount(species);
            fs = fe = 0;
            return count < formIn;
    }
}

void SavLGPE::setDexFlags(int index, int gender, int shiny, int baseSpecies)
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

void SavLGPE::dex(PKX& pk)
{
    int n = pk.species();
    int MaxSpeciesID = 809;
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
            u8 fc = dexFormCount(n); // TODO: PersonalLGPE::formCount(n);
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

void SavLGPE::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < maxBoxes(); box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            if (box * 30 + slot > 1000)
            {
                return;
            }
            auto pb7 = pkm(box, slot, crypted);
            if (!crypted)
            {
                pb7->encrypt();
            }
            pkm(*pb7, box, slot);
        }
    }
}

void SavLGPE::mysteryGift(WCX& wc, int& pos)
{
    WB7* wb7 = (WB7*)&wc;
    if (wb7->pokemon())
    {
        if (boxedPkm() == maxSlot())
        {
            Gui::warn("Too many Pok\u00E9mon", "Cannot inject");
            return;
        }
        PB7 pkm;
        pkm.species(wb7->species());
        pkm.alternativeForm(wb7->alternativeForm());
        if (wb7->level() > 0)
        {
            pkm.level(wb7->level());
            pkm.partyLevel(wb7->level());
        }
        else
        {
            pkm.level(rand() % 100 + 1);
            pkm.partyLevel(pkm.level());
        }
        if (wb7->metLevel() > 0)
        {
            pkm.metLevel(wb7->metLevel());
        }
        else
        {
            pkm.metLevel(pkm.level());
        }
        pkm.TID(wb7->TID());
        pkm.SID(wb7->SID());
        for (int i = 0; i < 4; i++)
        {
            pkm.move(i, wb7->move(i));
            pkm.relearnMove(i, wb7->move(i));
        }
        if (wb7->nature() == 255)
        {
            pkm.nature(rand() % 25);
        }
        else
        {
            pkm.nature(wb7->nature());
        }
        if (wb7->gender() == 3)
        {
            pkm.gender(rand() % 3);
        }
        else
        {
            pkm.gender(wb7->gender());
        }
        pkm.heldItem(wb7->heldItem());
        pkm.encryptionConstant(wb7->encryptionConstant());
        if (wb7->version() == 0)
        {
            pkm.version(wb7->version());
        }
        else
        {
            pkm.version(version());
        }
        pkm.language(language());
        pkm.ball(wb7->ball());
        pkm.country(country());
        pkm.region(subRegion());
        pkm.consoleRegion(consoleRegion());
        pkm.metLocation(wb7->metLocation());
        pkm.eggLocation(wb7->eggLocation());
        for (int i = 0; i < 6; i++)
        {
            pkm.awakened(i, wb7->awakened(i));
            pkm.ev(i, wb7->ev(i));
        }
        if (wb7->nickname((Language)language()).length() == 0)
        {
            pkm.nickname(i18n::species(language(), pkm.species()).c_str());
        }
        else
        {
            pkm.nickname(wb7->nickname((Language)language()).c_str());
            pkm.nicknamed(pkm.nickname() != i18n::species(language(), pkm.species()));
        }
        if (wb7->otName((Language)language()).length() == 0)
        {
            pkm.otName(otName().c_str());
            pkm.otGender(gender());
            pkm.currentHandler(0);
        }
        else
        {
            pkm.otName(wb7->otName((Language)language()).c_str());
            pkm.htName(otName().c_str());
            pkm.otGender(wb7->otGender());
            pkm.htGender(gender());
            pkm.otFriendship(PersonalSMUSUM::baseFriendship(pkm.formSpecies())); // TODO: PersonalLGPE
            pkm.currentHandler(1);
        }

        int perfectIVs = 0;
        for (int i = 0; i < 6; i++)
        {
            pkm.iv(rand() % 30 + 1); // Initialize IVs so that none are perfect (though they can be close)
            if (wb7->iv(i) - 0xFC < 3)
            {
                perfectIVs = wb7->iv(i) - 0xFB; // How many perfects should there be?
                break;
            }
        }
        if (perfectIVs > 0)
        {
            for (int i = 0; i < perfectIVs; i++)
            {
                u8 chosenIV;
                do {
                    chosenIV = rand() % 6;
                }
                while (pkm.iv(chosenIV) == 31);
                pkm.iv(chosenIV, 31);
            }
            for (int i = 0; i < 6; i++)
            {
                if (pkm.iv(i) != 31)
                {
                    pkm.iv(i, rand() % 32);
                }
            }
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                pkm.iv(i, rand() % 32);
            }
        }

        if (wb7->otGender() == 3)
        {
            pkm.TID(TID());
            pkm.SID(SID());
        }

        // Sets the ability to the one specific to the formSpecies and sets abilitynumber (Why? Don't quite understand that)
        switch (wb7->abilityType())
        {
            case 0:
            case 1:
            case 2:
                pkm.ability(wb7->abilityType());
                break;
            case 3:
            case 4:
                pkm.ability(rand() % (wb7->abilityType() - 1));
                break;
        }

        switch (wb7->PIDType())
        {
            case 0: // Fixed value
                pkm.PID(wb7->PID());
                break;
            case 1: // Random
                pkm.PID((u32)rand());
                break;
            case 2: // Always shiny
                pkm.PID((u32)rand());
                pkm.shiny(true);
                break;
            case 3: // Never shiny
                pkm.PID((u32)rand());
                pkm.shiny(false);
                break;
        }

        if (wb7->egg())
        {
            pkm.egg(true);
            pkm.eggYear(wb7->year());
            pkm.eggMonth(wb7->month());
            pkm.eggDay(wb7->day());
            pkm.nickname(i18n::species(language(), pkm.species()).c_str());
            pkm.nicknamed(true);
        }

        pkm.metDay(wb7->day());
        pkm.metMonth(wb7->month());
        pkm.metYear(wb7->year());
        pkm.currentFriendship(PersonalSMUSUM::baseFriendship(pkm.formSpecies())); // TODO: PersonalLGPE

        pkm.partyCP(pkm.CP());
        pkm.partyCurrHP(pkm.stat(0));
        for (int i = 0; i < 6; i++)
        {
            pkm.partyStat(pkm.stat(i));
        }
        
        pkm.height(rand() % 256);
        pkm.weight(rand() % 256);
        pkm.fatefulEncounter(true);

        pkm.refreshChecksum();
        SavLGPE::pkm(pkm, boxedPkm()); // qualify so there are no stupid errors
        boxedPkm(this->boxedPkm() + 1);
    }
    else if (wb7->item())
    {
        for (int itemNum = 0; itemNum < wb7->items(); itemNum++)
        {
            Pouch place = NormalItem;
            int slot = -1;
            static constexpr Pouch search[] = { NormalItem, TM, Medicine, Candy, ZCrystals, Ball, Battle };
            static constexpr int limits[] =   { 150,        108,60,       200,   150,       50,   150 };
            for (int i = 0; i < 7; i++)
            {
                for (int j = 0; j < limits[i]; j++)
                {
                    auto find = item(search[i], j);
                    if (!find)
                    {
                        break; // End of item list
                    }
                    if (((Item7b*)find.get())->id == wb7->object(itemNum))
                    {
                        slot = j;
                        place = search[i];
                        break;
                    }
                }
                if (slot != -1)
                {
                    break;
                }
            }

            if (slot != -1)
            {
                Item7b* inject = (Item7b*)item(place, slot).release();
                inject->count += wb7->objectQuantity(itemNum);
                item(*inject, place, slot);
                delete inject;
            }
            else
            {
                Item7b inject;
                inject.id = wb7->object(itemNum);
                inject.count = wb7->objectQuantity(itemNum);
                inject.newFlag = 1;
                if (inject.id >= 960) // Start of candies
                {
                    for (int i = 0; i < 200; i++)
                    {
                        if (!item(Candy, i)) // If slot is empty
                        {
                            item(inject, Candy, i);
                            return;
                        }
                    }
                }
                else // Ugh, I don't wanna implement this right now
                {
                    item(inject, NormalItem, 0); // TODO: make this less bad
                }
            }
        }
    }
    {
        Gui::warn("This is icky and currently unimplemented.", "Requires dumb stuff to happen");
    }
}

void SavLGPE::item(Item& item, Pouch pouch, u16 slot)
{
    switch (pouch)
    {
        case Pouch::Medicine:
            if (slot < 60)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (Medicine LGPE)");
            }
            break;
        case Pouch::TM:
            if (slot < 108)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + 0xF0 + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (TM LGPE)");
            }
            break;
        case Pouch::Candy:
            if (slot < 200)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + 0x2A0 + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (Candy LGPE)");
            }
            break;
        case Pouch::ZCrystals:
            if (slot < 150)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + 0x5C0 + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (ZCrystals LGPE)");
            }
            break;
        case Pouch::Ball:
            if (slot < 50)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + 0x818 + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (Ball LGPE)");
            }
            break;
        case Pouch::Battle:
            if (slot < 150)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + 0x8E0 + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (Battle LGPE)");
            }
            break;
        case Pouch::KeyItem:
        case Pouch::NormalItem:
            if (slot < 150)
            {
                std::copy((u8*)(&item), (u8*)(&item) + 4, data + 0xB38 + slot * 4);
            }
            else
            {
                Gui::warn("Why the fuck is this happenening?", "Please report this! (Normal/Key LGPE)");
            }
            break;
        default:
            Gui::warn("Something very, very weird happened.", "Please report where this happened.");
            break;
    }
}

std::unique_ptr<Item> SavLGPE::item(Pouch pouch, u16 slot) const
{
    Item7b* ret = new Item7b;
    switch (pouch)
    {
        case Pouch::Medicine:
            std::copy(data + slot * 4, data + (slot + 1) * 4, (u8*) ret);
            break;
        case Pouch::TM:
            std::copy(data + 0xF0 + slot * 4, data + 0xF0 + (slot + 1) * 4, (u8*) ret);
            break;
        case Pouch::Candy:
            std::copy(data + 0x2A0 + slot * 4, data + 0x2A0 + (slot + 1) * 4, (u8*) ret);
            break;
        case Pouch::ZCrystals:
            std::copy(data + 0x5C0 + slot * 4, data + 0x5C0 + (slot + 1) * 4, (u8*) ret);
            break;
        case Pouch::Ball:
            std::copy(data + 0x818 + slot * 4, data + 0x818 + (slot + 1) * 4, (u8*) ret);
            break;
        case Pouch::Battle:
            std::copy(data + 0x8E0 + slot * 4, data + 0x8E0 + (slot + 1) * 4, (u8*) ret);
            break;
        case Pouch::KeyItem:
        case Pouch::NormalItem:
            std::copy(data + 0xB38 + slot * 4, data + 0xB38 + (slot + 1) * 4, (u8*) ret);
            break;
        default:
            ret = nullptr;
            break;
    }
    if (ret && (ret->id == 0 || ret->count == 0))
    {
        ret = nullptr;
    }
    
    return std::unique_ptr<Item>(ret);
}