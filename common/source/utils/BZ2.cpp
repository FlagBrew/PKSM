/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "BZ2.hpp"
#include <algorithm>
#include <memory>

int BZ2::decompress(FILE* rawfile, std::vector<u8>& out)
{
    int bzerror;
    BZFILE* file = BZ2_bzReadOpen(&bzerror, rawfile, 0, false, nullptr, 0);
    if (bzerror != BZ_OK)
    {
        return bzerror;
    }

    out.clear();

    {
        std::unique_ptr<u8[]> read_buffer = std::unique_ptr<u8[]>(new u8[READ_SIZE]);

        while (bzerror == BZ_OK)
        {
            int actuallyRead = BZ2_bzRead(&bzerror, file, read_buffer.get(), READ_SIZE);
            if (bzerror == BZ_OK || bzerror == BZ_STREAM_END)
            {
                out.insert(out.end(), read_buffer.get(), read_buffer.get() + actuallyRead);
            }
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

int BZ2::compress(FILE* rawfile, const u8* data, std::size_t size)
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

int BZ2::decompress(const u8* data, std::size_t size, std::vector<u8>& out)
{
    FILE* f   = fmemopen(const_cast<u8*>(data), size, "rb");
    int error = decompress(f, out);
    fclose(f);
    return error;
}

int BZ2::compress(std::vector<u8>& out, const u8* data, std::size_t size)
{
    bz_stream strm = {.bzalloc = NULL, .bzfree = NULL, .opaque = NULL};

    int bzerror = BZ2_bzCompressInit(&strm, 5, 0, 0);
    if (bzerror != BZ_OK)
    {
        return bzerror;
    }

    std::size_t sizeCompressed = 0;

    std::unique_ptr<char[]> workBuf = std::unique_ptr<char[]>(new char[READ_SIZE]);

    out.clear();

    strm.avail_out = READ_SIZE;
    strm.avail_in  = size;
    strm.next_in   = (char*)data;
    strm.next_out  = workBuf.get();

    bzerror = BZ2_bzCompress(&strm, BZ_RUN);

    while (bzerror == BZ_FLUSH_OK || bzerror == BZ_RUN_OK || bzerror == BZ_FINISH_OK)
    {
        out.insert(out.end(), workBuf.get(), strm.next_out);

        strm.next_out = workBuf.get();

        bzerror = BZ2_bzCompress(&strm, BZ_FINISH);
    }

    if (bzerror != BZ_FINISH_OK)
    {
        out.clear();
        return bzerror;
    }
    else
    {
        out.insert(out.end(), workBuf.get(), strm.next_out);
    }

    return BZ_OK;
}
