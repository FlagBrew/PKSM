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

#include "sound.hpp"
#include "Configuration.hpp"
#include "Decoder.hpp"
#include "STDirectory.hpp"
#include "io.hpp"
#include "random.hpp"
#include "thread.hpp"
#include <3ds.h>
#include <atomic>
#include <unordered_map>

namespace
{
    // Size of a single wave buffer's memory in bytes
    constexpr size_t BUFFER_SIZE         = 32 * 1024;
    constexpr size_t NUM_CHANNELS        = 24;
    constexpr size_t BUFFERS_PER_CHANNEL = 2;
    s16* bufferMem                       = nullptr;
    std::array<ndspWaveBuf, NUM_CHANNELS * BUFFERS_PER_CHANNEL> buffers;
    std::array<std::unique_ptr<Decoder>, NUM_CHANNELS> decoders;
    // 0 is reserved for the background music
    std::atomic_flag occupiedChannels[NUM_CHANNELS];
    LightLock channelLock;
    LightEvent frameEvent;

    std::unordered_map<std::string, std::string> effects; // effect name to file name
    std::vector<std::string> bgm;
    size_t currentSong         = 0;
    std::atomic<bool> playing  = false;
    std::atomic<bool> finished = true;
    u8 currentVolume           = 0;

    void ndspFrameCallback(void*)
    {
        if (!playing)
        {
            return;
        }

        LightEvent_Signal(&frameEvent);
    }

    void fillBuffers(int channel, std::unique_ptr<Decoder>& decoder)
    {
        for (size_t buffer = channel * BUFFERS_PER_CHANNEL; buffer < (channel + 1) * BUFFERS_PER_CHANNEL; buffer++)
        {
            if (buffers[buffer].status == NDSP_WBUF_DONE)
            {
                // Decode data into the done buffer
                buffers[buffer].nsamples = decoder->decode((void*)buffers[buffer].data_pcm16, BUFFER_SIZE);
                // Correct size for stereo mode
                if (decoder->stereo())
                {
                    buffers[buffer].nsamples /= 2;
                }
                // Flush data if we actually decoded anything
                if (buffers[buffer].nsamples > 0)
                {
                    DSP_FlushDataCache(buffers[buffer].data_pcm16, BUFFER_SIZE);
                    ndspChnWaveBufAdd(channel, &buffers[buffer]);
                }
                // Otherwise, we're done! Make sure to break to not use the now-null decoder
                else
                {
                    buffers[buffer].status = NDSP_WBUF_DONE;
                    decoder                = nullptr;
                    break;
                }
            }
        }
    }

