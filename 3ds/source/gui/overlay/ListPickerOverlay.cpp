/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#include "ListPickerOverlay.hpp"
#include "gui.hpp"

void ListPickerOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_editor_10x2_idx, 0, 0);
    const size_t rows = hid.maxVisibleEntries() / 2;
    int x             = hid.index() < rows ? 2 : 200;
    int y             = (hid.index() % rows) * 24;
    Gui::drawSolidRect(x, y, 198, 23, COLOR_MASKBLACK);
    Gui::drawSolidRect(x, y, 198, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x, y, 1, 23, COLOR_YELLOW);
    Gui::drawSolidRect(x, y + 22, 198, 1, COLOR_YELLOW);
    Gui::drawSolidRect(x + 197, y, 1, 23, COLOR_YELLOW);
    for (size_t i = 0; i < hid.maxVisibleEntries(); i++)
    {
        size_t entry = hid.page() * hid.maxVisibleEntries() + i;
        if (entry >= entryCount())
        {
            break;
        }
        x = i < rows ? 4 : 203;
        Gui::text(entryLine(entry), x, (i % rows) * 24 + 4, FONT_SIZE_14, COLOR_WHITE,
            TextPosX::LEFT, TextPosY::TOP);
    }
}
