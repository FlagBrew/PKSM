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

#ifndef PK7_HPP
#define PK7_HPP

#include "PKX.hpp"
#include "PK6.hpp"

class PK7 : public PKX
{
friend class SavUSUM;
friend class SavSUMO;
protected:
    static constexpr u16 hyperTrainLookup[6] = {0, 1, 2, 5, 3, 4};

    void shuffleArray(u8 sv) override;
    void crypt(void) override;

    u8* data;

    u8* rawData(void) override { return data; }
    void reorderMoves(void) override;

public:
    PK7() { length = 232; data = new u8[length]; std::fill_n(data, length, 0); }
    PK7(u8* dt, bool ekx = false, bool party = false);
    virtual ~PK7() { delete[] data; };

    std::unique_ptr<PKX> clone(void) override;

    Generation generation(void) const override;

    u32 encryptionConstant(void) const override;
    void encryptionConstant(u32 v) override;
    u16 sanity(void) const override;
    void sanity(u16 v) override;
    u16 checksum(void) const override;
    void checksum(u16 v) override;
    u16 species(void) const override;
    void species(u16 v) override;
    u16 heldItem(void) const override;
    void heldItem(u16 v) override;
    u16 TID(void) const override;
    void TID(u16 v) override;
    u16 SID(void) const override;
    void SID(u16 v) override;
    u32 experience(void) const override;
    void experience(u32 v) override;
    u8 ability(void) const override;
    void ability(u8 v) override;
    u8 abilityNumber(void) const override;
    void abilityNumber(u8 v) override;
    void setAbility(u8 abilityNumber) override;
    u16 markValue(void) const override;
    void markValue(u16 v) override;
    u32 PID(void) const override;
    void PID(u32 v) override;
    u8 nature(void) const override;
    void nature(u8 v) override;
    bool fatefulEncounter(void) const override;
    void fatefulEncounter(bool v) override;
    u8 gender(void) const override;
    void gender(u8 g) override;
    u8 alternativeForm(void) const override;
    void alternativeForm(u8 v) override;
    u8 ev(u8 ev) const override;
    void ev(u8 ev, u8 v) override;
    u8 contest(u8 contest) const override;
    void contest(u8 contest, u8 v) override;

    u8 pelagoEventStatus(void) const;
    void pelagoEventStatus(u8 v);

    u8 pkrs(void) const override;
    void pkrs(u8 v) override;
    u8 pkrsDays(void) const override;
    void pkrsDays(u8 v) override;
    u8 pkrsStrain(void) const override;
    void pkrsStrain(u8 v) override;
    bool ribbon(u8 ribcat, u8 ribnum) const override;
    void ribbon(u8 ribcat, u8 ribnum, u8 v) override;
    
    std::string nickname(void) const override;
    void nickname(const char* v) override;
    u16 move(u8 move) const override;
    void move(u8 move, u16 v) override;
    u8 PP(u8 move) const override;
    void PP(u8 move, u8 v) override;
    u8 PPUp(u8 move) const override;
    void PPUp(u8 move, u8 v) override;
    u16 relearnMove(u8 move) const;
    void relearnMove(u8 move, u16 v);
    u8 iv(u8 iv) const override;
    void iv(u8 iv, u8 v) override;

    bool egg(void) const override;
    void egg(bool v) override;
    bool nicknamed(void) const override;
    void nicknamed(bool v) override;

    std::string htName(void) const;
    void htName(const char* v);
    u8 htGender(void) const;
    void htGender(u8 v);
    u8 currentHandler(void) const override;
    void currentHandler(u8 v) override;
    u8 geoRegion(u8 region) const;
    void geoRegion(u8 region, u8 v);
    u8 geoCountry(u8 country) const;
    void geoCountry(u8 country, u8 v);
    u8 htFriendship(void) const;
    void htFriendship(u8 v);
    u8 htAffection(void) const;
    void htAffection(u8 v);
    u8 htIntensity(void) const;
    void htIntensity(u8 v);
    u8 htMemory(void) const;
    void htMemory(u8 v);
    u8 htFeeling(void) const;
    void htFeeling(u8 v);
    u16 htTextVar(void) const;
    void htTextVar(u16 v);
    u8 fullness(void) const;
    void fullness(u8 v);
    u8 enjoyment(void) const;
    void enjoyment(u8 v);

    std::string otName(void) const override;
    void otName(const char* v) override;
    u8 otFriendship(void) const override;
    void otFriendship(u8 v) override;
    u8 otAffection(void) const;
    void otAffection(u8 v);
    u8 otIntensity(void) const;
    void otIntensity(u8 v);
    u8 otMemory(void) const;
    void otMemory(u8 v);
    u16 otTextVar(void) const;
    void otTextVar(u16 v);
    u8 otFeeling(void) const;
    void otFeeling(u8 v);
    u8 eggYear(void) const override;
    void eggYear(u8 v) override;
    u8 eggMonth(void) const override;
    void eggMonth(u8 v) override;
    u8 eggDay(void) const override;
    void eggDay(u8 v) override;
    u8 metYear(void) const override;
    void metYear(u8 v) override;
    u8 metMonth(void) const override;
    void metMonth(u8 v) override;
    u8 metDay(void) const override;
    void metDay(u8 v) override;
    u16 eggLocation(void) const override;
    void eggLocation(u16 v) override;
    u16 metLocation(void) const override;
    void metLocation(u16 v) override;
    u8 ball(void) const override;
    void ball(u8 v) override;
    u8 metLevel(void) const override;
    void metLevel(u8 v) override;
    u8 otGender(void) const override;
    void otGender(u8 v) override;
    
    bool hyperTrain(u8 num) const;
    void hyperTrain(u8 num, bool v);

    u8 version(void) const override;
    void version(u8 v) override;
    u8 country(void) const;
    void country(u8 v);
    u8 region(void) const;
    void region(u8 v);
    u8 consoleRegion(void) const;
    void consoleRegion(u8 v);
    u8 language(void) const override;
    void language(u8 v) override;

    u8 currentFriendship(void) const override;
    void currentFriendship(u8 v) override;
    u8 oppositeFriendship(void) const;
    void oppositeFriendship(u8 v);
    void refreshChecksum(void) override;
    u8 hpType(void) const override;
    void hpType(u8 v) override;
    u16 TSV(void) const override;
    u16 PSV(void) const override;
    u32 displayTID(void) const override { return (u32)(SID() << 16 | TID()) % 1000000; }
    u32 displaySID(void) const override { return (u32)(SID() << 16 | TID()) / 1000000; }
    u8 level(void) const override;
    void level(u8 v) override;
    bool shiny(void) const override;
    void shiny(bool v) override;
    u16 formSpecies(void) const override;
    u16 stat(const u8 stat) const override;

    int partyCurrHP(void) const override;
    void partyCurrHP(u16 v) override;
    int partyStat(const u8 stat) const override;
    void partyStat(const u8 stat, u16 v) override;
    int partyLevel() const override;
    void partyLevel(u8 v) override;
    
    std::unique_ptr<PKX> previous(void) const override;
};

#endif
