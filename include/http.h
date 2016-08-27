/*
* This file is part of EventAssistant
* Copyright (C) 2016 Bernardo Giordano
*
* This software is provided 'as-is', 
* without any express or implied warranty. 
* In no event will the authors be held liable for any damages 
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* and to alter it and redistribute it freely, 
* subject to the following restrictions:
*
* 1) The origin of this software must not be misrepresented; 
* 2) You must not claim that you wrote the original software. 
*
* Altered source versions must be plainly marked as such, 
* and must not be misrepresented as being the original software.
*/

#include <stdlib.h>
#include <3ds.h>

Result http_download(PrintConsole topScreen, PrintConsole bottomScreen, httpcContext *context);
void getText(PrintConsole topScreen, PrintConsole bottomScreen, char *url);
void printDistro(PrintConsole topScreen, PrintConsole bottomScreen, char *url);
void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int n);
Result downloadFile(PrintConsole topScreen, PrintConsole bottomScreen, char* url, char* path);
int injectBoxBin(PrintConsole topScreen, u8* mainbuf, int game, int NBOXES, char* urls[]);
Result printDB(PrintConsole topScreen, PrintConsole bottomScreen, u8* mainbuf, char *url, int i, int nInjected[], int game, int overwrite[]);
int massInjecter(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game);