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

#ifndef PGT_H
#define PGT_H

#include "common.h"

u16 pgt_get_id(u8* pgt);
u32 *pgt_get_title(u8* pgt, u32* dst);
u32 pgt_get_year(u8* pgt);
u32 pgt_get_month(u8* pgt);
u32 pgt_get_day(u8* pgt);
bool pgt_is_item(u8* pgt);
u16 pgt_get_item(u8* pgt);
bool pgt_is_pokemon(u8* pgt);
bool pgt_is_shiny(u8* pgt);
u16 pgt_get_tid(u8* pgt);
u16 pgt_get_sid(u8* pgt);
u16 pgt_get_move(u8* pgt, int index);
u16 pgt_get_species(u8* pgt);
u8 pgt_get_gender(u8* pgt);
u32 *pgt_get_ot(u8* pgt, u32* dst);
u8 pgt_get_level(u8* pgt);

#endif