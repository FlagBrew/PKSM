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

#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include "types.h"
#include <string>
#include <vector>
#include "colors.hpp"
#include "TextPos.hpp"

class Instructions
{
public:
    Instructions(const std::string& simpleInstructions = "");
    void draw() const;
    void addBox(bool top, int x, int y, int width, int height, u32 color, const std::string& text = "", u32 textColor = COLOR_BLACK);
    void addText(bool top, int x, int y, int maxWidth, TextPosX xPos, TextPosY yPos, u32 color, const std::string& text);
    void addCircle(bool top, int x, int y, int radius, u32 color);
    bool empty() const { return boxes.empty() && texts.empty() && circles.empty(); }
private:
    void dim() const;
    struct Box
    {
        Box(bool top, int x, int y, int w, int h, u32 color)
            : top(top), x(x), y(y), w(w), h(h), color(color) {}
        bool top;
        int x;
        int y;
        int w;
        int h;
        u32 color;
    };
    struct Text
    {
        Text(bool top, int x, int y, TextPosX xPos, TextPosY yPos, u32 color, std::string string)
             : top(top), x(x), y(y), xPos(xPos), yPos(yPos), color(color), string(string) {}
        bool top;
        int x;
        int y;
        TextPosX xPos;
        TextPosY yPos;
        u32 color;
        std::string string;
    };
    struct Circle
    {
        Circle(bool top, int x, int y, int radius, u32 color)
             : top(top), x(x), y(y), radius(radius), color(color) {}
        bool top;
        int x;
        int y;
        int radius;
        u32 color;
    };
    std::vector<Box> boxes;
    std::vector<Text> texts;
    std::vector<Circle> circles;
};

#endif
