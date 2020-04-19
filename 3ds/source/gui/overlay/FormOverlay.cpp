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

#include "FormOverlay.hpp"
#include "Configuration.hpp"
#include "PKFilter.hpp"
#include "PKX.hpp"
#include "Sav.hpp"
#include "gui.hpp"
#include "i18n.hpp"
#include "loader.hpp"

FormOverlay::FormOverlay(ReplaceableScreen& screen, const std::variant<std::shared_ptr<PKX>, std::shared_ptr<PKFilter>>& object, u16 formCount)
    : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")), object(object), hid(30, 6), formCount(formCount)
{
    hid.update(30);
    if (object.index() == 0)
    {
        hid.select(std::get<0>(object)->alternativeForm());
    }
    else
    {
        hid.select(std::get<1>(object)->alternativeForm());
    }
}

void FormOverlay::drawBottom() const
{
    dim();
    Gui::text(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
}

void FormOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_5x6_idx, 0, 0);

    int selectorX = (hid.index() % 6) * 67;
    int selectorY = (hid.index() / 6) * 48;
    int dx        = 66;
    int dy        = 47;
    if (selectorX == 335)
    {
        dx -= 1;
    }
    // Selector
    Gui::drawSolidRect(selectorX, selectorY, dx, dy, COLOR_MASKBLACK);
    Gui::drawSolidRect(selectorX, selectorY, dx, 1, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY, 1, dy, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX + dx - 1, selectorY, 1, dy, COLOR_YELLOW);
    Gui::drawSolidRect(selectorX, selectorY + dy - 1, dx, 1, COLOR_YELLOW);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 6; x++)
        {
            if (x + y * 6 >= formCount)
            {
                break;
            }
            switch (object.index())
            {
                case 0:
                {
                    Gui::pkm(std::get<0>(object)->species(), x + y * 6, std::get<0>(object)->generation(), std::get<0>(object)->gender(), x * 66 + 19,
                        y * 48 + 1);
                    const std::string& text = i18n::form(
                        Configuration::getInstance().language(), std::get<0>(object)->generation(), std::get<0>(object)->species(), x + y * 6);
                    Gui::text(
                        text, x * 67 + 32, y * 48 + 39, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 65.0f);
                }
                break;
                case 1:
                {
                    Gui::pkm(std::get<1>(object)->species(), x + y * 6, std::get<1>(object)->generation(), std::get<1>(object)->gender(), x * 66 + 19,
                        y * 48 + 1);
                    const std::string& text = i18n::form(
                        Configuration::getInstance().language(), std::get<1>(object)->generation(), std::get<1>(object)->species(), x + y * 6);
                    Gui::text(
                        text, x * 67 + 32, y * 48 + 39, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 65.0f);
                }
                break;
            }
        }
    }
}

void FormOverlay::update(touchPosition* touch)
{
    hid.update(formCount);
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        switch (object.index())
        {
            case 0:
                std::get<0>(object)->alternativeForm(hid.fullIndex());
                break;
            case 1:
                std::get<1>(object)->alternativeForm(hid.fullIndex());
                break;
        }
        parent->removeOverlay();
        return;
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
}
