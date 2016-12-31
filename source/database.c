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

#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "graphic.h"
#include "database.h"
#include "editor.h"
#include "util.h"
#include "fill.h"
#include "dex.h"

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
		case 136 : return 4;
		case 205 : return 2;
		case 504 : return 2;
		case 515 : return 2;
		case 551 : return 2;
		case 552 : return 2;
		case 2016 : return 3;		
	}	
	return 0;
}

void setBoxBin(u8* mainbuf, int game, int NBOXES, int N, char* path[]) {
	for (int i = 0; i < NBOXES; i++) {
		FILE *fptr = fopen(path[i], "rt");
		if (fptr == NULL)
			return;
		fseek(fptr, 0, SEEK_END);
		u32 contentsize = ftell(fptr);
		u8 *buf = (u8*)malloc(contentsize);
		if (buf == NULL) 
			return;
		rewind(fptr);
		fread(buf, contentsize, 1, fptr);
		fclose(fptr);

		int boxpos = 0;
		if (game == GAME_X || game == GAME_Y) 
			boxpos = 0x27A00 - 0x5400;
	
		if (game == GAME_OR || game == GAME_AS) 
			boxpos = 0x38400 - 0x5400;
		
		if (game == GAME_SUN || game == GAME_MOON)
			boxpos = 0x04E00;
		
		memcpy((void*)(mainbuf + boxpos + PKMNLENGTH * 30 * i), (const void*)buf, PKMNLENGTH * N);
		
		free(buf);
	}
}

int checkMultipleWC7(u8* mainbuf, int game, int i, int langCont, int nInjected[], int adapt) {
	int n = getN(i);
	if (n == 0)
		return 0;
	
	for (int j = 0; j < n; j++) {
		char *wc7path = (char*)malloc(40 * sizeof(char));
		switch (langCont) {
			case 0 : {
				snprintf(wc7path, 40, "romfs:/wc7/jpn/%d-%d.wc7", i, j + 1);
				break;
			} 
			case 1 : {
				snprintf(wc7path, 40, "romfs:/wc7/eng/%d-%d.wc7", i, j + 1);
				break;
			}
			case 2 : {
				snprintf(wc7path, 40, "romfs:/wc7/fre/%d-%d.wc7", i, j + 1);
				break;
			}
			case 3 : {
				snprintf(wc7path, 40, "romfs:/wc7/ita/%d-%d.wc7", i, j + 1);
				break;
			}
			case 4 : {
				snprintf(wc7path, 40, "romfs:/wc7/ger/%d-%d.wc7", i, j + 1);
				break;
			}
			case 5 : {
				snprintf(wc7path, 40, "romfs:/wc7/spa/%d-%d.wc7", i, j + 1);
				break;
			}
			case 6 : {
				snprintf(wc7path, 40, "romfs:/wc7/kor/%d-%d.wc7", i, j + 1);
				break;
			}
			case 7 : {
				snprintf(wc7path, 40, "romfs:/wc7/chs/%d-%d.wc7", i, j + 1);
				break;
			}
			case 8 : {
				snprintf(wc7path, 40, "romfs:/wc7/cht/%d-%d.wc7", i, j + 1);
				break;
			}
		}
		
		FILE *fptr = fopen(wc7path, "rt");
		if (fptr == NULL) {
			fclose(fptr);
			free(wc7path);
			return 15;
		}
		fseek(fptr, 0, SEEK_END);
		u32 contentsize = ftell(fptr);
		u8 *wc7buf = (u8*)malloc(contentsize);
		if (wc7buf == NULL) {
			fclose(fptr);
			free(wc7buf);
			free(wc7path);
			return 8;
		}
		rewind(fptr);
		fread(wc7buf, contentsize, 1, fptr);
		fclose(fptr);

		if (adapt)
			setLanguage(mainbuf, game, langCont);

		setWC(mainbuf, wc7buf, game, i, nInjected);

		free(wc7path);
		free(wc7buf);
	}
	return 1;
}

