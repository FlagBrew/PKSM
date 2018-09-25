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

#ifndef SAV_HPP
#define SAV_HPP

#include <memory>
#include <stdint.h>
#include "PKX.hpp"
#include "WCX.hpp"
#include "utils.hpp"
#include "mysterygift.hpp"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

class Sav
{
friend class ScriptScreen;
protected:
    static const u16 crc16[256];
    
    u8* data;
    static u16 ccitt16(const u8* buf, u32 len);
    static std::unique_ptr<Sav> checkDSType(u8* dt);
    static bool validSequence(u8* dt, u8* pattern, int shift = 0);

public:
    u8 boxes = 0;
    u32 length = 0;

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
    virtual void otName(const char* v) = 0;
    virtual u32 money(void) const = 0;
    virtual void money(u32 v) = 0;
    virtual u32 BP(void) const = 0;
    virtual void BP(u32 v) = 0;
    virtual u16 playedHours(void) const = 0;
    virtual void playedHours(u16 v) = 0;
    virtual u8 playedMinutes(void) const = 0;
    virtual void playedMinutes(u8 v) = 0;
    virtual u8 playedSeconds(void) const = 0;
    virtual void playedSeconds(u8 v) = 0;

    virtual u8 currentBox(void) const = 0;
    virtual void currentBox(u8 v) = 0;
    virtual u32 boxOffset(u8 box, u8 slot) const = 0;
    virtual u32 partyOffset(u8 slot) const = 0;
    
    virtual std::unique_ptr<PKX> pkm(u8 slot) const = 0;
    virtual std::unique_ptr<PKX> pkm(u8 box, u8 slot, bool ekx = false) const = 0;
    virtual void pkm(PKX& pk, u8 box, u8 slot) = 0;
    virtual std::shared_ptr<PKX> emptyPkm() const = 0;
    
    virtual void dex(PKX& pk) = 0;
    virtual int emptyGiftLocation(void) const = 0;
    virtual std::vector<MysteryGift::giftData> currentGifts(void) const = 0;
    virtual void mysteryGift(WCX& wc, int& pos) = 0;
    virtual void cryptBoxData(bool crypted) = 0;
    virtual std::string boxName(u8 box) const = 0;
    virtual void boxName(u8 box, std::string name) = 0;
    virtual u8 partyCount(void) const = 0;

    virtual int maxBoxes(void) const = 0;
    virtual size_t maxWondercards(void) const = 0;
    virtual u8 generation(void) const = 0;
};

#endif