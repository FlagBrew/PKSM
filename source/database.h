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

#pragma once

#include <stdlib.h>
#include <3ds.h>

int getN(int i);
void setBoxBin(u8* mainbuf, int game, int NBOXES, int N, char* path[]);
int checkMultipleWC6(u8* mainbuf, int game, int i, int langCont, int nInjected[], int adapt);
void eventDatabase6(u8* mainbuf, int game);
void eventDatabase5(u8* mainbuf, int game);
void eventDatabase4(u8* mainbuf, int game);
void massInjector(u8* mainbuf, int game);