int checkMultipleWC6(u8* mainbuf, int game, int i, int langCont, int nInjected[], int adapt) {
	int n = getN(i);
	if (n == 0)
		return 0;
	
	for (int j = 0; j < n; j++) {
		char *wc6path = (char*)malloc(40 * sizeof(char));
		switch (langCont) {
			case 0 : {
				snprintf(wc6path, 40, "romfs:/wc6/jpn/%d-%d.wc6", i, j + 1);
				break;
			} 
			case 1 : {
				snprintf(wc6path, 40, "romfs:/wc6/eng/%d-%d.wc6", i, j + 1);
				break;
			}
			case 2 : {
				snprintf(wc6path, 40, "romfs:/wc6/fre/%d-%d.wc6", i, j + 1);
				break;
			}
			case 3 : {
				snprintf(wc6path, 40, "romfs:/wc6/ita/%d-%d.wc6", i, j + 1);
				break;
			}
			case 4 : {
				snprintf(wc6path, 40, "romfs:/wc6/ger/%d-%d.wc6", i, j + 1);
				break;
			}
			case 5 : {
				snprintf(wc6path, 40, "romfs:/wc6/spa/%d-%d.wc6", i, j + 1);
				break;
			}
			case 6 : {
				snprintf(wc6path, 40, "romfs:/wc6/kor/%d-%d.wc6", i, j + 1);
				break;
			}
		}
		
		FILE *fptr = fopen(wc6path, "rt");
		if (fptr == NULL) {
			fclose(fptr);
			free(wc6path);
			return 15;
		}
		fseek(fptr, 0, SEEK_END);
		u32 contentsize = ftell(fptr);
		u8 *wc6buf = (u8*)malloc(contentsize);
		if (wc6buf == NULL) {
			fclose(fptr);
			free(wc6buf);
			free(wc6path);
			return 8;
		}
		rewind(fptr);
		fread(wc6buf, contentsize, 1, fptr);
		fclose(fptr);

		if (adapt)
			setLanguage(mainbuf, game, langCont);

		setWC(mainbuf, wc6buf, game, i, nInjected);

		free(wc6path);
		free(wc6buf);
	}
	return 1;
}

