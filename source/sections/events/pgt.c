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

#include "pgt.h"

// Dummy functions 

u16 pgt_get_id(u8* pgt) { 
	return 0; 
}

u32 *pgt_get_title(u8* pgt, u32* dst) {
	u16 src[2] = {0};
	utf16_to_utf32(dst, src, 2);
	return dst;
}

u32 pgt_get_year(u8* pgt) { 
	return 2000;
}

u32 pgt_get_month(u8* pgt) { 
	return 0; 
}

u32 pgt_get_day(u8* pgt) { 
	return 0; 
}

bool pgt_is_item(u8* pgt) {
	return true;
}

u16 pgt_get_item(u8* pgt) { 
	return 0; 
}

bool pgt_is_pokemon(u8* pgt) {
	return false;
}

bool pgt_is_shiny(u8* pgt) {
	return false;
}

u16 pgt_get_tid(u8* pgt) { 
	return *(u16*)(pgt + 0x68); 
}

u16 pgt_get_sid(u8* pgt) { 
	return 0; 
}

u16 pgt_get_move(u8* pgt, int index) { 
	return 0; 
}

u16 pgt_get_species(u8* pgt) { 
	return 0; 
}

u8 pgt_get_gender(u8* pgt) { 
	return 0; 
}

u32 *pgt_get_ot(u8* pgt, u32* dst) {
	u16 src[2] = {0};
	utf16_to_utf32(dst, src, 2);
	return dst;
}

u8 pgt_get_level(u8* pgt) { 
	return 0; 
}