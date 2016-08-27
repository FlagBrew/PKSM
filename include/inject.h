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

#define WC6LENGTH 264

void injectWC6(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void injectLanguage(u8* mainbuf, int i);
void injectMoney(u8* mainbuf, u64 i);
void injectItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[], int game);
void injectBP(u8* mainbuf, int i, int game);
void injectBadges(u8* mainbuf, int i);
void injectTM(u8* mainbuf, int game);
void refreshValues(PrintConsole topScreen, int injectCont[], int nInjected[]);
int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8* mainbuf, int game, int nInjected[], int injectCont[]);