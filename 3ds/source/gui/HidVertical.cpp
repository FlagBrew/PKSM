/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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
#include <3ds.h>

void HidVertical::update(size_t count)
{
    mCurrentTime = svcGetSystemTick();

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
        if (kDown & KEY_LEFT)
        {
            if (mIndex % mRows != mIndex)
            {
                mIndex -= mRows;
            }
            else
            {
                page_back();
                mIndex += mRows;
            }
        }
        else if (kDown & KEY_RIGHT)
        {
            if (maxEntries(count) < mRows)
            {
                page_forward();
            }
            else if (mIndex + mRows < mMaxVisibleEntries)
            {
                mIndex += mRows;
            }
            else
            {
                page_forward();
                mIndex -= mRows;
            }
        }
        else if (kDown & KEY_UP)
        {
            if (mIndex % mRows > 0)
            {
                mIndex -= 1;
            }
            else
            {
                mIndex = mIndex == mRows ? mMaxVisibleEntries - 1 : ((mMaxVisibleEntries - 1) % mRows);
            }
        }
        else if (kDown & KEY_DOWN)
        {
            if ((mIndex % mRows) < mRows - 1)
            {
                if (mIndex + mPage * mMaxVisibleEntries == count - 1)
                {
                    mIndex = (mIndex / mRows) * mRows;
                }
                else
                {
                    mIndex += 1;
                }
            }
            else
            {
                mIndex = mIndex == mRows - 1 ? 0 : mRows;
            }
        }
        else if (kHeld & KEY_LEFT)
        {
            if (mCurrentTime <= mLastTime + DELAY_TICKS)
            {
                return;
            }
            if (mIndex % mRows != mIndex)
            {
                mIndex -= mRows;
            }
            else
            {
                page_back();
                mIndex += mRows;
            }
        }
        else if (kHeld & KEY_RIGHT)
        {
            if (mCurrentTime <= mLastTime + DELAY_TICKS)
            {
                return;
            }
            if (maxEntries(count) < mRows)
            {
                page_forward();
            }
            else if (mIndex + mRows < mMaxVisibleEntries)
            {
                mIndex += mRows;
            }
            else
            {
                page_forward();
                mIndex -= mRows;
            }
        }
        else if (kHeld & KEY_UP)
        {
            if (mCurrentTime <= mLastTime + DELAY_TICKS)
            {
                return;
            }
            if (mIndex % mRows > 0)
            {
                mIndex -= 1;
            }
            else
            {
                mIndex = mIndex == mRows ? mMaxVisibleEntries - 1 : ((mMaxVisibleEntries - 1) % mRows);
            }
        }
        else if (kHeld & KEY_DOWN)
        {
            if (mCurrentTime <= mLastTime + DELAY_TICKS)
            {
                return;
            }
            if ((mIndex % mRows) < mRows - 1)
            {
                if (mIndex + mPage * mMaxVisibleEntries == count - 1)
                {
                    mIndex = (mIndex / mRows) * mRows;
                }
                else
                {
                    mIndex += 1;
                }
            }
            else
            {
                mIndex = mIndex == mRows - 1 ? 0 : mRows;
            }
        }
    }
    else
    {
        if (kDown & KEY_UP)
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
        else if (kDown & KEY_DOWN)
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
        else if (kHeld & KEY_UP)
        {
            if (mCurrentTime <= mLastTime + DELAY_TICKS)
            {
                return;
            }
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
            if (mCurrentTime <= mLastTime + DELAY_TICKS)
            {
                return;
            }
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