void eventDatabase7(u8* mainbuf, int game) {
	char *database[SMCOUNT];
	int *spriteArray = (int*)malloc(SMCOUNT * sizeof(int));
	filldatabase7(database, spriteArray);
	
	int currentEntry = 0;
	int page = 0;
	
	bool adapt = false;
	bool langVett[9];
	bool overwrite = false;
	int nInjected[1] = {0};
	int langSelected = -1;
	
	findFreeLocationWC(mainbuf, game, nInjected);
	
	while(aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);		
		if ((hidKeysDown() & KEY_TOUCH) && touch.px > 298 && touch.px < 320 && touch.py > 207 && touch.py < 225) break;
		
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
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
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
		
		if (hidKeysDown() & KEY_DRIGHT) {
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
			int i = page * 10 + currentEntry;
			// check for single wc7 events
			char *testpath = (char*)malloc(40 * sizeof(char));
			for (int j = 0; j < 9; j++) {
				switch (j) {
					case 0 : {
						snprintf(testpath, 40, "romfs:/wc7/jpn/%d.wc7", i);
						break;
					} 
					case 1 : {
						snprintf(testpath, 40, "romfs:/wc7/eng/%d.wc7", i);
						break;
					}
					case 2 : {
						snprintf(testpath, 40, "romfs:/wc7/fre/%d.wc7", i);
						break;
					}
					case 3 : {
						snprintf(testpath, 40, "romfs:/wc7/ita/%d.wc7", i);
						break;
					}
					case 4 : {
						snprintf(testpath, 40, "romfs:/wc7/ger/%d.wc7", i);
						break;
					}
					case 5 : {
						snprintf(testpath, 40, "romfs:/wc7/spa/%d.wc7", i);
						break;
					}
					case 6 : {
						snprintf(testpath, 40, "romfs:/wc7/kor/%d.wc7", i);
						break;
					}
					case 7 : {
						snprintf(testpath, 40, "romfs:/wc7/chs/%d.wc7", i);
						break;
					}
					case 8 : {
						snprintf(testpath, 40, "romfs:/wc7/cht/%d.wc7", i);
						break;
					}
				}
				FILE* f = fopen(testpath, "r");
				if (f) { langVett[j] = true; fclose(f); }
				else { langVett[j] = false; fclose(f); }
			}
			
			//check for multiple wc7 events
			int k, n = getN(i);
			if (n != 0) {
				for (int j = 0; j < 9; j++) {
					k = 0;
					for (int t = 0; t < n; t++) {
						switch (j) {
							case 0 : {
								snprintf(testpath, 40, "romfs:/wc7/jpn/%d-%d.wc7", i, t + 1);
								break;
							} 
							case 1 : {
								snprintf(testpath, 40, "romfs:/wc7/eng/%d-%d.wc7", i, t + 1);
								break;
							}
							case 2 : {
								snprintf(testpath, 40, "romfs:/wc7/fre/%d-%d.wc7", i, t + 1);
								break;
							}
							case 3 : {
								snprintf(testpath, 40, "romfs:/wc7/ita/%d-%d.wc7", i, t + 1);
								break;
							}
							case 4 : {
								snprintf(testpath, 40, "romfs:/wc7/ger/%d-%d.wc7", i, t + 1);
								break;
							}
							case 5 : {
								snprintf(testpath, 40, "romfs:/wc7/spa/%d-%d.wc7", i, t + 1);
								break;
							}
							case 6 : {
								snprintf(testpath, 40, "romfs:/wc7/kor/%d-%d.wc7", i, t + 1);
								break;
							}
							case 7 : {
								snprintf(testpath, 40, "romfs:/wc7/chs/%d-%d.wc7", i, t + 1);
								break;
							}
							case 8 : {
								snprintf(testpath, 40, "romfs:/wc7/cht/%d-%d.wc7", i, t + 1);
								break;
							}
						}
						FILE* f = fopen(testpath, "r");
						if (f) { k++; fclose(f); }
					}
					if (k == n) langVett[j] = true;
					else langVett[j] = false;
				}
			}
			
			// set first lang selected
			langSelected = -1;
			for (int i = 0; i < 9; i++) {
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
					if (touch.px > 114 && touch.px < 150 && touch.py > 50 && touch.py < 71 && langVett[0]) langSelected = 0;
					if (touch.px > 153 && touch.px < 189 && touch.py > 50 && touch.py < 71 && langVett[1]) langSelected = 1;
					if (touch.px > 192 && touch.px < 228 && touch.py > 50 && touch.py < 71 && langVett[2]) langSelected = 2;
					if (touch.px > 231 && touch.px < 267 && touch.py > 50 && touch.py < 71 && langVett[3]) langSelected = 3;
					if (touch.px > 270 && touch.px < 306 && touch.py > 50 && touch.py < 71 && langVett[4]) langSelected = 4;
					if (touch.px > 133 && touch.px < 169 && touch.py > 74 && touch.py < 95 && langVett[5]) langSelected = 5;
					if (touch.px > 172 && touch.px < 208 && touch.py > 74 && touch.py < 95 && langVett[6]) langSelected = 6;
					if (touch.px > 211 && touch.px < 247 && touch.py > 74 && touch.py < 95 && langVett[7]) langSelected = 7;
					if (touch.px > 250 && touch.px < 286 && touch.py > 74 && touch.py < 95 && langVett[8]) langSelected = 8;
					
					if (touch.px > 210 && touch.px < 246 && touch.py > 110 && touch.py < 131) {
						overwrite = true;
						nInjected[0] = 0;
					}
					if (touch.px > 249 && touch.px < 285 && touch.py > 110 && touch.py < 131) {
						overwrite = false;
						findFreeLocationWC(mainbuf, game, nInjected);
					}
					
					if (touch.px > 210 && touch.px < 246 && touch.py > 138 && touch.py < 159) adapt = true;
					if (touch.px > 249 && touch.px < 285 && touch.py > 138 && touch.py < 159) adapt = false;
				}
				
				if (hidKeysDown() & KEY_START) {
					if (nInjected[0] >= 48) 
						nInjected[0] = 0;
					
					int ret = checkMultipleWC7(mainbuf, game, i, langSelected, nInjected, adapt);
					if (ret != 0 && ret != 1) {
						infoDisp("Error during injection!");
						break;
					}
					else if (ret == 1) {
						infoDisp("Injection succeeded!");
						break;
					}
					
					char *wc7path = (char*)malloc(30 * sizeof(char));
					switch (langSelected) {
						case 0 : {
							snprintf(wc7path, 30, "romfs:/wc7/jpn/%d.wc7", i);
							break;
						} 
						case 1 : {
							snprintf(wc7path, 30, "romfs:/wc7/eng/%d.wc7", i);
							break;
						}
						case 2 : {
							snprintf(wc7path, 30, "romfs:/wc7/fre/%d.wc7", i);
							break;
						}
						case 3 : {
							snprintf(wc7path, 30, "romfs:/wc7/ita/%d.wc7", i);
							break;
						}
						case 4 : {
							snprintf(wc7path, 30, "romfs:/wc7/ger/%d.wc7", i);
							break;
						}
						case 5 : {
							snprintf(wc7path, 30, "romfs:/wc7/spa/%d.wc7", i);
							break;
						}
						case 6 : {
							snprintf(wc7path, 30, "romfs:/wc7/kor/%d.wc7", i);
							break;
						}
						case 7 : {
							snprintf(wc7path, 30, "romfs:/wc7/chs/%d.wc7", i);
							break;
						}
						case 8 : {
							snprintf(wc7path, 30, "romfs:/wc7/cht/%d.wc7", i);
							break;
						}
					}
					
					FILE *fptr = fopen(wc7path, "rt");
					if (fptr == NULL) {
						fclose(fptr);
						free(wc7path);
						infoDisp("Error during injection!");
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *wc7buf = (u8*)malloc(contentsize);
					if (wc7buf == NULL) {
						fclose(fptr);
						free(wc7buf);
						free(wc7path);
						infoDisp("Error during injection!");
						break;
					}
					rewind(fptr);
					fread(wc7buf, contentsize, 1, fptr);
					fclose(fptr);

					if (!(overwrite))
						findFreeLocationWC(mainbuf, game, nInjected);

					if (adapt)
						setLanguage(mainbuf, game, langSelected);

					setWC(mainbuf, wc7buf, game, i, nInjected);

					free(wc7path);
					free(wc7buf);					
					infoDisp("Injection succeeded!");
					break;
				}
				
				printDB7(spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0]);
			}
			
			free(testpath);
		}
		
		printDatabase6(database, currentEntry, page, spriteArray);
	}
	
	free(spriteArray);
}

