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
#include <unistd.h>

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
	char *forms;
	char *balls;
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
	S_FREEZEMSG_DEFAULT_DETAILS,				   // Line 34

	S_GRAPHIC_GAME_SELECTOR_INFO_CART_HAS_PRIO,    // Line 35
	S_GRAPHIC_GAME_SELECTOR_INDICATIONS,           // Line 36
	S_GRAPHIC_GAME_SELECTOR_GAME_X,                // Line 37
	S_GRAPHIC_GAME_SELECTOR_GAME_Y,                // Line 38
	S_GRAPHIC_GAME_SELECTOR_GAME_OS,               // Line 39
	S_GRAPHIC_GAME_SELECTOR_GAME_AS,               // Line 40
	S_GRAPHIC_GAME_SELECTOR_GAME_SUN,              // Line 41
	S_GRAPHIC_GAME_SELECTOR_GAME_MOON,             // Line 42

	S_GRAPHIC_GAME_SELECTOR_GAME_DIAMOND,          // Line 43
	S_GRAPHIC_GAME_SELECTOR_GAME_PEARL,            // Line 44
	S_GRAPHIC_GAME_SELECTOR_GAME_PLATINUM,         // Line 45
	S_GRAPHIC_GAME_SELECTOR_GAME_HG,               // Line 46
	S_GRAPHIC_GAME_SELECTOR_GAME_SS,               // Line 47
	S_GRAPHIC_GAME_SELECTOR_GAME_B,                // Line 48
	S_GRAPHIC_GAME_SELECTOR_GAME_W,                // Line 49
	S_GRAPHIC_GAME_SELECTOR_GAME_B2,               // Line 50
	S_GRAPHIC_GAME_SELECTOR_GAME_W2,               // Line 51

	S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING,        // Line 52
	S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING_DETAILS,// Line 53

	S_BANK_SIZE_ERROR,                             // Line 54
	S_BANK_Q_ERASE_SELECTED_BOX,                   // Line 55
	S_BANK_Q_SAVE_CHANGES,                         // Line 56
	S_BANK_Q_SAVE_POKEDEX_FLAGS,                   // Line 57
	S_BANK_PROGRESS_MESSAGE,                       // Line 58

	S_DATABASE_ERROR_INJECTION,                    // Line 59
	S_DATABASE_SUCCESS_INJECTION,                  // Line 60
	S_DATABASE_ITEM_NOT_AVAILABLE_XY,              // Line 61
	S_DATABASE_Q_SAVE_POKEDEX_FLAGS,               // Line 62
	S_DATABASE_PROGRESS_MESSAGE,                   // Line 63
	S_DATABASE_XD_COLLECTION_SUCCESS,              // Line 64
	S_DATABASE_COLOSSEUM_COLLECTION_SUCCESS,       // Line 65
	S_DATABASE_10TH_ANNIVERSARY_SUCCESS,           // Line 66
	S_DATABASE_N_COLLECTION_SUCCESS,               // Line 67
	S_DATABASE_ENTREE_FOREST_COLLECTION_SUCCESS,   // Line 68
	S_DATABASE_DREAM_RADAR_COLLECTION_SUCCESS,     // Line 69
	S_DATABASE_LIVING_DEX_SUCCESS,                 // Line 70
	S_DATABASE_DEOXYS_COLLECTION_SUCCESS,          // Line 71
	S_DATABASE_MY_POKEMON_RANCH_COLLECTION_SUCCESS,// Line 72
	S_DATABASE_KOREAN_COLLECTION_SUCCESS,          // Line 73

	S_EDITOR_LANGUAGE_SET_SUCCESS,                 // Line 74
	S_EDITOR_MISTERY_GIFT_CLEANED,                 // Line 75
	S_EDITOR_TEXT_CANCEL,                          // Line 76
	S_EDITOR_TEXT_SET,                             // Line 77
	S_EDITOR_TEXT_ENTER_NICKNAME_POKEMON,          // Line 78
	S_EDITOR_TEXT_ENTER_TRAINER_NAME,              // Line 79

	S_EDITOR_Q_CONFIRM_RELEASE,                    // Line 80
	S_EDITOR_RELEASED,                             // Line 81

	S_GRAPHIC_PROGRESSBAR_MESSAGE,                 // Line 82
	S_GRAPHIC_MENUDS_EVENTS,                       // Line 83
	S_GRAPHIC_MENUDS_OTHER,                        // Line 84
	S_GRAPHIC_MENUDS_INDICATIONS,                  // Line 85

	S_GRAPHIC_CREDITS_TITLE,                       // Line 86
	S_GRAPHIC_CREDITS_INDICATIONS,                 // Line 87

	S_GRAPHIC_DB6_TITLE,                           // Line 88
	S_GRAPHIC_DB6_INDICATIONS,                     // Line 89

	S_GRAPHIC_DB_INDICATIONS_INJECT,               // Line 90
	S_GRAPHIC_DB_INDICATIONS_SELECT,               // Line 91
	S_GRAPHIC_DB_LANGUAGES,                        // Line 92
	S_GRAPHIC_DB_OVERWRITE_WC,                     // Line 93
	S_GRAPHIC_DB_ADAPT_LANGUAGE_WC,                // Line 94
	S_GRAPHIC_DB_INJECT_WC_SLOT,                   // Line 95

	S_GRAPHIC_EDITOR_TITLE,                        // Line 96
	S_GRAPHIC_EDITOR_LANGUAGE,	                   // Line 97
	S_GRAPHIC_EDITOR_CLEAR_MYSTERY_GIFT_BOX,       // Line 98
	S_GRAPHIC_EDITOR_INDICATIONS,                  // Line 99

	S_GRAPHIC_INFOVIEWER_NICKNAME,                 // Line 100
	S_GRAPHIC_INFOVIEWER_OT,                       // Line 101
	S_GRAPHIC_INFOVIEWER_POKERUS,                  // Line 102
	S_GRAPHIC_INFOVIEWER_NATURE,                   // Line 103
	S_GRAPHIC_INFOVIEWER_ABILITY,                  // Line 104
	S_GRAPHIC_INFOVIEWER_ITEM,                     // Line 105
	S_GRAPHIC_INFOVIEWER_ESVTSV,                   // Line 106
	S_GRAPHIC_INFOVIEWER_TIDSID,                   // Line 107
	S_GRAPHIC_INFOVIEWER_HTOT_FRIENDSHIP,          // Line 108
	S_GRAPHIC_INFOVIEWER_HTOT_HIDDEN_POWER,        // Line 109

	S_GRAPHIC_INFOVIEWER_VALUE_HP,                 // Line 110
	S_GRAPHIC_INFOVIEWER_VALUE_ATTACK,             // Line 111
	S_GRAPHIC_INFOVIEWER_VALUE_DEFENSE,            // Line 112
	S_GRAPHIC_INFOVIEWER_VALUE_SP_ATK,             // Line 113
	S_GRAPHIC_INFOVIEWER_VALUE_SP_DEF,             // Line 114
	S_GRAPHIC_INFOVIEWER_VALUE_SPEED,              // Line 115
	S_GRAPHIC_INFOVIEWER_LV,                       // Line 116

	S_GRAPHIC_INFOVIEWER_MOVES,                    // Line 117
	S_GRAPHIC_INFOVIEWER_EGG_CYCLE,                // Line 118

	S_GRAPHIC_INFOVIEWER_YES,                      // Line 119
	S_GRAPHIC_INFOVIEWER_NO,                       // Line 120

	S_GRAPHIC_PKVIEWER_MENU_EDIT,                  // Line 121
	S_GRAPHIC_PKVIEWER_MENU_CLONE,                 // Line 122
	S_GRAPHIC_PKVIEWER_MENU_RELEASE,               // Line 123
	S_GRAPHIC_PKVIEWER_MENU_GENERATE,              // Line 124
	S_GRAPHIC_PKVIEWER_MENU_EXIT,                  // Line 125
	S_GRAPHIC_PKVIEWER_BOX,                        // Line 126
	S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT,          // Line 127
	S_GRAPHIC_PKVIEWER_OTA_INDICATIONS,            // Line 128
	S_GRAPHIC_PKVIEWER_TEAM,                       // Line 129

	S_GRAPHIC_PKVIEWER_CLONE_INDICATIONS,          // Line 130

	S_GRAPHIC_PKVIEWER_MENU_POKEMON_SELECTED,      // Line 131

	S_GRAPHIC_PKVIEWER_GENERATE_INDICATIONS,       // Line 132

	S_GRAPHIC_PKVIEWER_TIDSIDTSV,                  // Line 133
	S_GRAPHIC_PKVIEWER_SEED,                       // Line 134

	S_GRAPHIC_PKEDITOR_LEVEL,                      // Line 135
	S_GRAPHIC_PKEDITOR_NATURE,                     // Line 136
	S_GRAPHIC_PKEDITOR_ABILITY,                    // Line 137
	S_GRAPHIC_PKEDITOR_ITEM,                       // Line 138
	S_GRAPHIC_PKEDITOR_SHINY,                      // Line 139
	S_GRAPHIC_PKEDITOR_POKERUS,                    // Line 140
	S_GRAPHIC_PKEDITOR_OT,                         // Line 141
	S_GRAPHIC_PKEDITOR_NICKNAME,                   // Line 142
	S_GRAPHIC_PKEDITOR_FRIENDSHIP,                 // Line 143

	S_GRAPHIC_PKEDITOR_MENU_STATS,                 // Line 144
	S_GRAPHIC_PKEDITOR_MENU_MOVES,                 // Line 145
	S_GRAPHIC_PKEDITOR_MENU_SAVE,                  // Line 146

	S_GRAPHIC_PKEDITOR_STATS_HP,                   // Line 147
	S_GRAPHIC_PKEDITOR_STATS_ATTACK,               // Line 148
	S_GRAPHIC_PKEDITOR_STATS_DEFENSE,              // Line 149
	S_GRAPHIC_PKEDITOR_STATS_SP_ATTACK,            // Line 150
	S_GRAPHIC_PKEDITOR_STATS_SP_DEFENSE,           // Line 151
	S_GRAPHIC_PKEDITOR_STATS_SPEED,                // Line 152

	S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_1,   // Line 153
	S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_2,   // Line 154

	S_GRAPHIC_PKEDITOR_NATURE_NEUTRAL,             // Line 155
	S_GRAPHIC_PKEDITOR_NATURE_MIN_ATTACK,          // Line 156
	S_GRAPHIC_PKEDITOR_NATURE_MIN_DEFENSE,         // Line 157
	S_GRAPHIC_PKEDITOR_NATURE_MIN_SPEED,           // Line 158
	S_GRAPHIC_PKEDITOR_NATURE_MIN_SP_ATTACK,       // Line 159
	S_GRAPHIC_PKEDITOR_NATURE_MIN_SP_DEFENSE,      // Line 160
	S_GRAPHIC_PKEDITOR_NATURE_PLUS_ATTACK,         // Line 161
	S_GRAPHIC_PKEDITOR_NATURE_PLUS_DEFENSE,        // Line 162
	S_GRAPHIC_PKEDITOR_NATURE_PLUS_SPEED,          // Line 163
	S_GRAPHIC_PKEDITOR_NATURE_PLUS_SP_ATTACK,      // Line 164
	S_GRAPHIC_PKEDITOR_NATURE_PLUS_SP_DEFENSE,     // Line 165

	S_GRAPHIC_PKEDITOR_EGG_CYCLE,                  // Line 166
	S_GRAPHIC_PKEDITOR_SHINY_YES,                  // Line 167
	S_GRAPHIC_PKEDITOR_SHINY_NO,                   // Line 168
	S_GRAPHIC_PKEDITOR_POKERUS_YES,                // Line 169
	S_GRAPHIC_PKEDITOR_POKERUS_NO,                 // Line 170

	S_GRAPHIC_PKEDITOR_LBL_STATS,                  // Line 171
	S_GRAPHIC_PKEDITOR_LBL_IV,                     // Line 172
	S_GRAPHIC_PKEDITOR_LBL_EV,                     // Line 173
	S_GRAPHIC_PKEDITOR_LBL_TOTAL,                  // Line 174
	S_GRAPHIC_PKEDITOR_HIDDEN_POWER,               // Line 175

	S_GRAPHIC_PKEDITOR_MOVES,                      // Line 176
	S_GRAPHIC_PKEDITOR_RELEARN_MOVES,              // Line 177
	S_GRAPHIC_PKEDITOR_SELECTED_BYTE,              // Line 178

	S_GRAPHIC_PKEDITOR_ITEM_INDICATION,            // Line 179
	S_GRAPHIC_PKEDITOR_NATURE_INDICATION,          // Line 180
	S_GRAPHIC_PKEDITOR_BALL_INDICATION,            // Line 181
	S_GRAPHIC_PKEDITOR_HP_INDICATION,              // Line 182
	S_GRAPHIC_PKEDITOR_FORM_INDICATION,            // Line 183

	S_GRAPHIC_PKBANK_BANK_TITLE,                   // Line 184


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
void i18n_initTextSwkbd(SwkbdState* swkbd, AppTextCode leftButtonTextCode, AppTextCode rightButtonTextCode, AppTextCode hintTextCode);

