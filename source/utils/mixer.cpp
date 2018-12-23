/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

static Mix_Music* song;

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
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    song = Mix_LoadMUS("romfs:/audio.mp3");
    
    return true;
}

void SDLH_Exit(void)
{
    Mix_FreeMusic(song);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void SDLH_Play(void)
{
    Mix_PlayMusic(song, -1);
}