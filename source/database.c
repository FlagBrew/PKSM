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

int getI(char* str, bool is3ds) {
	int i = 0, mult = 1;
	for (int k = is3ds ? 3 : 2; k >= 0; k--) {
		i += (int)(str[k] - '0') * mult;
		mult *= 10;
	}
	return i;
}

void getSinglePathPGF(char* path, const int lang, const int i) {
	sprintf(path, "romfs:/pgf/%s/%d.pgf", tags[lang], i);
}

void getSinglePathPGT(char* path, const int lang, const int i) {
	sprintf(path, "romfs:/pgt/%s/%d.pgt", tags[lang], i);
}

void getSinglePathWCX(char* path, const int lang, const int i) {
	sprintf(path, "romfs:/wcx/%s/%d.wcx", tags[lang], i);
}

void getMultiplePathWCX(char* path, const int lang, const int i, const int j) {
	sprintf(path, "romfs:/wcx/%s/%d-%d.wcx", tags[lang], i, j);
}

void getSinglePathWCXFull(char* path, const int lang, const int i) {
	sprintf(path, "romfs:/wcxfull/%s/%d.wcxfull", tags[lang], i);
}

void getMultiplePathWCXFull(char* path, const int lang, const int i, const int j) {
	sprintf(path, "romfs:/wcxfull/%s/%d-%d.wcxfull", tags[lang], i, j);
}

void reloadPreviewBuf(u8* previewBuf, const int i, const int n) {
	char testpath[40];
	if (game_isgen7())
		getSinglePathWCXFull(testpath, n, i);
	else if (game_isgen6())
		getSinglePathWCX(testpath, n, i);
	
	FILE* f = fopen(testpath, "r");
	if (f) { 
		fseek(f, 0, SEEK_END);
		u32 sz = ftell(f);
		memset(previewBuf, 0, WCX_SIZE);
		rewind(f);
		if (sz == WCXFULL_SIZE) {
			fpos_t pos = 520;
			fsetpos(f, &pos);
		}
		fread(previewBuf, WCX_SIZE, 1, f);
		fclose(f); 
	} else { 
		fclose(f); 
	}	
}

void reloadMultiplePreviewBuf(u8* previewBuf, const int i, const int n, const int j) {
	char testpath[40];
	if (game_isgen7())
		getMultiplePathWCXFull(testpath, n, i, j);
	else if (game_isgen6())
		getMultiplePathWCX(testpath, n, i, j);
	
	FILE* f = fopen(testpath, "r");
	if (f) { 
		fseek(f, 0, SEEK_END);
		u32 sz = ftell(f);
		memset(previewBuf, 0, WCX_SIZE);
		rewind(f);
		if (sz == WCXFULL_SIZE) {
			fpos_t pos = 520;
			fsetpos(f, &pos);
		}
		fread(previewBuf, WCX_SIZE, 1, f);
		fclose(f); 
	} else { 
		fclose(f); 
	}
}

void findFreeLocationWC(u8 *mainbuf, int nInjected[]) {
	nInjected[0] = 0;
	int len = game_isgen6() ? 24 : 48;
	int temp;
	int offset = 0;
	if (game_getisXY())
		offset = 0x1BD00;
	else if (game_getisORAS())
		offset = 0x1CD00;
	else if (game_getisSUMO())
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

int getN(const int i) {
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
		case 223 : return 7;
		case 230 : return 8;
		case 238 : return 2;
		case 244 : return 3;
		case 504 : return 2;
		case 515 : return 2;
		case 551 : return 2;
		case 552 : return 2;
		case 1111 : return 6;
		case 1114 : return 8;
		case 2016 : return 3;		
	}	
	return 0;
}