void eventDatabase6(u8* mainbuf, int game) {
	char *database[2050];
	int *spriteArray = (int*)malloc(2050 * sizeof(int));
	filldatabase6(database, spriteArray);
	
	int currentEntry = 0;
	int page = 0;
	
	bool adapt = false;
	bool langVett[7];
	bool overwrite = false;
	int nInjected[1] = {0};
	int langSelected = -1;
	
	findFreeLocationWC(mainbuf, game, nInjected);
	
	while(aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);		
		if ((hidKeysDown() & KEY_TOUCH) && touch.px > 298 && touch.px < 320 && touch.py > 207 && touch.py < 225) break;
		
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
			else if (page == 0) page = 204;
		}
		
		if (hidKeysDown() & KEY_R) {
			if (page < 204) {
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
			else if (page == 204) page = 0;
		}
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
			if (currentEntry <= 4) {
				int temp;
				do {
					page--;
					if (page < 0) page = 204;
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
		
		if (hidKeysDown() & KEY_DRIGHT) {
			if (currentEntry <= 4) currentEntry += 5;
			else if (currentEntry >= 5) {
				int temp;
				do {
					page++;
					if (page > 204) page = 0;
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
			int i = page * 10 + currentEntry;
			// check for single wc6 events
			char *testpath = (char*)malloc(40 * sizeof(char));
			for (int j = 0; j < 7; j++) {
				switch (j) {
					case 0 : {
						snprintf(testpath, 40, "romfs:/wc6/jpn/%d.wc6", i);
						break;
					} 
					case 1 : {
						snprintf(testpath, 40, "romfs:/wc6/eng/%d.wc6", i);
						break;
					}
					case 2 : {
						snprintf(testpath, 40, "romfs:/wc6/fre/%d.wc6", i);
						break;
					}
					case 3 : {
						snprintf(testpath, 40, "romfs:/wc6/ita/%d.wc6", i);
						break;
					}
					case 4 : {
						snprintf(testpath, 40, "romfs:/wc6/ger/%d.wc6", i);
						break;
					}
					case 5 : {
						snprintf(testpath, 40, "romfs:/wc6/spa/%d.wc6", i);
						break;
					}
					case 6 : {
						snprintf(testpath, 40, "romfs:/wc6/kor/%d.wc6", i);
						break;
					}
				}
				FILE* f = fopen(testpath, "r");
				if (f) { langVett[j] = true; fclose(f); }
				else { langVett[j] = false; fclose(f); }
			}
			
			//check for multiple wc6 events
			int k, n = getN(i);
			if (n != 0) {
				for (int j = 0; j < 7; j++) {
					k = 0;
					for (int t = 0; t < n; t++) {
						switch (j) {
							case 0 : {
								snprintf(testpath, 40, "romfs:/wc6/jpn/%d-%d.wc6", i, t + 1);
								break;
							} 
							case 1 : {
								snprintf(testpath, 40, "romfs:/wc6/eng/%d-%d.wc6", i, t + 1);
								break;
							}
							case 2 : {
								snprintf(testpath, 40, "romfs:/wc6/fre/%d-%d.wc6", i, t + 1);
								break;
							}
							case 3 : {
								snprintf(testpath, 40, "romfs:/wc6/ita/%d-%d.wc6", i, t + 1);
								break;
							}
							case 4 : {
								snprintf(testpath, 40, "romfs:/wc6/ger/%d-%d.wc6", i, t + 1);
								break;
							}
							case 5 : {
								snprintf(testpath, 40, "romfs:/wc6/spa/%d-%d.wc6", i, t + 1);
								break;
							}
							case 6 : {
								snprintf(testpath, 40, "romfs:/wc6/kor/%d-%d.wc6", i, t + 1);
								break;
							}
						}
						FILE* f = fopen(testpath, "r");
						if (f) { k++; fclose(f); }
					}
					if (k == n) langVett[j] = true;
					else langVett[j] = false;
				}
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
					if (touch.px > 132 && touch.px < 168 && touch.py > 50 && touch.py < 71 && langVett[0]) langSelected = 0;
					if (touch.px > 171 && touch.px < 207 && touch.py > 50 && touch.py < 71 && langVett[1]) langSelected = 1;
					if (touch.px > 210 && touch.px < 246 && touch.py > 50 && touch.py < 71 && langVett[2]) langSelected = 2;
					if (touch.px > 249 && touch.px < 285 && touch.py > 50 && touch.py < 71 && langVett[3]) langSelected = 3;
					if (touch.px > 151 && touch.px < 187 && touch.py > 74 && touch.py < 95 && langVett[4]) langSelected = 4;
					if (touch.px > 190 && touch.px < 226 && touch.py > 74 && touch.py < 95 && langVett[5]) langSelected = 5;
					if (touch.px > 229 && touch.px < 265 && touch.py > 74 && touch.py < 95 && langVett[6]) langSelected = 6;
					
					if (touch.px > 210 && touch.px < 246 && touch.py > 110 && touch.py < 131) {
						overwrite = true;
						nInjected[0] = 0;
					}
					if (touch.px > 249 && touch.px < 285 && touch.py > 110 && touch.py < 131) {
						overwrite = false;
						findFreeLocationWC(mainbuf, game, nInjected);
					}
					
					if (touch.px > 210 && touch.px < 246 && touch.py > 138 && touch.py < 159) adapt = true;
					if (touch.px > 249 && touch.px < 285 && touch.py > 138 && touch.py < 159) adapt = false;
				}
				
				if (hidKeysDown() & KEY_START) {
					if (nInjected[0] >= 24) 
						nInjected[0] = 0;

					if ((game == GAME_X || game == GAME_Y) && i == 2048) {
						infoDisp("Item not available in XY!");
						break;
					}
					
					int ret = checkMultipleWC6(mainbuf, game, i, langSelected, nInjected, adapt);
					if (ret != 0 && ret != 1) {
						infoDisp("Error during injection!");
						break;
					}
					else if (ret == 1) {
						infoDisp("Injection succeeded!");
						break;
					}
					
					char *wc6path = (char*)malloc(30 * sizeof(char));
					switch (langSelected) {
						case 0 : {
							snprintf(wc6path, 30, "romfs:/wc6/jpn/%d.wc6", i);
							break;
						} 
						case 1 : {
							snprintf(wc6path, 30, "romfs:/wc6/eng/%d.wc6", i);
							break;
						}
						case 2 : {
							snprintf(wc6path, 30, "romfs:/wc6/fre/%d.wc6", i);
							break;
						}
						case 3 : {
							snprintf(wc6path, 30, "romfs:/wc6/ita/%d.wc6", i);
							break;
						}
						case 4 : {
							snprintf(wc6path, 30, "romfs:/wc6/ger/%d.wc6", i);
							break;
						}
						case 5 : {
							snprintf(wc6path, 30, "romfs:/wc6/spa/%d.wc6", i);
							break;
						}
						case 6 : {
							snprintf(wc6path, 30, "romfs:/wc6/kor/%d.wc6", i);
							break;
						}
					}
					
					FILE *fptr = fopen(wc6path, "rt");
					if (fptr == NULL) {
						fclose(fptr);
						free(wc6path);
						infoDisp("Error during injection!");
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *wc6buf = (u8*)malloc(contentsize);
					if (wc6buf == NULL) {
						fclose(fptr);
						free(wc6buf);
						free(wc6path);
						infoDisp("Error during injection!");
						break;
					}
					rewind(fptr);
					fread(wc6buf, contentsize, 1, fptr);
					fclose(fptr);

					if (!(overwrite))
						findFreeLocationWC(mainbuf, game, nInjected);

					if (adapt)
						setLanguage(mainbuf, game, langSelected);

					setWC(mainbuf, wc6buf, game, i, nInjected);

					free(wc6path);
					free(wc6buf);					
					infoDisp("Injection succeeded!");
					break;
				}
				
				printDB6(spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0]);
			}
			
			free(testpath);
		}
		
		printDatabase6(database, currentEntry, page, spriteArray);
	}
	
	free(spriteArray);
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
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
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
		
		if (hidKeysDown() & KEY_DRIGHT) {
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
						infoDisp("Error during injection!");
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *pgfbuf = (u8*)malloc(contentsize);
					if (pgfbuf == NULL) {
						fclose(fptr);
						free(pgfbuf);
						free(pgfpath);
						infoDisp("Error during injection!");
						break;
					}
					rewind(fptr);
					fread(pgfbuf, contentsize, 1, fptr);
					fclose(fptr);

					setWC(mainbuf, pgfbuf, game, i, nInjected);

					free(pgfpath);
					free(pgfbuf);					
					infoDisp("Injection succeeded!");
					break;
				}
				
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
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
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
		
		if (hidKeysDown() & KEY_DRIGHT) {
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
						infoDisp("Error during injection!");
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *pgtbuf = (u8*)malloc(contentsize);
					if (pgtbuf == NULL) {
						fclose(fptr);
						free(pgtbuf);
						free(pgtpath);
						infoDisp("Error during injection!");
						break;
					}
					rewind(fptr);
					fread(pgtbuf, contentsize, 1, fptr);
					fclose(fptr);

					setWC4(mainbuf, pgtbuf, game, i, nInjected, GBO);

					free(pgtpath);
					free(pgtbuf);					
					infoDisp("Injection succeeded!");
					break;
				}
				
				printDatabase4(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
			}
			
			isSelected = false;
			free(testpath);
		}
		
		printDatabase4(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
	}
	
	free(spriteArray);
}

void massInjector(u8* mainbuf, int game) {
	int currentEntry = 0;

	while(aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_B) {
			if (game == GAME_SUN || game == GAME_MOON) {
				if (confirmDisp("Save PokeDex flags?")) {
					u8 pkmn[PKMNLENGTH];
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
			break;
		}
		
 		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
			if (currentEntry >= 5) currentEntry -= 5;
		}
		
		if (hidKeysDown() & KEY_DRIGHT) {
			if (currentEntry <= 4) currentEntry += 5;
		}
		
		if (hidKeysDown() & KEY_START) {
			switch (currentEntry) {
				case 0 : {
					char *path[3] = {"romfs:/misc/xd/1.bin", "romfs:/misc/xd/2.bin", "romfs:/misc/xd/3.bin"};
					setBoxBin(mainbuf, game, 3, 30, path);
					infoDisp("XD collection set successfully!");
					break;
				}
				case 1 : {
					char *path[2] = {"romfs:/misc/colosseum/1.bin", "romfs:/misc/colosseum/2.bin"};
					setBoxBin(mainbuf, game, 2, 30, path);
					infoDisp("Colosseum col. set successfully!");
					break;
				}
				case 2 : {
					char *path[1] = {"romfs:/misc/10anni.bin"};
					setBoxBin(mainbuf, game, 1, 30, path);
					infoDisp("10th Anniversary set successfully!");
					break;
				}
				case 3 : {
					char *path[1] = {"romfs:/misc/coll_n.bin"};
					setBoxBin(mainbuf, game, 1, 15, path);
					infoDisp("N's collection injected successfully!");
					break;
				}
				case 4 : {
					char *path[1] = {"romfs:/misc/coll_entreeforest.bin"};
					setBoxBin(mainbuf, game, 1, 30, path);
					infoDisp("Entree Forest col. set successfully!");
					break;
				}
				case 5 : {
					char *path[1] = {"romfs:/misc/coll_dreamradar.bin"};
					setBoxBin(mainbuf, game, 1, 30, path);
					infoDisp("Dream Radar col. set successfully!");
					break;
				}
				case 6 : {
					FILE *fptr = fopen("romfs:/misc/living.bin", "rt");
					fseek(fptr, 0, SEEK_END);
					u32 size = ftell(fptr);
					u8* livingbuf = (u8*)malloc(size);
					memset(livingbuf, 0, size);
					rewind(fptr);
					fread(livingbuf, size, 1, fptr);
					fclose(fptr);
					memcpy(&mainbuf[getPkmnAddress(0, 0, game)], &livingbuf[0], (game < 4) ? 167272 : 186064);
					free(livingbuf);
					infoDisp("Living dex set successfully!");
					break;
				}
				case 7 : {
					char *path[] = {"romfs:/misc/coll_deoxysoblivia.bin"};
					setBoxBin(mainbuf, game, 1, 30, path);
					infoDisp("Deoxys collection set successfully!");
					break;
				}
				case 8 : {
					char *path[] = {"romfs:/misc/coll_ranch.bin"};
					setBoxBin(mainbuf, game, 1, 30, path);
					infoDisp("My Pokemon Ranch set successfully!");
					break;
				}
				case 9 : {
					char *path[] = {"romfs:/misc/coll_oskorea.bin"};
					setBoxBin(mainbuf, game, 1, 30, path);
					infoDisp("Korean collection set successfully!");
					break;
				}
			}
		}
		
		printMassInjector(currentEntry);
	}
}