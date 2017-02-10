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
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "editor.h"
#include "http.h"
#include "util.h"
#include "graphic.h"

void checkMaxValue(u8* pkmn, int byteEntry, int value, int max) {
	u8 temp = pkmn[byteEntry];
	pkmn[byteEntry]++;
	if (value > max)
		pkmn[byteEntry] = temp;
}

void loadLines(u8 *src, u8 *dst, u8 strlen,  u32 size) {
	u16 readnum = 3;
	u16 line = 0, chr; 

	while (readnum < size) {
		chr = 0;
		while (readnum < size && src[readnum] != '\n') {
		  dst[line * strlen + chr] = src[readnum];
		  readnum++;
		  chr++;
		}
		dst[line * strlen + chr] = 0;
		readnum++;
		line++;
	}
}

bool checkFile(char* path) {
	FILE *temp = fopen(path, "rt");
	if (temp == NULL) {
		fclose(temp);
		return false;
	}
	fclose(temp);
	return true;
}

void loadPersonal() {
	FILE *fptr = fopen("romfs:/personal/personal.bin", "rt");
	if (fptr == NULL) {
		fclose(fptr);
		return;
	}
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	u8 *buf = (u8*)malloc(size);
	if (buf == NULL) {
		fclose(fptr);
		free(buf);
		return;
	}
	rewind(fptr);
	fread(buf, size, 1, fptr);
	fclose(fptr);
	memcpy(personal.pkmData, buf, size);
	free(buf);
}

void loadFile(u8* buf, char* path) {
	FILE *fptr = fopen(path, "rt");
	if (fptr == NULL)
		return;
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	memset(buf, 0, size);
	rewind(fptr);
	fread(buf, size, 1, fptr);
	fclose(fptr);
}

void injectFromFile(u8* mainbuf, char* path, u32 offset) {
	FILE *fptr = fopen(path, "rt");
	if (fptr == NULL) {
		fclose(fptr);
		return;
	}
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	u8 *buf = (u8*)malloc(size);
	if (buf == NULL) {
		fclose(fptr);
		free(buf);
		return;
	}
	rewind(fptr);
	fread(buf, size, 1, fptr);
	fclose(fptr);

	memcpy(&mainbuf[offset], buf, size);
	free(buf);
}

bool isHBL() {
    u64 id;
    APT_GetProgramID(&id);

    return id != 0x000400000EC10000;
}

void fsStart() {
    if(isHBL()) {
        Handle fsHandle;
        srvGetServiceHandleDirect(&fsHandle, "fs:USER");
        FSUSER_Initialize(fsHandle);
        fsUseSession(fsHandle);
    }
}

void fsEnd() {
    if(isHBL())
        fsEndUseSession();
}

bool openSaveArch(FS_Archive *out, u64 id) {
	if (id == POKEBANK || !isHBL()) { //If we're using Pokebank or CIA
		u32 cardPath[3] = {MEDIATYPE_GAME_CARD, id, id >> 32}; //Card
		if (R_FAILED(FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, cardPath}))) { //If that fails, try digital
			u32 sdPath[3] = {MEDIATYPE_SD, id, id >> 32};
			if (R_FAILED(FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, sdPath})))
				return false;
			else
				return true;
		}
		else
			return true;
	}
	else {
		if (R_SUCCEEDED(FSUSER_OpenArchive(out, ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, ""))))
			return true;
		else
			return false;
	}

	return false;
}

