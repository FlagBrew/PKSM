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

#include "ThirtyChoice.hpp"
#include "gui.hpp"

void ThirtyChoice::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_5x6_idx, 0, 0);

    int x = (hid.index() % 6) * 67;
    int y = (hid.index() / 6) * 48;
    // Selector
    Gui::drawSolidRect(x, y, 66, 47, COLOR_MASKBLACK);
    Gui::drawSolidRect(x, y, 66, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x, y, 1, 47, COLOR_YELLOW);
    Gui::drawSolidRect(x + 65, y, 1, 47, COLOR_YELLOW);
    Gui::drawSolidRect(x, y + 46, 66, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 6; x++)
        {
            size_t index = x + y * 6 + hid.page() * hid.maxVisibleEntries();
            if (index >= (size_t)items)
            {
                break;
            }
            Gui::pkm(pkms[index].species, pkms[index].form, gen, 0, x * 67 + 18, y * 48 + 1);
            Gui::text(labels[index], x * 67 + 32, y * 48 + 39, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER, 65.0f);
        }
    }
}

void ThirtyChoice::update(touchPosition* touch)
{
    hid.update(items);
    if (hidKeysDown() & KEY_A)
    {
        finalVal = hid.fullIndex();
        done     = true;
    }
}
