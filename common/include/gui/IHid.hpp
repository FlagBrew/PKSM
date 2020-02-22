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

#ifndef IHID_HPP
#define IHID_HPP

#include "types.h"

enum class HidDirection
{
    VERTICAL,
    HORIZONTAL
};

template <HidDirection ListDirection, HidDirection PageDirection, u64 Delay>
class IHid
{
public:
    IHid(size_t entries, size_t columns)
    {
        mMaxVisibleEntries = entries;
        mColumns           = columns;
        mRows              = entries / columns;
        mIndex             = 0;
        mPage              = 0;
        mMaxPages          = 0;
        mLastTime          = 0;
    }
    virtual ~IHid() {}

    size_t index(void) const { return mIndex; }
    size_t maxVisibleEntries(void) const { return mMaxVisibleEntries; }
    int page(void) const { return mPage; }
    size_t fullIndex(void) const { return mIndex + mPage * mMaxVisibleEntries; }
    size_t maxEntries(size_t count) const
    {
        return (count - mPage * mMaxVisibleEntries) > mMaxVisibleEntries ? mMaxVisibleEntries - 1 : count - mPage * mMaxVisibleEntries - 1;
    }
    void page_back(void)
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
    void page_forward(void)
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
    void select(size_t index)
    {
        mIndex = index % mMaxVisibleEntries;
        mPage  = index / mMaxVisibleEntries;
    }

    void update(size_t count);

private:
    size_t mIndex;
    int mPage;
    size_t mMaxPages;
    size_t mMaxVisibleEntries;
    size_t mColumns;
    size_t mRows;
    u64 mLastTime;

    virtual bool downDown() const         = 0;
    virtual bool upDown() const           = 0;
    virtual bool leftDown() const         = 0;
    virtual bool rightDown() const        = 0;
    virtual bool leftTriggerDown() const  = 0;
    virtual bool rightTriggerDown() const = 0;
    virtual bool downHeld() const         = 0;
    virtual bool upHeld() const           = 0;
    virtual bool leftHeld() const         = 0;
    virtual bool rightHeld() const        = 0;
    virtual bool leftTriggerHeld() const  = 0;
    virtual bool rightTriggerHeld() const = 0;
    virtual u64 tick() const              = 0;
};

#include "IHid.tcc"

#endif
