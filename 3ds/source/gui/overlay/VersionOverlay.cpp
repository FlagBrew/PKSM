/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "VersionOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "pkx/PKX.hpp"
#include <algorithm>

VersionOverlay::VersionOverlay(ReplaceableScreen& screen, std::shared_ptr<pksm::PKX> pkm)
    : ReplaceableScreen(&screen), pkm(pkm), hid(40, 2)
{
    const auto& gameStrings = i18n::rawGames(Configuration::getInstance().language());
    for (size_t i = 0; i < gameStrings.size(); i++)
    {
        if (!gameStrings[i].empty())
        {
            games.emplace_back(pksm::GameVersion(i), gameStrings[i]);
        }
    }
    hid.update(games.size());
    hid.select(std::distance(
        games.begin(), std::find_if(games.begin(), games.end(),
                           [pkm](const std::pair<pksm::GameVersion, const std::string&>& pair) {
                               return pair.first == pkm->version();
                           })));
}

void VersionOverlay::drawBottom() const
{
    dim();
    Gui::text(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);
}

void VersionOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_editor_20x2_idx, 0, 0);
    int x = hid.index() < hid.maxVisibleEntries() / 2 ? 2 : 200;
    int y = (hid.index() % (hid.maxVisibleEntries() / 2)) * 12;
    Gui::drawSolidRect(x, y, 198, 11, COLOR_MASKBLACK);
    Gui::drawSolidRect(x, y, 198, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x, y, 1, 11, COLOR_YELLOW);
    Gui::drawSolidRect(x, y + 10, 198, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x + 197, y, 1, 11, COLOR_YELLOW);
    for (size_t i = 0; i < hid.maxVisibleEntries(); i++)
    {
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        if (hid.page() * hid.maxVisibleEntries() + i < games.size())
        {
            Gui::text(std::to_string((int)games[hid.page() * hid.maxVisibleEntries() + i].first) +
                          " - " + games[hid.page() * hid.maxVisibleEntries() + i].second,
                x, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, COLOR_WHITE,
                TextPosX::LEFT, TextPosY::TOP);
        }
        else
        {
            break;
        }
    }
}

void VersionOverlay::update(touchPosition* touch)
{
    hid.update(games.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->version(games[hid.fullIndex()].first);
        parent->removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
}
