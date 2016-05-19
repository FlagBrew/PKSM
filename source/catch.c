#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <math.h>
#include "catch.h"
#include "pid.h"

// next is poliwhirl
#define DEX 60 

#define DELAY 20

void check(int number[]) {
	if ((number[0] * 100 + number[1] * 10 + number[2]) > DEX) {
		number[0] = 0;
		number[1] = 6;
		number[2] = 0;
	}
	if ((number[0] * 100 + number[1] * 10 + number[2]) == 0) {
		number[0] = 0;
		number[1] = 0;
		number[2] = 1;
	}
}

void showC(int number[], int ratio[], char cur[]) {
	printf("\x1b[2;0HNumber in National Dex: %d%d%d\nHas a catch rate of %d\x1b[3;24H%c%c%c", number[0], number[1], number[2], ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1], cur[0], cur[1], cur[2]);
}

void catchrate() {
	int ratio[DEX] = {45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 120, 45, 255, 120, 45, 255, 120, 45, 255, 127, 255, 90, 255, 90, 190, 75, 255, 90, 235, 120, 45, 235, 120, 45, 150, 25, 190, 75, 170, 50, 255, 90, 255, 120, 45, 190, 75, 190, 75, 255, 50, 255, 90, 190, 75, 190, 75, 190, 75, 255};
	
	int posizione[1] = {0};
	char cursore[3] = {'^', ' ', ' '};
	
	int number[3] = {0, 0, 1};
	int HP_perc = 100;
	int bonusball = 1;
	int bonusstatus = 1;
	
	printf("\x1b[2J");
	printf("\x1b[47;30m              Catch Rate  Calculator              \x1b[0m");
	printf("--------------------------------------------------");
	showC(number, ratio, cursore);
	
	int t_frame = 1;
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_START)
			break;
		
		if (((kDown & KEY_DRIGHT) ^ (hidKeysHeld() & KEY_DRIGHT && t_frame % DELAY == 1)) && (posizione[0] < 2)) {
			posCursore(cursore, posizione, 1);
			showC(number, ratio, cursore);
		}
		
		if (((kDown & KEY_DLEFT) ^ (hidKeysHeld() & KEY_DLEFT && t_frame % DELAY == 1)) && (posizione[0] > 0)) {
			posCursore(cursore, posizione, -1);
			showC(number, ratio, cursore);
		}
		
		if ((kDown & KEY_DUP) ^ (hidKeysHeld() & KEY_DUP && t_frame % DELAY == 1)) {
			if (posizione[0] == 0) {
				if (number[posizione[0]] < 7) {
					number[posizione[0]]++;
					showC(number, ratio, cursore);
				}
			}
			if (posizione[0] == 1 || posizione[0] == 2) {
				if (number[posizione[0]] < 9) {
					number[posizione[0]]++;
					check(number);
					showC(number, ratio, cursore);
				}
			}

		}
		
		if ((kDown & KEY_DDOWN) ^ (hidKeysHeld() & KEY_DDOWN && t_frame % DELAY == 1)) {
			if (posizione[0] == 0 || posizione[0] == 1 || posizione[0] == 2) {
				if (number[posizione[0]] > 0) {
					number[posizione[0]]--;
					check(number);
					showC(number, ratio, cursore);
				}
			}
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		t_frame++;
		if (t_frame > 5000) t_frame = 1;
	}
}

// a = rate*bonusball*bonusstatus-(2/3)*rate*bonusball*nonustats*(HP_perc);