/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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

#include <stdlib.h>
#include <string.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include "vorbis.hpp"

static OggVorbis_File        vorbisFile;
static vorbis_info        *vi;
static FILE            *f;
static const size_t        buffSize = 8 * 4096;

static uint64_t fillVorbisBuffer(char* bufferOut);

VorbisDecoder::VorbisDecoder(const char* filename) {
    if((f = fopen(filename, "rb")) == NULL)
        return;

    if(ov_open(f, &vorbisFile, NULL, 0) < 0)
        return;

    if((vi = ov_info(&vorbisFile, -1)) == NULL)
        return;

    initialized = true;
}

VorbisDecoder::~VorbisDecoder(void) {
    ov_clear(&vorbisFile);
    fclose(f);
    initialized = false;
}

uint32_t VorbisDecoder::pos(void) {
    return ov_pcm_tell(&vorbisFile);
}

uint32_t VorbisDecoder::length(void) {
    return ov_pcm_total(&vorbisFile, -1);
}

uint32_t VorbisDecoder::decode(void* buffer)
{
    return fillVorbisBuffer(reinterpret_cast<char*>(buffer));
}

bool VorbisDecoder::stereo(void)
{
    return vi->channels-1;
}

uint32_t VorbisDecoder::sampleRate(void)
{
    return vi->rate;
}

uint32_t VorbisDecoder::bufferSize(void)
{
    return buffSize;
}

int isVorbis(const char *in)
{
    FILE *ft = fopen(in, "r");
    OggVorbis_File testvf;
    int err;

    if(ft == NULL)
        return -1;

    err = ov_test(ft, &testvf, NULL, 0);

    ov_clear(&testvf);
    fclose(ft);
    return err;
}

static uint64_t fillVorbisBuffer(char* bufferOut)
{
    uint64_t samplesRead = 0;
    int samplesToRead = buffSize;

    while(samplesToRead > 0)
    {
        static int current_section;
        int samplesJustRead =
            ov_read(&vorbisFile, bufferOut,
                    samplesToRead > 4096 ? 4096    : samplesToRead,
                    &current_section);

        if(samplesJustRead < 0)
            return samplesJustRead;
        else if(samplesJustRead == 0)
        {
            /* End of file reached. */
            break;
        }

        samplesRead += samplesJustRead;
        samplesToRead -= samplesJustRead;
        bufferOut += samplesJustRead;
    }

    return samplesRead / sizeof(int16_t);
}
