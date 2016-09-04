/*
* This file is part of EventAssistant
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
#include "http.h"
#include "util.h"

/* Errors
	 1: Game not found
	 2: OpenContext failed
	 3: AddRequestHeaderField failed
	 4: SSLOpt failed
	 5: BeginRequest failed
	 6: Response code failed
	 7: New header failed
	 8: Redirection failed
	 9: Download size error
	 10: Buffer alloc error
	 11: DownloadData failed
	 12: Feature N/A in XY
	 13: Switch game also in the app
	 14: Maximum item reached
	 15: File not available
	 16: Selected slot is empty
 */

void printfile(char* path) {
	FILE* f = fopen(path, "r");
	if (f) {
		char mystring[1000];
		while (fgets(mystring, sizeof(mystring), f)) {
			int a = strlen(mystring);
			if (mystring[a-1] == '\n') {
				mystring[a-1] = 0;
				if (mystring[a-2] == '\r')
					mystring[a-2] = 0;
			}
			puts(mystring);
		}
		fclose(f);
	}
	else printf("No textual preview available yet.");
}

void waitKey(u32 key) {
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		if (hidKeysDown() & key || hidKeysDown() & KEY_TOUCH) 
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
}

void errDisp(PrintConsole screen, int i, u16 columns) {
	char *errors[] = {"Error!", "Game not found", "OpenContext failed", "AddRequestHeaderField failed", "SSLOpt failed", "BeginRequest failed", "Response code failed", "New header failed", "Redirection failed", "Download size error", "Buffer alloc error", "DownloadData failed", "Feature N/A in XY", "Switch game also in the app", "Maximum item reached", "File not available", "Selected slot is empty"};
	int top = 12;
	u16 length = strlen(errors[i]);
	u16 left = (columns - length - 2) / 2;
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

void infoDisp(PrintConsole screen, int i, u16 columns) {
	char *infos[] = {"Injection succeeded", "Edit succeeded", "Saving game...", "Download succeeded", "Ready"};
	int top = 13; 
	u16 length = strlen(infos[i]);
	u16 left = (columns - length - 2) / 2;
	
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
			printf("\x1b[30;32m* %s\x1b[0m\n", lista[i]);
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
	char *ciaPath = (char*)malloc(100 * sizeof(char));
	
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);

	int hours = timeStruct->tm_hour;
	int minutes = timeStruct->tm_min;
	int seconds = timeStruct->tm_sec;
	int day = timeStruct->tm_mday;
	int month = timeStruct->tm_mon + 1;
	int year = timeStruct->tm_year +1900;
		
	snprintf(ciaPath, 100, "/3ds/data/EventAssistant/builds/EventAssistant_%i-%i-%i-%02i%02i%02i.cia", day, month, year, hours, minutes, seconds);

	Result ret = 0;
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");

	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;34m                     Updater                      \x1b[0m\n");
	printf("Downloading file to sdmc:\x1b[32m%s\x1b[0m\n", ciaPath);
	ret = downloadFile(bottomScreen, ciaUrl, ciaPath);
	consoleSelect(&topScreen);
	if (ret == 0) {
		printf("\nDownload of EventAssistant.cia \x1b[32msucceded!\x1b[0m Install  it using a CIA manager.\n\n");
		infoDisp(bottomScreen, 3, BOTTOM);
	}
	else {
		printf("\nDownload of EventAssistant.cia \x1b[31mfailed.\x1b[0m\nPlease report the issue to the dev.\n\n");
		errDisp(bottomScreen, 11, BOTTOM);
	}
	
	free(ciaPath);

	consoleSelect(&topScreen);
	printf("\x1b[29;13HTouch or press B to exit");

	waitKey(KEY_B);
}

void credits(PrintConsole topScreen, PrintConsole bottomScreen) {
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                     Credits                      \x1b[0m");

	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("\x1b[29;8HTouch or press B to exit");
	consoleSelect(&topScreen);

	printf("\n* smea for ctrulib\n* Kaphotics for PKHeX for wondercard workaround\n* J-D-K for direct save import/export\n* Slashcash for PCHex++ and lots of source code\n* MarcusD for romfs support\n* Hamcha for http certs\n* Gocario for algorithms\n* Nba_Yoh for received flags\n* Simona Mastroianni for database help\n* Federico Leuzzi, YodaDaCoda, SatansRoommate,\n  Immersion for testing\n* Shai Raba' for the icon\n* all the guys @3dshacks' discord\n\n  Full list available on github repo");

	waitKey(KEY_B);
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
