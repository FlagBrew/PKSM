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

#include "BallOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "loader.hpp"

void BallOverlay::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    dim();
    Gui::staticText(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_mtx_5x6_idx, 0, 0);

    int x = (hid.index() % 6) * 67;
    int y = (hid.index() / 6) * 48;
    // Selector
    C2D_DrawRectSolid(x, y, 0.5f, 66, 47, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, 66, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, 47, COLOR_YELLOW);
    C2D_DrawRectSolid(x + 65, y, 0.5f, 1, 47, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + 46, 0.5f, 66, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 6; x++)
        {
            if (x + y * 6 >= TitleLoader::save->maxBall())
            {
                break;
            }
            Gui::ball(x + y * 6 + 1, x * 67 + 24, y * 48 + 8);
            Gui::dynamicText(i18n::ball(Configuration::getInstance().language(), x + y * 6), x * 67 + 33, y * 48 + 30, FONT_SIZE_9, FONT_SIZE_9,
                COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        }
    }
}

void BallOverlay::update(touchPosition* touch)
{
    hid.update(TitleLoader::save->maxBall());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->ball((u8)hid.fullIndex() + 1);
        screen.removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        screen.removeOverlay();
        return;
    }
}
