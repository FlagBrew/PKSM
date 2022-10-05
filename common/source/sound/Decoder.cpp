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

#include <cstdio>
#include <cstring>

#include "Decoder.hpp"

#include "mp3.hpp"

// #include "vorbis.hpp"
// #include "wav.hpp"

std::unique_ptr<Decoder> Decoder::get(const std::string& fileName)
{
    FILE* fp              = fopen(fileName.c_str(), "r");
    std::string extension = fileName.substr(fileName.find_last_of(".") + 1);
    char magic[4];

    if (!fp)
    {
        // fprintf(stderr, "Err: Tried to get magic an audio file that does not exist?!");
        return nullptr;
    }

    fseek(fp, 0, SEEK_SET);
    fread(magic, 1, 4, fp);
    fclose(fp);

    // Wave
    /* if (!strncmp(magic, "RIFF", 4))
    {
        // fprintf(stderr, "Decoder: Using wav.");
        auto wavdec = std::make_unique<WavDecoder>(fileName);
        if (wavdec->good())
            return wavdec;
    }
    // Ogg or Opus
    else if (!strncmp(magic, "OggS", 4))
    {
        if (isVorbis(fileName) == 0)
        {
            // fprintf(stderr, "Decoder: Using ogg vorbis");
            auto vorbisdec = std::make_unique<VorbisDecoder>(fileName);
            if (vorbisdec->good())
                return vorbisdec;
        }
        // else
        //     fprintf(stderr, "Decoder: Unknown ogg codec");
    } */
    /*Mp3*/
    if (!strncasecmp(extension.c_str(), "MP3", 3))
    {
        // fprintf(stderr, "Decoder: Using mpeg3");
        auto mp3dec = std::make_unique<Mp3Decoder>(fileName);
        if (mp3dec->good())
        {
            return mp3dec;
        }
    }

    // fprintf(stderr, "Err: Failed to initalize decoder.");
    return nullptr;
}
