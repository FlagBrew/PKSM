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

#include "BZ2File.hpp"
#include <algorithm>

int BZ2File::read(FILE* rawfile, std::vector<u8>& out)
{
    int bzerror;
    BZFILE* file = BZ2_bzReadOpen(&bzerror, rawfile, 0, false, nullptr, 0);
    if (bzerror != BZ_OK)
    {
        return bzerror;
    }

    out.clear();

    u8 read_buffer[READ_SIZE];

    while (bzerror == BZ_OK)
    {
        int actuallyRead = BZ2_bzRead(&bzerror, file, read_buffer, READ_SIZE);
        if (bzerror == BZ_OK || bzerror == BZ_STREAM_END)
        {
            out.insert(out.end(), read_buffer, read_buffer + actuallyRead);
        }
    }

    int garbageError;
    BZ2_bzReadClose(&garbageError, file);

    if (bzerror != BZ_STREAM_END)
    {
        out.clear();
        return bzerror;
    }

    return BZ_OK;
}

int BZ2File::write(FILE* rawfile, const u8* data, std::size_t size)
{
    int bzerror;
    BZFILE* file = BZ2_bzWriteOpen(&bzerror, rawfile, 5, 0, 0);
    if (bzerror != BZ_OK)
    {
        return bzerror;
    }

    BZ2_bzWrite(&bzerror, file, const_cast<u8*>(data), size);

    if (bzerror != BZ_OK)
    {
        int garbageError;
        BZ2_bzWriteClose(&garbageError, file, true, nullptr, nullptr);
        return bzerror;
    }

    BZ2_bzWriteClose(&bzerror, file, false, nullptr, nullptr);

    return bzerror;
}
