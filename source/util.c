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
	memset(buf, 0, 1499);
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

int autoupdater() {
	int temp = 0;
	char* ver = (char*)malloc(6 * sizeof(u8));
	snprintf(ver, 6, "%d.%d.%d", V1, V2, V3);

	Result ret = downloadFile("https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/resources/ver.ver", "/3ds/data/PKSM/builds/ver.ver");
	if (ret != 0) {
		free(ver);
		return 1;
	}

	FILE *fptr = fopen("3ds/data/PKSM/builds/ver.ver", "rt");
	if (fptr == NULL) {
		fclose(fptr);
		free(ver);
		return 15;
	}
	fseek(fptr, 0, SEEK_END);
	u32 contentsize = ftell(fptr);
	char *verbuf = (char*)malloc(contentsize);
	if (verbuf == NULL) {
		fclose(fptr);
		free(verbuf);
		free(ver);
		return 8;
	}
	rewind(fptr);
	fread(verbuf, contentsize, 1, fptr);
	fclose(fptr);

	remove("/3ds/data/PKSM/builds/ver.ver");

	for (int i = 0; i < 5; i++)
		if (*(ver + i) == *(verbuf + i))
			temp++;

	free(ver);
	free(verbuf);

	if (temp < 5) {
		update();
		return 1;
	}
	return 0;
}

void update() {
	char *ciaUrl = "https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/PKSM/PKSM.cia";
	char *ciaPath = (char*)malloc(100 * sizeof(char));

	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);

	snprintf(ciaPath, 100, "/3ds/data/PKSM/builds/PKSM_%i-%i-%i-%02i%02i%02i.cia", timeStruct->tm_mday, timeStruct->tm_mon + 1,  timeStruct->tm_year + 1900, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);

	freezeMsg("Downloading latest build...");
	Result ret = downloadFile(ciaUrl, ciaPath);

	if (ret == 0) infoDisp("Download succeeded!");
	else infoDisp("Download failed!");

	free(ciaPath);
	infoDisp("Install the build through FBI!");
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
	
	bool speedy = false;
	
	FILE *bank = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	fseek(bank, 0, SEEK_END);
	u32 size = ftell(bank);
	fclose(bank);
	
	u32 box = size / (30 * PKMNLENGTH);
	u32 boxmax = 1000;

	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);

		if (hidKeysDown() & KEY_B) break;
		
		if (hidKeysDown() & KEY_L)
			speedy = false;

		if (hidKeysDown() & KEY_R)
			speedy = true;
		
		if ((hidKeysDown() & KEY_TOUCH) && !speedy) {
			if (touch.px > 223 && touch.px < 236 && touch.py > 46 && touch.py < 59) {
				if (box > 2) box--;
				else if (box == 2) box = boxmax;
			}
			
			if (touch.px > 240 && touch.px < 253 && touch.py > 46 && touch.py < 59) {
				if (box < boxmax) box++;
				else if (box == boxmax) box = 2;
			}
		}
		
		if ((hidKeysHeld() & KEY_TOUCH) && speedy) {
			if (touch.px > 223 && touch.px < 236 && touch.py > 46 && touch.py < 59) {
				if (box > 2) box--;
				else if (box == 2) box = boxmax;
			}
			
			if (touch.px > 240 && touch.px < 253 && touch.py > 46 && touch.py < 59) {
				if (box < boxmax) box++;
				else if (box == boxmax) box = 2;
			}
		}
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 260 && touch.px < 296 && touch.py > 42 && touch.py < 63) {
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
				infoDisp("Bank size changed!");
			}
			if (touch.px > 260 && touch.px < 296 && touch.py > 69 && touch.py < 90) {
				char *bakpath = (char*)malloc(80 * sizeof(char));
				
				time_t unixTime = time(NULL);
				struct tm* timeStruct = gmtime((const time_t *)&unixTime);		
				snprintf(bakpath, 80, "/3ds/data/PKSM/backup/main_%s_%i%i%i%02i%02i%02i", gamesList[game], timeStruct->tm_mday, timeStruct->tm_mon + 1, timeStruct->tm_year + 1900, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
				
				FILE *f = fopen(bakpath, "wb");
				fwrite(mainbuf, 1, sizeof(&mainbuf), f);
				fclose(f);
				
				free(bakpath);
				
				infoDisp("Save backup created!");
			}
			if (touch.px > 260 && touch.px < 296 && touch.py > 96 && touch.py < 117) {
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
			}
			
			if (touch.px > 298 && touch.px < 320 && touch.py > 207 && touch.py < 225) break;
		}

		printSettings(box, speedy, game);
	}
}