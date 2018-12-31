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

#include "Item.hpp"

Item::operator Item4() const
{
    Item4 ret;
    ret.id(id());
    ret.count(count());
    return ret;
}

Item::operator Item5() const
{
    Item5 ret;
    ret.id(id());
    ret.count(count());
    return ret;
}

Item::operator Item6() const
{
    Item6 ret;
    ret.id(id());
    ret.count(count());
    return ret;
}

Item::operator Item7() const
{
    Item7 ret;
    ret.id(std::min((int)id(), 0x3FF));
    ret.count(std::min((int)count(), 0x3FF));
    ret.freeSpaceIndex(0);
    ret.newFlag(false);
    ret.reserved(false);
    return ret;
}

Item::operator Item7b() const
{
    Item7b ret;
    ret.id(std::min((int)id(), 0x7FFF));
    ret.count(std::min((int)count(), 0x7FFF));
    ret.newFlag(false);
    ret.reserved(false);
    return ret;
}

Item7::operator Item7b() const
{
    Item7b ret;
    ret.id(id()); // Capped at 0x3FF, so no need to cap it at 0x7FFF
    ret.count(count());
    ret.newFlag(newFlag());
    ret.reserved(reserved());
    return ret;
}

Item7b::operator Item7() const
{
    Item7 ret;
    ret.id(std::min((int)id(), 0x3FF));
    ret.count(std::min((int)count(), 0x3FF));
    ret.freeSpaceIndex(0);
    ret.newFlag(newFlag());
    ret.reserved(reserved());
    return ret;
}