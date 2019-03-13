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

#ifndef SAVBW_HPP
#define SAVBW_HPP

#include "personal.hpp"
#include "Sav.hpp"
#include "PK5.hpp"
#include "PGF.hpp"

class SavBW : public Sav
{
protected:
    static constexpr u16 lengths[70] = {
        0x03E0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0,
        0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0,
        0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0,
        0x0FF0, 0x0FF0, 0x0FF0, 0x0FF0, 0x09C0, 0x0534, 0x0068,
        0x009C, 0x1338, 0x07C4, 0x0D54, 0x002C, 0x0658, 0x0A94,
        0x01AC, 0x03EC, 0x005C, 0x01E0, 0x00A8, 0x0460, 0x1400,
        0x02A4, 0x02DC, 0x034C, 0x03EC, 0x00F8, 0x02FC, 0x0094,
        0x035C, 0x01CC, 0x0168, 0x00EC, 0x01B0, 0x001C, 0x04D4,
        0x0034, 0x003C, 0x01AC, 0x0B90, 0x009C, 0x0850, 0x0028,
        0x0284, 0x0010, 0x005C, 0x016C, 0x0040, 0x00FC, 0x008C
    };

    static constexpr u32 chkofs[70] = {
        0x003E2, 0x013F2, 0x023F2, 0x033F2, 0x043F2, 0x053F2, 0x063F2,
        0x073F2, 0x083F2, 0x093F2, 0x0A3F2, 0x0B3F2, 0x0C3F2, 0x0D3F2,
        0x0E3F2, 0x0F3F2, 0x103F2, 0x113F2, 0x123F2, 0x133F2, 0x143F2,
        0x153F2, 0x163F2, 0x173F2, 0x183F2, 0x18DC2, 0x19336, 0x1946A,
        0x1959E, 0x1A93A, 0x1B1C6, 0x1BF56, 0x1C02E, 0x1C75A, 0x1D296,
        0x1D4AE, 0x1D8EE, 0x1D95E, 0x1DBE2, 0x1DCAA, 0x1E162, 0x1F602,
        0x1F9A6, 0x1FCDE, 0x2004E, 0x204EE, 0x205FA, 0x208FE, 0x20996,
        0x20D5E, 0x20FCE, 0x2116A, 0x212EE, 0x214B2, 0x2151E, 0x21AD6,
        0x21B36, 0x21C3E, 0x21EAE, 0x22A92, 0x22B9E, 0x23452, 0x2352A,
        0x23886, 0x23912, 0x23A5E, 0x23C6E, 0x23D42, 0x23EFE, 0x23F9A
    };

    static constexpr u32 blockOfs[70] = {
        0x00000, 0x00400, 0x01400, 0x02400, 0x03400, 0x04400, 0x05400,
        0x06400, 0x07400, 0x08400, 0x09400, 0x0A400, 0x0B400, 0x0C400,
        0x0D400, 0x0E400, 0x0F400, 0x10400, 0x11400, 0x12400, 0x13400,
        0x14400, 0x15400, 0x16400, 0x17400, 0x18400, 0x18E00, 0x19400,
        0x19500, 0x19600, 0x1AA00, 0x1B200, 0x1C000, 0x1C100, 0x1C800,
        0x1D300, 0x1D500, 0x1D900, 0x1DA00, 0x1DC00, 0x1DD00, 0x1E200,
        0x1F700, 0x1FA00, 0x1FD00, 0x20100, 0x20500, 0x20600, 0x20900,
        0x20A00, 0x20E00, 0x21000, 0x21200, 0x21300, 0x21500, 0x21600,
        0x21B00, 0x21C00, 0x21D00, 0x21F00, 0x22B00, 0x22C00, 0x23500,
        0x23600, 0x23900, 0x23A00, 0x23B00, 0x23D00, 0x23E00, 0x23F00
    };

    static constexpr u32 chkMirror[70] = {
        0x23F00, 0x23F02, 0x23F04, 0x23F06, 0x23F08, 0x23F0A, 0x23F0C,
        0x23F0E, 0x23F10, 0x23F12, 0x23F14, 0x23F16, 0x23F18, 0x23F1A,
        0x23F1C, 0x23F1E, 0x23F20, 0x23F22, 0x23F24, 0x23F26, 0x23F28,
        0x23F2A, 0x23F2C, 0x23F2E, 0x23F30, 0x23F32, 0x23F34, 0x23F36,
        0x23F38, 0x23F3A, 0x23F3C, 0x23F3E, 0x23F40, 0x23F42, 0x23F44,
        0x23F46, 0x23F48, 0x23F4A, 0x23F4C, 0x23F4E, 0x23F50, 0x23F52,
        0x23F54, 0x23F56, 0x23F58, 0x23F5A, 0x23F5C, 0x23F5E, 0x23F60,
        0x23F62, 0x23F64, 0x23F66, 0x23F68, 0x23F6A, 0x23F6C, 0x23F6E,
        0x23F70, 0x23F72, 0x23F74, 0x23F76, 0x23F78, 0x23F7A, 0x23F7C,
        0x23F7E, 0x23F80, 0x23F82, 0x23F84, 0x23F86, 0x23F88, 0x23F9A
    };

    int dexFormIndex(int species, int formct) const;
public:
    SavBW(u8* dt);
    virtual ~SavBW() { };

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
    void cryptMysteryGiftData(void);
    std::string boxName(u8 box) const override;
    void boxName(u8 box, std::string name) override;
    u8 partyCount(void) const override;
    void partyCount(u8 count) override;

    int maxBoxes(void) const override { return 24; }
    size_t maxWondercards(void) const override { return 12; }
    Generation generation(void) const override { return Generation::FIVE; }
    int maxSpecies(void) const { return 649; }
    int maxMove(void) const { return 559; }
    int maxItem(void) const { return 632; }
    int maxAbility(void) const { return 164; }
    int maxBall(void) const { return 0x19; }

    void item(Item& item, Pouch pouch, u16 slot) override;
    std::unique_ptr<Item> item(Pouch pouch, u16 slot) const override;
    std::vector<std::pair<Pouch, int>> pouches(void) const override;
    std::map<Pouch, std::vector<int>> validItems(void) const override;
    std::string pouchName(Pouch pouch) const override;

    u8 formCount(u16 species) const override { return PersonalBWB2W2::formCount(species); }
};

#endif
