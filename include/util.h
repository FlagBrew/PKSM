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

void waitKey(u32 key);
void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N);
void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page);
void update(PrintConsole topScreen, PrintConsole bottomScreen);
u32 CHKOffset(u32 i, int game);
u32 CHKLength(u32 i, int game);
u16 ccitt16(u8* data, u32 len);
void rewriteCHK(u8 *mainbuf, int game);
void credits(PrintConsole topScreen, PrintConsole bottomScreen);
bool isHBL();
void fsStart();
void fsEnd();
bool openSaveArch(FS_Archive *out, u64 id);
void errDisp(PrintConsole topScreen, int i);
void infoDisp(PrintConsole screen, int i);