void eventDatabase7(u8* mainbuf) {
	char *database[SMCOUNT];
	int *spriteArray = (int*)malloc(SMCOUNT * sizeof(int));
	u8 *previewbuf = (u8*)malloc(WCX_SIZE);
	memset(previewbuf, 0, WCX_SIZE);
	
	if (game_isgen7())
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
	
	findFreeLocationWC(mainbuf, nInjected);
	
	while(aptMainLoop()) {
		hidScanInput();
		calcCurrentEntryMorePagesReversed(&currentEntry, &page, fill_get_index()/10, 9, 5);
			
		if (hidKeysDown() & KEY_B)
			break;

		if (hidKeysDown() & KEY_A && spriteArray[page*10+currentEntry] != -1) {
			int total = (game_isgen7()) ? 9 : 7;
			int i = getI(database[page * 10 + currentEntry], true);
			// check for single wcx events
			char testpath[40];
			
			for (int j = 0; j < total; j++) {
				if (game_isgen7())
					getSinglePathWCXFull(testpath, j, i);
				else if (game_isgen6())
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
						if (game_isgen7())
							getMultiplePathWCXFull(testpath, j, i, t + 1);
						else if (game_isgen6())
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

#if PKSV
#else
				if (hidKeysDown() & KEY_Y) {
					if (!socket_init())
						break;
					
					do {
						hidScanInput();
						process_wcx(previewbuf);
						printDB7(previewbuf, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0], true);
					} while (aptMainLoop() && !(hidKeysDown() & KEY_B));
					socket_shutdown();
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
						findFreeLocationWC(mainbuf, nInjected);
					}
					
					if (touch.px > 231 && touch.px < 267 && touch.py > 138 && touch.py < 159) adapt = true;
					if (touch.px > 270 && touch.px < 306 && touch.py > 138 && touch.py < 159) adapt = false;
				}
				
#if PKSV
#else
				if (hidKeysDown() & KEY_START) {
					if (nInjected[0] >= 48) 
						nInjected[0] = 0;
					
					if (game_getisXY() && i == 2048) {
						infoDisp(i18n(S_DATABASE_ITEM_NOT_AVAILABLE_XY));
						break;
					}

					if (!(overwrite))
						findFreeLocationWC(mainbuf, nInjected);

					if (adapt)
						setSaveLanguage(mainbuf, langSelected);

					setWC(mainbuf, previewbuf, i, nInjected);
					
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
#endif
				printDB7(previewbuf, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0], false);
			}
		}
		
		printDatabase6(database, currentEntry, page, spriteArray);
	}
	
	free(spriteArray);
	free(previewbuf);
}

void eventDatabase5(u8* mainbuf) {
	int sz = game_isgen5() ? 170 : 190;
	char *database[sz];
	int *spriteArray = (int*)malloc(sz * sizeof(int));
	
	if (game_isgen5())
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
		calcCurrentEntryMorePagesReversed(&currentEntry, &page, fill_get_index()/10, 9, 5);
		
		if (hidKeysDown() & KEY_B)
			break;
		
		if (hidKeysDown() & KEY_A && spriteArray[page*10+currentEntry] != -1) {
			isSelected = true;
			int i = getI(database[page * 10 + currentEntry], false);

			char testpath[40];
			for (int j = 0; j < 7; j++) {
				if (game_isgen5())
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
#if PKSV
#else
				if (hidKeysDown() & KEY_START) {
					if (nInjected[0] >= (game_isgen5() ? 12 : 8)) 
						nInjected[0] = 0;
					
					char path[40];
					if (game_isgen5())
						getSinglePathPGF(path, langSelected, i);
					else
						getSinglePathPGT(path, langSelected, i);
					
					FILE *fptr = fopen(path, "rt");
					if (fptr == NULL) {
						fclose(fptr);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					fseek(fptr, 0, SEEK_END);
					u32 contentsize = ftell(fptr);
					u8 *buf = (u8*)malloc(contentsize);
					if (buf == NULL) {
						fclose(fptr);
						free(buf);
						infoDisp(i18n(S_DATABASE_ERROR_INJECTION));
						break;
					}
					rewind(fptr);
					fread(buf, contentsize, 1, fptr);
					fclose(fptr);

					setWC(mainbuf, buf, i, nInjected);

					free(buf);					
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
#endif
				printDatabaseListDS(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
			}
			
			isSelected = false;
		}
		
		printDatabaseListDS(database, currentEntry, page, spriteArray, isSelected, langSelected, langVett);
	}
	
	free(spriteArray);
}