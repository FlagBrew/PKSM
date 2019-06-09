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

#include "sound.hpp"
#include <3ds.h>
#include <unordered_map>
#include "Decoder.hpp"
#include "io.hpp"
#include "STDirectory.hpp"
#include "thread.hpp"
#include "Configuration.hpp"
#include "random.hpp"
#include <list>

struct EffectThreadArg;

static std::unordered_map<std::string, std::string> effects; // effect name to file name
static std::shared_ptr<Decoder> currentBGM = nullptr;
static std::vector<std::string> bgm;
static std::list<EffectThreadArg> effectThreads;
static size_t currentSong = 0;
static ndspWaveBuf bgmBuffers[2];
static s16* bgmData;
static bool sizeGood = false;
static bool playMusic;
static bool bgmDone = false;
static Handle effectCountMutex;
static u8 currentVolume = 0;

struct EffectThreadArg
{
    EffectThreadArg(std::shared_ptr<Decoder> decoder, s16* linearMem) : decoder(decoder), linearMem(linearMem), channel(effectThreads.size()), inUse(true) {}
    std::shared_ptr<Decoder> decoder;
    s16* linearMem;
    int channel;
    bool inUse;
};

static void clearDoneEffects()
{
    for (auto i = effectThreads.begin(); i != effectThreads.end(); i++)
    {
        if (!i->inUse)
        {
            linearFree(i->linearMem);
            i = effectThreads.erase(i);
            i--;
        }
    }
}

Result Sound::init()
{
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
    svcCreateMutex(&effectCountMutex, false);
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
    while (!bgmDone && !effectThreads.empty())
    {
        svcSleepThread(125000000); // wait for play and playEffect to be done
        clearDoneEffects();
    }
    ndspExit();
}

static void bgmPlayThread(void*)
{
    ndspChnReset(0);
    ndspChnWaveBufClear(0);
    ndspChnSetInterp(0, currentBGM->stereo() ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, currentBGM->sampleRate());
    ndspChnSetFormat(0, currentBGM->stereo() ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

    bgmBuffers[0].data_pcm16 = bgmData;
    bgmBuffers[1].data_pcm16 = bgmData + currentBGM->bufferSize();
    for (auto& buf : bgmBuffers)
    {
        buf.nsamples = currentBGM->decode((void*)buf.data_pcm16);
        if (currentBGM->stereo())
        {
            buf.nsamples /= 2;
        }
        ndspChnWaveBufAdd(0, &buf);
    }

    for (int i = 0; !ndspChnIsPlaying(0); i++)
    {
        svcSleepThread(1000000); // About one millisecond.
        if (i > 5 * 1000) // Timeout in about 5 seconds.
        {
            return;
        }
    }

    bool lastBuf = false;
    while (playMusic)
    {
        svcSleepThread(125000000);

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
                if (buf.nsamples == 0)
                {
                    lastBuf = true;
                    break;
                }
                if (currentBGM->stereo())
                {
                    buf.nsamples /= 2;
                }

                ndspChnWaveBufAdd(0, &buf);
            }
            DSP_FlushDataCache(buf.data_pcm16, currentBGM->bufferSize() * sizeof(u16)); // Huh?
        }
    }
    ndspChnWaveBufClear(0);
    bgmDone = true;
}

static void bgmControlThread(void*)
{
    while (playMusic)
    {
        HIDUSER_GetSoundVolume(&currentVolume);
        if (!ndspChnIsPlaying(0) || (currentVolume == 0 && bgm.size() > 1))
        {
            if (currentBGM)
            {
                currentBGM = nullptr;
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
            sizeGood = false;
            while (playMusic && !sizeGood)
            {
                svcSleepThread(2000); // Yield execution
            }
            if (!playMusic) // Make sure to not create a new thread if we're exiting
            {
                return;
            }
            bgmDone = false;
            Threads::create(&bgmPlayThread);
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
        svcSleepThread(250000000);
    }
    bgmDone = true;
}

void Sound::startBGM()
{
    if (!bgm.empty())
    {
        playMusic = true;
        Threads::create(&bgmControlThread);
    }
}

static void playEffectThread(void* rawArg)
{
    EffectThreadArg* arg = (EffectThreadArg*) rawArg;
    if (arg->channel < 24 && arg->linearMem) // Check to make sure that we have enough channels
    {
        ndspChnReset(arg->channel);
        ndspChnWaveBufClear(arg->channel);
        ndspChnSetInterp(0, arg->decoder->stereo() ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
        ndspChnSetRate(arg->channel, arg->decoder->sampleRate());
        ndspChnSetFormat(arg->channel, arg->decoder->stereo() ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

        ndspWaveBuf effectBuffer[2];
        effectBuffer[0].data_pcm16 = arg->linearMem;
        effectBuffer[1].data_pcm16 = arg->linearMem + arg->decoder->bufferSize();
        for (auto& buf : effectBuffer)
        {
            buf.nsamples = arg->decoder->decode((void*)buf.data_pcm16);
            if (arg->decoder->stereo())
            {
                buf.nsamples /= 2;
            }
            ndspChnWaveBufAdd(arg->channel, &buf);
        }

        for (int i = 0; !ndspChnIsPlaying(arg->channel); i++)
        {
            svcSleepThread(1000000); // About one millisecond.
            if (i > 5 * 1000) // Timeout in about 5 seconds.
            {
                arg->inUse = false;
                return;
            }
        }

        bool lastBuf = false;
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
                    if (buf.nsamples == 0)
                    {
                        lastBuf = true;
                        break;
                    }
                    if (arg->decoder->stereo())
                    {
                        buf.nsamples /= 2;
                    }

                    ndspChnWaveBufAdd(arg->channel, &buf);
                }
                DSP_FlushDataCache(buf.data_pcm16, arg->decoder->bufferSize() * sizeof(u16)); // Huh?
            }
        }
        ndspChnWaveBufClear(arg->channel);
    }
    arg->inUse = false;
}

void Sound::playEffect(const std::string& effectName)
{
    auto effect = effects.find(effectName);
    clearDoneEffects();
    if (effect != effects.end())
    {
        auto decoder = Decoder::get(effect->second);
        if (decoder && decoder->good())
        {
            effectThreads.emplace_back(decoder, (s16*)linearAlloc((decoder->bufferSize() * sizeof(u16)) * 2));
            Threads::create(&playEffectThread, (void*) &*effectThreads.rbegin());
        }
    }
}

// Must be called by the main thread. Will be called via extern function in Gui::mainLoop for sound.hpp to stay implementation-independent
void SOUND_correctBGMDataSize()
{
    if (currentBGM)
    {
        size_t sizeWanted = currentBGM->bufferSize() * sizeof(u16) * 2;
        if (linearGetSize(bgmData) < sizeWanted)
        {
            linearFree(bgmData);
            bgmData = (s16*)linearAlloc(sizeWanted);
        }
        sizeGood = (bgmData != nullptr);
    }
}
