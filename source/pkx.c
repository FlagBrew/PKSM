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

#include "pkx.h"

u8 pkx_get_HT(u8* pkmn) { 
	return *(u8*)(pkmn + 0xDE); 
}

u8 pkx_get_gender(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0x1D)) >> 1) & 0x3; 
}

u8 pkx_get_language(u8* pkmn) { 
	return *(u8*)(pkmn + 0xE3); 
}

u8 pkx_get_ability(u8* pkmn) { 
	return *(u8*)(pkmn + 0x14); 
}

u8 pkx_get_ability_number(u8* pkmn) { 
	return *(u8*)(pkmn + 0x15); 
}

u8 pkx_get_form(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0x1D)) >> 3); 
}

u16 pkx_get_item(u8* pkmn) { 
	return *(u16*)(pkmn + 0x0A); 
}

u8 pkx_get_hp_type(u8* pkmn) { 
	return 15 * ((getIV(pkmn, 0)& 1) 
		  + 2 * (getIV(pkmn, 1) & 1) 
		  + 4 * (getIV(pkmn, 2) & 1) 
		  + 8 * (getIV(pkmn, 3) & 1) 
		  + 16 * (getIV(pkmn, 4) & 1) 
		  + 32 * (getIV(pkmn, 5) & 1)) / 63; 
}

u8 pkx_get_ot_gender(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0xDD)) >> 7); 
}
