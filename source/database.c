/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#include "database.h"

char *tags[] = {"jpn", "eng", "fre", "ita", "ger", "spa", "kor", "chs", "cht"};

void getSinglePathPGF(char* path, int lang, int i) {
	sprintf(path, "romfs:/pgf/%s/%d.pgf", tags[lang], i);
}

void getSinglePathPGT(char* path, int lang, int i) {
	sprintf(path, "romfs:/pgt/%s/%d.pgt", tags[lang], i);
}

void getSinglePathWCX(char* path, int lang, int i) {
	sprintf(path, "romfs:/wcx/%s/%d.wcx", tags[lang], i);
}

void getMultiplePathWCX(char* path, int lang, int i, int j) {
	sprintf(path, "romfs:/wcx/%s/%d-%d.wcx", tags[lang], i, j);
}

void reloadPreviewBuf(u8* previewBuf, int i, int n) {
	char *testpath = (char*)malloc(40 * sizeof(char));
	getSinglePathWCX(testpath, n, i);
	
	FILE* f = fopen(testpath, "r");
	if (f) { 
		fseek(f, 0, SEEK_END);
		u32 sz = ftell(f);
		memset(previewBuf, 0, sz);
		rewind(f);
		fread(previewBuf, sz, 1, f);
		fclose(f); 
	} else { 
		fclose(f); 
	}

	free(testpath);	
}

void reloadMultiplePreviewBuf(u8* previewBuf, int i, int n, int j) {
	char *testpath = (char*)malloc(40 * sizeof(char));
	getMultiplePathWCX(testpath, n, i, j);
	
	FILE* f = fopen(testpath, "r");
	if (f) { 
		fseek(f, 0, SEEK_END);
		u32 sz = ftell(f);
		memset(previewBuf, 0, sz);
		rewind(f);
		fread(previewBuf, sz, 1, f);
		fclose(f); 
	} else { 
		fclose(f); 
	}

	free(testpath);	
}

void findFreeLocationWC(u8 *mainbuf, int game, int nInjected[]) {
	nInjected[0] = 0;
	int len = (game < 4) ? 24 : 48;
	int temp;
	int offset = 0;
	if (game == GAME_X || game == GAME_Y)
		offset = 0x1BD00;
	else if (game == GAME_OR || game == GAME_AS)
		offset = 0x1CD00;
	else if (game == GAME_SUN || game == GAME_MOON)
		offset = 0x65C00 + 0x100;
	
	for (int t = 0; t < len; t++) {
		temp = 0;
		for (int j = 0; j < WC6LENGTH; j++)
			if (*(mainbuf + offset + t * WC6LENGTH + j) == 0x00)
				temp++;
			
		if (temp == WC6LENGTH) {
			nInjected[0] = t;
			break;
		}
	}
}

int getN(int i) {
	switch (i) {
		case 1 : return 10;
		case 48 : return 4;
		case 71 : return 5;
		case 81 : return 11;
		case 82 : return 6;
		case 108 : return 2;
		case 132 : return 5;
		case 133 : return 5;
		case 134 : return 5;
		case 135 : return 5;
		case 136 : return 5;
		case 142 : return 5;
		case 184 : return 11;
		case 201 : return 3;
		case 204 : return 6; 		
		case 205 : return 2;
		case 211 : return 7;
		case 221 : return 6;
		case 223 : return 6;
		case 230 : return 8;
		case 504 : return 2;
		case 515 : return 2;
		case 551 : return 2;
		case 552 : return 2;
		case 1111 : return 6;
		case 2016 : return 3;		
	}	
	return 0;
}

