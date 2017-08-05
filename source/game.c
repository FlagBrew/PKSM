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

#include "game.h"

int game = 0;

int game_get() {
	return game;
}

void game_set(int value) {
	game = value;
}

bool game_isgen7() {
	return (game == GAME_SUN || game == GAME_MOON);
}

bool game_isgen6() {
	return (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS);
}

bool game_isgen5() {
	return (game == GAME_B1 || game == GAME_B2 || game == GAME_W1 || game == GAME_W2);
}

bool game_isgen4() {
	return (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS);
}

bool game_is3DS() {
	return game_isgen6() || game_isgen7();
}

bool game_isDS() {
	return game_isgen5() || game_isgen4();
}

bool game_getisXY() {
	return (game == GAME_X || game == GAME_Y);
}

bool game_getisORAS() {
	return (game == GAME_OR || game == GAME_AS);
}

bool game_getisSUMO() {
	return (game == GAME_SUN || game == GAME_MOON);
}

u8 game_get_country(u8* mainbuf) {
	if (game_getisSUMO())
		return *(u8*)(mainbuf + 0x01200 + 0x2F);
	else if (game_getisORAS())
		return *(u8*)(mainbuf +  + 0x27);
	else if (game_getisXY())
		return *(u8*)(mainbuf + 0x14000 + 0x27);
	
	return 0;
}

u8 game_get_region(u8* mainbuf) {
	if (game_getisSUMO())
		return *(u8*)(mainbuf + 0x01200 + 0x2E);
	else if (game_getisORAS())
		return *(u8*)(mainbuf + 0x14000 + 0x26);
	else if (game_getisXY())
		return *(u8*)(mainbuf + 0x14000 + 0x26);
	
	return 0;	
}

u8 game_get_console_region(u8* mainbuf) {
	if (game_getisSUMO())
		return *(u8*)(mainbuf + 0x01200 + 0x34);
	else if (game_getisORAS())
		return *(u8*)(mainbuf + 0x14000 + 0x2C);
	else if (game_getisXY())
		return *(u8*)(mainbuf + 0x14000 + 0x2C);
	
	return 0;	
}

u8 game_get_language(u8* mainbuf) {
	if (game_getisSUMO())
		return *(u8*)(mainbuf + 0x01200 + 0x35);
	else if (game_getisORAS())
		return *(u8*)(mainbuf + 0x14000 + 0x2D);
	else if (game_getisXY())
		return *(u8*)(mainbuf + 0x14000 + 0x2D);
	
	return 0;	
}