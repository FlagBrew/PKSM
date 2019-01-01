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

#include "mixer.hpp"
#include <vector>
#include "STDirectory.hpp"
#include "utils.hpp"
#include <algorithm>
#include "Configuration.hpp"
#include "random.hpp"

static Mix_Music* song;
static std::vector<std::string> songs;
static bool musicMutex = false;
static bool donePlaying = false;
static size_t currentSong = 0;
static u8 currentVolume = 0;

bool SDLH_Init(void)
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return false;
    }

    const int mix_flags = MIX_INIT_MP3;
    if ((Mix_Init(mix_flags) & mix_flags) != mix_flags)
    {
        fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    if (io::exists("/3ds/PKSM/songs/"))
    {
        STDirectory d("/3ds/PKSM/songs/");
        if (d.good() && d.count() > 0)
        {
            for (size_t i = 0; i < d.count(); i++)
            {
                if (!d.folder(i))
                {
                    std::string songName = d.item(i);
                    if (songName.substr(songName.size() - 3, 3) == "mp3")
                    {
                        songs.push_back("/3ds/PKSM/songs/" + songName);
                    }
                }
            }
        }
    }

    if (songs.empty())
    {
        songs.push_back("romfs:/audio.mp3");
    }

    std::sort(songs.begin(), songs.end());

    HIDUSER_GetSoundVolume(&currentVolume);
    
    return true;
}

void SDLH_Exit(void)
{
    if (musicMutex)
    {
        musicMutex = false;
    }
    while (!donePlaying) svcSleepThread(125000000); // wait for SDLH_Play to be done
    if (song)
    {
        Mix_FreeMusic(song);
    }
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void SDLH_Play(void)
{
    musicMutex = true;
    while (musicMutex)
    {
        HIDUSER_GetSoundVolume(&currentVolume);
        if (!Mix_PlayingMusic() || (currentVolume == 0 && songs.size() > 1))
        {
            if (song)
            {
                Mix_FreeMusic(song);
                song = nullptr;
            }
            if (Configuration::getInstance().randomMusic())
            {
                currentSong = randomNumbers() % songs.size();
            }
            else
            {
                currentSong = (currentSong + 1) % songs.size();
            }
            song = Mix_LoadMUS(songs[currentSong].c_str());
            Mix_PlayMusic(song, 1);
        }
        if (currentVolume == 0)
        {
            Mix_PauseMusic();
        }
        while (currentVolume == 0 && musicMutex)
        {
            HIDUSER_GetSoundVolume(&currentVolume);
            svcSleepThread(250000000);
        }
        if (Mix_PausedMusic() && musicMutex)
        {
            Mix_ResumeMusic();
        }
        svcSleepThread(250000000);
    }
    donePlaying = true;
}