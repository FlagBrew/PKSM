#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"
#include "PID.h"
#include "catch.h"

void intro(PrintConsole topScreen, PrintConsole bottomScreen){
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;30m     Pokemon Event Catchers Italia Tool v1.1      \x1b[0m");
	printf("\nA - PID Checker");
	printf("\nB - Catch rate calculator");
	printf("\nX - Worldwide distributions");
	printf("\nY - Our distributions");
	printf("\x1b[29;15HPress Start to exit.");	
	consoleSelect(&bottomScreen);
	printf("\nECI Tool is the official Homebrew of theFB community '\x1b[32mPokemon Event Catchers\nItalia\x1b[0m'.\n\nThis is meant to be a general purpose   application who could serve both event\ncollectors and classic players of the\ngame.\n\nYou can join us at:\n\x1b[32mfacebook.com/groups/PokemonEventCatchersItalia\x1b[0m");
	printf("\x1b[26;0HECI Tool");
	printf("\n\nDeveloped by Bernardo Giordano.");
}

int main() {	
	gfxInitDefault();
	PrintConsole topScreen, bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	consoleSelect(&topScreen);
	intro(topScreen, bottomScreen);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_START) 
			break; 
		
		if (kDown & KEY_A) {
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			printf("----------------------------------------");
			printf("\n\x1A\x1B - Move cursor\n");
			printf("\x18\x19 - Change values (0-9/A-F)\n");
			printf("SELECT - Reset values\n\n");
			printf("----------------------------------------");
			printf("\x1b[19;0H----------------------------------------");
			printf("\x1b[20;0HSometimes Highest IV test \x1b[31mdoesn't work\x1b[0m. This isn't related to the implementationof this software, but with the formulas behind this.\nFormulas can be found at:\n\x1b[32mbulbapedia.bulbagarden.net/wiki/Personality_value\x1b[0m");
			printf("\x1b[27;0H----------------------------------------");
			printf("\x1b[29;10HPress START to exit.");
			consoleSelect(&topScreen);
			PID();
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			consoleSelect(&topScreen);			
			intro(topScreen, bottomScreen);
		}
		
		if (kDown & KEY_X) {
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
			intro(topScreen, bottomScreen);
		}
		
		if (kDown & KEY_Y) {
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
			intro(topScreen, bottomScreen);
		}
		
		if (kDown & KEY_B) {
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			printf("----------------------------------------");
			printf("\n\x1A\x1B - Move cursor\n");
			printf("\x18\x19 - Change values\n");
			printf("SELECT - Reset values\n\n");
			printf("----------------------------------------");
			printf("\x1b[29;10HPress START to exit.");
			consoleSelect(&topScreen);
			catchrate();
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			consoleSelect(&topScreen);
			intro(topScreen, bottomScreen);
		}
					 
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}