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

#define TEXTURE_BACK_BUTTON 1
#define TEXTURE_BACKGROUND 2
#define TEXTURE_BALLS_BG 3
#define TEXTURE_BANK_TOP 4
#define TEXTURE_SELECTED_DS_BAR 5
#define TEXTURE_DS_BAR 6
#define TEXTURE_BLUE_TEXT_BOX 7 
#define TEXTURE_BOTTOM_BAR 8 
#define TEXTURE_DS_BOTTOM_BG 9 
#define TEXTURE_BOTTOM_POPUP 10 
#define TEXTURE_BOX_VIEW 11 
#define TEXTURE_BUTTON 12 
#define TEXTURE_CREDITS 13 
#define TEXTURE_DARK_BUTTON 14 
#define TEXTURE_DS_MENU_BOTTOM_BG 15 
#define TEXTURE_DS_POKEBALL 16 
#define TEXTURE_EDITOR_BOTTOM_BG 17 
#define TEXTURE_EDITOR_STATS 18 
#define TEXTURE_DS_EVENT_DATABASE_BG 19 
#define TEXTURE_EVENT_MENU_BOTTOM_BAR 20 
#define TEXTURE_EVENT_MENU_TOP_BAR_SELECTED 21 
#define TEXTURE_EVENT_MENU_TOP_BAR_NORMAL 22 
#define TEXTURE_EVENT_VIEW 23 
#define TEXTURE_FEMALE 24 
#define TEXTURE_GENERATION_BG 25 
#define TEXTURE_GENERATIONS 26
#define TEXTURE_HEX_BG 27
#define TEXTURE_HEX_BUTTON 28 
#define TEXTURE_HIDDEN_POWER_BG 29
#define TEXTURE_ICON_BANK 30 
#define TEXTURE_ICON_CREDITS 31 
#define TEXTURE_ICON_EDITOR 32 
#define TEXTURE_ICON_EVENTS 33
#define TEXTURE_ICON_SAVE 34
#define TEXTURE_ICON_SETTINGS 35 
#define TEXTURE_INCLUDE_INFO 36
#define TEXTURE_INFO_VIEW 37 
#define TEXTURE_ITEM 38
#define TEXTURE_L_BUTTON 39
#define TEXTURE_LANGUAGE_BUTTON_SELECTED 40
#define TEXTURE_LANGUAGE_BUTTON_NORMAL 41
#define TEXTURE_LEFT 42
#define TEXTURE_LIGHT_BUTTON 43
#define TEXTURE_LOGOS_3DS 44
#define TEXTURE_LOGOS_GEN4 45
#define TEXTURE_LOGOS_GEN5 46
#define TEXTURE_MAIN_MENU_BUTTON 47
#define TEXTURE_MAIN_MENU_DARK_BAR 48
#define TEXTURE_MALE 49
#define TEXTURE_MINI_BOX 50
#define TEXTURE_MINUS_BUTTON 51
#define TEXTURE_MINUS 52
#define TEXTURE_MOVES_BOTTOM 53
#define TEXTURE_NATURES 54
#define TEXTURE_NO_MOVE 55
#define TEXTURE_NORMAL_BAR 56
#define TEXTURE_NORMAL_L 57
#define TEXTURE_NORMAL_R 58
#define TEXTURE_OTA_BUTTON 59
#define TEXTURE_PLUS_BUTTON 60
#define TEXTURE_PLUS 61
#define TEXTURE_POKEBALL 62
#define TEXTURE_POKEMON_BOX 63
#define TEXTURE_R_BUTTON 64
#define TEXTURE_RED_BUTTON 65
#define TEXTURE_RIGHT 66
#define TEXTURE_SELECTED_L 67
#define TEXTURE_SELECTED_R 68
#define TEXTURE_SELECTOR_CLONING 69
#define TEXTURE_SELECTOR_NORMAL 70
#define TEXTURE_SETTING 71
#define TEXTURE_SHINY 72
#define TEXTURE_SUB_ARROW 73
#define TEXTURE_DS_TOP_BG 74
#define TEXTURE_TOP_MOVES 75
#define TEXTURE_TRANSFER_BUTTON 76
#define TEXTURE_WARNING_BOTTOM 77
#define TEXTURE_WARNING_TOP 78
#define TEXTURE_NORMAL_SPRITESHEET 79
#define TEXTURE_ALTERNATIVE_SPRITESHEET 80
#define TEXTURE_BALLS_SPRITESHEET 81
#define TEXTURE_TYPES_SPRITESHEET 82
#define TEXTURE_SHINY_SPRITESHEET 83
#define TEXTURE_CUBES_SPRITESHEET 84
#define TEXTURE_BOXES_SPRITESHEET 85

