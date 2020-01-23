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

#ifndef HID_HPP
#define HID_HPP

#include "types.h"

#define DELAY_TICKS 50000000

class Hid
{
public:
    virtual ~Hid(void) {}

    size_t fullIndex(void) const;
    size_t index(void) const;
    size_t maxEntries(size_t max) const;
    size_t maxVisibleEntries(void) const;
    int page(void) const;
    virtual void update(size_t count) = 0;
    void page_back(void);
    void page_forward(void);
    void select(size_t index);

protected:
    size_t mIndex;
    int mPage;
    size_t mMaxPages;
    size_t mMaxVisibleEntries;
    size_t mColumns;
    size_t mRows;
    u64 mCurrentTime;
    u64 mLastTime;
};

#endif
