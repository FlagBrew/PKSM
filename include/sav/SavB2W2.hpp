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

#ifndef SAVB2W2_HPP
#define SAVB2W2_HPP

#include "personal.hpp"
#include "Sav.hpp"
#include "PK5.hpp"
#include "PGF.hpp"

class SavB2W2 : public Sav
{
protected:
    static constexpr u16 lengths[74] = {
        0x03e0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0,
        0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0,
        0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0,
        0x0ff0, 0x0ff0, 0x0ff0, 0x0ff0, 0x09ec, 0x0534, 0x00b0,
        0x00a8, 0x1338, 0x07c4, 0x0d54, 0x0094, 0x0658, 0x0a94,
        0x01ac, 0x03ec, 0x005c, 0x01e0, 0x00a8, 0x0460, 0x1400,
        0x02a4, 0x00e0, 0x034c, 0x04e0, 0x00f8, 0x02fc, 0x0094,
        0x035c, 0x01d4, 0x01e0, 0x00f0, 0x01b4, 0x04dc, 0x0034,
        0x003c, 0x01ac, 0x0b90, 0x00ac, 0x0850, 0x0284, 0x0010,
        0x00a8, 0x016c, 0x0080, 0x00fc, 0x16a8, 0x0498, 0x0060,
        0x00fc, 0x03e4, 0x00f0, 0x0094
    };

    static constexpr u32 chkofs[74] = {
        0x003E2, 0x013F2, 0x023F2, 0x033F2, 0x043F2, 0x053F2, 0x063F2,
        0x073F2, 0x083F2, 0x093F2, 0x0A3F2, 0x0B3F2, 0x0C3F2, 0x0D3F2,
        0x0E3F2, 0x0F3F2, 0x103F2, 0x113F2, 0x123F2, 0x133F2, 0x143F2,
        0x153F2, 0x163F2, 0x173F2, 0x183F2, 0x18DEE, 0x19336, 0x194B2,
        0x195AA, 0x1A93A, 0x1B1C6, 0x1BF56, 0x1C096, 0x1C75A, 0x1D296,
        0x1D4AE, 0x1D8EE, 0x1D95E, 0x1DBE2, 0x1DCAA, 0x1E162, 0x1F602,
        0x1F9A6, 0x1FAE2, 0x1FE4E, 0x203E2, 0x204FA, 0x207FE, 0x20896,
        0x20C5E, 0x20ED6, 0x210E2, 0x211F2, 0x213B6, 0x218DE, 0x21936,
        0x21A3E, 0x21CAE, 0x22892, 0x229AE, 0x23252, 0x23586, 0x23612,
        0x237AA, 0x2396E, 0x23A82, 0x23BFE, 0x252AA, 0x2579A, 0x25862,
        0x259FE, 0x25DE6, 0x25EF2, 0x25FA2
    };

    static constexpr u32 blockOfs[74] = {
        0x00000, 0x00400, 0x01400, 0x02400, 0x03400, 0x04400, 0x05400,
        0x06400, 0x07400, 0x08400, 0x09400, 0x0A400, 0x0B400, 0x0C400,
        0x0D400, 0x0E400, 0x0F400, 0x10400, 0x11400, 0x12400, 0x13400,
        0x14400, 0x15400, 0x16400, 0x17400, 0x18400, 0x18E00, 0x19400,
        0x19500, 0x19600, 0x1AA00, 0x1B200, 0x1C000, 0x1C100, 0x1C800,
        0x1D300, 0x1D500, 0x1D900, 0x1DA00, 0x1DC00, 0x1DD00, 0x1E200,
        0x1F700, 0x1FA00, 0x1FB00, 0x1FF00, 0x20400, 0x20500, 0x20800,
        0x20900, 0x20D00, 0x20F00, 0x21100, 0x21200, 0x21400, 0x21900,
        0x21A00, 0x21B00, 0x21D00, 0x22900, 0x22A00, 0x23300, 0x23600,
        0x23700, 0x23800, 0x23A00, 0x23B00, 0x23C00, 0x25300, 0x25800,
        0x25900, 0x25A00, 0x25E00, 0x25F00
    };

    static constexpr u32 chkMirror[74] = {
        0x25F00, 0x25F02, 0x25F04, 0x25F06, 0x25F08, 0x25F0A, 0x25F0C,
        0x25F0E, 0x25F10, 0x25F12, 0x25F14, 0x25F16, 0x25F18, 0x25F1A,
        0x25F1C, 0x25F1E, 0x25F20, 0x25F22, 0x25F24, 0x25F26, 0x25F28,
        0x25F2A, 0x25F2C, 0x25F2E, 0x25F30, 0x25F32, 0x25F34, 0x25F36,
        0x25F38, 0x25F3A, 0x25F3C, 0x25F3E, 0x25F40, 0x25F42, 0x25F44,
        0x25F46, 0x25F48, 0x25F4A, 0x25F4C, 0x25F4E, 0x25F50, 0x25F52,
        0x25F54, 0x25F56, 0x25F58, 0x25F5A, 0x25F5C, 0x25F5E, 0x25F60,
        0x25F62, 0x25F64, 0x25F66, 0x25F68, 0x25F6A, 0x25F6C, 0x25F6E,
        0x25F70, 0x25F72, 0x25F74, 0x25F76, 0x25F78, 0x25F7A, 0x25F7C,
        0x25F7E, 0x25F80, 0x25F82, 0x25F84, 0x25F86, 0x25F88, 0x25F8A,
        0x25F8C, 0x25F8E, 0x25F90, 0x25FA2
    };

    int dexFormIndex(int species, int formct) const;
public:
    SavB2W2(u8* dt);
    virtual ~SavB2W2() { };

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
    void cryptBoxData(bool crypted) override;
    void cryptMysteryGiftData(void);
    std::string boxName(u8 box) const override;
    void boxName(u8 box, std::string name) override;
    u8 partyCount(void) const override;

    int maxBoxes(void) const override { return 24; }
    size_t maxWondercards(void) const override { return 12; }
    Generation generation(void) const override { return Generation::FIVE; }
    int maxSpecies(void) const { return 649; }
    int maxMove(void) const { return 559; }
    int maxItem(void) const { return 638; }
    int maxAbility(void) const { return 164; }
    int maxBall(void) const { return 0x19; }
};

#endif