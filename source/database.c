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
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x19\x18 - Move cursor\n");
	printf("L/R - Switch page\n");
	printf("A - Open/close entry\n");
	printf("----------------------------------------");
	printf("\n\nSpecial thanks to:\n\n* Simona Mastroianni\n* Federico Leuzzi\n* Shai Raba'\n* Cosimo Vivoli");
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
		
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_B)	
			break;
		
		if (kDown & KEY_R) {
			if (page < MAXPAGES) page++;
			else if (page == MAXPAGES) page = 0;
			consoleSelect(&topScreen);	
			printf("\x1b[2J");
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}
		
		if (kDown & KEY_L) {
			if (page > 0) page--;
			else if (page == 0) page = MAXPAGES;
			consoleSelect(&topScreen);	
			printf("\x1b[2J");
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);	
		}
		
		if (kDown & KEY_DUP) {
			if (currentEntry == 0) currentEntry = RIGHE - 1;
			else if (currentEntry > 0) currentEntry -= 1;
			
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}
		
		if (kDown & KEY_DDOWN) {
			if (currentEntry == RIGHE - 1) currentEntry = 0;
			else if (currentEntry < RIGHE - 1) currentEntry += 1;
			
			refreshDB(currentEntry, topScreen, database, RIGHE, page);			
		}

 		if (kDown & KEY_A)  {
			consoleSelect(&topScreen);
			printf("\x1b[2J");
			printDistro(topScreen, bottomScreen, links[currentEntry + page * RIGHE]);

			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			printf("----------------------------------------");
			printf("\x19\x18 - Move cursor\n");
			printf("L/R - Switch page\n");
			printf("A - Open/close entry\n");
			printf("----------------------------------------");
			printf("\x1b[27;0H    Please check your connection....");
			printf("\x1b[29;10HPress START to exit.");			
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
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("Source:\n\x1b[32m/r/pokemontrades/wiki/hackedevents\x1b[0m\n");
	printf("----------------------------------------");
	printf("\x1b[29;10HPress A to continue.");	
	
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked1.txt", 1);
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked2.txt", 2);
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked3.txt", 3);
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked4.txt", 4);
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked5.txt", 5);
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked6.txt", 6);
	printPSdates(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/resources/hacked7.txt", 7);
}