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

#ifndef SAV_HPP
#define SAV_HPP

#include <memory>
#include <stdint.h>
#include "PKX.hpp"
#include "WCX.hpp"
#include "utils.hpp"
#include "mysterygift.hpp"
#include "generation.hpp"
#include "game.hpp"
#include "Item.hpp"
#include "i18n.hpp"

namespace TitleLoader {
    void backupSave();
    void saveChanges();
    void saveToTitle(bool ask);
}

enum Pouch
{
    NormalItem,
    KeyItem,
    TM,
    Mail,
    Medicine,
    Berry,
    Ball,
    Battle,
    Candy,
    ZCrystals
};

class Sav
{
protected:
    int Box, Party, PokeDex, WondercardData, WondercardFlags;
    int PouchHeldItem, PouchKeyItem, PouchTMHM, PouchMedicine, PouchBerry;

    static constexpr u16 crc16[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };
    
    u8* data;
    u32 length = 0;
    Game game;
    static u16 ccitt16(const u8* buf, u32 len);
    static std::unique_ptr<Sav> checkDSType(u8* dt);
    static bool validSequence(u8* dt, u8* pattern, int shift = 0);

public:
    u8 boxes = 0;

    virtual ~Sav();
    virtual void resign(void) = 0;

    static bool isValidDSSave(u8* dt);
    static std::unique_ptr<Sav> getSave(u8* dt, size_t length);

    virtual u16 TID(void) const = 0;
    virtual void TID(u16 v) = 0;
    virtual u16 SID(void) const = 0;
    virtual void SID(u16 v) = 0;
    virtual u8 version(void) const = 0;
    virtual void version(u8 v) = 0;
    virtual u8 gender(void) const = 0;
    virtual void gender(u8 v) = 0;
    virtual u8 subRegion(void) const = 0;
    virtual void subRegion(u8 v) = 0;
    virtual u8 country(void) const = 0;
    virtual void country(u8 v) = 0;
    virtual u8 consoleRegion(void) const = 0;
    virtual void consoleRegion(u8 v) = 0;
    virtual u8 language(void) const = 0;
    virtual void language(u8 v) = 0;
    virtual std::string otName(void) const = 0;
    virtual void otName(const std::string& v) = 0;
    virtual u32 money(void) const = 0;
    virtual void money(u32 v) = 0;
    virtual u32 BP(void) const = 0;
    virtual void BP(u32 v) = 0;
    virtual u8 badges(void) const = 0;
    virtual u16 playedHours(void) const = 0;
    virtual void playedHours(u16 v) = 0;
    virtual u8 playedMinutes(void) const = 0;
    virtual void playedMinutes(u8 v) = 0;
    virtual u8 playedSeconds(void) const = 0;
    virtual void playedSeconds(u8 v) = 0;

    u32 displayTID(void) const;
    u32 displaySID(void) const;

    virtual u8 currentBox(void) const = 0;
    virtual void currentBox(u8 v) = 0;
    virtual u32 boxOffset(u8 box, u8 slot) const = 0;
    virtual u32 partyOffset(u8 slot) const = 0;
    
    virtual std::shared_ptr<PKX> pkm(u8 slot) const = 0;
    virtual void pkm(std::shared_ptr<PKX> pk, u8 slot) = 0;
    virtual std::shared_ptr<PKX> pkm(u8 box, u8 slot, bool ekx = false) const = 0;
    virtual void pkm(std::shared_ptr<PKX> pk, u8 box, u8 slot, bool applyTrade) = 0;
    void transfer(std::shared_ptr<PKX> &pk);
    virtual void trade(std::shared_ptr<PKX> pk) = 0; // Look into bank boolean parameter
    virtual std::shared_ptr<PKX> emptyPkm() const = 0;
    
    virtual void dex(std::shared_ptr<PKX> pk) = 0;
    // virtual int dexSeen(void) const = 0;
    virtual int dexCaught(void) const = 0;
    virtual int emptyGiftLocation(void) const = 0;
    virtual std::vector<MysteryGift::giftData> currentGifts(void) const = 0;
    virtual std::unique_ptr<WCX> mysteryGift(int pos) const = 0;
    virtual void mysteryGift(WCX& wc, int& pos) = 0;
    virtual void cryptBoxData(bool crypted) = 0;
    virtual std::string boxName(u8 box) const = 0;
    virtual void boxName(u8 box, const std::string& name) = 0;
    virtual u8 partyCount(void) const = 0;
    virtual void partyCount(u8 count) = 0;
    virtual void fixParty(void); // Has to be overridden by SavLGPE because it works stupidly

    virtual int maxSlot(void) const { return maxBoxes() * 30; }
    virtual int maxBoxes(void) const = 0;
    virtual size_t maxWondercards(void) const = 0;
    virtual int maxSpecies(void) const = 0;
    virtual int maxMove(void) const = 0;
    virtual int maxItem(void) const = 0;
    virtual int maxAbility(void) const = 0;
    virtual int maxBall(void) const = 0;
    virtual Generation generation(void) const = 0;

    virtual void item(Item& item, Pouch pouch, u16 slot) = 0;
    virtual std::unique_ptr<Item> item(Pouch pouch, u16 slot) const = 0;
    virtual std::vector<std::pair<Pouch, int>> pouches(void) const = 0;
    virtual std::map<Pouch, std::vector<int>> validItems(void) const = 0;
    virtual std::string pouchName(Pouch pouch) const = 0;

    u32 getLength() { return length; }
    u8* rawData() { return data; }

    // Personal interface
    virtual u8 formCount(u16 species) const = 0;
};

#endif
