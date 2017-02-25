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

int getActiveGBO(u8* mainbuf, int game);
int getActiveSBO(u8* mainbuf, int game);
u32 CHKOffset(u32 i, int game);
u32 CHKLength(u32 i, int game);
u16 getBlockID(u8* mainbuf, int csoff, u32 i);
u16 ccitt16(u8* data, u32 len);
u16 check16(u8 data[], u32 blockID, u32 len);
u32 BWCHKOff(u32 i, int game);
u32 BWCHKMirr(u32 i, int game);
void rewriteCHK(u8 *mainbuf, int game);
void rewriteCHK4(u8 *mainbuf, int game, int GBO, int SBO);
void resign(u8 *mainbuf);