#define FONT_SIZE_18 0.72f
#define FONT_SIZE_15 0.6f
#define FONT_SIZE_14 0.56f
#define FONT_SIZE_12 0.50f
#define FONT_SIZE_11 0.46f
#define FONT_SIZE_9 0.37f


#define BUTTONGREY RGBA8( 99, 110, 133, 255)
#define BUTTONBORD RGBA8(160, 164, 173, 255)

#define BARTEXT   RGBA8(114, 194, 255, 255)

#define BLUE      RGBA8(124, 147, 225, 255)
#define YELLOW    RGBA8(237, 247, 157, 255)
#define LIGHTBLUE RGBA8(187, 208, 254, 255)
#define DARKBLUE  RGBA8( 55,  89, 187, 255)
#define HIGHBLUE  RGBA8( 48,  65, 106, 255)
#define MASKBLACK RGBA8(  0,   0,   0, 190)
#define PALEBLUE  RGBA8( 90, 115, 164, 255)
#define MENUBLUE  RGBA8( 55,  89, 157, 255)

#define BACKGROUND_BLACK ABGR8(255, 0, 0, 0)
#define BLACK RGBA8(0, 0, 0, 255)
#define BUTTONGREEN RGBA8(116, 222, 126, 255)
#define BUTTONRED RGBA8(228, 109, 117, 255)
#define SHINYRED RGBA8(255, 17, 17, 255)
#define WHITE RGBA8(255, 255, 255, 255)
#define DS RGBA8(56, 56, 56, 255)
#define RED RGBA8(184, 32, 16, 255)

#define EGGSPRITEPOS 803

#define MOVEMENTSPEED 4

void GUIElementsInit();
void GUIElementsSpecify();
int getGUIElementsI18nSpecifyTotalElements();
void GUIElementsI18nSpecify(); // Needed for util.c when changing language
void GUITextsInit(); // Needed for util.c when changing language
void GUIGameElementsInit();
void initProgressLoadPNGInRAM(int total);
void GUIElementsExit();
void GUIElementsI18nExit(); // Needed for util.c when changing language
void init_font_cache();
void infoDisp(wchar_t* message);
int confirmDisp(wchar_t* message);
void freezeMsg(wchar_t* message);
void freezeMsgDetails(wchar_t* details);
void consoleMsg(char* details);
void progressBar(wchar_t* message, u32 pagesize, u32 sz);
void printCredits();
void printTitle(const wchar_t* title);
void gameSelectorMenu(int n);
void mainMenuDS(int currentEntry);
void menu(int menu[]);
void printDatabase6(char *database[], int currentEntry, int page, int spriteArray[]);
void printDatabaseListDS(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]);
void printDB7(u8* previewbuf, int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected, bool ota);
void printEditor(u8* mainbuf, u64 size, int currentEntry, int page);
void printPKEditor(u8* pkmn, int additional1, int additional2, int additional3, int mode, wchar_t* descriptions[]);
void printPKViewer(u8* mainbuf, u8* pkmn, bool isTeam, int currentEntry, int menuEntry, int box, int mode, int additional1, int additional2);
void printPKBank(u8* bankbuf, u8* mainbuf, u8* wirelessBuffer, u8* pkmnbuf, int currentEntry, int saveBox, int bankBox, bool isBufferized, bool isSeen, bool isWirelessActivated);
void printElement(u8* pkmn, int game, u16 n, int x, int y);
void printElementBlend(u8* pkmn, int game, u16 n, int x, int y, u32 color);
void printSettings(int box, int language);
void infoViewer(u8* pkmn);
void wcxInfoViewer(u8* buf);
void printfHexEditorInfo(u8* pkmn, int byte);
void printSaveEditorInfo(u8* pkmn, int byte);
void printDexViewer(u8* mainbuf, int currentEntry, int page, int seen, int caught);

void printSelector(int x, int y, int width, int height);
void printMenuTop();
void printMenuBottom();

