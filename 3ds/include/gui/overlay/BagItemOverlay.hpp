/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef BAGITEMOVERLAY_HPP
#define BAGITEMOVERLAY_HPP

#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "Hid.hpp"
#include "ReplaceableScreen.hpp"
#include "sav/Sav.hpp"
#include "spritesheets.h"
#include <string>
#include <vector>

class BagItemOverlay : public ReplaceableScreen
{
public:
    BagItemOverlay(ReplaceableScreen& screen,
        std::vector<std::pair<const std::string*, int>>& items, size_t selected,
        std::pair<pksm::Sav::Pouch, int> pouch, int slot, int& firstEmpty)
        : ReplaceableScreen(
              &screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("L_PAGE_PREV") + '\n' +
                           i18n::localize("R_PAGE_NEXT") + '\n' + i18n::localize("B_BACK")),
          validItems(items),
          items(items),
          pouch(pouch),
          hid(40, 2),
          origItem(selected),
          slot(slot),
          firstEmpty(firstEmpty)
    {
        instructions.addBox(
            false, 75, 30, 170, 23, COLOR_GREY, i18n::localize("SEARCH"), COLOR_WHITE);
        searchButton = std::make_unique<ClickButton>(
            75, 30, 170, 23,
            [this]()
            {
                searchBar();
                return false;
            },
            ui_sheet_emulated_box_search_idx, "", 0, COLOR_BLACK);
        hid.update(items.size());
        hid.select(selected);
    }

    void drawTop() const override;
    void drawBottom() const override;

    bool replacesTop() const override { return true; }

    void update(touchPosition* touch) override;

private:
    void searchBar();
    const std::vector<std::pair<const std::string*, int>> validItems;
    std::vector<std::pair<const std::string*, int>> items;
    std::pair<pksm::Sav::Pouch, int> pouch;
    std::string searchString    = "";
    std::string oldSearchString = "";
    std::unique_ptr<Button> searchButton;
    Hid<HidDirection::VERTICAL, HidDirection::HORIZONTAL> hid;
    int origItem;
    int slot;
    int& firstEmpty;
    bool justSwitched = true;
};

#endif
