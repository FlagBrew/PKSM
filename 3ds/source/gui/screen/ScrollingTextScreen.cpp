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

#include "ScrollingTextScreen.hpp"
#include "ViewOverlay.hpp"
#include "gui.hpp"

ScrollingTextScreen::ScrollingTextScreen(const std::string& text, std::shared_ptr<PKX> pk)
    : Screen(i18n::localize("UP_SCROLL_UP") + '\n' + i18n::localize("DOWN_SCROLL_DOWN") + '\n' + i18n::localize("B_BACK")),
      text(Gui::parseText(text, FONT_SIZE_12, 300.0f)),
      pkm(pk)
{
    if (pkm)
    {
        addOverlay<ViewOverlay>(this->pkm, false, "");
    }
}

void ScrollingTextScreen::update(touchPosition* touch)
{
    u32 down = hidKeysDown();
    if (down & KEY_DOWN && lineOffset + SHOWN_LINES < text->lineWidths.size())
    {
        lineOffset++;
    }
    if (down & KEY_UP && lineOffset > 0)
    {
        lineOffset--;
    }
    if (down & KEY_B)
    {
        Gui::screenBack();
        return;
    }
}

void ScrollingTextScreen::drawTop() const
{
    if (!overlay)
    {
        Gui::backgroundTop(true);
        Gui::backgroundAnimatedTop();
    }
}

void ScrollingTextScreen::drawBottom() const
{
    static const float lineMod = fontGetInfo(nullptr)->lineFeed * FONT_SIZE_12;
    Gui::drawSolidRect(0, 0, 320, 240, COLOR_MENUBLUE);
    Gui::drawSolidRect(10, 5, 300, 230, COLOR_WHITE);
    Gui::drawSolidRect(5, 10, 310, 220, COLOR_WHITE);
    Gui::drawSolidCircle(10, 10, 5, COLOR_WHITE);
    Gui::drawSolidCircle(310, 230, 5, COLOR_WHITE);
    Gui::drawSolidCircle(310, 10, 5, COLOR_WHITE);
    Gui::drawSolidCircle(10, 230, 5, COLOR_WHITE);

    Gui::text(text, 10, 7 - lineMod * lineOffset, FONT_SIZE_12, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
    Gui::drawSolidRect(0, 0, 320, 5, COLOR_MENUBLUE);
    Gui::drawSolidRect(0, 235, 320, 5, COLOR_MENUBLUE);

    if (text->lineWidths.size() > SHOWN_LINES && lineOffset + SHOWN_LINES < text->lineWidths.size())
    {
        Gui::drawSolidTriangle(310, 220, 300, 220, 305, 230, COLOR_BLACK);
    }
    if (text->lineWidths.size() > SHOWN_LINES && lineOffset != 0)
    {
        Gui::drawSolidTriangle(310, 20, 300, 20, 305, 10, COLOR_BLACK);
    }
}
