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

#ifndef SAVXY_HPP
#define SAVXY_HPP

#include "personal.hpp"
#include "Sav.hpp"
#include "PK6.hpp"
#include "WC6.hpp"

class SavXY : public Sav
{
protected:
    static constexpr u32 chkofs[55] = {
        0x00000, 0x00400, 0x01000, 0x01200, 0x01400, 0x01600, 0x01800,
        0x01A00, 0x01C00, 0x01E00, 0x02000, 0x04200, 0x04400, 0x04A00,
        0x05000, 0x0A000, 0x0F000, 0x14000, 0x14200, 0x14A00, 0x15000,
        0x15800, 0x16000, 0x16200, 0x16400, 0x16A00, 0x16C00, 0x17000,
        0x17800, 0x17A00, 0x17C00, 0x17E00, 0x18000, 0x18200, 0x18A00,
        0x19000, 0x19400, 0x1B000, 0x1B200, 0x1B400, 0x1B800, 0x1BC00,
        0x1D800, 0x1DC00, 0x1E400, 0x1E800, 0x1F200, 0x1F600, 0x1FE00,
        0x20C00, 0x20E00, 0x21000, 0x21E00, 0x22600, 0x57200
    };

    static constexpr u32 chklen[55] = {
        0x002C8, 0x00B88, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 
        0x001C0, 0x000BE, 0x00024, 0x02100, 0x00140, 0x00440, 0x00574, 
        0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x006A0,
        0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 
        0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 
        0x002F8, 0x01B40, 0x001F4, 0x001F0, 0x00216, 0x00390, 0x01A90, 
        0x00308, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 
        0x00078, 0x00200, 0x00C84, 0x00628, 0x34AD0, 0x0E058
    };

    int dexFormIndex(int species, int formct) const;
public:
    SavXY(u8* dt);
    virtual ~SavXY() { };

    void resign(void) override;

    u16 TID(void) const override;
    void TID(u16 v) override;
    u16 SID(void) const override;
    void SID(u16 v) override;
    u8 version(void) const override;
    void version(u8 v) override;
    u8 gender(void) const override;
    void gender(u8 v) override;
    u8 subRegion(void) const override;
    void subRegion(u8 v) override;
    u8 country(void) const override;
    void country(u8 v) override;
    u8 consoleRegion(void) const override;
    void consoleRegion(u8 v) override;
    u8 language(void) const override;
    void language(u8 v) override;
    std::string otName(void) const override;
    void otName(const char* v) override;
    u32 money(void) const override;
    void money(u32 v) override;
    u32 BP(void) const override;
    void BP(u32 v) override;
    u16 playedHours(void) const override;
    void playedHours(u16 v) override;
    u8 playedMinutes(void) const override;
    void playedMinutes(u8 v) override;
    u8 playedSeconds(void) const override;
    void playedSeconds(u8 v) override;

    u8 currentBox(void) const override;
    void currentBox(u8 v) override;
    u32 boxOffset(u8 box, u8 slot) const override;
    u32 partyOffset(u8 slot) const override;
    
    std::unique_ptr<PKX> pkm(u8 slot) const override;
    std::unique_ptr<PKX> pkm(u8 box, u8 slot, bool ekx = false) const override;

    // NOTICE: this sets a pkx into the savefile, not a pkx
    // that's because PKSM works with decrypted boxes and
    // crypts them back during resigning
    void pkm(PKX& pk, u8 box, u8 slot) override;
    void pkm(PKX& pk, u8 slot) override;

    std::shared_ptr<PKX> emptyPkm() const override;

    void dex(PKX& pk) override;
    int emptyGiftLocation(void) const override;
    std::vector<MysteryGift::giftData> currentGifts(void) const override;
    void mysteryGift(WCX& wc, int& pos) override;
    std::unique_ptr<WCX> mysteryGift(int pos) const override;
    void cryptBoxData(bool crypted) override;
    std::string boxName(u8 box) const override;
    void boxName(u8 box, std::string name) override;
    u8 partyCount(void) const override;
    void partyCount(u8 count) override;

    int maxBoxes(void) const override { return 31; }
    size_t maxWondercards(void) const override { return 24; }
    Generation generation(void) const override { return Generation::SIX; }
    int maxSpecies(void) const { return 721; }
    int maxMove(void) const { return 617; }
    int maxItem(void) const { return 717; }
    int maxAbility(void) const { return 188; }
    int maxBall(void) const { return 0x19; }

    void item(Item& item, Pouch pouch, u16 slot) override;
    std::unique_ptr<Item> item(Pouch pouch, u16 slot) const override;
    std::vector<std::pair<Pouch, int>> pouches(void) const override;
    std::map<Pouch, std::vector<int>> validItems(void) const override;
    std::string pouchName(Pouch pouch) const override;
};

#endif