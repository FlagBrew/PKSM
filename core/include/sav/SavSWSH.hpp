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

#ifndef SAVSWSH_HPP
#define SAVSWSH_HPP

#include "Sav8.hpp"

class SavSWSH : public Sav8
{
private:
    static constexpr std::array<u16, 25> gigaSpecies = {
        6, 12, 25, 52, 68, 94, 99, 131, 133, 143, 569, 823, 826, 834, 839, 841, 842, 844, 851, 858, 861, 869, 879, 884, 890};

public:
    SavSWSH(std::shared_ptr<u8[]> dt);

    u16 TID(void) const override;
    void TID(u16 v) override;
    u16 SID(void) const override;
    void SID(u16 v) override;
    u8 version(void) const override;
    void version(u8 v) override;
    u8 gender(void) const override;
    void gender(u8 v) override;
    std::string otName(void) const override;
    void otName(const std::string& v) override;
    Language language(void) const override;
    void language(Language v) override;
    std::string jerseyNum(void) const;
    void jerseyNum(const std::string& v);
    u32 money(void) const override;
    void money(u32 v) override;
    u32 BP(void) const override;
    void BP(u32 v) override;
    u8 badges(void) const override;
    u16 playedHours(void) const override;
    void playedHours(u16 v) override;
    u8 playedMinutes(void) const override;
    void playedMinutes(u8 v) override;
    u8 playedSeconds(void) const override;
    void playedSeconds(u8 v) override;
    // TODO: last saved time

    void item(const Item& item, Pouch pouch, u16 slot) override;
    std::unique_ptr<Item> item(Pouch pouch, u16 slot) const override;
    std::vector<std::pair<Pouch, int>> pouches(void) const override;
    std::map<Pouch, std::vector<int>> validItems(void) const override;
    std::string pouchName(Language lang, Pouch pouch) const override;

    u8 subRegion(void) const override { return 0; }     // Not applicable
    void subRegion(u8 v) override {}                    // Not applicable
    u8 country(void) const override { return 0; }       // Not applicable
    void country(u8 v) override {}                      // Not applicable
    u8 consoleRegion(void) const override { return 0; } // Not applicable
    void consoleRegion(u8 v) override {}                // Not applicable

    std::string boxName(u8 box) const override;
    void boxName(u8 box, const std::string& name) override;

    u8 currentBox(void) const override { return 0; } // TODO: confirm that this is not stored
    void currentBox(u8 v) override {}
    u32 boxOffset(u8 box, u8 slot) const override;
    u32 partyOffset(u8 slot) const override;
    u8 partyCount(void) const override;
    void partyCount(u8 count) override;

    std::shared_ptr<PKX> pkm(u8 slot) const override;
    std::shared_ptr<PKX> pkm(u8 box, u8 slot, bool ekx = false) const override;

    // NOTICE: this sets a pkx into the savefile, not a ekx
    // that's because PKSM works with decrypted boxes and
    // crypts them back during resigning
    bool pkm(std::shared_ptr<PKX> pk, u8 box, u8 slot, bool applyTrade) override;
    void pkm(std::shared_ptr<PKX> pk, u8 slot) override;

    void cryptBoxData(bool crypted) override;

    void dex(std::shared_ptr<PKX> pk) override;
    int dexSeen(void) const override;
    int dexCaught(void) const override;
    const std::set<int>& availableSpecies(void) const override;

    // TODO
    // Check whether gifts are stored, or whether some disgusting record system is used
    void mysteryGift(WCX& wc, int& pos) override {}
    std::unique_ptr<WCX> mysteryGift(int pos) const override;

    u8 formCount(u16 species) const override { return PersonalSWSH::formCount(species); }
};

#endif
