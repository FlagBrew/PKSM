/*
* This file is part of PKSM
* Copyright (C) 2016 Bernardo Giordano
*
* This software is provided 'as-is', 
* without any express or implied warranty. 
* In no event will the authors be held liable for any damages 
* arising from the use of this software.
*
* This code is subject to the following restrictions:
*
* 1) The origin of this software must not be misrepresented; 
* 2) You must not claim that you wrote the original software. 
*
*/

#pragma once

#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include "util.h"

#define BLACK RGBA8(0, 0, 0, 255)
#define RED RGBA8(255, 0, 0, 255)
#define GREY RGBA8(0xF0, 0xF0, 0xF0, 0xFF)
#define DARKGREY RGBA8(0x0, 0x0, 0x0, 0xFF - 40)
#define WHITE RGBA8(255, 255, 255, 255)
#define AZZURRO RGBA8(0x33, 0x99, 0xFF, 0xFF)
#define TRASPWHITE RGBA8(255,255,255,170)
#define DARKBLUE RGBA8(0, 0, 102, 0xFF)
#define DS RGBA8(48, 48, 48, 255)

#define FILL 5
#define SIZE 12

void GUIElementsInit();
void GUIElementsSpecify(int game);
void GUIGameElementsInit();
void GUIGameElementsExit();
void GUIElementsExit();
void init_font_cache();
void infoDisp(char* message);
int confirmDisp(char* message);
void freezeMsg(char* message);
void progressBar(char* message, u32 pagesize, u32 sz);
void autoupdaterMenu();
int giveTransparence();
void printPSDates();
void printCredits();
void printTitle(const char* title);
void gameSelectorMenu(int n);
void mainMenu(int currentEntry);
void mainMenuDS(int currentEntry);
void menu4(int currentEntry, char* menu[], int n);
void menu3(int currentEntry, char* menu[], int n);
void printDistribution(char *url);
void printDatabase6(char *database[], int currentEntry, int page, int spriteArray[]);
void printDatabase5(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]);
void printDatabase4(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]);
void printDB7(int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected);
void printDB6(int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected);
void printEditor(int currentEntry, int langCont, int badgeCont);
void printPKViewer(u8* mainbuf, int game, int currentEntry, int page, bool cloning);
void printPKEditor(u8* pkmn, int game, bool speedy);
void printPKBank(u8* bankbuf, u8* mainbuf, u8* pkmnbuf, int game, int currentEntry, int box, bool isBank);
void printMassInjector(int currentEntry);
void printMoves(u8* pkmn, int currentEntry, int entryBottom, int page);
void printItems(u8* pkmn, int currentEntry, int page);