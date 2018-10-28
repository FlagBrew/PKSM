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

#include "HidVertical.hpp"

void HidVertical::update(size_t count)
{
    mCurrentTime = svcGetSystemTick();
    if (mCurrentTime <= mLastTime + DELAY_TICKS)
    {
        return;
    }

    mMaxPages = (count % mMaxVisibleEntries == 0) ? count / mMaxVisibleEntries : count / mMaxVisibleEntries + 1;
    u64 kHeld = hidKeysHeld();
    u64 kDown = hidKeysDown();
    
    if (kDown & KEY_L)
    {
        page_back();
    }
    else if (kDown & KEY_R)
    {
        page_forward();
    }
    else if (mColumns > 1)
    {
        if (kHeld & KEY_LEFT)
        {
            if (mIndex % rows != mIndex)
            {
                mIndex -= rows;
            }
            else
            {
                page_back();
                mIndex += rows;
            }
        }
        else if (kHeld & KEY_RIGHT)
        {
            if (mIndex + rows < mMaxVisibleEntries)
            {
                mIndex += rows;
            }
            else
            {
                page_forward();
                mIndex -= rows;
            }
        }
        else if (kHeld & KEY_UP)
        {
            if (mIndex % rows > 0)
            {
                mIndex -= 1;
            }
            else
            {
                page_back();
                mIndex = mIndex == rows ? mMaxVisibleEntries - 1 : ((mMaxVisibleEntries - 1) % rows);
            }
        }
        else if (kHeld & KEY_DOWN)
        {
            if ((mIndex % rows) < rows - 1)
            {
                mIndex += 1;
            }
            else
            {
                page_forward();
                mIndex = mIndex == rows - 1 ? 0 : rows;
            }
        }
    }
    else
    {
        if (kHeld & KEY_UP)
        {
            if (mIndex > 0) 
            {
                mIndex--;
            }
            else if (mIndex == 0)
            {
                page_back();
                mIndex = maxEntries(count);
            }
        }
        else if (kHeld & KEY_DOWN)
        {
            if (mIndex < maxEntries(count))
            {
                mIndex++;
            }
            else if (mIndex == maxEntries(count))
            {
                page_forward();
                mIndex = 0;
            }
        }
    }

    if (mIndex > maxEntries(count))
    {
        mIndex = maxEntries(count);
    }
    mLastTime = mCurrentTime;
}