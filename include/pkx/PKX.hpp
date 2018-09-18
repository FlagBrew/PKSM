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

#ifndef PKX_HPP
#define PKX_HPP

#include <algorithm>
#include <memory>
#include <stdlib.h>

#include "personal.hpp"
#include "utils.hpp"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

class PKX
{
friend class HexEditScreen;
protected:
    u32 expTable(u8 row, u8 col) const;
    u32 seedStep(u32 seed);
    void reorderMoves(void);

    virtual void crypt(void) = 0;
    virtual void shuffleArray(void) = 0;
    virtual u8* rawData(void) = 0;

    u8 length = 0;
public:
    virtual void decrypt(void) = 0;
    virtual void encrypt(void) = 0;
    virtual std::unique_ptr<PKX> clone(void) = 0;
    virtual ~PKX() { };

    virtual u8 generation(void) const = 0;
    bool gen7(void) const;
    bool gen6(void) const;
    bool gen5(void) const;
    bool gen4(void) const;
    bool gen3(void) const;
    void fixMoves(void);

    // BLOCK A
    virtual u32 encryptionConstant(void) const = 0;
    virtual void encryptionConstant(u32 v) = 0;
    virtual u16 sanity(void) const = 0;
    virtual void sanity(u16 v) = 0;
    virtual u16 checksum(void) const = 0;
    virtual void checksum(u16 v) = 0;
    virtual u16 species(void) const = 0;
    virtual void species(u16 v) = 0;
    virtual u16 heldItem(void) const = 0;
    virtual void heldItem(u16 v) = 0;
    virtual u16 TID(void) const = 0;
    virtual void TID(u16 v) = 0;
    virtual u16 SID(void) const = 0;
    virtual void SID(u16 v) = 0;
    virtual u32 experience(void) const = 0;
    virtual void experience(u32 v) = 0;
    virtual u8 ability(void) const = 0;
    virtual void ability(u8 v) = 0;
    virtual u8 abilityNumber(void) const = 0;
    virtual void abilityNumber(u8 v) = 0;
    virtual u16 markValue(void) const = 0;
    virtual void markValue(u16 v) = 0;
    virtual u32 PID(void) const = 0;
    virtual void PID(u32 v) = 0;
    virtual u8 nature(void) const = 0;
    virtual void nature(u8 v) = 0;
    virtual bool fatefulEncounter(void) const = 0;
    virtual void fatefulEncounter(bool v) = 0;
    virtual u8 gender(void) const = 0;
    virtual void gender(u8 g) = 0;
    virtual u8 alternativeForm(void) const = 0;
    virtual void alternativeForm(u8 v) = 0;
    virtual u8 ev(u8 ev) const = 0;
    virtual void ev(u8 ev, u8 v) = 0;
    virtual u8 contest(u8 contest) const = 0;
    virtual void contest(u8 contest, u8 v) = 0;
    virtual u8 pkrs(void) const = 0;
    virtual void pkrs(u8 v) = 0;
    virtual u8 pkrsDays(void) const = 0;
    virtual void pkrsDays(u8 v) = 0;
    virtual u8 pkrsStrain(void) const = 0;
    virtual void pkrsStrain(u8 v) = 0;
    virtual bool ribbon(u8 ribcat, u8 ribnum) const = 0;
    virtual void ribbon(u8 ribcat, u8 ribnum, u8 v) = 0;

    // BLOCK B
    virtual std::string nickname(void) const = 0;
    virtual void nickname(const char* v) = 0;
    virtual u16 move(u8 move) const = 0;
    virtual void move(u8 move, u16 v) = 0;
    virtual u8 PP(u8 move) const = 0;
    virtual void PP(u8 move, u8 v) = 0;
    virtual u8 PPUp(u8 move) const = 0;
    virtual void PPUp(u8 move, u8 v) = 0;
    virtual u8 iv(u8 iv) const = 0;
    virtual void iv(u8 iv, u8 v) = 0;
    virtual bool egg(void) const = 0;
    virtual void egg(bool v) = 0;
    virtual bool nicknamed(void) const = 0;
    virtual void nicknamed(bool v) = 0;

    // BLOCK C
    virtual u8 currentHandler(void) const = 0;
    virtual void currentHandler(u8 v) = 0;

    // BLOCK D
    virtual std::string otName(void) const = 0;
    virtual void otName(const char* v) = 0;
    virtual u8 otFriendship(void) const = 0;
    virtual void otFriendship(u8 v) = 0;
    virtual u8 eggYear(void) const = 0;
    virtual void eggYear(u8 v) = 0;
    virtual u8 eggMonth(void) const = 0;
    virtual void eggMonth(u8 v) = 0;
    virtual u8 eggDay(void) const = 0;
    virtual void eggDay(u8 v) = 0;
    virtual u8 metYear(void) const = 0;
    virtual void metYear(u8 v) = 0;
    virtual u8 metMonth(void) const = 0;
    virtual void metMonth(u8 v) = 0;
    virtual u8 metDay(void) const = 0;
    virtual void metDay(u8 v) = 0;
    virtual u16 eggLocation(void) const = 0;
    virtual void eggLocation(u16 v) = 0;
    virtual u16 metLocation(void) const = 0;
    virtual void metLocation(u16 v) = 0;
    virtual u8 ball(void) const = 0;
    virtual void ball(u8 v) = 0;
    virtual u8 metLevel(void) const = 0;
    virtual void metLevel(u8 v) = 0;
    virtual u8 otGender(void) const = 0;
    virtual void otGender(u8 v) = 0;
    virtual u8 version(void) const = 0;
    virtual void version(u8 v) = 0;
    virtual u8 language(void) const = 0;
    virtual void language(u8 v) = 0;

    virtual u8 currentFriendship(void) const = 0;
    virtual void currentFriendship(u8 v) = 0;
    virtual void refreshChecksum(void) = 0;
    virtual u8 hpType(void) const = 0;
    virtual void hpType(u8 v) = 0;
    virtual u16 TSV(void) const = 0;
    virtual u16 PSV(void) const = 0;
    virtual u8 level(void) const = 0;
    virtual void level(u8 v) = 0;
    virtual bool shiny(void) const = 0;
    virtual void shiny(bool v) = 0;
    virtual u16 formSpecies(void) const = 0;
    virtual u16 stat(const u8 stat) const = 0;
};

#endif