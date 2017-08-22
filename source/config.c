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

#include "config.h"
#define CONFIG_SIZE 1

char* config_path = "sdmc:/3ds/data/PKSM/config.bin";

u8 pkx_set_lock; // 0: do as always 1: don't set flags and ht

u8 config_get_pkx_set_lock() {
	return pkx_set_lock;
}

void config_set_pkx_set_lock(u8 value) {
	pkx_set_lock = value;
}

void config_load() {
	FILE *fptr = fopen(config_path, "rt");
	if (fptr == NULL)
		return;
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	u8 *buf = (u8*)malloc(size);
	memset(buf, 0, size);
	rewind(fptr);
	fread(buf, size, 1, fptr);
	fclose(fptr);
	
	// load values
	pkx_set_lock = buf[0];
}

void config_set() {
	remove(config_path);
	
	u8 buf[CONFIG_SIZE];
	buf[0] = pkx_set_lock;
	
	file_write(config_path, buf, CONFIG_SIZE);
}

void config_init() {
	if (!checkFile(config_path)) {
		pkx_set_lock = 0;
		
		config_set();
	} else {
		config_load();
	}
}