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
#define DEBUG_I18N_LANG 5


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
	char *types;
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
	S_GRAPHIC_PKBANK_LV_PKMN,                      // Line 185
	S_GRAPHIC_PKBANK_OTID_PKMN,                    // Line 186
	S_GRAPHIC_PKBANK_SAVED_BOX_TITLE,              // Line 187

	S_GRAPHIC_PKBANK_MENU_VIEW,                    // Line 188
	S_GRAPHIC_PKBANK_MENU_CLEARBOX,                // Line 189
	S_GRAPHIC_PKBANK_MENU_RELEASE,                 // Line 190

	S_GRAPHIC_MASSINJECTOR_XD_COLLECTION,             // Line 191
	S_GRAPHIC_MASSINJECTOR_COLOSSEUM_COLLECTION,      // Line 192
	S_GRAPHIC_MASSINJECTOR_10TH_ANNIV_COLLECTION,     // Line 193
	S_GRAPHIC_MASSINJECTOR_N_COLLECTION,              // Line 194
	S_GRAPHIC_MASSINJECTOR_ENTREE_FOREST_COLLECTION,  // Line 195
	S_GRAPHIC_MASSINJECTOR_DREAM_RADAR_COLLECTION,    // Line 196
	S_GRAPHIC_MASSINJECTOR_LIVING_DEX,                // Line 197
	S_GRAPHIC_MASSINJECTOR_OBLIVIA_DEOXYS_COLLECTION, // Line 198
	S_GRAPHIC_MASSINJECTOR_PKMN_RANCH_COLLECTION,     // Line 199
	S_GRAPHIC_MASSINJECTOR_KOR_EVENTS_COLLECTION,     // Line 200

	S_GRAPHIC_MASSINJECTOR_MESSAGE_REPLACE_BOXES,     // Line 201
	S_GRAPHIC_MASSINJECTOR_TITLE,                     // Line 202
	S_GRAPHIC_MASSINJECTOR_INDICATION,                // Line 203

	S_GRAPHIC_SETTINGS_BANK_SIZE,                     // Line 204
	S_GRAPHIC_SETTINGS_BACKUP_SAVE,                   // Line 205
	S_GRAPHIC_SETTINGS_BACKUP_BANK,                   // Line 206
	S_GRAPHIC_SETTINGS_INDICATION,                    // Line 207

	S_BUTTON_YES,                                     // Line 208
	S_BUTTON_NO,                                      // Line 209

	S_GRAPHIC_HEXEDITOR_SPECIES,                      // Line 210
	S_GRAPHIC_HEXEDITOR_HELD_ITEM,                    // Line 211
	S_GRAPHIC_HEXEDITOR_TID,                          // Line 212
	S_GRAPHIC_HEXEDITOR_SID,                          // Line 213
	S_GRAPHIC_HEXEDITOR_ABILITY,                      // Line 214
	S_GRAPHIC_HEXEDITOR_NATURE,                       // Line 215

	S_GRAPHIC_HEXEDITOR_FATEFUL_ENCOUNTER_FLAG,       // Line 216
	S_GRAPHIC_HEXEDITOR_GENDER,                       // Line 217
	S_GRAPHIC_HEXEDITOR_GENDER_MALE,                  // Line 218
	S_GRAPHIC_HEXEDITOR_GENDER_FEMALE,                // Line 219
	S_GRAPHIC_HEXEDITOR_GENDER_GENDERLESS,            // Line 220
	S_GRAPHIC_HEXEDITOR_ALTERNATIVE_FORM,             // Line 221

	S_GRAPHIC_HEXEDITOR_EV_HP,                        // Line 222
	S_GRAPHIC_HEXEDITOR_EV_ATK,                       // Line 223
	S_GRAPHIC_HEXEDITOR_EV_DEF,                       // Line 224
	S_GRAPHIC_HEXEDITOR_EV_SPE,                       // Line 225
	S_GRAPHIC_HEXEDITOR_EV_SPA,                       // Line 226
	S_GRAPHIC_HEXEDITOR_EV_SPD,                       // Line 227

	S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_COOL,           // Line 228
	S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_BEAUTY,         // Line 229
	S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_CUTE,           // Line 230
	S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_SMART,          // Line 231
	S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_TOUGH,          // Line 232
	S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_SHEEN,          // Line 233

	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_KALOS,        // Line 234
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_G3_HOENN,     // Line 235
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_SINNOH,       // Line 236
	S_GRAPHIC_HEXEDITOR_RIBBON_BEST_FRIENDS,          // Line 237
	S_GRAPHIC_HEXEDITOR_RIBBON_TRAINING,              // Line 238
	S_GRAPHIC_HEXEDITOR_RIBBON_SKILLFUL_BATTLER,      // Line 239
	S_GRAPHIC_HEXEDITOR_RIBBON_BATTLER_EXPERT,        // Line 240
	S_GRAPHIC_HEXEDITOR_RIBBON_EFFORT,                // Line 241

	S_GRAPHIC_HEXEDITOR_RIBBON_ALERT,                 // Line 242
	S_GRAPHIC_HEXEDITOR_RIBBON_SHOCK,                 // Line 243
	S_GRAPHIC_HEXEDITOR_RIBBON_DOWNCAST,              // Line 244
	S_GRAPHIC_HEXEDITOR_RIBBON_CARELESS,              // Line 245
	S_GRAPHIC_HEXEDITOR_RIBBON_RELAX,                 // Line 246
	S_GRAPHIC_HEXEDITOR_RIBBON_SNOOZE,                // Line 247
	S_GRAPHIC_HEXEDITOR_RIBBON_SMILE,                 // Line 248
	S_GRAPHIC_HEXEDITOR_RIBBON_GORGEOUS,              // Line 249

	S_GRAPHIC_HEXEDITOR_RIBBON_ROYAL,                 // Line 250
	S_GRAPHIC_HEXEDITOR_RIBBON_GORGEOUS_ROYAL,        // Line 251
	S_GRAPHIC_HEXEDITOR_RIBBON_ARTIST,                // Line 252
	S_GRAPHIC_HEXEDITOR_RIBBON_FOOTPRINT,             // Line 253
	S_GRAPHIC_HEXEDITOR_RIBBON_RECORD,                // Line 254
	S_GRAPHIC_HEXEDITOR_RIBBON_LEGEND,                // Line 255
	S_GRAPHIC_HEXEDITOR_RIBBON_COUNTRY,               // Line 256
	S_GRAPHIC_HEXEDITOR_RIBBON_NATIONAL,              // Line 257

	S_GRAPHIC_HEXEDITOR_RIBBON_EARTH,                 // Line 258
	S_GRAPHIC_HEXEDITOR_RIBBON_WORLD,                 // Line 259
	S_GRAPHIC_HEXEDITOR_RIBBON_CLASSIC,               // Line 260
	S_GRAPHIC_HEXEDITOR_RIBBON_PREMIER,               // Line 261
	S_GRAPHIC_HEXEDITOR_RIBBON_EVENT,                 // Line 262
	S_GRAPHIC_HEXEDITOR_RIBBON_BIRTHDAY,              // Line 263
	S_GRAPHIC_HEXEDITOR_RIBBON_SPECIAL,               // Line 264
	S_GRAPHIC_HEXEDITOR_RIBBON_SOUVENIR,              // Line 265

	S_GRAPHIC_HEXEDITOR_RIBBON_WISHING,               // Line 266
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_BATTLE,       // Line 267
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_REGIONAL,     // Line 268
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_NATIONAL,     // Line 269
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_WORLD,        // Line 270
	S_GRAPHIC_HEXEDITOR_RIBBON_38,                    // Line 271
	S_GRAPHIC_HEXEDITOR_RIBBON_39,                    // Line 272
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_G6_HOENN,     // Line 273

	S_GRAPHIC_HEXEDITOR_RIBBON_CONTEST_STAR,          // Line 274
	S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_COOLNESS,       // Line 275
	S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_BEAUTY,         // Line 276
	S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_CUTENESS,       // Line 277
	S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_CLEVERNESS,     // Line 278
	S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_TOUGHNESS,      // Line 279
	S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_ALOLA,        // Line 280
	S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_ROYALE,         // Line 281

	S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_TREE_GREAT,     // Line 282
	S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_TREE_MASTER,    // Line 283
	S_GRAPHIC_HEXEDITOR_RIBBON_51,                    // Line 284
	S_GRAPHIC_HEXEDITOR_RIBBON_52,                    // Line 285
	S_GRAPHIC_HEXEDITOR_RIBBON_53,                    // Line 286
	S_GRAPHIC_HEXEDITOR_RIBBON_54,                    // Line 287
	S_GRAPHIC_HEXEDITOR_RIBBON_55,                    // Line 288
	S_GRAPHIC_HEXEDITOR_RIBBON_56,                    // Line 289

	S_GRAPHIC_HEXEDITOR_NICKNAME,                     // Line 290

	S_GRAPHIC_HEXEDITOR_MOVE1,                        // Line 291
	S_GRAPHIC_HEXEDITOR_MOVE2,                        // Line 292
	S_GRAPHIC_HEXEDITOR_MOVE3,                        // Line 293
	S_GRAPHIC_HEXEDITOR_MOVE4,                        // Line 294

	S_GRAPHIC_HEXEDITOR_MOVE1_PP,                     // Line 295
	S_GRAPHIC_HEXEDITOR_MOVE2_PP,                     // Line 296
	S_GRAPHIC_HEXEDITOR_MOVE3_PP,                     // Line 297
	S_GRAPHIC_HEXEDITOR_MOVE4_PP,                     // Line 298

	S_GRAPHIC_HEXEDITOR_MOVE1_PPUP,                   // Line 299
	S_GRAPHIC_HEXEDITOR_MOVE2_PPUP,                   // Line 300
	S_GRAPHIC_HEXEDITOR_MOVE3_PPUP,                   // Line 301
	S_GRAPHIC_HEXEDITOR_MOVE4_PPUP,                   // Line 302

	S_GRAPHIC_HEXEDITOR_RELEARN_MOVE1,                // Line 303
	S_GRAPHIC_HEXEDITOR_RELEARN_MOVE2,                // Line 304
	S_GRAPHIC_HEXEDITOR_RELEARN_MOVE3,                // Line 305
	S_GRAPHIC_HEXEDITOR_RELEARN_MOVE4,                // Line 306

	S_GRAPHIC_HEXEDITOR_IS_NICKNAMED,                 // Line 307
	S_GRAPHIC_HEXEDITOR_IS_EGG,                       // Line 308

	S_GRAPHIC_HEXEDITOR_HELD_TRAINER_NAME,            // Line 309
	S_GRAPHIC_HEXEDITOR_HELD_TRAINER_FRIENDSHIP,      // Line 310
	S_GRAPHIC_HEXEDITOR_HELD_TRAINER_AFFECTION,       // Line 311
	S_GRAPHIC_HEXEDITOR_HELD_TRAINER_INTENSITY,       // Line 312
	S_GRAPHIC_HEXEDITOR_HELD_TRAINER_MEMORY,          // Line 313
	S_GRAPHIC_HEXEDITOR_HELD_TRAINER_FEELING,         // Line 314

	S_GRAPHIC_HEXEDITOR_FULLNESS,                     // Line 315
	S_GRAPHIC_HEXEDITOR_ENJOYMENT,                    // Line 316

	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_NAME,        // Line 317
	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_FRIENDSHIP,  // Line 318
	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_AFFECTION,   // Line 319
	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_INTENSITY,   // Line 320
	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_MEMORY,      // Line 321
	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_FEELING,     // Line 322

	S_GRAPHIC_HEXEDITOR_EGG_YEAR,                     // Line 323
	S_GRAPHIC_HEXEDITOR_EGG_MONTH,                    // Line 324
	S_GRAPHIC_HEXEDITOR_EGG_DAY,                      // Line 325

	S_GRAPHIC_HEXEDITOR_MET_YEAR,                     // Line 326
	S_GRAPHIC_HEXEDITOR_MET_MONTH,                    // Line 327
	S_GRAPHIC_HEXEDITOR_MET_DAY,                      // Line 328
	S_GRAPHIC_HEXEDITOR_MET_LEVEL,                    // Line 329

	S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_GENDER,      // Line 330

	S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_HP,             // Line 331
	S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_ATK,            // Line 332
	S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_DEF,            // Line 333
	S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPATK,          // Line 334
	S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPDEF,          // Line 335
	S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPEED,          // Line 336


	S_HEXEDITOR_DESC_0x00,                            // Line 337
	S_HEXEDITOR_DESC_0x01,                            // Line 338
	S_HEXEDITOR_DESC_0x02,                            // Line 339
	S_HEXEDITOR_DESC_0x03,                            // Line 340
	S_HEXEDITOR_DESC_0x04,                            // Line 341
	S_HEXEDITOR_DESC_0x05,                            // Line 342
	S_HEXEDITOR_DESC_0x06,                            // Line 343
	S_HEXEDITOR_DESC_0x07,                            // Line 344
	S_HEXEDITOR_DESC_0x08,                            // Line 345
	S_HEXEDITOR_DESC_0x09,                            // Line 346
	S_HEXEDITOR_DESC_0x0A,                            // Line 347
	S_HEXEDITOR_DESC_0x0B,                            // Line 348
	S_HEXEDITOR_DESC_0x0C,                            // Line 349
	S_HEXEDITOR_DESC_0x0D,                            // Line 350
	S_HEXEDITOR_DESC_0x0E,                            // Line 351
	S_HEXEDITOR_DESC_0x0F,                            // Line 352
	S_HEXEDITOR_DESC_0x10,                            // Line 353
	S_HEXEDITOR_DESC_0x11,                            // Line 354
	S_HEXEDITOR_DESC_0x12,                            // Line 355
	S_HEXEDITOR_DESC_0x13,                            // Line 356
	S_HEXEDITOR_DESC_0x14,                            // Line 357
	S_HEXEDITOR_DESC_0x15,                            // Line 358
	S_HEXEDITOR_DESC_0x16,                            // Line 359
	S_HEXEDITOR_DESC_0x17,                            // Line 360
	S_HEXEDITOR_DESC_0x18,                            // Line 361
	S_HEXEDITOR_DESC_0x19,                            // Line 362
	S_HEXEDITOR_DESC_0x1A,                            // Line 363
	S_HEXEDITOR_DESC_0x1B,                            // Line 364
	S_HEXEDITOR_DESC_0x1C,                            // Line 365
	S_HEXEDITOR_DESC_0x1D,                            // Line 366
	S_HEXEDITOR_DESC_0x1E,                            // Line 367
	S_HEXEDITOR_DESC_0x1F,                            // Line 368
	S_HEXEDITOR_DESC_0x20,                            // Line 369
	S_HEXEDITOR_DESC_0x21,                            // Line 370
	S_HEXEDITOR_DESC_0x22,                            // Line 371
	S_HEXEDITOR_DESC_0x23,                            // Line 372
	S_HEXEDITOR_DESC_0x24,                            // Line 373
	S_HEXEDITOR_DESC_0x25,                            // Line 374
	S_HEXEDITOR_DESC_0x26,                            // Line 375
	S_HEXEDITOR_DESC_0x27,                            // Line 376
	S_HEXEDITOR_DESC_0x28,                            // Line 377
	S_HEXEDITOR_DESC_0x29,                            // Line 378
	S_HEXEDITOR_DESC_0x2A,                            // Line 379
	S_HEXEDITOR_DESC_0x2B,                            // Line 380
	S_HEXEDITOR_DESC_0x2C,                            // Line 381
	S_HEXEDITOR_DESC_0x2D,                            // Line 382
	S_HEXEDITOR_DESC_0x2E,                            // Line 383
	S_HEXEDITOR_DESC_0x2F,                            // Line 384
	S_HEXEDITOR_DESC_0x30,                            // Line 385
	S_HEXEDITOR_DESC_0x31,                            // Line 386
	S_HEXEDITOR_DESC_0x32,                            // Line 387
	S_HEXEDITOR_DESC_0x33,                            // Line 388
	S_HEXEDITOR_DESC_0x34,                            // Line 389
	S_HEXEDITOR_DESC_0x35,                            // Line 390
	S_HEXEDITOR_DESC_0x36,                            // Line 391
	S_HEXEDITOR_DESC_0x37,                            // Line 392
	S_HEXEDITOR_DESC_0x38,                            // Line 393
	S_HEXEDITOR_DESC_0x39,                            // Line 394
	S_HEXEDITOR_DESC_0x3A,                            // Line 395
	S_HEXEDITOR_DESC_0x40,                            // Line 396
	S_HEXEDITOR_DESC_0x41,                            // Line 397
	S_HEXEDITOR_DESC_0x42,                            // Line 398
	S_HEXEDITOR_DESC_0x43,                            // Line 399
	S_HEXEDITOR_DESC_0x44,                            // Line 400
	S_HEXEDITOR_DESC_0x45,                            // Line 401
	S_HEXEDITOR_DESC_0x46,                            // Line 402
	S_HEXEDITOR_DESC_0x47,                            // Line 403
	S_HEXEDITOR_DESC_0x48,                            // Line 404
	S_HEXEDITOR_DESC_0x49,                            // Line 405
	S_HEXEDITOR_DESC_0x4A,                            // Line 406
	S_HEXEDITOR_DESC_0x4B,                            // Line 407
	S_HEXEDITOR_DESC_0x4C,                            // Line 408
	S_HEXEDITOR_DESC_0x4D,                            // Line 409
	S_HEXEDITOR_DESC_0x4E,                            // Line 410
	S_HEXEDITOR_DESC_0x4F,                            // Line 411
	S_HEXEDITOR_DESC_0x50,                            // Line 412
	S_HEXEDITOR_DESC_0x51,                            // Line 413
	S_HEXEDITOR_DESC_0x52,                            // Line 414
	S_HEXEDITOR_DESC_0x53,                            // Line 415
	S_HEXEDITOR_DESC_0x54,                            // Line 416
	S_HEXEDITOR_DESC_0x55,                            // Line 417
	S_HEXEDITOR_DESC_0x56,                            // Line 418
	S_HEXEDITOR_DESC_0x57,                            // Line 419
	S_HEXEDITOR_DESC_0x5A,                            // Line 420
	S_HEXEDITOR_DESC_0x5B,                            // Line 421
	S_HEXEDITOR_DESC_0x5C,                            // Line 422
	S_HEXEDITOR_DESC_0x5D,                            // Line 423
	S_HEXEDITOR_DESC_0x5E,                            // Line 424
	S_HEXEDITOR_DESC_0x5F,                            // Line 425
	S_HEXEDITOR_DESC_0x60,                            // Line 426
	S_HEXEDITOR_DESC_0x61,                            // Line 427
	S_HEXEDITOR_DESC_0x62,                            // Line 428
	S_HEXEDITOR_DESC_0x63,                            // Line 429
	S_HEXEDITOR_DESC_0x64,                            // Line 430
	S_HEXEDITOR_DESC_0x65,                            // Line 431
	S_HEXEDITOR_DESC_0x66,                            // Line 432
	S_HEXEDITOR_DESC_0x67,                            // Line 433
	S_HEXEDITOR_DESC_0x68,                            // Line 434
	S_HEXEDITOR_DESC_0x69,                            // Line 435
	S_HEXEDITOR_DESC_0x6A,                            // Line 436
	S_HEXEDITOR_DESC_0x6B,                            // Line 437
	S_HEXEDITOR_DESC_0x6C,                            // Line 438
	S_HEXEDITOR_DESC_0x6D,                            // Line 439
	S_HEXEDITOR_DESC_0x6E,                            // Line 450
	S_HEXEDITOR_DESC_0x6F,                            // Line 441
	S_HEXEDITOR_DESC_0x70,                            // Line 442
	S_HEXEDITOR_DESC_0x71,                            // Line 443
	S_HEXEDITOR_DESC_0x72,                            // Line 444
	S_HEXEDITOR_DESC_0x74,                            // Line 445
	S_HEXEDITOR_DESC_0x75,                            // Line 446
	S_HEXEDITOR_DESC_0x76,                            // Line 447
	S_HEXEDITOR_DESC_0x77,                            // Line 448
	S_HEXEDITOR_DESC_0x78,                            // Line 449
	S_HEXEDITOR_DESC_0x79,                            // Line 450
	S_HEXEDITOR_DESC_0x7A,                            // Line 451
	S_HEXEDITOR_DESC_0x7B,                            // Line 452
	S_HEXEDITOR_DESC_0x7C,                            // Line 453
	S_HEXEDITOR_DESC_0x7D,                            // Line 454
	S_HEXEDITOR_DESC_0x7E,                            // Line 455
	S_HEXEDITOR_DESC_0x7F,                            // Line 456
	S_HEXEDITOR_DESC_0x80,                            // Line 457
	S_HEXEDITOR_DESC_0x81,                            // Line 458
	S_HEXEDITOR_DESC_0x82,                            // Line 459
	S_HEXEDITOR_DESC_0x83,                            // Line 460
	S_HEXEDITOR_DESC_0x84,                            // Line 461
	S_HEXEDITOR_DESC_0x85,                            // Line 462
	S_HEXEDITOR_DESC_0x86,                            // Line 463
	S_HEXEDITOR_DESC_0x87,                            // Line 464
	S_HEXEDITOR_DESC_0x88,                            // Line 465
	S_HEXEDITOR_DESC_0x89,                            // Line 466
	S_HEXEDITOR_DESC_0x8A,                            // Line 467
	S_HEXEDITOR_DESC_0x8B,                            // Line 468
	S_HEXEDITOR_DESC_0x8C,                            // Line 469
	S_HEXEDITOR_DESC_0x8D,                            // Line 470
	S_HEXEDITOR_DESC_0x8E,                            // Line 471
	S_HEXEDITOR_DESC_0x8F,                            // Line 472
	S_HEXEDITOR_DESC_0x92,                            // Line 473
	S_HEXEDITOR_DESC_0x93,                            // Line 474
	S_HEXEDITOR_DESC_0x94,                            // Line 475
	S_HEXEDITOR_DESC_0x95,                            // Line 476
	S_HEXEDITOR_DESC_0x96,                            // Line 477
	S_HEXEDITOR_DESC_0x97,                            // Line 478
	S_HEXEDITOR_DESC_0x98,                            // Line 479
	S_HEXEDITOR_DESC_0x99,                            // Line 480
	S_HEXEDITOR_DESC_0x9A,                            // Line 481
	S_HEXEDITOR_DESC_0x9B,                            // Line 482
	S_HEXEDITOR_DESC_0x9C,                            // Line 483
	S_HEXEDITOR_DESC_0x9D,                            // Line 484
	S_HEXEDITOR_DESC_0xA2,                            // Line 485
	S_HEXEDITOR_DESC_0xA3,                            // Line 486
	S_HEXEDITOR_DESC_0xA4,                            // Line 487
	S_HEXEDITOR_DESC_0xA5,                            // Line 488
	S_HEXEDITOR_DESC_0xA6,                            // Line 499
	S_HEXEDITOR_DESC_0xA8,                            // Line 490
	S_HEXEDITOR_DESC_0xA9,                            // Line 491
	S_HEXEDITOR_DESC_0xAE,                            // Line 492
	S_HEXEDITOR_DESC_0xAF,                            // Line 493
	S_HEXEDITOR_DESC_0xB0,                            // Line 494
	S_HEXEDITOR_DESC_0xB1,                            // Line 495
	S_HEXEDITOR_DESC_0xB2,                            // Line 496
	S_HEXEDITOR_DESC_0xB3,                            // Line 497
	S_HEXEDITOR_DESC_0xB4,                            // Line 498
	S_HEXEDITOR_DESC_0xB5,                            // Line 499
	S_HEXEDITOR_DESC_0xB6,                            // Line 500
	S_HEXEDITOR_DESC_0xB7,                            // Line 501
	S_HEXEDITOR_DESC_0xB8,                            // Line 502
	S_HEXEDITOR_DESC_0xB9,                            // Line 503
	S_HEXEDITOR_DESC_0xBA,                            // Line 504
	S_HEXEDITOR_DESC_0xBB,                            // Line 505
	S_HEXEDITOR_DESC_0xBC,                            // Line 506
	S_HEXEDITOR_DESC_0xBD,                            // Line 507
	S_HEXEDITOR_DESC_0xBE,                            // Line 508
	S_HEXEDITOR_DESC_0xBF,                            // Line 509
	S_HEXEDITOR_DESC_0xC0,                            // Line 510
	S_HEXEDITOR_DESC_0xC1,                            // Line 511
	S_HEXEDITOR_DESC_0xC2,                            // Line 512
	S_HEXEDITOR_DESC_0xC3,                            // Line 513
	S_HEXEDITOR_DESC_0xC4,                            // Line 514
	S_HEXEDITOR_DESC_0xC5,                            // Line 515
	S_HEXEDITOR_DESC_0xC6,                            // Line 516
	S_HEXEDITOR_DESC_0xC7,                            // Line 517
	S_HEXEDITOR_DESC_0xCA,                            // Line 518
	S_HEXEDITOR_DESC_0xCB,                            // Line 519
	S_HEXEDITOR_DESC_0xCC,                            // Line 520
	S_HEXEDITOR_DESC_0xCD,                            // Line 521
	S_HEXEDITOR_DESC_0xCE,                            // Line 522
	S_HEXEDITOR_DESC_0xD0,                            // Line 523
	S_HEXEDITOR_DESC_0xD1,                            // Line 524
	S_HEXEDITOR_DESC_0xD2,                            // Line 525
	S_HEXEDITOR_DESC_0xD3,                            // Line 526
	S_HEXEDITOR_DESC_0xD4,                            // Line 527
	S_HEXEDITOR_DESC_0xD5,                            // Line 528
	S_HEXEDITOR_DESC_0xD6,                            // Line 529
	S_HEXEDITOR_DESC_0xD8,                            // Line 530
	S_HEXEDITOR_DESC_0xDA,                            // Line 531
	S_HEXEDITOR_DESC_0xDC,                            // Line 532
	S_HEXEDITOR_DESC_0xDD,                            // Line 533
	S_HEXEDITOR_DESC_0xDE,                            // Line 534
	S_HEXEDITOR_DESC_0xDF,                            // Line 535
	S_HEXEDITOR_DESC_0xE0,                            // Line 536
	S_HEXEDITOR_DESC_0xE1,                            // Line 537
	S_HEXEDITOR_DESC_0xE2,                            // Line 538
	S_HEXEDITOR_DESC_0xE3,                            // Line 539

	S_HTTP_SERVER_RUNNING,                            // Line 540
	S_HTTP_BUFFER_ALLOC_FAILED,                       // Line 541
	S_HTTP_SOCINIT_FAILED,                            // Line 542
	S_HTTP_SOCKET_UNACCESSIBLE,                       // Line 543
	S_HTTP_BINDING_FAILED,                            // Line 544
	S_HTTP_LISTENING_FAILED,                          // Line 545
	S_HTTP_ERROR_PROCESSING_PHASE,                    // Line 546

	S_HTTP_HTTP_CONTEXT_OPEN_FAILED,                  // Line 547
	S_HTTP_DOWNLOAD_ASSETS_FAILED,                    // Line 548
	S_HTTP_ADD_REQUEST_HEADER_FIELD_FAILED,           // Line 549
	S_HTTP_SET_SSLOPT_FAILED,                         // Line 550
	S_HTTP_BEGIN_HTTP_REQUEST_FAILED,                 // Line 551
	S_HTTP_RECEIVE_STATUS_CODE_FAILED,                // Line 552
	S_HTTP_REDIRECTION_FAILED,                        // Line 553
	S_HTTP_RECEIVE_DOWNLOAD_SIZE_FAILED,              // Line 554
	S_HTTP_ALLOC_MEMORY_FAILED,                       // Line 555

	S_UTIL_BANK_CHANGING_SIZE,                        // Line 556
	S_UTIL_BANK_SIZE_CHANGED,                         // Line 557
	S_UTIL_BACKUP_SAVE_CREATED,                       // Line 558
	S_UTIL_BACKUP_BANK_CREATED,                       // Line 559
} AppTextCode;


struct ArrayUTF32 i18n_FileToArrayUTF32(char* filepath);
struct i18n_files i18n_getFilesPath();
wchar_t* s_utf32(char* str);
wchar_t* ss_utf32(char* str, int size);

// struct ArrayUTF32 ArrayUTF32_copy(struct ArrayUTF32 from);
void ArrayUTF32_sort_starting_index(struct ArrayUTF32 *arr, int index);
void ArrayUTF32_sort_starting_index_with_sort_func(struct ArrayUTF32 *arr, int index, int (*f)(const wchar_t *a,const wchar_t *b));
void ArrayUTF32_sort(struct ArrayUTF32 *arr);
void debuglogf(const char* format, ...);

wchar_t* i18n(AppTextCode code);
void i18n_init();
void i18n_initTextSwkbd(SwkbdState* swkbd, AppTextCode leftButtonTextCode, AppTextCode rightButtonTextCode, AppTextCode hintTextCode);

