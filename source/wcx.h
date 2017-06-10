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

bool wcx_is_bean(u8* wcx);
bool wcx_is_bp(u8* wcx);
bool wcx_is_egg(u8* wcx);
bool wcx_is_item(u8* wcx);
bool wcx_is_pokemon(u8* wcx);
bool wcx_is_shiny(u8* wcx);
u16 wcx_get_ability(u8* wcx);
u8 wcx_get_ability_type(u8* wcx);
u8 wcx_get_ball(u8* wcx);
u32 wcx_get_day(u8* wcx);
u32 wcx_get_encryption_constant(u8* wcx);
u8 wcx_get_flags(u8* wcx);
u8 wcx_get_form(u8* wcx);
u8 wcx_get_gender(u8* wcx);
u16 wcx_get_held_item(u8* wcx);
u16 wcx_get_id(u8* wcx);
u16 wcx_get_item(u8* wcx);
u16 wcx_get_item_quantity(u8* wcx);
u8 wcx_get_language(u8* wcx);
u8 wcx_get_level(u8* wcx);
u8 wcx_get_met_level(u8* wcx);
u32 wcx_get_month(u8* wcx);
u16 wcx_get_move(u8* wcx, int index);
bool wcx_get_multiobtainable(u8* wcx);
u8 wcx_get_nature(u8* wcx);
u32 *wcx_get_nickname(u8* wcx, u32* dst);
u8 wcx_get_origin_game(u8* wcx);
u32 *wcx_get_ot(u8* wcx, u32* dst);
u8 wcx_get_pidtype(u8* wcx);
u32 wcx_get_rawdate(u8* wcx);
u16 wcx_get_sid(u8* wcx);
u16 wcx_get_species(u8* wcx);
u16 wcx_get_tid(u8* wcx);
u32 *wcx_get_title(u8* wcx, u32* dst);
u8 wcx_get_type(u8* wcx);
u32 wcx_get_year(u8* wcx);