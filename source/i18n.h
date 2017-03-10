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

// debuglogf displays message on the app if defined to 1
#define DEBUG_I18N 1
// Force the lang to display in PKSM. Useful when debugging another lang than the 3DS lang
// Langs ID are defined in ctrulib : https://smealum.github.io/ctrulib/cfgu_8h.html
// CFG_LANGUAGE_JP = 0
// CFG_LANGUAGE_EN = 1
// CFG_LANGUAGE_FR = 2
// CFG_LANGUAGE_DE = 3
// CFG_LANGUAGE_IT = 4
// CFG_LANGUAGE_ES = 5
// CFG_LANGUAGE_ZH = 6
// CFG_LANGUAGE_KO = 7
// CFG_LANGUAGE_NL = 8
// CFG_LANGUAGE_PT = 9
// CFG_LANGUAGE_RU = 10
// CFG_LANGUAGE_TW = 11
// #define DEBUG_I18N_LANG 4


/**
 * Localization files
 */
struct i18n_files {
	char *abilities;
	char *species;
	char *natures;
	char *moves;
	char *items;
	char *hp;
	char *app;
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
typedef enum {
    S_DOWNLOADING_ASSETS = 0,                      // Line 1
	S_GUI_ELEMENTS_LOADING_FILES,                  // Line 2
	S_GUI_ELEMENTS_LOADING_LOCALES,                // Line 3
	S_GUI_ELEMENTS_LOADING_LOCALES_ABILITIES,      // Line 4
	S_GUI_ELEMENTS_LOADING_LOCALES_MOVES,          // Line 5
	S_GUI_ELEMENTS_LOADING_LOCALES_SORTING_MOVES,  // Line 6
	S_GUI_ELEMENTS_LOADING_LOCALES_NATURES,        // Line 7
	S_GUI_ELEMENTS_LOADING_LOCALES_ITEMS,          // Line 8
	S_GUI_ELEMENTS_LOADING_LOCALES_SORTING_ITEMS,  // Line 9
	S_GUI_ELEMENTS_LOADING_LOCALES_HP,             // Line 10
	S_GUI_ELEMENTS_LOADING_FONTS,                  // Line 11
	S_GUI_ELEMENTS_LOADING_FONTS_CACHE_FONT,       // Line 12
	S_GUI_ELEMENTS_LOADING_DONE,                   // Line 13

	S_MAIN_CREATING_DEFAULT_BANK,                  // Line 14
	S_MAIN_BACKING_UP_BANK,                        // Line 15
	S_MAIN_RESTART_APP,                            // Line 16
	S_MAIN_MISSING_ASSETS,                         // Line 17
	S_MAIN_LOADING_SAVE,                           // Line 18
	S_MAIN_GAME_NOT_FOUND,                         // Line 19
	S_MAIN_INCORRECT_SAVE_SIZE,                    // Line 20
	S_MAIN_NO_CARTRIDGE,                           // Line 21

	S_MAIN_MENU_MANAGEMENT,                        // Line 22
	S_MAIN_MENU_EVENTS,                            // Line 23
	S_MAIN_MENU_CREDITS,                           // Line 24
	S_MAIN_MENU_INDICATION,                        // Line 25
	S_MAIN_MENU_INDICATION_EXIT,                   // Line 26
	S_MAIN_MENU_MANAGEMENT_MENU_EDITOR,            // Line 27
	S_MAIN_MENU_MANAGEMENT_SAVE_INFO,              // Line 28
	S_MAIN_MENU_MANAGEMENT_EXTRA_STORAGE,          // Line 29
	S_MAIN_MENU_MANAGEMENT_MASS_INJECTOR,          // Line 30

	S_MAIN_Q_SAVE_CHANGES,                         // Line 31
	S_INFORMATION_MESSAGE_PRESS_A,                 // Line 32
	S_CONFIRMATION_MESSAGE_PRESS_A_OR_B,           // Line 33
	S_FREEZEMSG_DEFAULT_DETAILS,				   // Line 34,

	S_GRAPHIC_GAME_SELECTOR_INFO_CART_HAS_PRIO,    // Line 35,
	S_GRAPHIC_GAME_SELECTOR_INDICATIONS,           // Line 36,
	S_GRAPHIC_GAME_SELECTOR_GAME_X,                // Line 37,
	S_GRAPHIC_GAME_SELECTOR_GAME_Y,                // Line 38,
	S_GRAPHIC_GAME_SELECTOR_GAME_OS,               // Line 39,
	S_GRAPHIC_GAME_SELECTOR_GAME_AS,               // Line 40,
	S_GRAPHIC_GAME_SELECTOR_GAME_SUN,              // Line 41,
	S_GRAPHIC_GAME_SELECTOR_GAME_MOON,             // Line 42,

	S_GRAPHIC_GAME_SELECTOR_GAME_DIAMOND,          // Line 43,
	S_GRAPHIC_GAME_SELECTOR_GAME_PEARL,            // Line 44,
	S_GRAPHIC_GAME_SELECTOR_GAME_PLATINUM,         // Line 45,
	S_GRAPHIC_GAME_SELECTOR_GAME_HG,               // Line 46,
	S_GRAPHIC_GAME_SELECTOR_GAME_SS,               // Line 47,
	S_GRAPHIC_GAME_SELECTOR_GAME_B,                // Line 48,
	S_GRAPHIC_GAME_SELECTOR_GAME_W,                // Line 49,
	S_GRAPHIC_GAME_SELECTOR_GAME_B2,               // Line 50,
	S_GRAPHIC_GAME_SELECTOR_GAME_W2,               // Line 51,

	S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING,        // Line 52,
	S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING_DETAILS,// Line 53
} AppTextCode;


struct ArrayUTF32 i18n_FileToArrayUTF32(char* filepath);
struct i18n_files i18n_getFilesPath();
wchar_t* s_utf32(char* str);
wchar_t* ss_utf32(char* str, int size);

// struct ArrayUTF32 ArrayUTF32_copy(struct ArrayUTF32 from);
void ArrayUTF32_sort_starting_index(struct ArrayUTF32 *arr, int index);
void ArrayUTF32_sort(struct ArrayUTF32 *arr);
void debuglogf(const char* format, ...);

wchar_t* i18n(AppTextCode code);
void i18n_init();
