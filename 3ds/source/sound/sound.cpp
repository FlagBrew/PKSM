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
#include "random.hpp>"

static std::unordered_map<std::string, Decoder*> effects;
static std::unique_ptr<Decoder> currentBGM = nullptr;
static std::vector<std::string> bgm;
static size_t currentSong = 0;
static ndspWaveBuf buffer[2];
static bool playMusic;
static bool bgmDone = false;
static Handle effectCountMutex;
static u8 effectsPlaying = 0;
static u8 currentVolume = 0;
static int unusedEffectChannel = 24;

static void safeEffectsUp()
{
    svcWaitSynchronization(effectCountMutex, U64_MAX);
    effectsPlaying++;
    svcReleaseMutex(effectCountMutex);
}

static void safeEffectsDown()
{
    svcWaitSynchronization(effectCountMutex, U64_MAX);
    effectsPlaying--;
    svcReleaseMutex(effectCountMutex);
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
                bgm.emplace_back(dir.item(i));
            }
        }
    }
    svcCreateMutex(&effectCountMutex, false);
    return ndspInit();
}

void Sound::registerEffect(const std::string& fileName)
{
    if (io::exists(fileName))
    {
        auto dec = Decoder::get(fileName);
        if (dec && dec->good())
        {
            effects.emplace(fileName, std::move(dec));
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
    while (!bgmDone && effectsPlaying != 0)
    {
        svcSleepThread(125000000); // wait for play and playEffect to be done
    }
    ndspExit();
}

static void bgmPlayThread(void*)
{
    // TODO
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
            currentBGM = std::unique_ptr<Decoder>(Decoder::get(bgm[currentSong]));
            Threads::create(&bgmPlayThread);
        }
        if (currentVolume == 0)
        {
            for (size_t i = 0; i < currentBGM->channels(); i++)
            {
                ndspChnSetPaused(i, true);
            }
        }
        while (currentVolume == 0 && playMusic)
        {
            HIDUSER_GetSoundVolume(&currentVolume);
            svcSleepThread(250000000);
        }
        if (ndspChnIsPaused(0) && playMusic)
        {
            for (size_t i = 0; i < currentBGM->channels(); i++)
            {
                ndspChnSetPaused(i, false);
            }
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

static void playEffectThread(void* arg)
{
    Decoder* decoder = (Decoder*) arg;
    int channelStart = unusedEffectChannel - decoder->channels();
    if (channelStart >= currentBGM->channels()) // Check to make sure that we have enough channels
    {
        safeEffectsUp();
        // TODO
        safeEffectsDown();
    }
}

void Sound::playEffect(const std::string& effectName)
{
    auto effect = effects.find(effectName);
    if (effect != effects.end())
    {
        Threads::create(&playEffectThread, (void*)effect->second);
    }
}
