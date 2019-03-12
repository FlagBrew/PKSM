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

#include "FormSelectionScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "Configuration.hpp"

void FormSelectionScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_mtx_5x6_idx, 0, 0);

    int x = (hid.index() % 6) * 67;
    int y = (hid.index() / 6) * 48;
    int dx = 66;
    int dy = 47;
    if (x == 335)
    {
        dx -= 1;
    }
    // Selector
    C2D_DrawRectSolid(x, y, 0.5f, dx, dy, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, dx, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, dy, COLOR_YELLOW);
    C2D_DrawRectSolid(x + dx - 1, y, 0.5f, 1, dy, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + dy - 1, 0.5f, dx, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 6; x++)
        {
            if (x + y * 6 >= formCount)
            {
                break;
            }
            Gui::pkm(pkm->species(), x + y * 6, TitleLoader::save->generation(), pkm->gender(), x * 66 + 19, y * 48 + 1);
            std::string text = StringUtils::wrap(i18n::form(Configuration::getInstance().language(), pkm->species(), x + y * 6, TitleLoader::save->generation()), FONT_SIZE_9, 65.0f, 2);
            Gui::dynamicText(text, x * 67 + 32, y * 48 + 39, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
            //Gui::dynamicText(x * 50, y * 48 + 30, 50, i18n::form(Configuration::getInstance().language(), pkm->species(), x + y * 8, TitleLoader::save->generation()), FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
        }
    }
}

void FormSelectionScreen::update(touchPosition* touch)
{
    hid.update(formCount);
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->alternativeForm(hid.fullIndex());
        done = true;
        return;
    }
    else if (downKeys & KEY_B)
    {
        done = true;
        return;
    }
}
