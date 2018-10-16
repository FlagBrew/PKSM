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

#include "ItemSelectionScreen.hpp"
#include "gui.hpp"
#include "Configuration.hpp"
#include "loader.hpp"

void ItemSelectionScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_editor_20x2_idx, 0, 0);
    int x = hid.index() < hid.maxVisibleEntries() / 2 ? 2 : 200;
    int y = (hid.index() % (hid.maxVisibleEntries() / 2)) * 12;
    C2D_DrawRectSolid(x, y, 0.5f, 198, 11, COLOR_MASKBLACK);
    C2D_DrawRectSolid(x, y, 0.5f, 198, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y, 0.5f, 1, 11, COLOR_YELLOW);
    C2D_DrawRectSolid(x, y + 10, 0.5f, 198, 1, COLOR_YELLOW);
    C2D_DrawRectSolid(x + 197, y, 0.5f, 1, 11, COLOR_YELLOW);
    for (size_t i = 0; i < hid.maxVisibleEntries(); i++)
    {
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        std::string text = i18n::sortedItem(Configuration::getInstance().language(), hid.page() * hid.maxVisibleEntries() + i);
        text = std::to_string(hid.page() * hid.maxVisibleEntries() + i) + " - " + text;
        Gui::dynamicText(text, x, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
    }
}

void ItemSelectionScreen::update(touchPosition* touch)
{
    hid.update(TitleLoader::save->maxItem());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        pkm->heldItem((u16) i18n::itemFromSort(Configuration::getInstance().language(), hid.fullIndex()));
        done = true;
        return;
    }
    else if (downKeys & KEY_B)
    {
        done = true;
        return;
    }
}