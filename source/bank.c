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

#include <3ds.h>
#include <string.h>
#include "graphic.h"
#include "editor.h"
#include "bank.h"

void bank(u8* mainbuf, int game) {
	FILE *fptr = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	
	if (size % (30 * PKMNLENGTH)) {
		fclose(fptr);
		infoDisp("Bank.bin has a bad size!");
		return;
	}
	u8 *bankbuf = (u8*)malloc(size * sizeof(u8));
	if (bankbuf == NULL) {
		fclose(fptr);
		free(bankbuf);
		return;
	}
	rewind(fptr);
	fread(bankbuf, size, 1, fptr);
	fclose(fptr);
	
	int boxmax = (game < 4) ? 30 : 31;
	
	bool isBufferized = false;
	bool isBank = false;
	int box = 0;
	int currentEntry = 0;
	int coordinate[2] = {0, 0};
	int pastbox = 0;
	bool wasBank = false;
	
	// initializing pkmn buffer
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	memset(pkmn, 0, PKMNLENGTH);
	
	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		
		if (hidKeysDown() & KEY_B) break;
		
		if ((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 44 && touch.py < 74)) {
			int temp = box;
			box = pastbox;
			pastbox = temp;
			
			if (!(isBank)) {
				boxmax = size / (30 * PKMNLENGTH) - 1;
				isBank = true;
			}
			else {
				boxmax = (game < 4) ? 30 : 31;
				isBank = false;
			}
		}
		
		if (hidKeysDown() & KEY_R) {
			if (box < boxmax) box++;
			else if (box == boxmax) box = 0;
		}
		
		if (hidKeysDown() & KEY_L) {
			if (box > 0) box--;
			else if (box == 0) box = boxmax;
		}
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 2 && touch.px < 17 && touch.py > 11 && touch.py < 33) {
				if (box > 0) box--;
				else if (box == 0) box = boxmax;
			}
			
			if (touch.px > 185 && touch.px < 200 && touch.py > 11 && touch.py < 33) {
				if (box < boxmax) box++;
				else if (box == boxmax) box = 0;
			}
			
			if ((touch.px > 214 && touch.px < 320 && touch.py > 76 && touch.py < 106) && !(isBufferized)) {
				if (confirmDisp("Erase the selected box?")) {
					u8 tmp[PKMNLENGTH];
					memset(tmp, 0, PKMNLENGTH);
					for (u32 i = 0; i < 30; i++) {
						if (isBank) memcpy(&bankbuf[box * 30 * PKMNLENGTH + i * PKMNLENGTH], tmp, PKMNLENGTH);
						else setPkmn(mainbuf, box, i, tmp, game);
					}
				}
			}
			
			// shift
			if (touch.px > 214 && touch.px < 320 && touch.py > 106 && touch.py < 136) {
				u8 buffer[PKMNLENGTH];
				u8 temp[PKMNLENGTH];
				memset(buffer, 0, PKMNLENGTH);
				memset(temp, 0, PKMNLENGTH);
				
				if (isBank) {
					for (u32 i = 0; i < 30; i++) {
						getPkmn(mainbuf, pastbox, i, buffer, game); // getpkmn -> buffer
						memcpy(temp, &bankbuf[box * 30 * PKMNLENGTH + i * PKMNLENGTH], PKMNLENGTH); // memcpy bank -> temp
						setPkmn(mainbuf, pastbox, i, temp, game); // setpkmn -> temp
						memcpy(&bankbuf[box * 30 * PKMNLENGTH + i * PKMNLENGTH], buffer, PKMNLENGTH); // memcpy bank -> buffer
					}
				} else {
					for (u32 i = 0; i < 30; i++) {
						memcpy(buffer, &bankbuf[pastbox * 30 * PKMNLENGTH + i * PKMNLENGTH], PKMNLENGTH);
						getPkmn(mainbuf, box, i, temp, game); 
						memcpy(&bankbuf[pastbox * 30 * PKMNLENGTH + i * PKMNLENGTH], temp, PKMNLENGTH);
						setPkmn(mainbuf, box, i, buffer, game);
					}
				}
			}
			
			if (touch.px > 288 && touch.px < 310 && touch.py > 217 && touch.py < 235) break;
		}
		
		if (hidKeysDown() & KEY_DRIGHT) {
			if (currentEntry < 29) currentEntry++;
			else if (currentEntry == 29) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 29;
		}
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry >= 6) currentEntry -= 6;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry <= 23) currentEntry += 6;
		}
		
		if ((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 12 && touch.py < 42)) {
			if (isBufferized) {
				u8 tmp[PKMNLENGTH];
				
				if (wasBank == isBank && coordinate[0] == box && coordinate[1] == currentEntry)
					memset(pkmn, 0, PKMNLENGTH);
				else if (isBank) {
					memcpy(tmp, &bankbuf[box * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
					memcpy(&bankbuf[box * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], pkmn, PKMNLENGTH);
					if (wasBank) memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], tmp, PKMNLENGTH);
					else setPkmn(mainbuf, coordinate[0], coordinate[1], tmp, game);
					memset(pkmn, 0, PKMNLENGTH);
				}
				else {
					getPkmn(mainbuf, box, currentEntry, tmp, game);
					setPkmn(mainbuf, box, currentEntry, pkmn, game);
					if (wasBank) memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], tmp, PKMNLENGTH);
					else setPkmn(mainbuf, coordinate[0], coordinate[1], tmp, game);
					memset(pkmn, 0, PKMNLENGTH);
				}
				isBufferized = false;
			}
			else {
				int k = 0;

				coordinate[0] = box;
				coordinate[1] = currentEntry;
				wasBank = isBank;
				if (isBank) memcpy(pkmn, &bankbuf[box * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
				else getPkmn(mainbuf, box, currentEntry, pkmn, game);
							
				u8 tmp[PKMNLENGTH];
				memset(tmp, 0, PKMNLENGTH);
				for (int i = 0; i < PKMNLENGTH; i++)
					if (pkmn[i] == tmp[i])
						k++;
					
				if (k != PKMNLENGTH)
					isBufferized = true;
			}
		}

		if (hidKeysHeld() & KEY_TOUCH) {
			if (touch.px > 4 && touch.px < 38 && touch.py > 45 && touch.py < 75) currentEntry = 0;
			if (touch.px > 38 && touch.px < 72 && touch.py > 45 && touch.py < 75) currentEntry = 1;
			if (touch.px > 72 && touch.px < 106 && touch.py > 45 && touch.py < 75) currentEntry = 2;
			if (touch.px > 106 && touch.px < 140 && touch.py > 45 && touch.py < 75) currentEntry = 3;
			if (touch.px > 140 && touch.px < 174 && touch.py > 45 && touch.py < 75) currentEntry = 4;
			if (touch.px > 174 && touch.px < 208 && touch.py > 45 && touch.py < 75) currentEntry = 5;
			if (touch.px > 4 && touch.px < 38 && touch.py > 75 && touch.py < 105) currentEntry = 6;
			if (touch.px > 38 && touch.px < 72 && touch.py > 75 && touch.py < 105) currentEntry = 7;
			if (touch.px > 72 && touch.px < 106 && touch.py > 75 && touch.py < 105) currentEntry = 8;
			if (touch.px > 106 && touch.px < 140 && touch.py > 75 && touch.py < 105) currentEntry = 9;
			if (touch.px > 140 && touch.px < 174 && touch.py > 75 && touch.py < 105) currentEntry = 10;
			if (touch.px > 174 && touch.px < 208 && touch.py > 75 && touch.py < 105) currentEntry = 11;
			if (touch.px > 4 && touch.px < 38 && touch.py > 105 && touch.py < 135) currentEntry = 12;
			if (touch.px > 38 && touch.px < 72 && touch.py > 105 && touch.py < 135) currentEntry = 13;
			if (touch.px > 72 && touch.px < 106 && touch.py > 105 && touch.py < 135) currentEntry = 14;
			if (touch.px > 106 && touch.px < 140 && touch.py > 105 && touch.py < 135) currentEntry = 15;
			if (touch.px > 140 && touch.px < 174 && touch.py > 105 && touch.py < 135) currentEntry = 16;
			if (touch.px > 174 && touch.px < 208 && touch.py > 105 && touch.py < 135) currentEntry = 17;
			if (touch.px > 4 && touch.px < 38 && touch.py > 135 && touch.py < 165) currentEntry = 18;
			if (touch.px > 38 && touch.px < 72 && touch.py > 135 && touch.py < 165) currentEntry = 19;
			if (touch.px > 72 && touch.px < 106 && touch.py > 135 && touch.py < 165) currentEntry = 20;
			if (touch.px > 106 && touch.px < 140 && touch.py > 135 && touch.py < 165) currentEntry = 21;
			if (touch.px > 140 && touch.px < 174 && touch.py > 135 && touch.py < 165) currentEntry = 22;
			if (touch.px > 174 && touch.px < 208 && touch.py > 135 && touch.py < 165) currentEntry = 23;
			if (touch.px > 4 && touch.px < 38 && touch.py > 165 && touch.py < 195) currentEntry = 24;
			if (touch.px > 38 && touch.px < 72 && touch.py > 165 && touch.py < 195) currentEntry = 25;
			if (touch.px > 72 && touch.px < 106 && touch.py > 165 && touch.py < 195) currentEntry = 26;
			if (touch.px > 106 && touch.px < 140 && touch.py > 165 && touch.py < 195) currentEntry = 27;
			if (touch.px > 140 && touch.px < 174 && touch.py > 165 && touch.py < 195) currentEntry = 28;
			if (touch.px > 174 && touch.px < 208 && touch.py > 165 && touch.py < 195) currentEntry = 29;			
		}

		printPKBank(bankbuf, mainbuf, pkmn, game, currentEntry, box, isBank);
	}
	
	if (confirmDisp("Save bank.bin changes?")) {
		FILE *new = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
		fwrite(bankbuf, 1, size, new);
		fclose(new);
	}

	free(bankbuf);
	free(pkmn);
}