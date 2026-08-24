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

#include "sound.hpp"
#include "Configuration.hpp"
#include "Mp3Decoder.hpp"
#include "random.hpp"
#include "STDirectory.hpp"
#include "thread.hpp"
#include "utils/logging.hpp"
#include <3ds.h>
#include <array>
#include <atomic>
#include <string>
#include <vector>

namespace
{
    constexpr const char* SONG_DIR = "/3ds/PKSM/songs";

    // The background music is the only thing PKSM plays, so it owns the one NDSP
    // channel and the two wave buffers that feed it
    constexpr int BGM_CHANNEL    = 0;
    constexpr size_t BUFFER_SIZE = 32 * 1024;
    constexpr size_t NUM_BUFFERS = 2;

    s16* bufferMem = nullptr;
    std::array<ndspWaveBuf, NUM_BUFFERS> buffers;
    std::unique_ptr<Mp3Decoder> decoder;
    LightEvent frameEvent;

    std::vector<std::string> bgm;
    size_t currentSong = 0;
    // Only set once init has claimed NDSP and the buffers; without songs there is
    // nothing to start or tear down
    bool ready                 = false;
    std::atomic<bool> playing  = false;
    std::atomic<bool> finished = true;

    void ndspFrameCallback(void*)
    {
        if (!playing)
        {
            return;
        }

        // FIXME: there's a nasty deadlock somewhere in here
        LightEvent_Signal(&frameEvent);
    }

    void fillBuffers()
    {
        for (auto& buffer : buffers)
        {
            if (buffer.status == NDSP_WBUF_DONE)
            {
                buffer.nsamples = decoder->decode((void*)buffer.data_pcm16, BUFFER_SIZE);
                // Correct size for stereo mode
                if (decoder->stereo())
                {
                    buffer.nsamples /= 2;
                }
                // Flush data if we actually decoded anything
                if (buffer.nsamples > 0)
                {
                    DSP_FlushDataCache(buffer.data_pcm16, BUFFER_SIZE);
                    ndspChnWaveBufAdd(BGM_CHANNEL, &buffer);
                }
                // Otherwise the song is over: drop the decoder and stop touching it
                else
                {
                    buffer.status = NDSP_WBUF_DONE;
                    decoder       = nullptr;
                    break;
                }
            }
        }
    }

    // Picks the next playable song, dropping entries that stopped decoding
    std::unique_ptr<Mp3Decoder> nextSong()
    {
        std::unique_ptr<Mp3Decoder> ret = nullptr;
        while (!ret && !bgm.empty())
        {
            if (Configuration::getInstance().randomMusic())
            {
                currentSong = pksm::randomNumber(0, bgm.size() - 1);
            }
            else
            {
                currentSong = (currentSong + 1) % bgm.size();
            }
            ret = Mp3Decoder::open(bgm[currentSong]);
            if (!ret)
            {
                bgm.erase(bgm.begin() + currentSong);
            }
        }
        return ret;
    }

    void playSong(std::unique_ptr<Mp3Decoder> song)
    {
        if (!song)
        {
            return;
        }

        ndspChnReset(BGM_CHANNEL);
        ndspChnSetInterp(BGM_CHANNEL, song->stereo() ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
        ndspChnSetRate(BGM_CHANNEL, song->sampleRate());
        ndspChnSetFormat(
            BGM_CHANNEL, song->stereo() ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

        // Set both buffers into the done state to be decoded into
        for (auto& buffer : buffers)
        {
            buffer.status = NDSP_WBUF_DONE;
        }

        decoder = std::move(song);
        fillBuffers();
    }

    void soundThread()
    {
        finished = false;
        while (playing)
        {
            u8 volume = 0;
            HIDUSER_GetSoundVolume(&volume);

            // Replace the song if the volume slider is pushed all the way down and we
            // haven't already replaced it, or the decoder is gone and the channel is
            // done or paused
            bool replaceSong =
                (volume == 0 && bgm.size() > 1 && !ndspChnIsPaused(BGM_CHANNEL)) ||
                (!decoder && (!ndspChnIsPlaying(BGM_CHANNEL) || ndspChnIsPaused(BGM_CHANNEL)));
            if (!replaceSong && decoder)
            {
                fillBuffers();
            }
            else if (!bgm.empty())
            {
                playSong(nextSong());
            }

            // Pause the song if the volume slider is all the way down
            ndspChnSetPaused(BGM_CHANNEL, volume == 0);

            LightEvent_Wait(&frameEvent);
        }
        finished = true;
    }

    void stopThread()
    {
        if (playing)
        {
            playing = false;
            // Signal and wait for sound thread to end
            LightEvent_Signal(&frameEvent);
            while (!finished)
            {
                svcSleepThread(125000000);
            }
            ndspChnReset(BGM_CHANNEL);
        }
    }
}

Result Sound::init()
{
    if (!Mp3Decoder::initLibrary())
    {
        return -1;
    }
    Logging::startupLog("sound", "decoder init ok");

    STDirectory dir(SONG_DIR);
    if (dir.good())
    {
        for (size_t i = 0; i < dir.count(); i++)
        {
            std::string path = std::string(SONG_DIR) + "/" + dir.item(i);
            if (!dir.folder(i) && Mp3Decoder::open(path))
            {
                bgm.push_back(path);
            }
        }
    }

    if (bgm.empty())
    {
        Logging::startupLog("sound", "no bgm found");
        Mp3Decoder::exitLibrary();
        return 0;
    }
    Logging::startupLog("sound", "loaded {} songs", bgm.size());

    LightEvent_Init(&frameEvent, RESET_ONESHOT);

    Result res = ndspInit();
    if (R_FAILED(res))
    {
        Mp3Decoder::exitLibrary();
        return res;
    }
    ndspSetCallback(ndspFrameCallback, nullptr);
    Logging::startupLog("sound", "ndsp init ok");

    bufferMem = (s16*)linearAlloc(BUFFER_SIZE * NUM_BUFFERS);
    if (!bufferMem)
    {
        ndspExit();
        Mp3Decoder::exitLibrary();
        return -1;
    }

    for (size_t buffer = 0; buffer < NUM_BUFFERS; buffer++)
    {
        buffers[buffer].data_pcm16 = bufferMem + buffer * BUFFER_SIZE / sizeof(s16);
        buffers[buffer].status     = NDSP_WBUF_DONE;
        buffers[buffer].nsamples   = 0;
    }
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ready = true;
    Logging::startupLog("sound", "init ok");

    return 0;
}

void Sound::start()
{
    if (!ready)
    {
        return;
    }
    playing = true;
    Threads::background(16 * 1024, soundThread);
}

void Sound::exit()
{
    if (!ready)
    {
        return;
    }
    stopThread();
    decoder = nullptr;
    linearFree(bufferMem);
    bufferMem = nullptr;
    ndspExit();
    Mp3Decoder::exitLibrary();
    ready = false;
}
