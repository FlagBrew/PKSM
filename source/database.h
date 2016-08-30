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

int printDB(PrintConsole topScreen, PrintConsole bottomScreen, u8* mainbuf, int i, int nInjected[], int game, int overwrite[]);
void eventDatabase(PrintConsole topScreen, PrintConsole bottomScreen, u8* mainbuf, int game);
void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *path, int n);
void psDates(PrintConsole topScreen, PrintConsole bottomScreen);
int massInjecter(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game);