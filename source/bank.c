/* This file is part of PKSM

Copyright (C) 2016 Bernardo Giordano

>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#include <3ds.h>
#include <string.h>
#include "graphic.h"
#include "editor.h"
#include "bank.h"
#include "dex.h"

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

	bool isSeen = false;
	bool bufferizedfrombank = false;
	bool isBufferized = false;
	int saveBox = 0, bankBox = 0;
	int currentEntry = 30;
	int coordinate[2] = {0, 0};
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	memset(pkmn, 0, PKMNLENGTH);
	
	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		
		if (hidKeysDown() & KEY_B) {
			if (isBufferized) {
				if (bufferizedfrombank)
					memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], pkmn, PKMNLENGTH);
				else 
					setPkmn(mainbuf, coordinate[0], coordinate[1], pkmn, game);
				
				memset(pkmn, 0, PKMNLENGTH);
				isBufferized = false;
			} else
				break;
		}
		
		if (hidKeysDown() & KEY_DRIGHT)
			if (currentEntry < 59) 
				currentEntry++;
		
		if (hidKeysDown() & KEY_DLEFT)
			if (currentEntry > 0) 
				currentEntry--;
		
		if (hidKeysDown() & KEY_DUP)
			if (currentEntry >= 6) 
				currentEntry -= 6;
		
		if (hidKeysDown() & KEY_DDOWN)
			if (currentEntry <= 53) 
				currentEntry += 6;
		
		if (hidKeysDown() & KEY_R) {
			if (currentEntry < 30) {
				if (bankBox < size / (30 * PKMNLENGTH) - 1) 
					bankBox++;
				else if (bankBox == size / (30 * PKMNLENGTH) - 1) 
					bankBox = 0;
			} else {
				if (saveBox < ((game < 4) ? 30 : 31)) 
					saveBox++;
				else if (saveBox == ((game < 4) ? 30 : 31)) 
					saveBox = 0;
			}
		}
		
		if (hidKeysDown() & KEY_L) {
			if (currentEntry < 30) {
				if (bankBox > 0) 
					bankBox--;
				else if (bankBox == 0) 
					bankBox = size / (30 * PKMNLENGTH) - 1;
			} else {
				if (saveBox > 0) 
					saveBox--;
				else if (saveBox == 0) 
					saveBox = (game < 4) ? 30 : 31;	
			}
		}
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 7 && touch.px < 23 && touch.py > 17 && touch.py < 37) {
				if (saveBox > 0) 
					saveBox--;
				else if (saveBox == 0) 
					saveBox = (game < 4) ? 30 : 31;
			}
			
			if (touch.px > 185 && touch.px < 201 && touch.py > 17 && touch.py < 37) {
				if (saveBox < ((game < 4) ? 30 : 31)) 
					saveBox++;
				else if (saveBox == ((game < 4) ? 30 : 31)) 
					saveBox = 0;
			}
			
			if ((touch.px > 208 && touch.px < 317 && touch.py > 70 && touch.py < 97) && !(isBufferized)) {
				if (confirmDisp("Erase the selected box?")) {
					u8 tmp[PKMNLENGTH];
					memset(tmp, 0, PKMNLENGTH);
					for (u32 i = 0; i < 30; i++) {
						if (currentEntry < 30) 
							memcpy(&bankbuf[bankBox * 30 * PKMNLENGTH + i * PKMNLENGTH], tmp, PKMNLENGTH);
						else 
							setPkmn(mainbuf, saveBox, i, tmp, game);
					}
				}
			}
			
			if (touch.px > 242 && touch.px < 285 && touch.py > 5 && touch.py < 25) {
				u8 buffer[PKMNLENGTH];
				u8 temp[PKMNLENGTH];
				memset(buffer, 0, PKMNLENGTH);
				memset(temp, 0, PKMNLENGTH);
				
				for (u32 i = 0; i < 30; i++) {
					getPkmn(mainbuf, saveBox, i, buffer, game); // getpkmn -> buffer
					memcpy(temp, &bankbuf[bankBox * 30 * PKMNLENGTH + i * PKMNLENGTH], PKMNLENGTH); // memcpy bank -> temp
					setPkmn(mainbuf, saveBox, i, temp, game); // setpkmn -> temp
					memcpy(&bankbuf[bankBox * 30 * PKMNLENGTH + i * PKMNLENGTH], buffer, PKMNLENGTH); // memcpy bank -> buffer
				}
			}
			
			if ((touch.px > 208 && touch.px < 317 && touch.py > 97 && touch.py < 124) && !(isBufferized)) {
				u8 tmp[PKMNLENGTH];
				memset(tmp, 0, PKMNLENGTH);
				if (currentEntry < 30) 
					memcpy(&bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], tmp, PKMNLENGTH);
				else 
					setPkmn(mainbuf, saveBox, currentEntry - 30, tmp, game);
			}
				
			if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) break;
		}
		
		if (hidKeysHeld() & KEY_TOUCH) {
			int x_start, y_start = 45;
			for (int i = 0; i < 5; i++) {
				x_start = 4;
				for (int j = 0; j < 6; j++) {
					if ((touch.px > x_start) && (touch.px < (x_start + 34)) && (touch.py > y_start) && (touch.py < (y_start + 30)))
						currentEntry = 30 + i * 6 + j;
					x_start += 34;
				}
				y_start += 30;
			}
		}
		
		if (((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touch.px > 208 && touch.px < 317 && touch.py > 43 && touch.py < 74)) && !isBufferized) {
			isSeen = true;
			u8 tmp[PKMNLENGTH];
			memset(tmp, 0, PKMNLENGTH);
			
			if (currentEntry < 30)
				memcpy(tmp, &bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
			else 
				getPkmn(mainbuf, saveBox, currentEntry - 30, tmp, game);
			
			while (aptMainLoop() && getPokedexNumber(tmp)) {
				hidScanInput();
				touchPosition touch;
				hidTouchRead(&touch);
				
				if ((hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_TOUCH && touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240)) 
					break;
				
				printPKBank(bankbuf, mainbuf, tmp, game, currentEntry, saveBox, bankBox, isBufferized, isSeen);
			}
			
			isSeen = false;
		}
		
 		if (hidKeysDown() & KEY_A) {
			if (isBufferized) {
				if (!(game < 4 && getPokedexNumber(pkmn) > 721 && currentEntry > 29)) { // prevent that gen7 stuff goes into gen6 save
					u8 tmp[PKMNLENGTH];
					
					if ((bufferizedfrombank == (currentEntry < 30)) && (coordinate[0] == ((currentEntry < 30) ? bankBox : saveBox)) && (coordinate[1] == currentEntry))
						memcpy(&bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], pkmn, PKMNLENGTH);
					else if ((!bufferizedfrombank == (currentEntry > 29)) && (coordinate[0] == ((currentEntry < 30) ? bankBox : saveBox)) && (coordinate[1] == currentEntry - 30))
						setPkmn(mainbuf, saveBox, currentEntry - 30, pkmn, game);
					else if (currentEntry < 30) {
						memcpy(tmp, &bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
						memcpy(&bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], pkmn, PKMNLENGTH);
						if (bufferizedfrombank) 
							memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], tmp, PKMNLENGTH);
						else 
							setPkmn(mainbuf, coordinate[0], coordinate[1], tmp, game);
					}
					else {
						getPkmn(mainbuf, saveBox, currentEntry - 30, tmp, game);
						setPkmn(mainbuf, saveBox, currentEntry - 30, pkmn, game);
						if (bufferizedfrombank) 
							memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], tmp, PKMNLENGTH);
						else 
							setPkmn(mainbuf, coordinate[0], coordinate[1], tmp, game);
					}
					memset(pkmn, 0, PKMNLENGTH);
					isBufferized = false;
				}
			}
			else {
				u8 tmp[PKMNLENGTH];
				memset(tmp, 0, PKMNLENGTH);
			
				coordinate[0] = (currentEntry < 30) ? bankBox : saveBox;
				coordinate[1] = (currentEntry > 29) ? (currentEntry - 30) : currentEntry;
				
				if (currentEntry < 30) {
					memcpy(pkmn, &bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
					memcpy(&bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], tmp, PKMNLENGTH);
				} else {
					getPkmn(mainbuf, saveBox, currentEntry - 30, pkmn, game);
					setPkmn(mainbuf, saveBox, currentEntry - 30, tmp, game);
				}
							
				isBufferized = true;
				bufferizedfrombank = (currentEntry < 30) ? true : false;
			}
		}
		printPKBank(bankbuf, mainbuf, pkmn, game, currentEntry, saveBox, bankBox, isBufferized, isSeen);
	}
	
	if (confirmDisp("Save bank.bin changes?")) {
		FILE *new = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
		fwrite(bankbuf, 1, size, new);
		fclose(new);
	}
	free(bankbuf);

	if (game == GAME_SUN || game == GAME_MOON) {
		if (confirmDisp("Save PokeDex flags?")) {
			int end = (game < 4) ? 31 : 32;
			char* step = (char*)malloc(20);
			
			for (u32 i = 0; i < end; i++) {
				for (u32 j = 0; j < 30; j++) {
					snprintf(step, 20, "Box %lu / Slot %lu", i + 1, j + 1);
					freezeMsg(step);
					getPkmn(mainbuf, i, j, pkmn, game);
					setDex(mainbuf, pkmn, game);
				}
			}
			free(step);
		}
	}
	free(pkmn);
}