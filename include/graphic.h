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

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "common.h"

#define SPRITESHEET 2
#define TEXTURE_BACKGROUND 3
#define TEXTURE_BALLS_BG 4
#define TEXTURE_BANK_TOP 5
#define TEXTURE_BOX_VIEW 6
#define TEXTURE_CREDITS 7
#define TEXTURE_EDITOR_BOTTOM_BG 8
#define TEXTURE_EDITOR_STATS 9
#define TEXTURE_EVENT_VIEW 10
#define TEXTURE_GENERATION_BG 11
#define TEXTURE_HEX_BG 12
#define TEXTURE_HIDDEN_POWER_BG 13
#define TEXTURE_INFO_VIEW 14
#define TEXTURE_LOGOS_3DS 15
#define TEXTURE_LOGOS_GEN4 16
#define TEXTURE_LOGOS_GEN5 17
#define TEXTURE_MOVES_BOTTOM 18
#define TEXTURE_NATURES 19
#define TEXTURE_TOP_MOVES 20
#define TEXTURE_WARNING_BOTTOM 21
#define TEXTURE_WARNING_TOP 22
#define TEXTURE_NORMAL_SPRITESHEET 23
#define TEXTURE_ALTERNATIVE_SPRITESHEET 24
#define TEXTURE_BALLS_SPRITESHEET 25
#define TEXTURE_TYPES_SPRITESHEET 26
#define TEXTURE_SHINY_SPRITESHEET 27
#define TEXTURE_CUBES_SPRITESHEET 28
#define TEXTURE_BOXES_SPRITESHEET 29

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
void GUIGameElementsInit();
void GUIElementsExit();
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
void menu(int menu[]);
void printEventList(char *database[], int currentEntry, int page, int spriteArray[]);
void printEventInjector(u8* previewbuf, int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected, bool ota);
void printPKEditor(u8* pkmn, int additional1, int additional2, int additional3, int mode, wchar_t* descriptions[]);
void printPKViewer(u8* mainbuf, u8* pkmn, bool isTeam, int currentEntry, int menuEntry, int box, int mode, int additional1, int additional2);
void printPKBank(u8* bankbuf, u8* mainbuf, u8* wirelessBuffer, u8* pkmnbuf, int currentEntry, int saveBox, int bankBox, bool isBufferized, bool isSeen, bool isWirelessActivated);
void printElement(u8* pkmn, int game, u16 n, int x, int y);
void printElementBlend(u8* pkmn, int game, u16 n, int x, int y, u32 color);
void printSettings(u8* config_buf, int currentEntry, int configSize, wchar_t* descriptions[]);
void infoViewer(u8* pkmn);
void wcxInfoViewer(u8* buf);
void printfHexEditorInfo(u8* pkmn, int byte);
void printDexViewer(u8* mainbuf, int currentEntry, int page, int seen, int caught);
void printfConfigEditorInfo(int currentEntry);

void printSelector(int x, int y, int width, int height);
void printMenuTop();
void printMenuBottom();

#endif