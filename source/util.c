/*
* This file is part of EventAssistant
* Copyright (C) 2016 Bernardo Giordano
*
* This software is provided 'as-is', 
* without any express or implied warranty. 
* In no event will the authors be held liable for any damages 
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* and to alter it and redistribute it freely, 
* subject to the following restrictions:
*
* 1) The origin of this software must not be misrepresented; 
* 2) You must not claim that you wrote the original software. 
*
* Altered source versions must be plainly marked as such, 
* and must not be misrepresented as being the original software.
*/

#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <string.h>
#include "http.h"

void errDisp(PrintConsole screen, int i) {
	char *errors[] = {"Error!", "Game not found", "OpenContext failed", "AddRequestHeaderField failed", "SSLOpt failed", "BeginRequest failed", "Response code failed", "New header failed", "Redirection failed", "Download size error", "Buffer alloc error", "DownloadData failed", "Eon Ticket N/A in XY", "Switch game also in the app", "Maximum item reached", "File not available", "Selected slot is empty"};
	int top = 12;
	u16 length = strlen(errors[i]);
	u16 left = (40 - length - 2) / 2;
	u16 extra = (length - 4) / 2;
	
	consoleSelect(&screen);
	printf("\x1b[%d;%uH\x1b[47;31m", top, left);
	for (u16 j = 0; j < length + 2; j++)
		printf(" ");
	
	printf("\x1b[%d;%uH", ++top, left);
	for (u16 j = 0; j < extra; j++)
		printf(" ");
	
	printf("%s", errors[0]);
	
	for (u16 j = 0; j < extra; j++)
		printf(" ");
	
	if (extra % 2)
		printf("\x1b[%d;%uH ", top, left + length + 1);
	
	printf("\x1b[%d;%uH", ++top, left);
	for (u16 j = 0; j < length + 2; j++)
		printf(" "); 
	
	printf("\x1b[%d;%uH %s ", ++top, left, errors[i]);
	
	printf("\x1b[%d;%uH", ++top, left);
	for (u16 j = 0; j < length + 2; j++)
		printf(" ");
	
	printf("\x1b[0m");
}

void infoDisp(PrintConsole screen, int i) {
	char *infos[] = {"Injection succeeded", "Edit succeeded", "Saving game...", "Download succeeded"};
	int top = 13; // (29 - 3) / 2
	u16 length = strlen(infos[i]);
	u16 left = (40 - length - 2) / 2;
	
	consoleSelect(&screen);
	printf("\x1b[%d;%uH\x1b[47;32m", top, left);
	for (u16 j = 0; j < length + 2; j++)
		printf(" ");
	
	printf("\x1b[%d;%uH %s ", ++top, left, infos[i]);
	
	printf("\x1b[%d;%uH", ++top, left);
	for (u16 j = 0; j < length + 2; j++)
		printf(" ");
	
	printf("\x1b[0m");
}

void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N) {
	consoleSelect(&topScreen);
	printf("\x1b[2;0H\x1b[30;0m");
	for (int i = 0; i < N; i++) {
		if (i == currentEntry)
			printf("\x1b[47;1;34m* %s\x1b[0m\n", lista[i]);
		else
			printf("\x1b[0m* %s\n", lista[i]);
	}
}

void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page) {
	consoleSelect(&topScreen);
	printf("\x1b[2;0H\x1b[30;0m");
	for (int i = 0; i < N; i++) {
		if (i == currentEntry)
			printf("\x1b[30;32m%s\x1b[0m\n", lista[i + page * N]);
		else
			printf("\x1b[0m%s\n", lista[i + page * N]);
	}
}

