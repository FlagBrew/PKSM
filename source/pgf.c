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

#include "pgf.h"

u8 pgf_get_ball(u8* pgf) { 
	return *(u8*)(pgf + 0x0E); 
}

u16 pgf_get_held_item(u8* pgf) { 
	return *(u16*)(pgf + 0x10); 
}

u16 pgf_get_id(u8* pgf) { 
	return *(u16*)(pgf); 
}

u32 *pgf_get_title(u8* pgf, u32* dst) {
	u16 src[0x4A];
	memcpy(src, &pgf[0x60], 0x4A);
	
	utf16_to_utf32(dst, src, 0x4A);
	return dst;
}

u32 pgf_get_year(u8* pgf) { 
	return (u32)(*(u16*)(pgf + 0xAE));
}

u32 pgf_get_month(u8* pgf) { 
	return (u32)(*(u8*)(pgf + 0xAD));
}

u32 pgf_get_day(u8* pgf) { 
	return (u32)(*(u8*)(pgf + 0xAC));
}

u8 pgf_get_type(u8* pgf) { 
	return *(u8*)(pgf + 0xB3); 
}

bool pgf_is_item(u8* pgf) {
	return pgf_get_type(pgf) == 2;
}

u16 pgf_get_item(u8* pgf) { 
	return *(u16*)(pgf + 0x10); 
}

bool pgf_is_pokemon(u8* pgf) {
	return pgf_get_type(pgf) == 1;
}

bool pgf_is_shiny(u8* pgf) {
	return pgf_get_pidtype(pgf) == 2;
}

u8 pgf_get_pidtype(u8* pgf) { 
	return *(u8*)(pgf + 0x37); 
}

u16 pgf_get_tid(u8* pgf) { 
	return *(u16*)(pgf); 
}

u16 pgf_get_sid(u8* pgf) { 
	return *(u16*)(pgf + 0x2); 
}

u16 pgf_get_move(u8* pgf, int index) { 
	return *(u16*)(pgf + 0x12 + index*2); 
}

u16 pgf_get_species(u8* pgf) { 
	return *(u16*)(pgf + 0x1A); 
}

u8 pgf_get_gender(u8* pgf) { 
	return *(u8*)(pgf + 0x35); 
}

u32 *pgf_get_ot(u8* pgf, u32* dst) {
	u16 src[0x4A];
	memcpy(src, &pgf[0x10], 0X4A);
	
	utf16_to_utf32(dst, src, 0x4A);
	return dst;
}

u8 pgf_get_level(u8* pgf) { 
	return *(u8*)(pgf + 0x5B); 
}