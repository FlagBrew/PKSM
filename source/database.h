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

#define SMCOUNT 2050

void findFreeLocationWC(u8 *mainbuf, int game, int nInjected[]);
int getN(int i);
void setBoxBin(u8* mainbuf, int game, int NBOXES, int N, char* path[]);
int checkMultipleWCX(u8* mainbuf, int game, int i, int langCont, int nInjected[], int adapt);
void eventDatabase7(u8* mainbuf, int game);
void eventDatabase6(u8* mainbuf, int game);
void eventDatabase5(u8* mainbuf, int game);
void eventDatabase4(u8* mainbuf, int game, int GBO, int SBO);
void massInjector(u8* mainbuf, int game);