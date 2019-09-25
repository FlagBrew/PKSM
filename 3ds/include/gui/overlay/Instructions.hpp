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

#include "TextPos.hpp"
#include "colors.hpp"
#include "types.h"
#include <string>
#include <vector>

class Instructions
{
public:
    Instructions(const std::string& simpleInstructions = "");
#if defined(_3DS)
    void drawTop() const;
    void drawBottom() const;
#elif defined(__SWITCH__)
    void draw() const;
#endif
    void addBox(bool top, int x, int y, int width, int height, PKSM_Color color, const std::string& text = "", PKSM_Color textColor = COLOR_BLACK);
    void addText(bool top, int x, int y, int maxWidth, TextPosX xPos, TextPosY yPos, PKSM_Color color, const std::string& text);
    void addCircle(bool top, int x, int y, int radius, PKSM_Color color);
    void addLine(bool top, int x1, int y1, int x2, int y2, int width, PKSM_Color color);
    bool empty() const { return boxes.empty() && texts.empty() && circles.empty() && lines.empty(); }

private:
    void dim() const;
    struct Box
    {
        Box(bool top, int x, int y, int w, int h, PKSM_Color color) : top(top), x(x), y(y), w(w), h(h), color(color) {}
        bool top;
        int x;
        int y;
        int w;
        int h;
        PKSM_Color color;
    };
    struct Line
    {
        Line(bool top, int x1, int y1, int x2, int y2, int w, PKSM_Color color) : top(top), x1(x1), y1(y1), x2(x2), y2(y2), w(w), color(color) {}
        bool top;
        int x1;
        int y1;
        int x2;
        int y2;
        int w;
        PKSM_Color color;
    };
    struct Text
    {
        Text(bool top, int x, int y, int maxWidth, TextPosX xPos, TextPosY yPos, PKSM_Color color, const std::string& string)
            : top(top), x(x), y(y), maxWidth(maxWidth), xPos(xPos), yPos(yPos), color(color), string(string)
        {
        }
        bool top;
        int x;
        int y;
        int maxWidth;
        TextPosX xPos;
        TextPosY yPos;
        PKSM_Color color;
        std::string string;
    };
    struct Circle
    {
        Circle(bool top, int x, int y, int radius, PKSM_Color color) : top(top), x(x), y(y), radius(radius), color(color) {}
        bool top;
        int x;
        int y;
        int radius;
        PKSM_Color color;
    };
    std::vector<Box> boxes;
    std::vector<Line> lines;
    std::vector<Text> texts;
    std::vector<Circle> circles;
};

#endif
