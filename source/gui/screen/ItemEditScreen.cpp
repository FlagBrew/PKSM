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

#include "ItemEditScreen.hpp"
#include "gui.hpp"

int ItemEditScreen::run()
{
    while (aptMainLoop() && !finished)
    {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_renderTargetTop, COLOR_BLACK);

        draw();
        touchPosition touch;
        hidTouchRead(&touch);
        update(&touch);

        C3D_FrameEnd(0);
        Gui::clearTextBufs();
    }
    return finalVal;
}

void ItemEditScreen::draw() const
{
    if (firstDraw)
    {
        C2D_SceneBegin(g_renderTargetBottom);
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, C2D_Color32(0, 0, 0, 128));
        firstDraw = false;
    }

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
        if (i + hid.page() * hid.maxVisibleEntries() >= items.size())
        {
            break;
        }
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        Gui::dynamicText(items[i + hid.page() * hid.maxVisibleEntries()].first, x, (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
    }
}

void ItemEditScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();
    hid.update(items.size());
    if (downKeys & KEY_A)
    {
        finalVal = hid.fullIndex();
        finished = true;
    }
    else if (downKeys & KEY_B)
    {
        finalVal = origItem;
        finished = true;
    }
}