void eventDatabase7(u8* mainbuf, int game) {
	char *database[SMCOUNT];
	int *spriteArray = (int*)malloc(SMCOUNT * sizeof(int));
	u8 *previewbuf = (u8*)malloc(WCX_SIZE);
	memset(previewbuf, 0, WCX_SIZE);
	
	if (game == GAME_SUN || game == GAME_MOON)
		filldatabase7(database, spriteArray);
	else
		filldatabase6(database, spriteArray);
	
	int currentEntry = 0;
	int page = 0;
	
	bool adapt = false;
	bool langVett[9];
	bool overwrite = false;
	int nInjected[1] = {0};
	int langSelected = -1;
	
	int currentMultipleWCSelected = 0;
	
	findFreeLocationWC(mainbuf, game, nInjected);
	
	while(aptMainLoop()) {
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)
			break;
		
		if (hidKeysDown() & KEY_L) {
			if (page > 0) {
				int temp;
				do {
					page--;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page--;
				} while (temp == 10);
			}
			else if (page == 0) page = SMCOUNT / 10 - 1;
		}
		
		if (hidKeysDown() & KEY_R) {
			if (page < SMCOUNT / 10 - 1) {
				int temp;
				do {
					page++;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page++;
				} while (temp == 10);
			}
			else if (page == SMCOUNT / 10 - 1) page = 0;
		}
		
		if (hidKeysDown() & KEY_UP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_LEFT) {
			if (currentEntry <= 4) {
				int temp;
				do {
					page--;
					if (page < 0) page = SMCOUNT / 10 - 1;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page--;
				} while (temp == 10);
			}
			else if (currentEntry >= 5) currentEntry -= 5;
		}
		
		if (hidKeysDown() & KEY_RIGHT) {
			if (currentEntry <= 4) currentEntry += 5;
			else if (currentEntry >= 5) {
				int temp;
				do {
					page++;
					if (page > SMCOUNT / 10 - 1) page = 0;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page++;
				} while (temp == 10);
			}
		}
		
		if (hidKeysDown() & KEY_A && spriteArray[page*10+currentEntry] != -1) {
			int total = (game == GAME_SUN || game == GAME_MOON) ? 9 : 7;
			int i = page * 10 + currentEntry;
			// check for single wcx events
			char *testpath = (char*)malloc(40 * sizeof(char));
			
			for (int j = 0; j < total; j++) {
				getSinglePathWCX(testpath, j, i);

				FILE* f = fopen(testpath, "r");
				if (f) { 
					langVett[j] = true;
					fclose(f);
				} else { 
					langVett[j] = false; 
					fclose(f); 
				}
			}
			
			//check for multiple wcx events
			int k, n = getN(i);
			if (n != 0) {
				for (int j = 0; j < total; j++) {
					k = 0;
					for (int t = 0; t < n; t++) {	
						getMultiplePathWCX(testpath, j, i, t + 1);

						FILE* f = fopen(testpath, "r");
						if (f) {
							k++;
							if (currentMultipleWCSelected == 0)
								currentMultipleWCSelected = t + 1;
							fclose(f); 
						}
					}
					if (k == n) langVett[j] = true;
					else langVett[j] = false;
				}
			}
			
			// set first lang selected
			langSelected = -1;
			for (int i = 0; i < total; i++) {
				if (langVett[i]) {
					langSelected = i;
					break;
				}
			}
			
			if (langSelected != -1) {
				reloadPreviewBuf(previewbuf, i, langSelected);
				reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
			}
			
			while (aptMainLoop()) {
				hidScanInput();
				touchPosition touch;
				hidTouchRead(&touch);
				
				if (hidKeysDown() & KEY_B) {
					currentMultipleWCSelected = 0;
					break;
				}
				
				if (hidKeysDown() & KEY_L) {
					int n = getN(i);
					if (n != 0) {
						currentMultipleWCSelected--;
						if (currentMultipleWCSelected == 0) currentMultipleWCSelected = n;
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
				}
				
				if (hidKeysDown() & KEY_R) {
					int n = getN(i);
					if (n != 0) {
						currentMultipleWCSelected = (currentMultipleWCSelected + 1) % n;
						if (currentMultipleWCSelected == 0) currentMultipleWCSelected = 1;
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
				}

#ifdef PKSV
#else
				if (hidKeysDown() & KEY_Y) {
					if (!init())
						break;
					
					do {
						hidScanInput();
						process_wcx(previewbuf);
						printDB7(previewbuf, game, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0], true);
					} while (aptMainLoop() && !(hidKeysDown() & KEY_B));
					shutDownSoc();
				}
#endif
				
				if (hidKeysHeld() & KEY_TOUCH) {
					if (touch.px > 114 && touch.px < 150 && touch.py > 50 && touch.py < 71 && langVett[0]) {
						langSelected = 0;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					if (touch.px > 153 && touch.px < 189 && touch.py > 50 && touch.py < 71 && langVett[1]) {
						langSelected = 1;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					if (touch.px > 192 && touch.px < 228 && touch.py > 50 && touch.py < 71 && langVett[2]) {
						langSelected = 2;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					if (touch.px > 231 && touch.px < 267 && touch.py > 50 && touch.py < 71 && langVett[3]) {
						langSelected = 3;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					if (touch.px > 270 && touch.px < 306 && touch.py > 50 && touch.py < 71 && langVett[4]) {
						langSelected = 4;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					if (touch.px > 153 && touch.px < 189 && touch.py > 74 && touch.py < 95 && langVett[5]) {
						langSelected = 5;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					if (touch.px > 192 && touch.px < 228 && touch.py > 74 && touch.py < 95 && langVett[6]) {
						langSelected = 6;
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					
					if (total == 9) {
						if (touch.px > 231 && touch.px < 267 && touch.py > 74 && touch.py < 95 && langVett[7]) {
							langSelected = 7;
							reloadPreviewBuf(previewbuf, i, langSelected);
							reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
						}
						if (touch.px > 270 && touch.px < 306 && touch.py > 74 && touch.py < 95 && langVett[8]) {
							langSelected = 8;
							reloadPreviewBuf(previewbuf, i, langSelected);
							reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
						}
					}
					
					if (touch.px > 231 && touch.px < 267 && touch.py > 110 && touch.py < 131) {
						overwrite = true;
						nInjected[0] = 0;
					}
					if (touch.px > 270 && touch.px < 306 && touch.py > 110 && touch.py < 131) {
						overwrite = false;
						findFreeLocationWC(mainbuf, game, nInjected);
					}
					
					if (touch.px > 231 && touch.px < 267 && touch.py > 138 && touch.py < 159) adapt = true;
					if (touch.px > 270 && touch.px < 306 && touch.py > 138 && touch.py < 159) adapt = false;
				}
				
				#ifdef PKSV
				#else
				if (hidKeysDown() & KEY_START) {
					if (nInjected[0] >= 48) 
						nInjected[0] = 0;
					
					if ((game == GAME_X || game == GAME_Y) && i == 2048) {
						infoDisp(i18n(S_DATABASE_ITEM_NOT_AVAILABLE_XY));
						break;
					}

					if (!(overwrite))
						findFreeLocationWC(mainbuf, game, nInjected);

					if (adapt)
						setSaveLanguage(mainbuf, game, langSelected);

					setWC(mainbuf, previewbuf, game, i, nInjected);
					
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
				#endif
				
				printDB7(previewbuf, game, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0], false);
			}
			
			free(testpath);
		}
		
		printDatabase6(database, currentEntry, page, spriteArray);
	}
	
	free(spriteArray);
	free(previewbuf);
}

void eventDatabase5(u8* mainbuf, int game) {
	bool isGen5 = game == GAME_B1 || game == GAME_B2 || game == GAME_W1 || game == GAME_W2;
	int sz = isGen5 ? 170 : 190;
	char *database[sz];
	int *spriteArray = (int*)malloc(sz * sizeof(int));
	
	if (isGen5)
		filldatabase5(database, spriteArray);
	else
		filldatabase4(database, spriteArray);
	
	int currentEntry = 0;
	int page = 0;

	int nInjected[1] = {0};
	bool langVett[7];
	bool isSelected = false;
	int langSelected = -1;
	
	while(aptMainLoop()) {
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)
			break;
		
		if (hidKeysDown() & KEY_L) {
			if (page > 0) {
				int temp;
				do {
					page--;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page--;
				} while (temp == 10);
			}
			else if (page == 0) page = isGen5 ? 16 : 18;
		}
		
		if (hidKeysDown() & KEY_R) {
			if (page < (isGen5 ? 16 : 18)) {
				int temp;
				do {
					page++;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page++;
				} while (temp == 10);
			}
			else if (page == (isGen5 ? 16 : 18)) page = 0;
		}
		
		if (hidKeysDown() & KEY_UP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_LEFT) {
			if (currentEntry <= 4) {
				int temp;
				do {
					page--;
					if (page < 0) page = (isGen5 ? 16 : 18);
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page--;
				} while (temp == 10);
			}
			else if (currentEntry >= 5) currentEntry -= 5;
		}
		
		if (hidKeysDown() & KEY_RIGHT) {
			if (currentEntry <= 4) currentEntry += 5;
			else if (currentEntry >= 5) {
				int temp;
				do {
					page++;
					if (page > (isGen5 ? 16 : 18)) page = 0;
					temp = 0;
					for (int i = 0; i < 10; i++)
						if (strcmp(database[page*10+i], " ") == 0)
							temp++;
					
					if (temp == 10) 
						page++;
				} while (temp == 10);
			}
		}
		
		if (hidKeysDown() & KEY_A && spriteArray[page*10+currentEntry] != -1) {
			isSelected = true;
			int i = page * 10 + currentEntry;

			char *testpath = (char*)malloc(40 * sizeof(char));
			for (int j = 0; j < 7; j++) {
				if (isGen5)
					getSinglePathPGF(testpath, j, i);
				else
					getSinglePathPGT(testpath, j, i);

				FILE* f = fopen(testpath, "r");
				if (f) { langVett[j] = true; fclose(f); }
				else { langVett[j] = false; fclose(f); }
			}
			
			// set first lang selected
			langSelected = -1;
			for (int i = 0; i < 7; i++) {
				if (langVett[i]) {
					langSelected = i;
					break;
				}
			}
			
			while (aptMainLoop()) {
				hidScanInput();
				touchPosition touch;
				hidTouchRead(&touch);
				
				if (hidKeysDown() & KEY_B) break;
				
				if (hidKeysHeld() & KEY_TOUCH) {
					if (touch.px > 25 && touch.px < 61 && touch.py > 178 && touch.py < 202 && langVett[0]) langSelected = 0;
					if (touch.px > 63 && touch.px < 99 && touch.py > 178 && touch.py < 202 && langVett[1]) langSelected = 1;
					if (touch.px > 101 && touch.px < 137 && touch.py > 178 && touch.py < 202 && langVett[2]) langSelected = 2;
					if (touch.px > 139 && touch.px < 175 && touch.py > 178 && touch.py < 202 && langVett[3]) langSelected = 3;
					if (touch.px > 177 && touch.px < 213 && touch.py > 178 && touch.py < 202 && langVett[4]) langSelected = 4;
					if (touch.px > 215 && touch.px < 251 && touch.py > 178 && touch.py < 202 && langVett[5]) langSelected = 5;
					if (touch.px > 253 && touch.px < 289 && touch.py > 178 && touch.py < 202 && langVett[6]) langSelected = 6;
				}
				
				#ifdef PKSV
				#else
				if (hidKeysDown() & KEY_START) {
					if (nInjected[0] >= (isGen5 ? 12 : 8)) 
						nInjected[0] = 0;
					
					char *path = (char*)malloc(30 * sizeof(char));
					if (isGen5)
						getSinglePathPGF(path, langSelected, i);
					else
						getSinglePathPGT(path, langSelected, i);
					
					FILE *fptr = fopen(path, "rt");
					if (fptr == NULL) {
						fclose(fptr);
						free(path);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *buf = (u8*)malloc(contentsize);
					if (buf == NULL) {
						fclose(fptr);
						free(buf);
						free(path);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					rewind(fptr);
					fread(buf, contentsize, 1, fptr);
					fclose(fptr);

					setWC(mainbuf, buf, game, i, nInjected);

					free(path);
					free(buf);					
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
				#endif
				
				printDatabaseListDS(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
			}
			
			isSelected = false;
			free(testpath);
		}
		
		printDatabaseListDS(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
	}
	
	free(spriteArray);
}