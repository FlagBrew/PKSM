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

void reloadPreviewBuf(u8* previewBuf, int i, int n) {
	char *testpath = (char*)malloc(40 * sizeof(char));
	switch (n) {
		case 0 : {
			snprintf(testpath, 40, "romfs:/wcx/jpn/%d.wcx", i);
			break;
		} 
		case 1 : {
			snprintf(testpath, 40, "romfs:/wcx/eng/%d.wcx", i);
			break;
		}
		case 2 : {
			snprintf(testpath, 40, "romfs:/wcx/fre/%d.wcx", i);
			break;
		}
		case 3 : {
			snprintf(testpath, 40, "romfs:/wcx/ita/%d.wcx", i);
			break;
		}
		case 4 : {
			snprintf(testpath, 40, "romfs:/wcx/ger/%d.wcx", i);
			break;
		}
		case 5 : {
			snprintf(testpath, 40, "romfs:/wcx/spa/%d.wcx", i);
			break;
		}
		case 6 : {
			snprintf(testpath, 40, "romfs:/wcx/kor/%d.wcx", i);
			break;
		}
		case 7 : {
			snprintf(testpath, 40, "romfs:/wcx/chs/%d.wcx", i);
			break;
		}
		case 8 : {
			snprintf(testpath, 40, "romfs:/wcx/cht/%d.wcx", i);
			break;
		}
	}
	
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
	switch (n) {
		case 0 : {
			snprintf(testpath, 40, "romfs:/wcx/jpn/%d-%d.wcx", i, j);
			break;
		} 
		case 1 : {
			snprintf(testpath, 40, "romfs:/wcx/eng/%d-%d.wcx", i, j);
			break;
		}
		case 2 : {
			snprintf(testpath, 40, "romfs:/wcx/fre/%d-%d.wcx", i, j);
			break;
		}
		case 3 : {
			snprintf(testpath, 40, "romfs:/wcx/ita/%d-%d.wcx", i, j);
			break;
		}
		case 4 : {
			snprintf(testpath, 40, "romfs:/wcx/ger/%d-%d.wcx", i, j);
			break;
		}
		case 5 : {
			snprintf(testpath, 40, "romfs:/wcx/spa/%d-%d.wcx", i, j);
			break;
		}
		case 6 : {
			snprintf(testpath, 40, "romfs:/wcx/kor/%d-%d.wcx", i, j);
			break;
		}
		case 7 : {
			snprintf(testpath, 40, "romfs:/wcx/chs/%d-%d.wcx", i, j);
			break;
		}
		case 8 : {
			snprintf(testpath, 40, "romfs:/wcx/cht/%d-%d.wcx", i, j);
			break;
		}
	}
	
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
		case 223 : return 5;
		case 504 : return 2;
		case 515 : return 2;
		case 551 : return 2;
		case 552 : return 2;
		case 1111 : return 6;
		case 2016 : return 3;		
	}	
	return 0;
}

