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

#include <opusfile.h>

#include "opus.hpp"

static OggOpusFile*        opusFile;
static const OpusHead*    opusHead;
static const size_t        buffSize = 32 * 1024;

static uint64_t fillOpusBuffer(int16_t* bufferOut);


OpusDecoder::OpusDecoder(const char* filename) {
    int err = 0;

    if((opusFile = op_open_file(filename, &err)) == NULL)
        return;

    if((err = op_current_link(opusFile)) < 0)
        return;

    opusHead = op_head(opusFile, err);

    initialized = true;
}

OpusDecoder::~OpusDecoder(void) {
    op_free(opusFile);
    initialized = false;
}

uint32_t OpusDecoder::pos(void) {
    return op_pcm_tell(opusFile);
}

uint32_t OpusDecoder::length(void) {
    return op_pcm_total(opusFile, -1);
}

uint32_t OpusDecoder::decode(void* buffer)
{
    return fillOpusBuffer(static_cast<int16_t*>(buffer));
}

bool OpusDecoder::stereo(void)
{
    return true;
}

uint32_t OpusDecoder::sampleRate(void)
{
    return 48000;
}

uint32_t OpusDecoder::bufferSize(void)
{
    return buffSize;
}

int isOpus(const char* in)
{
    int err = 0;
    OggOpusFile* opusTest = op_test_file(in, &err);

    op_free(opusTest);
    return err;
}

static uint64_t fillOpusBuffer(int16_t* bufferOut)
{
    uint64_t samplesRead = 0;
    int samplesToRead = buffSize;

    while(samplesToRead > 0)
    {
        int samplesJustRead = op_read_stereo(opusFile, bufferOut,
                samplesToRead > 120*48*2 ? 120*48*2 : samplesToRead);

        if(samplesJustRead < 0)
            return samplesJustRead;
        else if(samplesJustRead == 0)
        {
            /* End of file reached. */
            break;
        }

        samplesRead += samplesJustRead * 2;
        samplesToRead -= samplesJustRead * 2;
        bufferOut += samplesJustRead * 2;
    }

    return samplesRead;
}
