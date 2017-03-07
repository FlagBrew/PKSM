/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   Multi-Language support added by Naxann
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
#include <stdarg.h>
#include <wchar.h>

#define DEBUG_I18N 1

/**
 * Localization files
 */
struct i18n_files {
	char* abilities;
	char* species;
	char* natures;
	char* moves;
	char* items;
	char* hp;
	char* app;
};

/**
 * Array of strings in UTF32
 */
struct ArrayUTF32 {
	int length;
	wchar_t** items;
	wchar_t** sortedItems;
	int* sortedItemsID;
	bool sorted;
};


/**
 * Labels of localization strings used in the application
 */
enum i18n_App_Labels {
	MENU_1_TEST = 0
};


struct ArrayUTF32 i18n_FileToArrayUTF32(char* filepath);
struct i18n_files i18n_getFilesPath();
wchar_t* s_utf32(char* str);
wchar_t* ss_utf32(char* str, int size);

// struct ArrayUTF32 ArrayUTF32_copy(struct ArrayUTF32 from);
void ArrayUTF32_sort_starting_index(struct ArrayUTF32 *arr, int index);
void ArrayUTF32_sort(struct ArrayUTF32 *arr);
void debuglogf(const char* format, ...);