int checkMultipleWCX(u8* mainbuf, int game, int i, int langCont, int nInjected[], int adapt) {
	int n = getN(i);
	if (n == 0)
		return 0;
	
	for (int j = 0; j < n; j++) {
		char *wcxpath = (char*)malloc(40 * sizeof(char));
		switch (langCont) {
			case 0 : {
				snprintf(wcxpath, 40, "romfs:/wcx/jpn/%d-%d.wcx", i, j + 1);
				break;
			} 
			case 1 : {
				snprintf(wcxpath, 40, "romfs:/wcx/eng/%d-%d.wcx", i, j + 1);
				break;
			}
			case 2 : {
				snprintf(wcxpath, 40, "romfs:/wcx/fre/%d-%d.wcx", i, j + 1);
				break;
			}
			case 3 : {
				snprintf(wcxpath, 40, "romfs:/wcx/ita/%d-%d.wcx", i, j + 1);
				break;
			}
			case 4 : {
				snprintf(wcxpath, 40, "romfs:/wcx/ger/%d-%d.wcx", i, j + 1);
				break;
			}
			case 5 : {
				snprintf(wcxpath, 40, "romfs:/wcx/spa/%d-%d.wcx", i, j + 1);
				break;
			}
			case 6 : {
				snprintf(wcxpath, 40, "romfs:/wcx/kor/%d-%d.wcx", i, j + 1);
				break;
			}
			case 7 : {
				snprintf(wcxpath, 40, "romfs:/wcx/chs/%d-%d.wcx", i, j + 1);
				break;
			}
			case 8 : {
				snprintf(wcxpath, 40, "romfs:/wcx/cht/%d-%d.wcx", i, j + 1);
				break;
			}
		}
		
		FILE *fptr = fopen(wcxpath, "rt");
		if (fptr == NULL) {
			fclose(fptr);
			free(wcxpath);
			return 15;
		}
		fseek(fptr, 0, SEEK_END);
		u32 contentsize = ftell(fptr);
		u8 *wcxbuf = (u8*)malloc(contentsize);
		if (wcxbuf == NULL) {
			fclose(fptr);
			free(wcxbuf);
			free(wcxpath);
			return 8;
		}
		rewind(fptr);
		fread(wcxbuf, contentsize, 1, fptr);
		fclose(fptr);

		if (adapt)
			setSaveLanguage(mainbuf, game, langCont);

		setWC(mainbuf, wcxbuf, game, i, nInjected);

		free(wcxpath);
		free(wcxbuf);
	}
	return 1;
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
				switch (j) {
					case 0 : {
						snprintf(testpath, 40, "romfs:/wcx/jpn/%d.wcx", i);
						break;
					} 
					case 1 : {
						snprintf(testpath, 40, "romfs:/wcx/eng/%d.wcx", i);
						break;
					}
					case 2 : {
						snprintf(testpath, 40, "romfs:/wcx/fre/%d.wcx", i);
						break;
					}
					case 3 : {
						snprintf(testpath, 40, "romfs:/wcx/ita/%d.wcx", i);
						break;
					}
					case 4 : {
						snprintf(testpath, 40, "romfs:/wcx/ger/%d.wcx", i);
						break;
					}
					case 5 : {
						snprintf(testpath, 40, "romfs:/wcx/spa/%d.wcx", i);
						break;
					}
					case 6 : {
						snprintf(testpath, 40, "romfs:/wcx/kor/%d.wcx", i);
						break;
					}
					case 7 : {
						snprintf(testpath, 40, "romfs:/wcx/chs/%d.wcx", i);
						break;
					}
					case 8 : {
						snprintf(testpath, 40, "romfs:/wcx/cht/%d.wcx", i);
						break;
					}
				}
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
						switch (j) {
							case 0 : {
								snprintf(testpath, 40, "romfs:/wcx/jpn/%d-%d.wcx", i, t + 1);
								break;
							} 
							case 1 : {
								snprintf(testpath, 40, "romfs:/wcx/eng/%d-%d.wcx", i, t + 1);
								break;
							}
							case 2 : {
								snprintf(testpath, 40, "romfs:/wcx/fre/%d-%d.wcx", i, t + 1);
								break;
							}
							case 3 : {
								snprintf(testpath, 40, "romfs:/wcx/ita/%d-%d.wcx", i, t + 1);
								break;
							}
							case 4 : {
								snprintf(testpath, 40, "romfs:/wcx/ger/%d-%d.wcx", i, t + 1);
								break;
							}
							case 5 : {
								snprintf(testpath, 40, "romfs:/wcx/spa/%d-%d.wcx", i, t + 1);
								break;
							}
							case 6 : {
								snprintf(testpath, 40, "romfs:/wcx/kor/%d-%d.wcx", i, t + 1);
								break;
							}
							case 7 : {
								snprintf(testpath, 40, "romfs:/wcx/chs/%d-%d.wcx", i, t + 1);
								break;
							}
							case 8 : {
								snprintf(testpath, 40, "romfs:/wcx/cht/%d-%d.wcx", i, t + 1);
								break;
							}
						}
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
					
					int ret = checkMultipleWCX(mainbuf, game, i, langSelected, nInjected, adapt);
					if (ret != 0 && ret != 1) {
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					
					else if (ret == 1) {
						infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
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
				
				printDB7(previewbuf, game, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0]);
			}
			
			free(testpath);
		}
		
		printDatabase6(database, currentEntry, page, spriteArray);
	}
	
	free(spriteArray);
	free(previewbuf);
}

