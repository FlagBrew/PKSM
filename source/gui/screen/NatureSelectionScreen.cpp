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

#include "NatureSelectionScreen.hpp"
#include "gui.hpp"
#include "Configuration.hpp"

static constexpr std::string_view stats[] = {
    "Attack",
    "Defense",
    "Speed",
    "Sp. Atk.",
    "Sp. Def."
};

void NatureSelectionScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_editor_6x6_idx, 0, 0);

    Gui::staticText(0, 12, 65, "Neutral", FONT_SIZE_11, FONT_SIZE_11, COLOR_YELLOW);
    for (int i = 0; i < 5; i++)
    {
        Gui::staticText(i * 67 + 66, 12, 66, std::string("-") + std::string(stats[i]), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
        Gui::staticText(0, i * 40 + 52, 65, std::string("+") + std::string(stats[i]), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
    }

    int x = (hid.index() % 5) * 67 + 66;
    int y = (hid.index() / 5) * 40 + 40;
    // Selector
    C2D_DrawRectSolid(x, y, 0.5f, 66, 39, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, 66, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, 39, COLOR_YELLOW);
    C2D_DrawRectSolid(x + 65, y, 0.5f, 1, 39, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + 38, 0.5f, 66, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            Gui::staticText(x * 67 + 66, y * 40 + 52, 66, i18n::nature(Configuration::getInstance().language(), x + y * 5), FONT_SIZE_11, FONT_SIZE_11, x == y ? COLOR_YELLOW : COLOR_WHITE);
        }
    }
}

void NatureSelectionScreen::update(touchPosition* touch)
{
    hid.update(25);
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->nature((u8) hid.fullIndex());
        done = true;
        return;
    }
    else if (downKeys & KEY_B)
    {
        done = true;
        return;
    }
}