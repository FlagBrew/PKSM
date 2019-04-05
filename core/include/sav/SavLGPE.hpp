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

#ifndef SAVLGPE_HPP
#define SAVLGPE_HPP

#include "Sav.hpp"

class SavLGPE : public Sav
{
protected:
    static constexpr u32 chkofs[21] = { 
        0x00000, 0x00E00, 0x01000, 0x01200, 0x02A00, 0x04C00, 0x05600,
        0x05800, 0x05A00, 0x05C00, 0x45400, 0x45600, 0x46600, 0x47800,
        0x47A00, 0x4DC00, 0x4DE00, 0x4E000, 0x4E200, 0xB7A00, 0xB7C00
    };

    static constexpr u32 chklen[21] = { 
        0xD90, 0x200, 0x168, 0x1800, 0x20E8, 0x930, 0x004, 
        0x130, 0x012, 0x3F7A0, 0x008, 0xE90, 0x10A4, 0x0F0, 
        0x6010, 0x200, 0x098, 0x068, 0x69780, 0x0B0, 0x940
    };

    static constexpr u16 formtable[62] = {
        3, 2, 6, 3, 9, 2, 15, 2, 18, 2, 19, 2,
        20, 2, 26, 2, 27, 2, 28, 2, 37, 2, 38, 2,
        50, 2, 51, 2, 52, 2, 53, 2, 65, 2, 74, 2,
        75, 2, 76, 2, 80, 2, 88, 2, 89, 2, 94, 2,
        103, 2, 105, 2, 115, 2, 127, 2, 130, 2,
        142, 2, 150, 3
    };

    int dexFormIndex(int species, int formct, int start) const;
    int dexFormCount(int species) const;
    void setDexFlags(int index, int gender, int shiny, int baseSpecies);
    bool sanitizeFormsToIterate(int species, int& fs, int& fe, int formIn) const;

public:
    SavLGPE(u8* dt);
    ~SavLGPE();

    u16 check16(u8* buf, u32 blockID, u32 len) const;
    void resign(void) override;

    u16 boxedPkm(void) const;
    void boxedPkm(u16 v);
    u16 followPkm(void) const;
    void followPkm(u16 v);
    u16 partyBoxSlot(u8 slot) const;
    void partyBoxSlot(u8 slot, u16 v);
    void compressBox(void);
    void fixParty(void) override;

    u16 TID(void) const override;
    void TID(u16 v) override;
    u16 SID(void) const override;
    void SID(u16 v) override;
    u8 version(void) const override;
    void version(u8 v) override;
    u8 gender(void) const override;
    void gender(u8 v) override;
    u8 subRegion(void) const override { return 0; } // TODO
    void subRegion(u8 v) override { (void) v; } // TODO
    u8 country(void) const override { return 0; } // TODO
    void country(u8 v) override { (void) v; } // TODO
    u8 consoleRegion(void) const override { return 0; } // TODO
    void consoleRegion(u8 v) override { (void) v; } // TODO
    u8 language(void) const override;
    void language(u8 v) override;
    std::string otName(void) const override;
    void otName(const std::string& v) override;
    u32 money(void) const override;
    void money(u32 v) override;
    u32 BP(void) const override { return 0; } // TODO
    void BP(u32 v) override { (void) v; } // TODO
    u8 badges(void) const override;
    u16 playedHours(void) const override;
    void playedHours(u16 v) override;
    u8 playedMinutes(void) const override;
    void playedMinutes(u8 v) override;
    u8 playedSeconds(void) const override;
    void playedSeconds(u8 v) override;

    u8 currentBox(void) const override { return 0; } // TODO
    void currentBox(u8 v) override { (void) v; } // TODO
    u32 boxOffset(u8 box, u8 slot) const override;
    u32 partyOffset(u8 slot) const override;
    
    // Will never be encrypted: part of normal box stuff
    std::shared_ptr<PKX> pkm(u8 slot) const override;
    std::shared_ptr<PKX> pkm(u8 box, u8 slot, bool ekx = false) const override;

    // NOTICE: this sets a pkx into the savefile, not a pkx
    // that's because PKSM works with decrypted boxes and
    // crypts them back during resigning
    void pkm(std::shared_ptr<PKX> pk, u8 box, u8 slot, bool applyTrade) override;
    void pkm(std::shared_ptr<PKX> pk, u8 slot) override;

    void trade(std::shared_ptr<PKX> pk) override;
    std::shared_ptr<PKX> emptyPkm() const override;

    void dex(std::shared_ptr<PKX> pk) override;
    int emptyGiftLocation(void) const override { return 0; } // Data not stored
    std::vector<MysteryGift::giftData> currentGifts(void) const override { return {}; } // Data not stored
    void mysteryGift(WCX& wc, int& pos) override;
    std::unique_ptr<WCX> mysteryGift(int pos) const override;
    void cryptBoxData(bool crypted) override;
    std::string boxName(u8 box) const override;
    void boxName(u8 box, const std::string& name) override;
    u8 partyCount(void) const override;
    void partyCount(u8 count) override;

    int maxSlot(void) const override { return 1000; }
    int maxBoxes(void) const override { return 34; } // ish; stupid 1000-slot box makes this dumb
    size_t maxWondercards(void) const override { return 1; } // Data not stored
    Generation generation(void) const override { return Generation::LGPE; }
    int maxSpecies(void) const { return 809; } // This is going to be FUN: only numbers 1-151, 808, & 809
    int maxMove(void) const { return 742; }
    int maxItem(void) const { return 1057; }
    int maxAbility(void) const { return 233; } // Same as G7
    int maxBall(void) const { return 0x1A; } // Same as G7

    void item(Item& item, Pouch pouch, u16 slot) override;
    std::unique_ptr<Item> item(Pouch pouch, u16 slot) const override;
    std::vector<std::pair<Pouch, int>> pouches(void) const override;
    std::map<Pouch, std::vector<int>> validItems(void) const override;
    std::string pouchName(Pouch pouch) const override;

    u8 formCount(u16 species) const override { return PersonalLGPE::formCount(species); }
};

#endif
