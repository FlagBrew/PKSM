#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "util.h"
#include "http.h"
#include "fill.h"

//MAX ENTRIES PER PAGE: 27
#define RIGHE 27
#define MAXPAGES 1

void eventDatabase(PrintConsole topScreen, PrintConsole bottomScreen) {		
	char *database0[RIGHE];
	char *links0[RIGHE];
	char *database1[RIGHE];
	char *links1[RIGHE];
	
	fill(database0, database1, links0, links1);
	
	int currentEntry = 0;
	int page = 0;
	
	// consoleSelect(&bottomScreen);
	// printf("\x1b[2J");
	// printf("----------------------------------------");
	// printf("\x19\x18 - Move cursor\n");
	// printf("L/R - Switch page\n");
	// printf("A - Open/close entry\n");
	// printf("----------------------------------------");
	// printf("\x1b[27;0H    Please check your connection....");
	// printf("\x1b[29;10HPress START to exit.");
	consoleSelect(&topScreen);		
	printf("\x1b[2J");	
	printf("Page: %d", page);
	
	refresh(currentEntry, topScreen, database0, RIGHE);

	consoleSelect(&topScreen);			
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_START)	
			break;
		
		if (kDown & KEY_R) {
			if (page < MAXPAGES) {
				page++;
				consoleSelect(&topScreen);	
				printf("\x1b[2J");
				printf("Page: %d", page);
				if (page == 1) {
					refresh(currentEntry, topScreen, database1, RIGHE);
				}
			}
		}
		
		if (kDown & KEY_L) {
			if (page > 0) {
				page--;
				consoleSelect(&topScreen);	
				printf("\x1b[2J");
				printf("Page: %d", page);
				if (page == 0) {
					refresh(currentEntry, topScreen, database0, RIGHE);
				}
			}
		}
		
		// if (kDown & KEY_DUP) {
			// if (page == 0) {
				// if (currentEntry == 0) {
					// currentEntry = RIGHE - 1;
					// refresh(currentEntry, topScreen, database0, RIGHE);
				// }
				// else if (currentEntry > 0) {
					// currentEntry--;
					// refresh(currentEntry, topScreen, database0, RIGHE);
				// }
			// }
			// if (page == 1) {
				// if (currentEntry == 0) {
					// currentEntry = RIGHE - 1;
					// refresh(currentEntry, topScreen, database1, RIGHE);
				// }
				// else if (currentEntry > 0) {
					// currentEntry--;
					// refresh(currentEntry, topScreen, database1, RIGHE);
				// }
			// }
		// }
		
		// if (kDown & KEY_DDOWN) {
			// if (page == 0) {
				// if (currentEntry == RIGHE - 1) {
					// currentEntry = 0;
					// refresh(currentEntry, topScreen, database0, RIGHE);
				// }
				// else if (currentEntry < RIGHE - 1) {
					// currentEntry++;
					// refresh(currentEntry, topScreen, database0, RIGHE);
				// }
			// }
			// if (page == 1) {
				// if (currentEntry == RIGHE - 1) {
					// currentEntry = 0;
					// refresh(currentEntry, topScreen, database1, RIGHE);
				// }
				// else if (currentEntry < RIGHE - 1) {
					// currentEntry++;
					// refresh(currentEntry, topScreen, database1, RIGHE);
				// }
			// }
		// }

 		// if (kDown & KEY_A)  {
			// consoleSelect(&topScreen);
			// printf("\x1b[2J");
			// if (page == 0)
				// getText(links0[currentEntry]);
			// if (page == 1) 
				// getText(links1[currentEntry]);
		
			// while (aptMainLoop()) {
				// gspWaitForVBlank();
				// hidScanInput();

				// u32 kDown = hidKeysDown();
				
				// if (kDown & KEY_START)
					// break;
			// } 
			// consoleSelect(&topScreen);
			// printf("\x1b[2J");	
			// printf("Page: %d", page);			
			// if (page == 0) 
				// refresh(currentEntry, topScreen, database0, RIGHE);
			
			// if (page == 1)
				// refresh(currentEntry, topScreen, database1, RIGHE);
		// } 
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
}