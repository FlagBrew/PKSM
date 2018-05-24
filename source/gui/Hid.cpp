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

#include "Hid.hpp"

size_t Hid::index(void)
{
    return mIndex;
}

size_t Hid::fullIndex(void)
{
    return mIndex + mPage * mMaxVisibleEntries;
}

int Hid::page(void)
{
    return mPage;
}

size_t Hid::maxVisibleEntries(void)
{
    return mMaxVisibleEntries;
}

size_t Hid::maxEntries(size_t count)
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

void Hid::update(size_t count) {
    size_t maxentries = maxEntries(count);
    mMaxPages = (count % mMaxVisibleEntries == 0) ? count / mMaxVisibleEntries : count / mMaxVisibleEntries + 1;

    u64 kHeld = hidKeysHeld();
    u64 kDown = hidKeysDown();
    bool sleep = false;
    
    if (kDown & KEY_L)
    {
        page_back();
        if (mIndex > maxEntries(count))
        {
            mIndex = maxEntries(count);
        }
    }
    else if (kDown & KEY_R)
    {
        page_forward();
        if (mIndex > maxEntries(count))
        {
            mIndex = maxEntries(count);
        }
    }
    else if (mColumns > 1)
    {
        if (kHeld & KEY_LEFT)
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
            sleep = true;
        }
        else if (kHeld & KEY_RIGHT)
        {
            if (mIndex < maxentries)
            {
                mIndex++;
            }
            else if (mIndex == maxentries)
            {
                page_forward();
                mIndex = 0;
            }
            sleep = true;
        }
        else if (kHeld & KEY_UP)
        {
            if (mIndex <= mColumns - 1)
            {
                page_back();
                if (mIndex > maxEntries(count))
                {
                    mIndex = maxEntries(count);
                }
            }
            else if (mIndex >= mColumns)
            {			
                mIndex -= mColumns;
            }
            sleep = true;
        }
        else if (kHeld & KEY_DOWN)
        {
            if ((int)(maxentries - mColumns) >= 0)
            {
                if (mIndex <= maxentries - mColumns)
                {
                    mIndex += mColumns;
                }
                else if (mIndex >= maxentries - mColumns + 1)
                {
                    page_forward();
                    if (mIndex > maxEntries(count))
                    {
                        mIndex = maxEntries(count);
                    }
                }
                sleep = true;
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
            sleep = true;
        }
        else if (kHeld & KEY_DOWN)
        {
            if (mIndex < maxentries)
            {
                mIndex++;
            }
            else if (mIndex == maxentries)
            {
                page_forward();
                mIndex = 0;
            }
            sleep = true;
        }
    }

    if (sleep)
    {
        svcSleepThread(FASTSCROLL_WAIT);
    }
}