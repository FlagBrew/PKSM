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

#ifndef UTIL_H
#define UTIL_H

#include "common.h"

void checkMaxValueBetweenBounds(u8* buf, int byte, int start, int len, int max);
void checkMaxValue(u8* pkmn, int byteEntry, int value, int max);
bool checkFile(const char* path);
void loadPersonal();
void loadFile(u8* buf, char* path);
void injectFromFile(u8* mainbuf, char* path, u32 offset);
void printfile(char* path);
bool isHBL();
void fsStart();
void fsEnd();
bool openSaveArch(FS_Archive *out, u64 id);
int ArrayUTF32_sort_cmp_PKMN_Things_List(const wchar_t *a,const wchar_t *b);

#endif