/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef CLICKABLE_HPP
#define CLICKABLE_HPP

#include <functional>
#include <3ds.h>

class Clickable
{
public:
    Clickable(int x, int y, u16 w, u16 h, std::function<bool()> function)
            : xPos(x), yPos(y), width(w), height(h), noArg(function) {}
    virtual ~Clickable() { };

    // returns return value of callback, or, if it's not executed, false
    virtual bool update(touchPosition* touch);
    virtual void draw() const = 0;
    // Useful for toggle buttons that need to only be used once and not again until they're released
    virtual bool clicked(touchPosition* touch)
    {
        return touch->px >= xPos && touch->px <= xPos + width && touch->py >= yPos && touch->py <= yPos + height;
    }

protected:
    int xPos, yPos;
    u16 width, height;
    std::function<bool()> noArg;
};

#endif