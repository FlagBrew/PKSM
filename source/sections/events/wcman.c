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

#include "wcman.h"

u16 wc_get_held_item(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_held_item(wc);
	}
	return pgf_get_held_item(wc);
}

u8 wc_get_ball(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_ball(wc);
	}
	return pgf_get_ball(wc);
}

u16 wc_get_id(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_id(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_id(wc);
	}
	return pgt_get_id(wc);
}

u32 *wc_get_title(u8* wc, u32* dst) {
	if (game_is3DS())
	{
		return wcx_get_title(wc, dst);
	}
	else if (game_isgen5())
	{
		return pgf_get_title(wc, dst);
	}
	return pgt_get_title(wc, dst);
}

u32 wc_get_year(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_year(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_year(wc);
	}
	return pgt_get_year(wc);
}

u32 wc_get_month(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_month(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_month(wc);
	}
	return pgt_get_month(wc);
}

u32 wc_get_day(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_day(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_day(wc);
	}
	return pgt_get_day(wc);
}

bool wc_is_item(u8* wc) {
	if (game_is3DS())
	{
		return wcx_is_item(wc);
	}
	else if (game_isgen5())
	{
		return pgf_is_item(wc);
	}
	return pgt_is_item(wc);
}

u16 wc_get_item(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_item(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_item(wc);
	}
	return pgt_get_item(wc);
}

bool wc_is_pokemon(u8* wc) {
	if (game_is3DS())
	{
		return wcx_is_pokemon(wc);
	}
	else if (game_isgen5())
	{
		return pgf_is_pokemon(wc);
	}
	return pgt_is_pokemon(wc);
}

bool wc_is_shiny(u8* wc) {
	if (game_is3DS())
	{
		return wcx_is_shiny(wc);
	}
	else if (game_isgen5())
	{
		return pgf_is_shiny(wc);
	}
	return pgt_is_shiny(wc);
}

u16 wc_get_tid(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_tid(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_tid(wc);
	}
	return pgt_get_tid(wc);
}

u16 wc_get_sid(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_sid(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_sid(wc);
	}
	return pgt_get_sid(wc);
}

u16 wc_get_move(u8* wc, int index) { 
	if (game_is3DS())
	{
		return wcx_get_move(wc, index);
	}
	else if (game_isgen5())
	{
		return pgf_get_move(wc, index);
	}
	return pgt_get_move(wc, index);
}

u16 wc_get_species(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_species(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_species(wc);
	}
	return pgt_get_species(wc);
}

u8 wc_get_gender(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_gender(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_gender(wc);
	}
	return pgt_get_gender(wc);
}

u32 *wc_get_ot(u8* wc, u32* dst) {
	if (game_is3DS())
	{
		return wcx_get_ot(wc, dst);
	}
	else if (game_isgen5())
	{
		return pgf_get_ot(wc, dst);
	}
	return pgt_get_ot(wc, dst);
}

u8 wc_get_level(u8* wc) { 
	if (game_is3DS())
	{
		return wcx_get_level(wc);
	}
	else if (game_isgen5())
	{
		return pgf_get_level(wc);
	}
	return pgt_get_level(wc);
}