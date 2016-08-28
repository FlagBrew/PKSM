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
#include "util.h"
#include "http.h"
#include "fill.h"

#define MAXPAGES 75
#define RIGHE 27

void eventDatabase(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game) {
	char *database[RIGHE * (MAXPAGES + 1)];
	char *links[RIGHE * (MAXPAGES + 1)];
	
	filldatabase(database, links);
	
	int currentEntry = 0;
	int page = 0;
	int nInjected[1] = {0};
	int overwrite[1] = {1};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m  Move cursor\n");
	printf("\x1b[32mL/R\x1b[0m Switch page\n");
	printf("\x1b[32mA\x1b[0m   Open/close entry\n");
	printf("----------------------------------------");
	printf("\x1b[29;12H\x1b[47;34mPress B to exit.\x1b[0m");
	
	consoleSelect(&topScreen);		
	printf("\x1b[2J");
	printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
	
	refreshDB(currentEntry, topScreen, database, RIGHE, page);

	consoleSelect(&topScreen);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)	
			break;
		
		if (hidKeysDown() & KEY_R) {
			if (page < MAXPAGES) page++;
			else if (page == MAXPAGES) page = 0;
			consoleSelect(&topScreen);	
			printf("\x1b[2J");
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}
		
		if (hidKeysDown() & KEY_L) {
			if (page > 0) page--;
			else if (page == 0) page = MAXPAGES;
			consoleSelect(&topScreen);	
			printf("\x1b[2J");
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);	
		}
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry == 0) currentEntry = RIGHE - 1;
			else if (currentEntry > 0) currentEntry -= 1;
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry == RIGHE - 1) currentEntry = 0;
			else if (currentEntry < RIGHE - 1) currentEntry += 1;
			refreshDB(currentEntry, topScreen, database, RIGHE, page);			
		}

 		if (hidKeysDown() & KEY_A)  {
			consoleSelect(&topScreen);
			printf("\x1b[2J");
			Result ret = printDB(topScreen, bottomScreen, mainbuf, links[currentEntry + page * RIGHE], (currentEntry + page * RIGHE), nInjected, game, overwrite);
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			printf("----------------------------------------");
			printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
			printf("\x1b[32mL/R\x1b[0m - Switch page\n");
			printf("\x1b[32mA\x1b[0m - Open/close entry\n");
			printf("----------------------------------------");
			
			if (ret == 0) infoDisp(bottomScreen, (int)ret);
			if (ret != -1 && ret != 0)
				errDisp(bottomScreen, (int)ret);

			consoleSelect(&bottomScreen);
			printf("\x1b[29;12H\x1b[47;31mPress B to exit.\x1b[0m");			
			consoleSelect(&topScreen);
			printf("\x1b[2J");	
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}	 
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
}

void psDates(PrintConsole topScreen, PrintConsole bottomScreen) {
	int i = 0;
	char *tmpUrl = (char*)malloc(100 * sizeof(char));
	snprintf(tmpUrl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/hacked%d.txt", i + 1);
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("Source:\n\x1b[32m/r/pokemontrades/wiki/hackedevents\x1b[0m\n");
	printf("----------------------------------------");
	printf("\n\x1b[32mL/R\x1b[0m - Switch page");
	printf("\x1b[29;12H\x1b[47;34mPress B to exit.\x1b[0m");
	
	printPSdates(topScreen, bottomScreen, tmpUrl, i + 1);

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)	
			break;	
		
		if (hidKeysDown() & KEY_R) {
			if (i < 6) i++;
			else if (i == 6) i = 0;
			snprintf(tmpUrl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/hacked%d.txt", i + 1);
			printPSdates(topScreen, bottomScreen, tmpUrl, i + 1);
		}

		if (hidKeysDown() & KEY_L) {
			if (i > 0) i--;
			else if (i == 0) i = 6;
			snprintf(tmpUrl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/hacked%d.txt", i + 1);
			printPSdates(topScreen, bottomScreen, tmpUrl, i + 1);
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	
	free(tmpUrl);
}