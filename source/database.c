#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "util.h"
#include "http.h"
#include "fill.h"

#define MAXPAGES 75
#define RIGHE 27

void eventDatabase(PrintConsole topScreen, PrintConsole bottomScreen) {
	char *database[RIGHE * (MAXPAGES + 1)];
	char *links[RIGHE * (MAXPAGES + 1)];
	
	filldatabase(database, links);
	
	int currentEntry = 0;
	int page = 0;
	int nInjected[1] = {0};
	int game[1] = {0};
	int overwrite[1] = {1};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
	printf("\x1b[32mL/R\x1b[0m - Switch page\n");
	printf("\x1b[32mA\x1b[0m - Open/close entry\n");
	printf("----------------------------------------");
	printf("\x1b[27;0H    Please check your connection....");
	printf("\x1b[29;12HPress B to exit.");
	
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
			Result ret = printDB(topScreen, bottomScreen, links[currentEntry + page * RIGHE], (currentEntry + page * RIGHE), nInjected, game, overwrite);
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			printf("----------------------------------------");
			printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
			printf("\x1b[32mL/R\x1b[0m - Switch page\n");
			printf("\x1b[32mA\x1b[0m - Open/close entry\n");
			printf("----------------------------------------");
			
			if (ret == 1) printf("\x1b[6;0HFollowing wondercard has been injected:\n\x1b[32m%s\x1b[0m\nin location %d.\nStatus: \x1b[32msucceeded\x1b[0m!", database[currentEntry + page * RIGHE], nInjected[0]);
			else if (ret == -1) printf("\x1b[6;0HAn error occurred during injection.\n\x1b[32m/JKSV/Saves/[game]/EventAssistant/main\x1b[0m\nnot found.");
			else if (ret == -2) printf("\x1b[6;0HAn error occurred during injection.\nError in \x1b[31mhttpcOpenContext\x1b[0m.");
			else if (ret == -3) printf("\x1b[6;0HAn error occurred during injection.\nError in \x1b[31mhttpcAddRequestHeaderField\x1b[0m.");
			else if (ret == -4) printf("\x1b[6;0HAn error occurred during injection.\nError in \x1b[31mhttpcSetSSLOpt\x1b[0m.");
			else if (ret == -5) printf("\x1b[6;0HAn error occurred during injection.\nError in \x1b[31mhttpcBeginRequest\x1b[0m.");
			else if (ret == -6) printf("\x1b[6;0HAn error occurred during injection.\n\x1b[31mWC6 file not available\x1b[0m. Try another\nlanguage.");
			else if (ret == -7) printf("\x1b[6;0HAn error occurred during injection.\nError in \x1b[31mhttpcGetDownloadSizeState\x1b[0m.");
			else if (ret == -8) printf("\x1b[6;0HAn error occurred during injection.\n\x1b[31mFailure to malloc wc6 buffer\x1b[0m.");
			else if (ret == -9) printf("\x1b[6;0HAn error occurred during injection.\nError in \x1b[31mhttpcDownloadData\x1b[0m.");
			else if (ret == -10) printf("\x1b[6;0HAn error occurred during injection.\n\x1b[31mFailure to malloc temp chk var\x1b[0m."); 
			else if (ret == -11) printf("\x1b[6;0HAn error occurred during injection.\n\x1b[31mReached the maximum number of injected\nwc6\x1b[0m.Restart.");
			else if (ret == -12) printf("\x1b[6;0HAn error occurred during injection.\n\x1b[31mEon Ticket is not available on XY\x1b[0m.");

			printf("\x1b[29;12HPress B to exit.");			
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
	printf("\x1b[29;12HPress B to exit.");
	
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
}