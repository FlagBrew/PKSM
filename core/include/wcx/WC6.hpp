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

#ifndef WC6_HPP
#define WC6_HPP

#include "WCX.hpp"

class WC6 : public WCX
{
protected:
    u8 data[264];

public:
    static const u16 length     = 264;
    static const u16 lengthFull = 784;

    WC6(u8* dt, bool full = false);
    virtual ~WC6(){};

    Generation generation(void) const override;
    bool bean(void) const override;
    bool BP(void) const override;
    bool item(void) const override;
    bool pokemon(void) const override;
    bool power(void) const override;
    std::string title(void) const override;
    u8 type(void) const override;
    u32 year(void) const override;
    u32 month(void) const override;
    u32 day(void) const override;
    void year(u32 v) override;
    void month(u32 v) override;
    void day(u32 v) override;
    u16 ID(void) const override;
    u16 object(void) const override;
    u16 objectQuantity(void) const;
    bool multiObtainable(void) const override;
    u32 rawDate(void) const override;
    void rawDate(u32 value) override;
    u8 flags(void) const override;
    u8 cardLocation(void) const override;
    bool used(void) const override;
    bool oncePerDay(void) const;

    // Pokemon properties
    bool egg(void) const override;
    bool shiny(void) const override;
    u16 ability(void) const override;
    u8 abilityType(void) const override;
    u8 ball(void) const override;
    u32 encryptionConstant(void) const;
    u8 alternativeForm(void) const override;
    u8 gender(void) const override;
    u16 heldItem(void) const override;
    Language language(void) const override;
    u8 level(void) const override;
    u8 metLevel(void) const override;
    u16 move(u8 index) const override;
    u16 relearnMove(u8 index) const;
    u8 nature(void) const override;
    std::string nickname(void) const override;
    u8 version(void) const;
    std::string otName(void) const override;
    u8 otGender(void) const;
    u8 otIntensity(void) const;
    u8 otMemory(void) const;
    u16 otTextvar(void) const;
    u8 otFeeling(void) const;
    u16 TID(void) const override;
    u16 SID(void) const override;
    u8 PIDType(void) const override;
    u32 PID(void) const override;
    u16 species(void) const override;
    u16 eggLocation(void) const override;
    u16 metLocation(void) const override;
    u8 contest(u8 index) const override;
    u8 iv(u8 index) const override;
    u8 ev(u8 index) const;
    u16 additionalItem(void) const;
    bool ribbon(u8 category, u8 index) const override;

    u16 formSpecies(void) const override;

    int size(void) const override { return length; }
    const u8* rawData(void) const override { return data; }
};

#endif
