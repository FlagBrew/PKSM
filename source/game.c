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

u8 game_get_country(u8* mainbuf) {
	if (ISSUMO)
		return *(u8*)(mainbuf + 0x01200 + 0x2F);
	else if (ISORAS)
		return *(u8*)(mainbuf +  + 0x27);
	else if (ISXY)
		return *(u8*)(mainbuf + 0x14000 + 0x27);
	
	return 0;
}

u8 game_get_region(u8* mainbuf) {
	if (ISSUMO)
		return *(u8*)(mainbuf + 0x01200 + 0x2E);
	else if (ISORAS)
		return *(u8*)(mainbuf + 0x14000 + 0x26);
	else if (ISXY)
		return *(u8*)(mainbuf + 0x14000 + 0x26);
	
	return 0;	
}

u8 game_get_console_region(u8* mainbuf) {
	if (ISSUMO)
		return *(u8*)(mainbuf + 0x01200 + 0x34);
	else if (ISORAS)
		return *(u8*)(mainbuf + 0x14000 + 0x2C);
	else if (ISXY)
		return *(u8*)(mainbuf + 0x14000 + 0x2C);
	
	return 0;	
}

u8 game_get_language(u8* mainbuf) {
	if (ISSUMO)
		return *(u8*)(mainbuf + 0x01200 + 0x35);
	else if (ISORAS)
		return *(u8*)(mainbuf + 0x14000 + 0x2D);
	else if (ISXY)
		return *(u8*)(mainbuf + 0x14000 + 0x2D);
	
	return 0;	
}