void settingsMenu(u8* mainbuf, int game) {
	char *gamesList[] = {"X", "Y", "OR", "AS", "S", "M", "D", "P", "PL", "HG", "SS", "B", "W", "W2", "B2"};
	
	int speed = 0;
	bool operationDone = false;
	FILE *bank = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	fseek(bank, 0, SEEK_END);
	u32 size = ftell(bank);
	fclose(bank);
	
	u32 box = size / (30 * PKMNLENGTH);
	u32 boxmax = 1000;

	while (aptMainLoop() && !operationDone) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);

		if (hidKeysDown() & KEY_B) break;
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 169 && touch.px < 186 && touch.py > 65 && touch.py < 83) {
				if (box > 2) box--;
				else if (box == 2) box = boxmax;
			}

			if (touch.px > 228 && touch.px < 245 && touch.py > 65 && touch.py < 83) {
				if (box < boxmax) box++;
				else if (box == boxmax) box = 2;
			}
		}
		
		if (hidKeysHeld() & KEY_TOUCH) {
			if (touch.px > 169 && touch.px < 186 && touch.py > 65 && touch.py < 83) {
				if (speed < -30) {
					if (box > 2) box--;
					else if (box == 2) box = boxmax;
				} else
					speed--;
			} else if (touch.px > 228 && touch.px < 245 && touch.py > 65 && touch.py < 83) {
				if (speed > 30) {
					if (box < boxmax) box++;
					else if (box == boxmax) box = 2;
				} else
					speed++;
			} else
				speed = 0;
		} else
			speed = 0;
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 189 && touch.px < 225 && touch.py > 64 && touch.py < 85) {
				freezeMsg("Changing size...");
				if (size < box * 30 * PKMNLENGTH) { // i box sono maggiori
					FILE *buf = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
					fseek(buf, 0, SEEK_END);
					u32 size_temp = ftell(buf);
					u8 *bankbuf = (u8*)malloc(size_temp * sizeof(u8));
					rewind(buf);
					fread(bankbuf, size_temp, 1, buf);
					fclose(buf);
					
					FILE *bak = fopen("/3ds/data/PKSM/bank/bank.bak", "wb");
					fwrite(bankbuf, 1, size_temp, bak);
					fclose(bak);
					
					u8* newbank = (u8*)malloc(box * 30 * PKMNLENGTH);
					memset(newbank, 0, box * 30 * PKMNLENGTH);
					memcpy(newbank, bankbuf, size_temp);
					
					FILE *newbankfile = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
					fwrite(newbank, 1, box * 30 * PKMNLENGTH, newbankfile);
					fclose(newbankfile);
 					
					free(bankbuf);
					free(newbank);					
				}
				else if (size > box * 30 * PKMNLENGTH) { // i box sono minori
					FILE *buf = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
					fseek(buf, 0, SEEK_END);
					u32 size_temp = ftell(buf);
					u8 *bankbuf = (u8*)malloc(size_temp * sizeof(u8));
					rewind(buf);
					fread(bankbuf, size_temp, 1, buf);
					fclose(buf);
					
					FILE *bak = fopen("/3ds/data/PKSM/bank/bank.bak", "wb");
					fwrite(bankbuf, 1, size_temp, bak);
					fclose(bak);
					
					u8* newbank = (u8*)malloc(box * 30 * PKMNLENGTH);
					memset(newbank, 0, box * 30 * PKMNLENGTH);
					memcpy(newbank, bankbuf, box * 30 * PKMNLENGTH);
					
					FILE *newbankfile = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
					fwrite(newbank, 1, box * 30 * PKMNLENGTH, newbankfile);
					fclose(newbankfile);
					
					free(bankbuf);
					free(newbank);					
				}
				operationDone = true;
				infoDisp("Bank size changed!");
			}
			if (touch.px > 60 && touch.px < 260 && touch.py > 100 && touch.py < 140) {
				char *bakpath = (char*)malloc(80 * sizeof(char));
				
				time_t unixTime = time(NULL);
				struct tm* timeStruct = gmtime((const time_t *)&unixTime);		
				snprintf(bakpath, 80, "/3ds/data/PKSM/backup/main_%s_%i%i%i%02i%02i%02i", gamesList[game], timeStruct->tm_mday, timeStruct->tm_mon + 1, timeStruct->tm_year + 1900, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
				
				FILE *f = fopen(bakpath, "wb");
				fwrite(mainbuf, 1, sizeof(&mainbuf), f);
				fclose(f);
				
				free(bakpath);
				operationDone = true;
				infoDisp("Save backup created!");
			}
			if (touch.px > 60 && touch.px < 260 && touch.py > 140 && touch.py < 180) {
				FILE *bak = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
				fseek(bak, 0, SEEK_END);
				size = ftell(bak);
				u8* bankbuf = (u8*)malloc(size * sizeof(u8));
				
				rewind(bak);
				fread(bankbuf, size, 1, bak);
				fclose(bak);
				
				FILE *new = fopen("/3ds/data/PKSM/bank/bank.bak", "wb");
				fwrite(bankbuf, 1, size, new);
				fclose(new);
				
				free(bankbuf);	

				infoDisp("Bank backup created!");
				operationDone = true;
			}
		}
		printSettings(box);
	}
}