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

#include "opus.hpp"

OpusDecoder::OpusDecoder(const std::string& filename) {
    int err = 0;

    if(((opusFile = op_open_file(filename.c_str(), &err)) == NULL) && err != OPUS_OK) {
        fprintf(stderr, "Err: Opus decoder failed to init %d.", err);
        return;
    }

    if((err = op_current_link(opusFile)) < 0)
        return;

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

int isOpus(const std::string& in)
{
    int err = 0;
    OggOpusFile* opusTest = op_test_file(in.c_str(), &err);

    op_free(opusTest);
    return err;
}

uint64_t OpusDecoder::fillOpusBuffer(int16_t* bufferOut)
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
