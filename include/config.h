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

#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

#define CONFIG_SIZE 240
#define CONFIG_USED 38

void config_load(void);
void config_set(void);
void config_init(void);
void configMenu(void);

struct {
	u8 pksmLanguage;
	u8 automaticSaveBackup;
	u16 storageSize;
	u16 defaultTID;
	u16 defaultSID;
	u8 defaultOTName[24];
	u8 defaultNationality;
	u8 editInTransfers;
	u8 defaultDay;
	u8 defaultMonth;
	u8 defaultYear;
	u8 fixBadSectors;
} PKSM_Configuration;

#endif