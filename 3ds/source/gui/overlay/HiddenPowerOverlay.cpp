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

#include "HiddenPowerOverlay.hpp"
#include "gui.hpp"
#include "Configuration.hpp"

void HiddenPowerOverlay::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    dim();
    Gui::staticText(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_mtx_4x4_idx, 0, 0);
    int x = (hid.index() % 4) * 100;
    int y = (hid.index() / 4) * 60;
    // Selector
    C2D_DrawRectSolid(x, y, 0.5f, 99, 59, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, 99, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, 59, COLOR_YELLOW);
    C2D_DrawRectSolid(x + 98, y, 0.5f, 1, 59, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + 58, 0.5f, 99, 1, COLOR_YELLOW);
    for (int i = 0; i < 16; i++)
    {
        Gui::type(Configuration::getInstance().language(), (u8) i + 1, 23 + (i % 4) * 100, 20 + (i / 4) * 60);
    }
}

void HiddenPowerOverlay::update(touchPosition* touch)
{
    hid.update(16);
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->hpType((u8) hid.fullIndex());
        screen.removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        screen.removeOverlay();
        return;
    }
}
