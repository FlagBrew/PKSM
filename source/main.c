#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"
#include "PID.h"
#include "catch.h"

#define ENTRIES 4

void refresh(int currentEntry) {	
	char *menuEntries[ENTRIES] = {"PID Checker", "Capture probability Calculator", "Worldwide distributions", "Our distributions"};

	printf("\x1b[2;0H");
	for (int i = 0; i < ENTRIES; i++) {
		if (i == currentEntry)
			printf("\x1b[32m%s\x1b[0m\n", menuEntries[i]);
		else 
			printf("%s\n", menuEntries[i]);
	}
}

void intro(PrintConsole topScreen, PrintConsole bottomScreen, int currentEntry){
	consoleSelect(&bottomScreen);
	printf("\nECI Tool is the official Homebrew of theFB community '\x1b[32mPokemon Event Catchers\nItalia\x1b[0m'.\n\nThis is meant to be a general purpose   application who could serve both event\ncollectors and classic players of the\ngame.\n\nYou can join us at:\n\x1b[32mfacebook.com/groups/PokemonEventCatchersItalia\x1b[0m");
	printf("\x1b[26;0HECI Tool");
	printf("\n\nDeveloped by Bernardo Giordano.");
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;30m      Pokemon Event Catchers Italia Tool v1.3     \x1b[0m\n");

	refresh(currentEntry);
	
	printf("\x1b[8;0H-------------====== \x1b[32mWhat's New\x1b[0m ======-------------");
	printf("\x1b[9;0H");
	
	getText("http://eventcatchersitalia.altervista.org/10/info.txt");
	
	printf("\x1b[29;15HPress Start to exit.");	
}

int main() {	
	gfxInitDefault();
	PrintConsole topScreen, bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	int doAction = 0;
	int currentEntry = 0;
	
	consoleSelect(&topScreen);
	intro(topScreen, bottomScreen, currentEntry);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		
		if ((kDown & KEY_DUP) && (currentEntry > 0)) {
			currentEntry--;
			refresh(currentEntry);
		}
		
		if ((kDown & KEY_DDOWN) && (currentEntry < ENTRIES - 1)) {
			currentEntry++;
			refresh(currentEntry);
		}
		
		if (kDown & KEY_A) {
			doAction = currentEntry++;
		}
		
		if (doAction != 0) {
			if (doAction == 1) {
				PID();
				consoleSelect(&bottomScreen);
				printf("\x1b[2J");
				consoleSelect(&topScreen);			
				intro(topScreen, bottomScreen, currentEntry);				
			}
			
			else if (doAction == 2) {
				catchrate();
				consoleSelect(&bottomScreen);
				printf("\x1b[2J");
				consoleSelect(&topScreen);
				intro(topScreen, bottomScreen, currentEntry);
			}
			
			else if (doAction == 3) {
				consoleSelect(&bottomScreen);
				printf("\x1b[2J");
				printf("----------------------------------------");
				printf("NA  - North America");
				printf("\nPAL - Europe, Australia");
				printf("\nJPN - Japan");
				printf("\nKOR - South Korea");
				printf("\nALL - All regions available\n");
				printf("----------------------------------------");
				printf("\x1b[27;0H    Please check your connection....");
				printf("\x1b[29;10HPress A to continue.");
				consoleSelect(&topScreen);		
				printf("\x1b[2J");
				getText("http://eventcatchersitalia.altervista.org/10/worldwide1.txt");
				
				while (aptMainLoop()) {
					gspWaitForVBlank();
					hidScanInput();

					u32 kDown = hidKeysDown();
					if (kDown & KEY_A) 
						break; 			 
				}
				
				printf("\x1b[2J");
				getText("http://eventcatchersitalia.altervista.org/10/worldwide2.txt");
				
				while (aptMainLoop()) {
					gspWaitForVBlank();
					hidScanInput();

					u32 kDown = hidKeysDown();
					if (kDown & KEY_A) 
						break; 			 
				}
				consoleSelect(&bottomScreen);
				printf("\x1b[2J");
				consoleSelect(&topScreen);
				intro(topScreen, bottomScreen, currentEntry);
			}
			
			else if (doAction == 4) {
				consoleSelect(&bottomScreen);
				printf("\x1b[2J");
				printf("----------------------------------------");
				printf("NA  - North America");
				printf("\nPAL - Europe, Australia");
				printf("\nJPN - Japan");
				printf("\nKOR - South Korea");
				printf("\nALL - All regions available\n");
				printf("----------------------------------------");
				printf("\x1b[27;0H    Please check your connection....");
				printf("\x1b[29;10HPress A to continue.");
				consoleSelect(&topScreen);		
				printf("\x1b[2J");
				getText("http://eventcatchersitalia.altervista.org/10/giveaway.txt");
				
				while (aptMainLoop()) {
					gspWaitForVBlank();
					hidScanInput();

					u32 kDown = hidKeysDown();
					if (kDown & KEY_A) 
						break; 			 
				}
				
				consoleSelect(&bottomScreen);
				printf("\x1b[2J");
				consoleSelect(&topScreen);
				intro(topScreen, bottomScreen, currentEntry);
			}
			doAction = 0;
		}
		
		if (kDown & KEY_START) 
			break; 
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}