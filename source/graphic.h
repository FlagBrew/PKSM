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

#define BUTTONGREY RGBA8( 99, 110, 133, 255)
#define BUTTONBORD RGBA8(160, 164, 173, 255)

#define BARTEXT   RGBA8(114, 194, 255, 255)

#define BLUE      RGBA8(104, 137, 205, 255)
#define YELLOW    RGBA8(237, 247, 157, 255)
#define LIGHTBLUE RGBA8(187, 208, 254, 255)
#define DARKBLUE  RGBA8( 35,  69, 167, 255)
#define HIGHBLUE  RGBA8( 28,  45,  86, 255)
#define MASKBLACK RGBA8(  0,   0,   0, 180)
#define PALEBLUE  RGBA8( 70,  95, 144, 255)
#define MENUBLUE  RGBA8( 35,  69, 137, 255)

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
void GUIElementsSpecify(int game);
int getGUIElementsI18nSpecifyTotalElements(int game);
void GUIElementsI18nSpecify(int game); // Needed for util.c when changing language
void GUITextsInit(); // Needed for util.c when changing language
void GUIGameElementsInit();
void initProgressLoadPNGInRAM(int total);
void GUIGameElementsExit();
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
void menu(wchar_t* menu[]);
void printDatabase6(char *database[], int currentEntry, int page, int spriteArray[]);
void printDatabase5(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]);
void printDatabase4(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]);
void printDB7(int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected);
void printDB6(int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected);
void printEditor(u8* mainbuf, int game, u64 size, int currentEntry, int page);
void printPKEditor(u8* pkmn, int game, int additional1, int additional2, int additional3, int mode, wchar_t* descriptions[]);
void printPKViewer(u8* mainbuf, u8* pkmn, bool isTeam, int game, int currentEntry, int menuEntry, int box, int mode, int additional1, int additional2);
void printPKBank(u8* bankbuf, u8* mainbuf, u8* pkmnbuf, int game, int currentEntry, int saveBox, int bankBox, bool isBufferized, bool isSeen);
void printElement(u8* pkmn, int game, u16 n, int x, int y);
void printElementBlend(u8* pkmn, int game, u16 n, int x, int y, u32 color);
void printSettings(int box, int language);
void infoViewer(u8* pkmn, int game);
void printfHexEditorInfo(u8* pkmn, int byte);
void printDexViewer(u8* mainbuf, int game, int currentEntry, int page);

void printSelector(int x, int y, int width, int height);
void printMenuTop();
void printMenuBottom();

