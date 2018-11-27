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

#ifndef ITEM_HPP
#define ITEM_HPP

#include <3ds.h>

struct Item {}; // Superstruct of all items

struct Item456 : public Item // Item before Gen 7
{
    unsigned int id : 16;
    unsigned int count : 16;
};

struct Item7 : public Item // Gen 7 item
{
    unsigned int id : 10;
    unsigned int count : 10;
    unsigned int freeSpaceIndex : 10;
    unsigned int newFlag : 1;
    unsigned int reserved : 1;
};

struct Item7b : public Item // LGPE item
{
    unsigned int id : 15;
    unsigned int count : 15;
    unsigned int newFlag : 1;
    unsigned int reserved : 1;
};

#endif