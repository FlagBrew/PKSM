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

#define MULTIPLESELECTIONSIZE 30

void bank_free_multiple_selection_location(const int box, const int slot);
void bank_free_multiple_selection_buffer();
bool bank_get_selected_slot(const int box, const int slot);
void bank_set_selected_slot(const int box, const int slot);

void clearMarkings(u8* pkmn);
bool bank_getIsInternetWorking();
void bank(u8* mainbuf);