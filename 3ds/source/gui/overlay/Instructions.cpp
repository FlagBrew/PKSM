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

#include "Instructions.hpp"
#include "gui.hpp"

Instructions::Instructions(const std::string& simpleInstructions)
{
    if (!simpleInstructions.empty())
    {
        int lines = 1;
        std::string wrapped = StringUtils::wrap(simpleInstructions, FONT_SIZE_12, 300);
        for (auto i = wrapped.begin(); i != wrapped.end(); i++)
        {
            if (*i == '\n')
            {
                lines++;
            }
        }
        int height = fontGetInfo()->lineFeed * (lines + 2);
        addBox(true, 40, 220 - height, 320, height, COLOR_GREY);
        addBox(true, 44, 224 - height, 312, height - 8, COLOR_DARKGREY, wrapped, COLOR_WHITE);
    }
}

void Instructions::addBox(bool top, int x, int y, int width, int height, u32 color, const std::string& text, u32 textColor)
{
    boxes.emplace_back(top, x, y, width, height, color);
    if (!text.empty())
    {
        addText(top, x + width/2, y + height/2, width, TextPosX::CENTER, TextPosY::CENTER, textColor, text);
    }
}

void Instructions::addText(bool top, int x, int y, int maxWidth, TextPosX xPos, TextPosY yPos, u32 color, const std::string& text)
{
    texts.emplace_back(top, x, y, xPos, yPos, color, StringUtils::wrap(text, FONT_SIZE_12, maxWidth));
}

void Instructions::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    dim();
    C2D_SceneBegin(g_renderTargetBottom);
    dim();
    C3D_RenderTarget* target = nullptr;

    for (auto& box : boxes)
    {
        target = box.top ? g_renderTargetTop : g_renderTargetBottom;
        C2D_SceneBegin(target);
        C2D_DrawRectSolid(box.x, box.y, 0.5f, box.w, box.h, box.color);
    }
    for (auto& text : texts)
    {
        Gui::dynamicText(text.string, text.x, text.y, FONT_SIZE_12, FONT_SIZE_12, text.color, text.xPos, text.yPos);
    }
}

void Instructions::dim() const
{
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, COLOR_MASKBLACK);
}
