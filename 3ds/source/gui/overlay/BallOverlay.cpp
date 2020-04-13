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

#include "BallOverlay.hpp"
#include "Configuration.hpp"
#include "PKX.hpp"
#include "Sav.hpp"
#include "gui.hpp"
#include "i18n.hpp"
#include "loader.hpp"
#include <algorithm>

BallOverlay::BallOverlay(ReplaceableScreen& screen, std::shared_ptr<PKX> pkm)
    : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
      pkm(pkm),
      hid(30, 6),
      balls(TitleLoader::save->availableBalls().begin(), TitleLoader::save->availableBalls().end())
{
    std::sort(balls.begin(), balls.end());
    hid.update(24);
    auto index = std::find(balls.begin(), balls.end(), pkm->ball());
    if (index != balls.end())
    {
        hid.select(std::distance(balls.begin(), index));
    }
    else
    {
        hid.select(0);
    }
}

void BallOverlay::drawBottom() const
{
    dim();
    Gui::text(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
}

void BallOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_5x6_idx, 0, 0);

    int selectorX = (hid.index() % 6) * 67;
    int selectorY = (hid.index() / 6) * 48;
    // Selector
    Gui::drawSolidRect(selectorX, selectorY, 66, 47, COLOR_MASKBLACK);
    Gui::drawSolidRect(selectorX, selectorY, 66, 1, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY, 1, 47, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX + 65, selectorY, 1, 47, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY + 46, 66, 1, COLOR_YELLOW);

    for (size_t y = 0; y < 5; y++)
    {
        for (size_t x = 0; x < 6; x++)
        {
            if (x + y * 6 >= balls.size())
            {
                break;
            }
            Gui::ball(balls[x + y * 6], x * 67 + 24, y * 48 + 8);
            Gui::text(i18n::ball(Configuration::getInstance().language(), balls[x + y * 6]), x * 67 + 33, y * 48 + 30, FONT_SIZE_9, COLOR_WHITE,
                TextPosX::CENTER, TextPosY::TOP);
        }
    }
}

void BallOverlay::update(touchPosition* touch)
{
    hid.update(balls.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->ball(balls[hid.fullIndex()]);
        parent->removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
}
