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

#ifndef MP3DECODER_HPP
#define MP3DECODER_HPP

#include "types.h"
#include <memory>
#include <mpg123.h>
#include <string>

// One open MP3 stream. Sound is the only caller: it opens a song, asks for its
// format once, then pulls PCM until the stream runs dry.
class Mp3Decoder
{
public:
    // mpg123 wants one process-wide setup and teardown
    static bool initLibrary();
    static void exitLibrary();

    // nullptr when the file is not a playable MP3
    static std::unique_ptr<Mp3Decoder> open(const std::string& fileName);

    ~Mp3Decoder();

    Mp3Decoder(const Mp3Decoder&)            = delete;
    Mp3Decoder& operator=(const Mp3Decoder&) = delete;

    // Number of PCM samples written; 0 once the stream is over
    u32 decode(void* buffer, size_t bufferSize);

    bool stereo() const { return channels > 1; }

    u32 sampleRate() const { return rate; }

private:
    Mp3Decoder(mpg123_handle* handle, long sampleRate, int channelCount)
        : mh(handle), rate(sampleRate), channels(channelCount)
    {
    }

    mpg123_handle* mh;
    long rate;
    int channels;
};

#endif
