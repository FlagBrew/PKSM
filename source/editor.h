/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#pragma once
#include "common.h"

#define PGTLENGTH 260
#define PGFLENGTH 204
#define WC6LENGTH 264
#define PKMNLENGTH 232
#define NICKNAMELENGTH 26
#define BOXMAX 31

#define ED_STANDARD 0
#define ED_MENU 1
#define ED_CLONE 2
#define ED_GENERATE 3
#define ED_OTA 4
#define ED_SEED 5

#define ED_BASE 0
#define ED_STATS 1
#define ED_MOVES 2
#define ED_ITEMS 3
#define ED_NATURES 4
#define ED_BALLS 5
#define ED_HIDDENPOWER 6
#define ED_FORMS 7
#define ED_HEX 8

#define CHERISH_BALL 16

bool isBattleBoxed(u8* mainbuf, int game, int box, int slot);
bool checkHTLegality(u8* mainbuf, u8* pkmn, int game);

u32 *getSaveOT(u8* mainbuf, int game, u32* dst);
u8 getSaveGender(u8* mainbuf, int game);
u16 getSaveTID(u8* mainbuf, int game);
u16 getSaveSID(u8* mainbuf, int game);
u16 getSaveTSV(u8* mainbuf, int game);
u32 getSaveSeed(u8* mainbuf, int game, int index);
u8 getSaveLanguage(u8* mainbuf, int game);

void setWC(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void setWC4(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[], int GBO);
void setSaveLanguage(u8* mainbuf, int game, int i);

void parseSaveHexEditor(u8* mainbuf, int game, int byte);

void saveFileEditor(u8* mainbuf, int game, u64 size);
void pokemonEditor(u8* mainbuf, int game);
