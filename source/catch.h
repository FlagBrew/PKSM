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

#define DEX 721

void check(int number[]);
void printCursoreC(char cur[]);
void posCursore (char cursore[], int pos[], int temp);
void showC(int number[], int ratio[], int HP_perc, float bonusballvett[], int bonusindex, float status[], int statusindex, int r, int gen, float captureOgenV[], float captureOgenVI[], int captureOindex, int citra);
void catchrate(PrintConsole topScreen, PrintConsole bottomScreen);