/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Instructions.hpp"
#include "gui.hpp"

Instructions::Instructions(const std::string& simpleInstructions)
{
    if (!simpleInstructions.empty())
    {
        auto instrText = Gui::parseText(simpleInstructions, FONT_SIZE_12, 300);
        int height     = fontGetInfo(NULL)->lineFeed * FONT_SIZE_12 * instrText->lines() + 20;
        addBox(true, 40, 220 - height, 320, height, COLOR_GREY);
        addBox(true, 44, 224 - height, 312, height - 8, COLOR_DARKGREY, simpleInstructions,
            COLOR_WHITE);
    }
}

void Instructions::addBox(bool top, int x, int y, int width, int height, PKSM_Color color,
    const std::string& text, PKSM_Color textColor)
{
    boxes.emplace_back(top, x, y, width, height, color);
    if (!text.empty())
    {
        addText(top, x + width / 2, y + height / 2, width, TextPosX::CENTER, TextPosY::CENTER,
            textColor, text);
    }
}

void Instructions::addLine(bool top, int x1, int y1, int x2, int y2, int w, PKSM_Color color)
{
    lines.emplace_back(top, x1, y1, x2, y2, w, color);
}

void Instructions::addText(bool top, int x, int y, int maxWidth, TextPosX xPos, TextPosY yPos,
    PKSM_Color color, const std::string& text)
{
    texts.emplace_back(top, x, y, maxWidth, xPos, yPos, color, text);
}

void Instructions::addCircle(bool top, int x, int y, int radius, PKSM_Color color)
{
    circles.emplace_back(top, x, y, radius, color);
}

void Instructions::drawTop() const
{
    dim();

    for (const auto& box : boxes)
    {
        if (box.top)
        {
            Gui::drawSolidRect(box.x, box.y, box.w, box.h, box.color);
        }
    }
    for (const auto& line : lines)
    {
        if (line.top)
        {
            Gui::drawLine(line.x1, line.y1, line.x2, line.y2, line.w, line.color);
        }
    }
    for (const auto& circle : circles)
    {
        if (circle.top)
        {
            Gui::drawSolidCircle(circle.x, circle.y, circle.radius, circle.color);
        }
    }
    for (const auto& text : texts)
    {
        if (text.top)
        {
            Gui::text(text.string, text.x, text.y, FONT_SIZE_12, text.color, text.xPos, text.yPos,
                TextWidthAction::WRAP, text.maxWidth);
        }
    }
}

void Instructions::drawBottom() const
{
    dim();

    for (const auto& box : boxes)
    {
        if (!box.top)
        {
            Gui::drawSolidRect(box.x, box.y, box.w, box.h, box.color);
        }
    }
    for (const auto& line : lines)
    {
        if (!line.top)
        {
            Gui::drawLine(line.x1, line.y1, line.x2, line.y2, line.w, line.color);
        }
    }
    for (const auto& circle : circles)
    {
        if (!circle.top)
        {
            Gui::drawSolidCircle(circle.x, circle.y, circle.radius, circle.color);
        }
    }
    for (const auto& text : texts)
    {
        if (!text.top)
        {
            Gui::text(text.string, text.x, text.y, FONT_SIZE_12, text.color, text.xPos, text.yPos,
                TextWidthAction::WRAP, text.maxWidth);
        }
    }
}

void Instructions::dim() const
{
    Gui::drawSolidRect(0, 0, 400, 240, COLOR_MASKBLACK);
}
