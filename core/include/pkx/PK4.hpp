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

#ifndef PK4_HPP
#define PK4_HPP

#include "PKX.hpp"
#include "personal.hpp"

class PK4 : public PKX
{
protected:
    static constexpr u8 beasts[4]  = {251, 243, 244, 245};
    static constexpr u16 banned[8] = {15, 19, 57, 70, 250, 249, 127, 431};

    void shuffleArray(u8 sv) override;
    void crypt(void) override;

public:
    PK4()
    {
        directAccess = false;
        length       = 136;
        data         = new u8[length];
        std::fill_n(data, length, 0);
    }
    PK4(u8* dt, bool ekx = false, bool party = false, bool directAccess = false);
    virtual ~PK4()
    {
        if (!directAccess)
        {
            delete[] data;
        }
    }

    std::shared_ptr<PKX> clone(void) const override;

    Generation generation(void) const;

    u32 encryptionConstant(void) const override;
    void encryptionConstant(u32 v) override;
    u8 currentFriendship(void) const override;
    void currentFriendship(u8 v) override;
    u8 currentHandler(void) const override;
    void currentHandler(u8 v) override;
    u8 abilityNumber(void) const override;
    void abilityNumber(u8 v) override;
    void setAbility(u8 abilityNumber) override;

    u32 PID(void) const override;
    void PID(u32 v) override;
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
    u8 otFriendship(void) const override;
    void otFriendship(u8 v) override;
    u8 ability(void) const override;
    void ability(u8 v) override;
    u16 markValue(void) const override;
    void markValue(u16 v) override;
    u8 language(void) const override;
    void language(u8 v) override;
    u8 ev(u8 ev) const override;
    void ev(u8 ev, u8 v) override;
    u8 contest(u8 contest) const override;
    void contest(u8 contest, u8 v) override;
    bool ribbon(u8 ribcat, u8 ribnum) const override;
    void ribbon(u8 ribcat, u8 ribnum, u8 v) override;

    u16 move(u8 move) const override;
    void move(u8 move, u16 v) override;
    u16 relearnMove(u8 move) const override;
    void relearnMove(u8 move, u16 v) override;
    u8 PP(u8 move) const override;
    void PP(u8 move, u8 v) override;
    u8 PPUp(u8 move) const override;
    void PPUp(u8 move, u8 v) override;
    u8 iv(u8 iv) const override;
    void iv(u8 iv, u8 v) override;
    bool egg(void) const override;
    void egg(bool v) override;
    bool nicknamed(void) const override;
    void nicknamed(bool v) override;
    bool fatefulEncounter(void) const override;
    void fatefulEncounter(bool v) override;
    u8 gender(void) const override;
    void gender(u8 g) override;
    u8 alternativeForm(void) const override;
    void alternativeForm(u8 v) override;
    u8 shinyLeaf(void) const;
    void shinyLeaf(u8 v);
    u8 nature(void) const override;
    void nature(u8 v) override;
    bool hiddenAbility(void) const;
    void hiddenAbility(bool v);
    bool nPokemon(void) const;
    void nPokemon(bool v);

    std::string nickname(void) const override;
    void nickname(const std::string& v) override;
    u8 version(void) const override;
    void version(u8 v) override;

    std::string otName(void) const override;
    void otName(const std::string& v) override;
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
    u8 pkrs(void) const override;
    void pkrs(u8 v) override;
    u8 pkrsDays(void) const override;
    void pkrsDays(u8 v) override;
    u8 pkrsStrain(void) const override;
    void pkrsStrain(u8 v) override;
    u8 ball(void) const override;
    void ball(u8 v) override;
    u8 metLevel(void) const override;
    void metLevel(u8 v) override;
    u8 otGender(void) const override;
    void otGender(u8 v) override;
    u8 encounterType(void) const;
    void encounterType(u8 v);
    u8 characteristic(void) const;

    void refreshChecksum(void) override;
    u8 hpType(void) const override;
    void hpType(u8 v) override;
    u16 TSV(void) const override;
    u16 PSV(void) const override;
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

    std::shared_ptr<PKX> next(Sav& save) const override;

    inline u8 baseHP(void) const override { return PersonalDPPtHGSS::baseHP(formSpecies()); }
    inline u8 baseAtk(void) const override { return PersonalDPPtHGSS::baseAtk(formSpecies()); }
    inline u8 baseDef(void) const override { return PersonalDPPtHGSS::baseDef(formSpecies()); }
    inline u8 baseSpe(void) const override { return PersonalDPPtHGSS::baseSpe(formSpecies()); }
    inline u8 baseSpa(void) const override { return PersonalDPPtHGSS::baseSpa(formSpecies()); }
    inline u8 baseSpd(void) const override { return PersonalDPPtHGSS::baseSpd(formSpecies()); }
    inline u8 type1(void) const override { return PersonalDPPtHGSS::type1(formSpecies()); }
    inline u8 type2(void) const override { return PersonalDPPtHGSS::type2(formSpecies()); }
    inline u8 genderType(void) const override { return PersonalDPPtHGSS::gender(formSpecies()); }
    inline u8 baseFriendship(void) const override { return PersonalDPPtHGSS::baseFriendship(formSpecies()); }
    inline u8 expType(void) const override { return PersonalDPPtHGSS::expType(formSpecies()); }
    inline u8 abilities(u8 n) const override { return PersonalDPPtHGSS::ability(formSpecies(), n); }
    inline u16 formStatIndex(void) const override { return PersonalDPPtHGSS::formStatIndex(formSpecies()); }

private:
    bool directAccess;
};

#endif
