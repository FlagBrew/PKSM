/*
* This file is part of EventAssistant
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

#include <stdlib.h>
#include <3ds.h>

#define TOP 50
#define BOTTOM 40

void printfile(char* path);
void waitKey(u32 key);
void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N);
void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page);
void update(PrintConsole topScreen, PrintConsole bottomScreen);
void credits(PrintConsole topScreen, PrintConsole bottomScreen);
bool isHBL();
void fsStart();
void fsEnd();
bool openSaveArch(FS_Archive *out, u64 id);
void errDisp(PrintConsole screen, int i, u16 columns);
void infoDisp(PrintConsole screen, int i, u16 columns);