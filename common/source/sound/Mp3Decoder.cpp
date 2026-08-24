/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Mp3Decoder.hpp"
#include <cstdint>
#include <cstring>

bool Mp3Decoder::initLibrary()
{
    return mpg123_init() == MPG123_OK;
}

void Mp3Decoder::exitLibrary()
{
    mpg123_exit();
}

std::unique_ptr<Mp3Decoder> Mp3Decoder::open(const std::string& fileName)
{
    std::string extension = fileName.substr(fileName.find_last_of(".") + 1);
    if (strncasecmp(extension.c_str(), "MP3", 3) != 0)
    {
        return nullptr;
    }

    int err               = 0;
    mpg123_handle* handle = mpg123_new(nullptr, &err);
    if (!handle)
    {
        return nullptr;
    }

    long rate    = 0;
    int channels = 0;
    int encoding = 0;
    if (mpg123_open(handle, fileName.c_str()) != MPG123_OK ||
        mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK)
    {
        mpg123_close(handle);
        mpg123_delete(handle);
        return nullptr;
    }

    // Pin the format we just read: without this mpg123 may switch mid-stream and
    // the channel would keep playing at the old rate
    mpg123_format_none(handle);
    mpg123_format(handle, rate, channels, encoding);

    return std::unique_ptr<Mp3Decoder>(new Mp3Decoder(handle, rate, channels));
}

Mp3Decoder::~Mp3Decoder()
{
    mpg123_close(mh);
    mpg123_delete(mh);
}

u32 Mp3Decoder::decode(void* buffer, size_t bufferSize)
{
    size_t done = 0;
    mpg123_read(mh, reinterpret_cast<unsigned char*>(buffer), bufferSize, &done);
    return done / sizeof(int16_t);
}
