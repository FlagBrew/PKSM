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
#include <list>
#include <unordered_map>

namespace
{
    struct EffectThreadArg
    {
        EffectThreadArg(std::shared_ptr<Decoder> decoder, s16* linearMem, int channel) : decoder(decoder), linearMem(linearMem), channel(channel)
        {
            inUse.test_and_set();
        }
        std::shared_ptr<Decoder> decoder;
        s16* linearMem;
        int channel;
        std::atomic_flag inUse;
    };

    std::unordered_map<std::string, std::string> effects; // effect name to file name
    std::shared_ptr<Decoder> currentBGM = nullptr;
    std::vector<std::string> bgm;
    std::list<EffectThreadArg> effectThreads;
    size_t currentSong            = 0;
    ndspWaveBuf bgmBuffers[2]     = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
    s16* bgmData                  = nullptr;
    std::atomic<bool> sizeGood    = false;
    std::atomic<bool> playMusic   = false;
    std::atomic<bool> bgmDone     = true;
    std::atomic<bool> exitBGM     = false;
    u8 currentVolume              = 0;
    std::vector<int> freeChannels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

    void clearDoneEffects()
    {
        auto i = effectThreads.begin();
        while (i != effectThreads.end())
        {
            if (!i->inUse.test_and_set())
            {
                freeChannels.push_back(i->channel);
                linearFree(i->linearMem);
                i = effectThreads.erase(i);
            }
            else
            {
                i++;
            }
        }
    }
    void bgmPlayThread(void*)
    {
        ndspChnReset(0);
        ndspChnWaveBufClear(0);
        ndspChnSetInterp(0, currentBGM->stereo() ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
        ndspChnSetRate(0, currentBGM->sampleRate());
        ndspChnSetFormat(0, currentBGM->stereo() ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

        bgmBuffers[0].data_pcm16 = bgmData;
        bgmBuffers[1].data_pcm16 = bgmData + currentBGM->bufferSize();
        bool lastBuf             = false;
        for (auto& buf : bgmBuffers)
        {
            buf.nsamples = currentBGM->decode((void*)buf.data_pcm16);
            if (currentBGM->stereo())
            {
                buf.nsamples /= 2;
            }
            if (buf.nsamples > 0)
            {
                DSP_FlushDataCache(buf.data_pcm16, currentBGM->bufferSize() * sizeof(u16));
                ndspChnWaveBufAdd(0, &buf);
            }
            else
            {
                buf.status = NDSP_WBUF_DONE;
                lastBuf    = true;
            }
        }

        for (int i = 0; !ndspChnIsPlaying(0); i++)
        {
            svcSleepThread(1000000); // About one millisecond.
            if (i > 5 * 1000)        // Timeout in about 5 seconds.
            {
                bgmDone = true;
                return;
            }
        }

        while (playMusic)
        {
            svcSleepThread(12500000);

            if (exitBGM)
            {
                break;
            }
            if (lastBuf == true)
            {
                if (bgmBuffers[0].status == NDSP_WBUF_DONE && bgmBuffers[1].status == NDSP_WBUF_DONE)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }

            for (auto& buf : bgmBuffers)
            {
                if (buf.status == NDSP_WBUF_DONE)
                {
                    buf.nsamples = currentBGM->decode((void*)buf.data_pcm16);
                    if (currentBGM->stereo())
                    {
                        buf.nsamples /= 2;
                    }
                    if (buf.nsamples <= 0)
                    {
                        lastBuf = true;
                        break;
                    }

                    DSP_FlushDataCache(buf.data_pcm16, currentBGM->bufferSize() * sizeof(u16));
                    ndspChnWaveBufAdd(0, &buf);
                }
            }
        }
        ndspChnWaveBufClear(0);
        bgmDone = true;
    }
    void bgmControlThread(void*)
    {
        if (bgm.empty())
        {
            return;
        }
        while (playMusic)
        {
            if (bgm.empty())
            {
                bgmDone = true;
                return;
            }
            HIDUSER_GetSoundVolume(&currentVolume);
            if (bgmDone || (currentVolume == 0 && bgm.size() > 1))
            {
                if (currentBGM)
                {
                    exitBGM = true;
                    while (!bgmDone)
                    {
                        svcSleepThread(125000000);
                    }
                    currentBGM = nullptr;
                    exitBGM    = false;
                }
                if (Configuration::getInstance().randomMusic())
                {
                    currentSong = randomNumbers() % bgm.size();
                }
                else
                {
                    currentSong = (currentSong + 1) % bgm.size();
                }
                currentBGM = Decoder::get(bgm[currentSong]);
                if (currentBGM)
                {
                    sizeGood = false;
                    while (playMusic && !sizeGood)
                    {
                        svcSleepThread(125000000); // Yield execution
                    }
                    if (!playMusic) // Make sure to not create a new thread if we're exiting
                    {
                        return;
                    }
                    bgmDone = !Threads::create(&bgmPlayThread);
                }
                else
                {
                    bgm.erase(bgm.begin() + currentSong);
                }
            }
            if (currentVolume == 0)
            {
                ndspChnSetPaused(0, true);
            }
            while (currentVolume == 0 && playMusic)
            {
                HIDUSER_GetSoundVolume(&currentVolume);
                svcSleepThread(250000000);
            }
            if (ndspChnIsPaused(0) && playMusic)
            {
                ndspChnSetPaused(0, false);
            }
            if (playMusic)
            {
                svcSleepThread(250000000);
            }
        }
        bgmDone = true;
    }
    void playEffectThread(void* rawArg)
    {
        EffectThreadArg* arg = (EffectThreadArg*)rawArg;
        if (arg->linearMem) // Did the linearAlloc work properly?
        {
            ndspChnReset(arg->channel);
            ndspChnWaveBufClear(arg->channel);
            ndspChnSetInterp(arg->channel, arg->decoder->stereo() ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
            ndspChnSetRate(arg->channel, arg->decoder->sampleRate());
            ndspChnSetFormat(arg->channel, arg->decoder->stereo() ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

            ndspWaveBuf effectBuffer[2] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
            effectBuffer[0].data_pcm16  = arg->linearMem;
            effectBuffer[1].data_pcm16  = arg->linearMem + arg->decoder->bufferSize();
            bool lastBuf                = false;
            for (auto& buf : effectBuffer)
            {
                buf.nsamples = arg->decoder->decode((void*)buf.data_pcm16);
                if (arg->decoder->stereo())
                {
                    buf.nsamples /= 2;
                }
                if (buf.nsamples > 0)
                {
                    DSP_FlushDataCache(buf.data_pcm16, arg->decoder->bufferSize() * sizeof(u16));
                    ndspChnWaveBufAdd(arg->channel, &buf);
                }
                else
                {
                    buf.status = NDSP_WBUF_DONE;
                    lastBuf    = true;
                }
            }

            for (int i = 0; !ndspChnIsPlaying(arg->channel); i++)
            {
                svcSleepThread(1000000); // About one millisecond.
                if (i > 5 * 1000)        // Timeout in about 5 seconds.
                {
                    arg->inUse.clear();
                    return;
                }
            }

            while (playMusic)
            {
                svcSleepThread(125000000);

                if (lastBuf == true)
                {
                    if (effectBuffer[0].status == NDSP_WBUF_DONE && effectBuffer[1].status == NDSP_WBUF_DONE)
                    {
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }

                for (auto& buf : effectBuffer)
                {
                    if (buf.status == NDSP_WBUF_DONE)
                    {
                        buf.nsamples = arg->decoder->decode((void*)buf.data_pcm16);
                        if (arg->decoder->stereo())
                        {
                            buf.nsamples /= 2;
                        }
                        if (buf.nsamples <= 0)
                        {
                            lastBuf = true;
                            break;
                        }

                        DSP_FlushDataCache(buf.data_pcm16, arg->decoder->bufferSize() * sizeof(u16));
                        ndspChnWaveBufAdd(arg->channel, &buf);
                    }
                }
            }
            ndspChnWaveBufClear(arg->channel);
            ndspChnReset(arg->channel);
        }
        arg->inUse.clear();
    }
}

Result Sound::init()
{
    playMusic = true;
    STDirectory dir("/3ds/PKSM/songs");
    if (dir.good())
    {
        for (size_t i = 0; i < dir.count(); i++)
        {
            if (!dir.folder(i))
            {
                bgm.emplace_back("/3ds/PKSM/songs/" + dir.item(i));
            }
        }
    }
    Result res = ndspInit();
    if (R_FAILED(res))
    {
        return res;
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
    if (playMusic)
    {
        playMusic = false;
    }
    else
    {
        bgmDone = true;
    }
    while (!bgmDone || !effectThreads.empty())
    {
        svcSleepThread(125000000); // wait for play and playEffect to be done
        clearDoneEffects();
    }
    ndspExit();
}

void Sound::startBGM()
{
    if (!bgm.empty())
    {
        Threads::create(&bgmControlThread);
    }
}

void Sound::playEffect(const std::string& effectName)
{
    if (currentVolume > 0)
    {
        auto effect = effects.find(effectName);
        clearDoneEffects();
        if (effect != effects.end() && !freeChannels.empty())
        {
            auto decoder = Decoder::get(effect->second);
            if (decoder && decoder->good())
            {
                effectThreads.emplace_back(decoder, (s16*)linearAlloc((decoder->bufferSize() * sizeof(u16)) * 2), freeChannels.back());
                freeChannels.pop_back();

                if (!Threads::create(&playEffectThread, (void*)&effectThreads.back()))
                {
                    effectThreads.back().inUse.clear();
                }
            }
        }
    }
}

// Must be called by the main thread. Will be called via extern function in Gui::mainLoop for sound.hpp to stay implementation-independent
void SOUND_correctBGMDataSize()
{
    if (currentBGM)
    {
        size_t sizeWanted = currentBGM->bufferSize() * sizeof(u16) * 2;
        if (bgmData == nullptr)
        {
            bgmData = (s16*)linearAlloc(sizeWanted);
        }
        else if (linearGetSize(bgmData) < sizeWanted)
        {
            linearFree(bgmData);
            bgmData = (s16*)linearAlloc(sizeWanted);
        }
        sizeGood = (bgmData != nullptr);
    }
}
