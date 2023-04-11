/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp3.hpp"

Mp3Decoder::Mp3Decoder(const std::string& filename)
{
    int err      = 0;
    int encoding = 0;

    if ((mh = mpg123_new(NULL, &err)) == NULL)
    {
        // fprintf(stderr, "Error: %s\n", mpg123_plain_strerror(err));
        return;
    }

    if (mpg123_open(mh, filename.c_str()) != MPG123_OK ||
        mpg123_getformat(mh, (long*)&rate, (int*)&channels, &encoding) != MPG123_OK)
    {
        // fprintf(stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh));
        return;
    }

    /*
     * Ensure that this output format will not change (it might, when we allow
     * it).
     */
    mpg123_format_none(mh);
    mpg123_format(mh, rate, channels, encoding);

    initialized = true;
}

Mp3Decoder::~Mp3Decoder(void)
{
    mpg123_close(mh);
    mpg123_delete(mh);
    initialized = false;
}

uint32_t Mp3Decoder::pos(void)
{
    return mpg123_tell(mh);
}

uint32_t Mp3Decoder::length(void)
{
    return mpg123_length(mh);
}

uint32_t Mp3Decoder::decode(void* buffer, size_t bufferSize)
{
    size_t done = 0;
    mpg123_read(mh, reinterpret_cast<unsigned char*>(buffer), bufferSize, &done);
    return done / (sizeof(int16_t));
}

bool Mp3Decoder::stereo(void)
{
    return channels - 1;
}

uint32_t Mp3Decoder::sampleRate(void)
{
    return rate;
}