void update(PrintConsole topScreen, PrintConsole bottomScreen) {
	char *ciaUrl = "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/EventAssistant/EventAssistant.cia";
	char *ciaPath = "/EventAssistant.cia";

	Result ret = 0;
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");

	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;34m                     Updater                      \x1b[0m\n");

	ret = downloadFile(topScreen, bottomScreen, ciaUrl, ciaPath);
	consoleSelect(&topScreen);
	if (ret == 0) {
		printf("\nDownload of EventAssistant.cia \x1b[32msucceded!\x1b[0m Install  it using a CIA manager.\n\n");
		infoDisp(bottomScreen, 3);
	}
	else {
		printf("\nDownload of EventAssistant.cia \x1b[31mfailed.\x1b[0m\nPlease report the issue to the dev.\n\n");
		errDisp(bottomScreen, 11);
	}

	consoleSelect(&topScreen);
	printf("\x1b[29;15H\x1b[47;34mPress Start to exit.\x1b[0m");

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		if (hidKeysDown() & KEY_START)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
}


u32 CHKOffset(u32 i, int game) {
	if (game == 0 || game == 1) {
		const u32 _xy[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1AC00, 0x1B400, 0x1B600, 0x1B800, 0x1BE00, 0x1C000, 0x1C400, 0x1CC00, 0x1CE00, 0x1D000, 0x1D200, 0x1D400, 0x1D600, 0x1DE00, 0x1E400, 0x1E800, 0x20400, 0x20600, 0x20800, 0x20C00, 0x21000, 0x22C00, 0x23000, 0x23800, 0x23C00, 0x24600, 0x24A00, 0x25200, 0x26000, 0x26200, 0x26400, 0x27200, 0x27A00, 0x5C600, };
		return _xy[i] - 0x5400;
	}

	else if (game == 2 || game == 3) {
		const u32 _oras[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1B600, 0x1BE00, 0x1C000, 0x1C200, 0x1C800, 0x1CA00, 0x1CE00, 0x1D600, 0x1D800, 0x1DA00, 0x1DC00, 0x1DE00, 0x1E000, 0x1E800, 0x1EE00, 0x1F200, 0x20E00, 0x21000, 0x21400, 0x21800, 0x22000, 0x23C00, 0x24000, 0x24800, 0x24C00, 0x25600, 0x25A00, 0x26200, 0x27000, 0x27200, 0x27400, 0x28200, 0x28A00, 0x28E00, 0x30A00, 0x38400, 0x6D000, };
		return _oras[i] - 0x5400;
	}
	else return 0;
}

u32 CHKLength(u32 i, int game) {
	if (game == 0 || game == 1) {
		const u32 _xy[] = { 0x002C8, 0x00B88, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 0x001C0, 0x000BE, 0x00024, 0x02100, 0x00140, 0x00440, 0x00574, 0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x006A0, 0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 0x002F8, 0x01B40, 0x001F4, 0x001F0, 0x00216, 0x00390, 0x01A90, 0x00308, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 0x00078, 0x00200, 0x00C84, 0x00628, 0x34AD0, 0x0E058, };
		return _xy[i];
	}

	else if (game == 2 || game == 3) {
		const u32 _oras[] = { 0x002C8, 0x00B90, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 0x001C0, 0x000BE, 0x00024, 0x02100, 0x00130, 0x00440, 0x00574, 0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x011CC, 0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 0x002F8, 0x01B40, 0x001F4, 0x003E0, 0x00216, 0x00640, 0x01A90, 0x00400, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 0x00078, 0x00200, 0x00C84, 0x00628, 0x00400, 0x07AD0, 0x078B0, 0x34AD0, 0x0E058, };
		return _oras[i];
	}
	else return 0;
}

u16 ccitt16(u8* data, u32 len) {
	u16 crc = 0xFFFF;

	for (u32 i = 0; i < len; i++) {
		crc ^= (u16) (data[i] << 8);

		for (u32 j = 0; j < 0x8; j++) {
			if ((crc & 0x8000) > 0)
				crc = (u16) ((crc << 1) ^ 0x1021);
			else
				crc <<= 1;
		}
	}

	return crc;
}

void rewriteCHK(u8 *mainbuf, int game) {
	u8 blockCount = 0;
	u32 csoff = 0;

	if (game == 2 || game == 3) {
		blockCount = 58;
		csoff = 0x7B21A - 0x5400;
	}

	if (game == 0 || game == 1) {
		blockCount = 55;
		csoff = 0x6A81A - 0x5400;
	}

	u8* tmp = (u8*)malloc(0x35000 * sizeof(u8));
	u16 cs;

	for (u32 i = 0; i < blockCount; i++) {
		memcpy(tmp, mainbuf + CHKOffset(i, game), CHKLength(i, game));
		cs = ccitt16(tmp, CHKLength(i, game));
		memcpy(mainbuf + csoff + i * 8, &cs, 2);
	}

	free(tmp);
}

void credits(PrintConsole topScreen, PrintConsole bottomScreen) {
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                     Credits                      \x1b[0m");

	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("\x1b[29;12H\x1b[47;34mPress B to exit.\x1b[0m");
	consoleSelect(&topScreen);

	printf("\n* smea for ctrulib\n* Kaphotics for PKHeX for wondercard workaround\n* J-D-K for direct save import/export\n* Slashcash for PCHex++ and lots of source code\n* Hamcha for http certs\n* Gocario for algorithms\n* LiquidFenrir for some http structures\n* Nba_Yoh for received flags\n* Simona Mastroianni for database help\n* Federico Leuzzi, YodaDaCoda, SatansRoommate for testing\n* Shai Raba' for the icon\n* all the guys @3dshacks' discord\n\n  Full list available on github repo");

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		if (hidKeysDown() & KEY_B)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
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
    bool ret = false;
    if(isHBL()) { //3DSX
        Result res = FSUSER_OpenArchive(out, ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, ""));

        if(res == 0)
            ret = true;
    }
    else { //CIA version
        //Try cart first
        u32 path[3] = {MEDIATYPE_GAME_CARD, id, id >> 32};
        Result res = FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, path});
        if(res) {
            //Try SD
            u32 path[3] = {MEDIATYPE_SD, id, id >> 32};
            res = FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, path});
            if(res)
                ret = false;
            else
                ret = true;
        }
        else
            ret = true;
    }

    return ret;
}
