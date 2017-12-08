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

#include "wcx.h"

u16 wcx_get_id(u8* wcx) { 
	return *(u16*)(wcx + 0x0); 
}

u32 *wcx_get_title(u8* wcx, u32* dst) {
	u16 src[72];
	memcpy(src, &wcx[0x2], 72);
	
	utf16_to_utf32(dst, src, 72);
	return dst;
}

u32 wcx_get_rawdate(u8* wcx) { 
	return *(u32*)(wcx + 0x4C); 
}

u32 wcx_get_year(u8* wcx) { 
	u32 year = wcx_get_rawdate(wcx) / 10000;
	return year < 2000 ? year + 2000 : year;
}

u32 wcx_get_month(u8* wcx) { 
	return wcx_get_rawdate(wcx) % 10000 / 100; 
}

u32 wcx_get_day(u8* wcx) { 
	return wcx_get_rawdate(wcx) % 100; 
}

u8 wcx_get_type(u8* wcx) { 
	return *(u8*)(wcx + 0x51); 
}

u8 wcx_get_flags(u8* wcx) { 
	return *(u8*)(wcx + 0x52); 
}

bool wcx_get_multiobtainable(u8* wcx) {
	return *(u8*)(wcx + 0x53) == 1;
}

bool wcx_is_bp(u8* wcx) {
	return wcx_get_type(wcx) == 3;
}

bool wcx_is_bean(u8* wcx) {
	return wcx_get_type(wcx) == 2;
}

bool wcx_is_item(u8* wcx) {
	return wcx_get_type(wcx) == 1;
}

u16 wcx_get_item(u8* wcx) { 
	return *(u16*)(wcx + 0x68); 
}

u16 wcx_get_item_quantity(u8* wcx) { 
	return *(u16*)(wcx + 0x6A); 
}

bool wcx_is_pokemon(u8* wcx) {
	return wcx_get_type(wcx) == 0;
}

u8 wcx_get_pidtype(u8* wcx) { 
	return *(u8*)(wcx + 0xA3); 
}

bool wcx_is_shiny(u8* wcx) {
	return wcx_get_pidtype(wcx) == 2;
}

u16 wcx_get_tid(u8* wcx) { 
	return *(u16*)(wcx + 0x68); 
}

u16 wcx_get_sid(u8* wcx) { 
	return *(u16*)(wcx + 0x6A); 
}

u8 wcx_get_origin_game(u8* wcx) { 
	return *(u8*)(wcx + 0x6C); 
}

u32 wcx_get_encryption_constant(u8* wcx) { 
	return *(u32*)(wcx + 0x70); 
}

u8 wcx_get_ball(u8* wcx) { 
	return *(u8*)(wcx + 0x76); 
}

u16 wcx_get_held_item(u8* wcx) { 
	return *(u16*)(wcx + 0x78); 
}

u16 wcx_get_move(u8* wcx, int index) { 
	return *(u16*)(wcx + 0x7A + index*2); 
}

u16 wcx_get_species(u8* wcx) { 
	return *(u16*)(wcx + 0x82); 
}

u8 wcx_get_form(u8* wcx) { 
	return *(u8*)(wcx + 0x84); 
}

u8 wcx_get_language(u8* wcx) { 
	return *(u8*)(wcx + 0x85); 
}

u32 *wcx_get_nickname(u8* wcx, u32* dst) {
	u16 src[NICKNAMELENGTH];
	memcpy(src, &wcx[0x86], NICKNAMELENGTH);
	
	utf16_to_utf32(dst, src, NICKNAMELENGTH);
	return dst;
}

u8 wcx_get_nature(u8* wcx) { 
	return *(u8*)(wcx + 0xA0); 
}

u8 wcx_get_gender(u8* wcx) { 
	return *(u8*)(wcx + 0xA1); 
}

u16 wcx_get_ability(u8* wcx) {
	u16 ability = 0;
	u8 abilitynum = 0;
	u8 type = wcx_get_ability_type(wcx);
	if (type == 2)
		abilitynum = 2;
	else if (type == 4)
		abilitynum = 2;
	else
		abilitynum = 0;
	memcpy(&ability, &personal.pkmData[wcx_get_species(wcx)][0x09 + abilitynum], 1);
	return ability;
}

u8 wcx_get_ability_type(u8* wcx) { 
	return *(u8*)(wcx + 0xA2); 
}

u8 wcx_get_met_level(u8* wcx) { 
	return *(u8*)(wcx + 0xA8); 
}

u32 *wcx_get_ot(u8* wcx, u32* dst) {
	u16 src[NICKNAMELENGTH];
	memcpy(src, &wcx[0xB6], NICKNAMELENGTH);
	
	utf16_to_utf32(dst, src, NICKNAMELENGTH);
	return dst;
}

u8 wcx_get_level(u8* wcx) { 
	return *(u8*)(wcx + 0xD0); 
}

bool wcx_is_egg(u8* wcx) { 
	return *(u8*)(wcx + 0xD1) == 1; 
}

void wcxfull_to_wcx(u8* dst, u8* src) {
	memcpy(dst, src + 0x208, 264);
}

void wcx_set_rawdate(u8* wcx, u32 value) { 
	memcpy(wcx + 0x4C, &value, sizeof(u32)); 
}