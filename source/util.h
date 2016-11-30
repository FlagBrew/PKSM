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

#include <stdlib.h>
#include <3ds.h>

#define POKEBANK 0x00040000000C9B00

#define V1 3
#define V2 1
#define V3 2

#define DAY 12
#define MONTH 9
#define YEAR 16

#define GAME_X 0
#define GAME_Y 1
#define GAME_OR 2
#define GAME_AS 3
#define GAME_SUN 4
#define GAME_MOON 5
#define GAME_DIAMOND 6
#define GAME_PEARL 7
#define GAME_PLATINUM 8
#define GAME_HG 9
#define GAME_SS 10
#define GAME_B1 11
#define GAME_W1 12
#define GAME_B2 13
#define GAME_W2 14

void loadPersonal();
void loadFile(u8* buf, char* path);
void injectFromFile(u8* mainbuf, char* path, u32 offset);
int autoupdater();
void update();
void printfile(char* path);
bool isHBL();
void fsStart();
void fsEnd();
bool openSaveArch(FS_Archive *out, u64 id);
void settingsMenu(u8* mainbuf, int game);