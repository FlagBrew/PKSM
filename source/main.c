#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"
#include "PID.h"
#include "catch.h"

void intro(PrintConsole topScreen, PrintConsole bottomScreen){
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;30m     Pokemon Event Catchers Italia Tool v1.2      \x1b[0m");
	printf("\n\x1b[32mA\x1b[0m - PID Checker");
	printf("\n\x1b[32mB\x1b[0m - Capture probability calculator");
	printf("\n\x1b[32mX\x1b[0m - Worldwide distributions");
	printf("\n\x1b[32mY\x1b[0m - Our distributions");
	printf("\n\n\n-------------====== \x1b[32mWhat's New\x1b[0m ======-------------");
	printf("\x1b[9;0H");
	
	getText("http://eventcatchersitalia.altervista.org/10/info.txt");
	
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
			printf("\x1A\x1B - Move cursor\n");
			printf("\x18\x19 - Change values (0-9/A-F)\n");
			printf("SELECT - Reset values\n");
			printf("----------------------------------------");
			printf("\x1b[16;0H----------------------------------------");
			printf("About Characteristic's test:");
			printf("\x1b[19;0HFrom Generation IV onward, Pokemon have a Characteristic which indicates their\nhighest IV. In the case of a tie, the\npersonality value is used to determine  which stat wins the tie.\nThe first stat checked that is tied for highest wins the tie and will determine the Characteristic.");
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
			printf("\x1A\x1B - Move cursor\n");
			printf("\x18\x19 - Change values\n");
			printf("SELECT - Reset values\n");
			printf("----------------------------------------");
			printf("\nSleep, freeze - \x1b[32m2\x1b[0mx (III/IV), \x1b[32m2.5\x1b[0mx (V/VI)");
			printf("Burn, paralysis, poison - \x1b[32m1.5\x1b[0mx");
			printf("\nNo status conditions - \x1b[32m1\x1b[0mx\n\n");
			printf("----------------------------------------");
			printf("\nPoke/Friend/Premier/Luxury/Heal ball- \x1b[32m1\x1b[mx");
			printf("Mega/Safari/Sport ball - \x1b[32m1.5\x1b[0mx");
			printf("\nUltra ball - \x1b[32m2\x1b[0mx");
			printf("\nLure/Net/Repeat ball - \x1b[32m3\x1b[mx (cond. only)");
			printf("\nDive/Dusk ball - \x1b[32m3.5\x1b[0mx (cond. only)");
			printf("\nMoon/Fast/Nest/Timer/Quick ball - \x1b[32m4\x1b[mx");
			printf("\nFirst turn Quick ball - \x1b[32m5\x1b[0mx");
			printf("\nLove/Level ball - \x1b[32m8\x1b[0mx (cond. only)");
			printf("\nMaster ball - \x1b[32m255\x1b[0mx");
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