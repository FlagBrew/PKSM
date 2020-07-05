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

#include "FortyChoice.hpp"
#include "gui.hpp"

void FortyChoice::drawTop() const
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
        if (i + hid.page() * hid.maxVisibleEntries() >= (size_t)items)
        {
            break;
        }
        x = i < hid.maxVisibleEntries() / 2 ? 4 : 203;
        Gui::text(labels[i + hid.page() * hid.maxVisibleEntries()], x,
            (i % (hid.maxVisibleEntries() / 2)) * 12, FONT_SIZE_9, COLOR_WHITE, TextPosX::LEFT,
            TextPosY::TOP);
    }
}

void FortyChoice::drawBottom() const
{
    Gui::backgroundBottom(false);
    Gui::text(question, 160, 120, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
}

void FortyChoice::update(touchPosition* touch)
{
    hid.update(items);
    if (hidKeysDown() & KEY_A)
    {
        finalValue = hid.fullIndex();
        done       = true;
    }
}
