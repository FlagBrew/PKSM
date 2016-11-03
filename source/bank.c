#include <3ds.h>
#include <string.h>
#include "graphic.h"
#include "editor.h"
#include "bank.h"

void bank(u8* mainbuf, int game) {
	// loading bank data
	FILE *fptr = fopen("/3ds/data/EventAssistant/bank/bank.bin", "rt");
	fseek(fptr, 0, SEEK_END);
	u8 *bankbuf = (u8*)malloc(30 * BANKBOXMAX * PKMNLENGTH * sizeof(u8));
	if (bankbuf == NULL) {
		fclose(fptr);
		free(bankbuf);
		return;
	}
	rewind(fptr);
	fread(bankbuf, 30 * BANKBOXMAX * PKMNLENGTH, 1, fptr);
	fclose(fptr);
	
	int boxmax = 30;
	
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
		
		if ((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 54 && touch.py < 84)) {
			int temp = box;
			box = pastbox;
			pastbox = temp;
			
			if (!(isBank)) {
				boxmax = BANKBOXMAX - 1;
				isBank = true;
			}
			else {
				boxmax = 30;
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
			
			if (touch.px > 214 && touch.px < 320 && touch.py > 86 && touch.py < 116) {
				u8* tmp = (u8*)malloc(PKMNLENGTH * sizeof(u8));
				memset(tmp, 0, PKMNLENGTH);
				for (u32 i = 0; i < 30; i++) {
					if (isBank) memcpy(&bankbuf[box * 30 * PKMNLENGTH + i * PKMNLENGTH], pkmn, PKMNLENGTH);
					else setPkmn(mainbuf, box, i, pkmn, game);
				}
				free(tmp);
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
		
		if ((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 22 && touch.py < 52)) {
			if (isBufferized) {
				int k = 0;
				u8* tmp = (u8*)malloc(PKMNLENGTH * sizeof(u8));
				
				if (wasBank == isBank && coordinate[0] == box && coordinate[1] == currentEntry) {
					memset(pkmn, 0, PKMNLENGTH);
					isBufferized = false;					
				}

				else if (isBank) {
					memcpy(tmp, &bankbuf[box * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
					for (int i = 0; i < PKMNLENGTH; i++)
						if (tmp[i] == 0)
							k++;
						
					if (k == PKMNLENGTH) {				
						memcpy(&bankbuf[box * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], pkmn, PKMNLENGTH);
						memset(pkmn, 0, PKMNLENGTH);
						if (wasBank) {
							memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], pkmn, PKMNLENGTH);
						}
						else {
							setPkmn(mainbuf, coordinate[0], coordinate[1], pkmn, game);
						}
						memset(pkmn, 0, PKMNLENGTH);
						isBufferized = false;
					}
				}
				else {
					getPkmn(mainbuf, box, currentEntry, tmp, game);
					for (int i = 0; i < PKMNLENGTH; i++)
						if (tmp[i] == 0)
							k++;

					if (k == PKMNLENGTH) {
						setPkmn(mainbuf, box, currentEntry, pkmn, game);
						memset(pkmn, 0, PKMNLENGTH);
						if (wasBank) {
							memcpy(&bankbuf[coordinate[0] * 30 * PKMNLENGTH + coordinate[1] * PKMNLENGTH], pkmn, PKMNLENGTH);
						}
						else {
							setPkmn(mainbuf, coordinate[0], coordinate[1], pkmn, game);
						}
						memset(pkmn, 0, PKMNLENGTH);
						isBufferized = false;
					}
				}
				free(tmp);
			}
			else {
				int k = 0;

				coordinate[0] = box;
				coordinate[1] = currentEntry;
				wasBank = isBank;
				if (isBank) {
					memcpy(pkmn, &bankbuf[box * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
				}
				else {
					getPkmn(mainbuf, box, currentEntry, pkmn, game);
				}
							
				u8* tmp = (u8*)malloc(PKMNLENGTH * sizeof(u8));
				memset(tmp, 0, PKMNLENGTH);
				for (int i = 0; i < PKMNLENGTH; i++)
					if (pkmn[i] == tmp[i])
						k++;
					
				if (k != PKMNLENGTH)
					isBufferized = true;
				
				free(tmp);
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
	
	FILE *new = fopen("/3ds/data/EventAssistant/bank/bank.bin", "wb");
	fwrite(bankbuf, 1, 30 * BANKBOXMAX * PKMNLENGTH, new);
	fclose(new);
	
	free(bankbuf);
	free(pkmn);
}