void eventDatabase5(u8* mainbuf, int game) {
	char *database[170];
	int *spriteArray = (int*)malloc(170 * sizeof(int));
	filldatabase5(database, spriteArray);
	
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
			else if (page == 0) page = 16;
		}
		
		if (hidKeysDown() & KEY_R) {
			if (page < 16) {
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
			else if (page == 16) page = 0;
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
					if (page < 0) page = 16;
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
					if (page > 16) page = 0;
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
				switch (j) {
					case 0 : {
						snprintf(testpath, 40, "romfs:/pgf/jpn/%d.pgf", i);
						break;
					} 
					case 1 : {
						snprintf(testpath, 40, "romfs:/pgf/eng/%d.pgf", i);
						break;
					}
					case 2 : {
						snprintf(testpath, 40, "romfs:/pgf/fre/%d.pgf", i);
						break;
					}
					case 3 : {
						snprintf(testpath, 40, "romfs:/pgf/ita/%d.pgf", i);
						break;
					}
					case 4 : {
						snprintf(testpath, 40, "romfs:/pgf/ger/%d.pgf", i);
						break;
					}
					case 5 : {
						snprintf(testpath, 40, "romfs:/pgf/spa/%d.pgf", i);
						break;
					}
					case 6 : {
						snprintf(testpath, 40, "romfs:/pgf/kor/%d.pgf", i);
						break;
					}
				}
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
					if (nInjected[0] >= 12) 
						nInjected[0] = 0;
					
					char *pgfpath = (char*)malloc(30 * sizeof(char));
					switch (langSelected) {
						case 0 : {
							snprintf(pgfpath, 30, "romfs:/pgf/jpn/%d.pgf", i);
							break;
						} 
						case 1 : {
							snprintf(pgfpath, 30, "romfs:/pgf/eng/%d.pgf", i);
							break;
						}
						case 2 : {
							snprintf(pgfpath, 30, "romfs:/pgf/fre/%d.pgf", i);
							break;
						}
						case 3 : {
							snprintf(pgfpath, 30, "romfs:/pgf/ita/%d.pgf", i);
							break;
						}
						case 4 : {
							snprintf(pgfpath, 30, "romfs:/pgf/ger/%d.pgf", i);
							break;
						}
						case 5 : {
							snprintf(pgfpath, 30, "romfs:/pgf/spa/%d.pgf", i);
							break;
						}
						case 6 : {
							snprintf(pgfpath, 30, "romfs:/pgf/kor/%d.pgf", i);
							break;
						}
					}
					
					FILE *fptr = fopen(pgfpath, "rt");
					if (fptr == NULL) {
						fclose(fptr);
						free(pgfpath);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *pgfbuf = (u8*)malloc(contentsize);
					if (pgfbuf == NULL) {
						fclose(fptr);
						free(pgfbuf);
						free(pgfpath);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					rewind(fptr);
					fread(pgfbuf, contentsize, 1, fptr);
					fclose(fptr);

					setWC(mainbuf, pgfbuf, game, i, nInjected);

					free(pgfpath);
					free(pgfbuf);					
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
				#endif
				
				printDatabase5(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
			}
			
			isSelected = false;
			free(testpath);
		}
		
		printDatabase5(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
	}
	
	free(spriteArray);
}

void eventDatabase4(u8* mainbuf, int game, int GBO, int SBO) {
	char *database[190];
	int *spriteArray = (int*)malloc(190 * sizeof(int));
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
			else if (page == 0) page = 18;
		}
		
		if (hidKeysDown() & KEY_R) {
			if (page < 18) {
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
			else if (page == 18) page = 0;
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
					if (page < 0) page = 18;
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
					if (page > 18) page = 0;
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
				switch (j) {
					case 0 : {
						snprintf(testpath, 40, "romfs:/pgt/jpn/%d.pgt", i);
						break;
					} 
					case 1 : {
						snprintf(testpath, 40, "romfs:/pgt/eng/%d.pgt", i);
						break;
					}
					case 2 : {
						snprintf(testpath, 40, "romfs:/pgt/fre/%d.pgt", i);
						break;
					}
					case 3 : {
						snprintf(testpath, 40, "romfs:/pgt/ita/%d.pgt", i);
						break;
					}
					case 4 : {
						snprintf(testpath, 40, "romfs:/pgt/ger/%d.pgt", i);
						break;
					}
					case 5 : {
						snprintf(testpath, 40, "romfs:/pgt/spa/%d.pgt", i);
						break;
					}
					case 6 : {
						snprintf(testpath, 40, "romfs:/pgt/kor/%d.pgt", i);
						break;
					}
				}
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
					if (nInjected[0] >= 8) 
						nInjected[0] = 0;
					
					char *pgtpath = (char*)malloc(30 * sizeof(char));
					switch (langSelected) {
						case 0 : {
							snprintf(pgtpath, 30, "romfs:/pgt/jpn/%d.pgt", i);
							break;
						} 
						case 1 : {
							snprintf(pgtpath, 30, "romfs:/pgt/eng/%d.pgt", i);
							break;
						}
						case 2 : {
							snprintf(pgtpath, 30, "romfs:/pgt/fre/%d.pgt", i);
							break;
						}
						case 3 : {
							snprintf(pgtpath, 30, "romfs:/pgt/ita/%d.pgt", i);
							break;
						}
						case 4 : {
							snprintf(pgtpath, 30, "romfs:/pgt/ger/%d.pgt", i);
							break;
						}
						case 5 : {
							snprintf(pgtpath, 30, "romfs:/pgt/spa/%d.pgt", i);
							break;
						}
						case 6 : {
							snprintf(pgtpath, 30, "romfs:/pgt/kor/%d.pgt", i);
							break;
						}
					}
					
					FILE *fptr = fopen(pgtpath, "rt");
					if (fptr == NULL) {
						fclose(fptr);
						free(pgtpath);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *pgtbuf = (u8*)malloc(contentsize);
					if (pgtbuf == NULL) {
						fclose(fptr);
						free(pgtbuf);
						free(pgtpath);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					rewind(fptr);
					fread(pgtbuf, contentsize, 1, fptr);
					fclose(fptr);

					setWC4(mainbuf, pgtbuf, game, i, nInjected, GBO);

					free(pgtpath);
					free(pgtbuf);					
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
				#endif
				
				printDatabase4(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
			}
			
			isSelected = false;
			free(testpath);
		}
		
		printDatabase4(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
	}
	
	free(spriteArray);
}