    void setDecoder(int channel, std::unique_ptr<Decoder> decoder)
    {
        if (decoder)
        {
            ndspChnReset(channel);
            ndspChnSetInterp(channel, decoder->stereo() ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
            ndspChnSetRate(channel, decoder->sampleRate());
            ndspChnSetFormat(channel, decoder->stereo() ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

            // Set all channels into the done state to be decoded into
            for (size_t buffer = channel * BUFFERS_PER_CHANNEL; buffer < (channel + 1) * BUFFERS_PER_CHANNEL; buffer++)
            {
                buffers[buffer].status = NDSP_WBUF_DONE;
            }

            fillBuffers(channel, decoder);
            decoders[channel] = std::move(decoder);
        }
    }

    std::unique_ptr<Decoder> getNextBgm()
    {
        std::unique_ptr<Decoder> ret = nullptr;
        while (!ret && !bgm.empty())
        {
            if (Configuration::getInstance().randomMusic())
            {
                currentSong = randomNumbers() % bgm.size();
            }
            else
            {
                currentSong = (currentSong + 1) % bgm.size();
            }
            ret = Decoder::get(bgm[currentSong]);
            if (!(ret && ret->good()))
            {
                bgm.erase(bgm.begin() + currentSong);
            }
        }
        return ret;
    }

    void soundThread(void*)
    {
        finished = false;
        while (playing)
        {
            // Get volume for later usage
            HIDUSER_GetSoundVolume(&currentVolume);
            // Explicitly do the BGM channel with its special handling:
            // Replace the song if the volume slider is pushed all the way down and we haven't already replaced it or the decoder is gone and the
            // channel is done or paused
            bool replaceBGM =
                (currentVolume == 0 && bgm.size() > 1 && !ndspChnIsPaused(0)) || (!decoders[0] && (!ndspChnIsPlaying(0) || ndspChnIsPaused(0)));
            // If the decoder still exists and we're not about to replace it, fill the buffers
            if (!replaceBGM && decoders[0])
            {
                fillBuffers(0, decoders[0]);
            }
            // Otherwise, if there's anything to replace it with, then do so
            else if (!bgm.empty())
            {
                setDecoder(0, getNextBgm());
            }

            // Pause the song if the volume slider is all the way down
            if (currentVolume == 0)
            {
                ndspChnSetPaused(0, true);
            }
            else
            {
                ndspChnSetPaused(0, false);
            }

            // And fill all the other buffers
            for (size_t channel = 1; channel < NUM_CHANNELS; channel++)
            {
                // If the decoder still exists, fill the buffers
                if (decoders[channel])
                {
                    fillBuffers(channel, decoders[channel]);
                }
                // Otherwise, once it's done playing, add the channel back into the pool
                else if (!ndspChnIsPlaying(channel))
                {
                    occupiedChannels[channel].clear();
                }
            }

            LightEvent_Wait(&frameEvent);
        }
        finished = true;
    }
}

Result Sound::init()
{
    LightEvent_Init(&frameEvent, RESET_ONESHOT);
    LightLock_Init(&channelLock);
    STDirectory dir("/3ds/PKSM/songs");
    if (dir.good())
    {
        for (size_t i = 0; i < dir.count(); i++)
        {
            if (!dir.folder(i))
            {
                auto decoder = Decoder::get("/3ds/PKSM/songs/" + dir.item(i));
                if (decoder && decoder->good())
                {
                    bgm.emplace_back("/3ds/PKSM/songs/" + dir.item(i));
                }
            }
        }
    }
    Result res = ndspInit();
    ndspSetCallback(ndspFrameCallback, nullptr);
    if (R_FAILED(res))
    {
        return res;
    }
    bufferMem = (s16*)linearAlloc(BUFFER_SIZE * BUFFERS_PER_CHANNEL * NUM_CHANNELS);
    if (!bufferMem)
    {
        return -1;
    }
    for (size_t buffer = 0; buffer < NUM_CHANNELS * BUFFERS_PER_CHANNEL; buffer++)
    {
        buffers[buffer].data_pcm16 = bufferMem + buffer * BUFFER_SIZE / sizeof(s16);
        buffers[buffer].status     = NDSP_WBUF_DONE;
        buffers[buffer].nsamples   = 0;
    }
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    return 0;
}

void Sound::registerEffect(const std::string& effectName, const std::string& fileName)
{
    if (io::exists(fileName))
    {
        auto dec = Decoder::get(fileName);
        if (dec && dec->good())
        {
            effects.emplace(effectName, fileName);
        }
    }
}

void Sound::exit()
{
    stop();
    linearFree(bufferMem);
    ndspExit();
}

void Sound::start()
{
    playing = true;
    Threads::create(&soundThread, nullptr, 16 * 1024);
}

void Sound::stop()
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
        for (size_t i = 0; i < NUM_CHANNELS; i++)
        {
            ndspChnReset(i);
        }
    }
}

void Sound::playEffect(const std::string& effectName)
{
    if (currentVolume > 0)
    {
        auto effect = effects.find(effectName);
        if (effect != effects.end())
        {
            auto decoder = Decoder::get(effect->second);
            if (decoder && decoder->good())
            {
                // First channel is reserved for BGM
                for (size_t channel = 1; channel < NUM_CHANNELS; channel++)
                {
                    if (!occupiedChannels[channel].test_and_set())
                    {
                        setDecoder(channel, std::move(decoder));
                        break;
                    }
                }
            }
        }
    }
}
