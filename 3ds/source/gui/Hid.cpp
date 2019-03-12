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

#include "Hid.hpp"

size_t Hid::index(void) const
{
    return mIndex;
}

size_t Hid::fullIndex(void) const
{
    return mIndex + mPage * mMaxVisibleEntries;
}

int Hid::page(void) const
{
    return mPage;
}

size_t Hid::maxVisibleEntries(void) const
{
    return mMaxVisibleEntries;
}

size_t Hid::maxEntries(size_t count) const
{
    return (count - mPage*mMaxVisibleEntries) > mMaxVisibleEntries ? mMaxVisibleEntries - 1 : count - mPage*mMaxVisibleEntries - 1;
}

void Hid::page_back(void)
{
    if (mPage > 0) 
    {
        mPage--;
    }
    else if (mPage == 0)
    {
        mPage = mMaxPages - 1;
    }
}

void Hid::page_forward(void)
{
    if (mPage < (int)mMaxPages - 1)
    {
        mPage++;
    }
    else if (mPage == (int)mMaxPages - 1)
    {		
        mPage = 0;
    }
}

void Hid::select(size_t index)
{
    mIndex = index % mMaxVisibleEntries;
    mPage = index / mMaxVisibleEntries;
}