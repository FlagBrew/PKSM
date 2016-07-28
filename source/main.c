#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"
#include "PID.h"
#include "catch.h"
#include "util.h"
#include "database.h"

#define ENTRIES 10

#define V1 1
#define V2 8
#define V3 1

void intro(PrintConsole topScreen, PrintConsole bottomScreen, int currentEntry, char* menuEntries[]){
	consoleSelect(&bottomScreen);
	printf("\x1b[26;0HEvent Assistant v%d.%d.%d", V1, V2, V3);
	printf("\n\nBernardo Giordano & ctrulib");
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;34m                 Event  Assistant                 \x1b[0m\n");

	refresh(currentEntry, topScreen, menuEntries, ENTRIES);
	
	consoleSelect(&topScreen);
	printf("\x1b[29;15HPress Start to exit.");
}

int main() {	
	gfxInitDefault();
	PrintConsole topScreen, bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	char *menuEntries[ENTRIES] = {"Gen VI's Event Database", "Wi-Fi distributions", "Code distributions", "Local distributions", "Capture probability calculator", "PID Checker", "Common PS dates database", "Changelog", "FAQ & instructions", "Update"};
	
	int currentEntry = 0;
	
	consoleSelect(&topScreen);
	intro(topScreen, bottomScreen, currentEntry, menuEntries);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_START) 
			break; 
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry == 0) {
				currentEntry = ENTRIES - 1;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
			}
			else if (currentEntry > 0) {
				currentEntry--;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
			}
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry == ENTRIES - 1) {
				currentEntry = 0;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
			}
			else if (currentEntry < ENTRIES - 1) {
				currentEntry++;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
			}
		}
		
		if (hidKeysDown() & KEY_A) {
			switch (currentEntry) {
				case 0 : {
					eventDatabase(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 1 :  {
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/worldwide1.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 2 : {
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/worldwide2.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 3 : {
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/local.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 4 : {
					catchrate(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 5 : {
					PID(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);			
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 6 : {
					psDates(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 7 : {			
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/info.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;					
				}
				
				case 8 : {
					faq(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}	

				case 9 : {
					update(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}				
			}
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}