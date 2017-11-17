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

#ifndef GAME_H
#define GAME_H

#include "common.h"

#define GAME_X 		  0
#define GAME_Y 		  1
#define GAME_OR 	  2
#define GAME_AS 	  3
#define GAME_SUN 	  4
#define GAME_MOON 	  5
#define GAME_US       6
#define GAME_UM       7
#define GAME_DIAMOND  8
#define GAME_PEARL 	  9
#define GAME_PLATINUM 10
#define GAME_HG 	  11
#define GAME_SS 	 12
#define GAME_B1 	 13
#define GAME_W1 	 14
#define GAME_B2 	 15
#define GAME_W2 	 16

int game_get();
void game_set(int value);

bool game_isgen7();
bool game_isgen6();
bool game_isgen5();
bool game_isgen4();
bool game_is3DS();
bool game_isDS();
bool game_getisXY();
bool game_getisORAS();
bool game_getisSUMO();
bool game_getisUSUM();

u8 game_get_country(u8* mainbuf);
u8 game_get_region(u8* mainbuf);
u8 game_get_console_region(u8* mainbuf);
u8 game_get_language(u8* mainbuf);

void game_fill_offsets();

#endif