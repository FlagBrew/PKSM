#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"
#include "catch.h"
#include "util.h"
#include "database.h"
#include "inject.h"
#include "pokemon.h"

#define ENTRIES 10

#define V1 2
#define V2 1
#define V3 0

void intro(PrintConsole topScreen, PrintConsole bottomScreen, int currentEntry, char* menuEntries[]){
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("\x1b[26;0HEvent Assistant v%d.%d.%d", V1, V2, V3);
	printf("\n\nBernardo Giordano & ctrulib");
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                  EventAssistant                  \x1b[0m\n");

	refresh(currentEntry, topScreen, menuEntries, ENTRIES);

	consoleSelect(&topScreen);
	printf("\x1b[29;10H\x1b[47;34mPress Start to save, B to exit\x1b[0m");
}

int main() {
	gfxInitDefault();
	aptInit();

	PrintConsole topScreen, bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	int game = 0;
	bool save = true;
	
	//X, Y, OR, AS
	const u64 ids[4] = {0x0004000000055D00, 0x0004000000055E00, 0x000400000011C400, 0x000400000011C500};
	char *gamesList[4] = {"Pokemon X", "Pokemon Y", "Pokemon Omega Ruby", "Pokemon Alpha Sapphire"};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[14;5HPress A to continue, B to exit");
	
	consoleSelect(&topScreen);
	printf("\x1b[31mCHOOSE GAME.\x1b[0m Cart has priority over digital copy.");
	refresh(game, topScreen, gamesList, 4);

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B) {
			aptExit();
			gfxExit();
			return 0;
		}
		
		if (hidKeysDown() & KEY_DUP) {
			if (game == 0) {
				game = 3;
				refresh(game, topScreen, gamesList, 4);
			}
			else if (game > 0) {
				game--;
				refresh(game, topScreen, gamesList, 4);	
			}
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (game == 3) {
				game = 0;
				refresh(game, topScreen, gamesList, 4);	
			}
			else if (game < 3) {
				game++;
				refresh(game, topScreen, gamesList, 4);
			}
		}
		
		if (hidKeysDown() & KEY_A)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	
	fsStart();
	FS_Archive saveArch;	
	
	if (!(openSaveArch(&saveArch, ids[game]))) {
		errDisp(bottomScreen, 1);
		aptExit();
		gfxExit();
		return -1;
	}

	//Open main
	Handle mainHandle;
	FSUSER_OpenFile(&mainHandle, saveArch, fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);

	//Get size 
	u64 mainSize;
	FSFILE_GetSize(mainHandle, &mainSize);
	
	switch(game) {
		case 0 : {
			if (mainSize != 415232) {
				errDisp(bottomScreen, 13);
				aptExit();
				gfxExit();
				return -1;
			}
			break;
		}
		case 1 : {
			if (mainSize != 415232) {
				errDisp(bottomScreen, 13);
				aptExit();
				gfxExit();
				return -1;
			}
			break;
		}
		case 2 : {
			if (mainSize != 483328) {
				errDisp(bottomScreen, 13);
				aptExit();
				gfxExit();
				return -1;
			}
			break;
		}
		case 3 : {
			if (mainSize != 483328) {
				errDisp(bottomScreen, 13);
				aptExit();
				gfxExit();
				return -1;
			}
			break;
		}
	}
	
	//allocate mainbuf
	u8* mainbuf = malloc(mainSize);
	
	//Read main 
	FSFILE_Read(mainHandle, NULL, 0, mainbuf, mainSize);	

	char *menuEntries[ENTRIES] = {"Gen VI's Event Database", "Gen VI's Save file editor", "Gen VI's Pokemon editor", "Wi-Fi distributions", "Code distributions", "Local distributions", "Capture probability calculator", "Common PS dates database", "Credits", "Update .cia to latest commit build"};
	int currentEntry = 0;
	
	// initializing save file editor variables
	int nInjected[3] = {0, 0, 0};
	int injectCont[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	// initializing pokemon editor variables
	int pokemonCont[7] = {0, 0, 0, 0, 0, 0, 0};
	/*
		0 : currentEntry
	    1 : boxnumber
		2 : indexnumber
		3 : nature counter
		4 : hidden power counter
		5 : clone boxnumber
		6 : clone indexnumber
	*/

	consoleSelect(&topScreen);
	printf("\x1b[0m");
	intro(topScreen, bottomScreen, currentEntry, menuEntries);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		if (hidKeysDown() & KEY_START)
			break;
		
		if (hidKeysDown() & KEY_B) {
			save = false;
			break;
		}

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
					eventDatabase(topScreen, bottomScreen, mainbuf, game);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					printf("\x1b[2J");
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 1 : {
					int ret = saveFileEditor(topScreen, bottomScreen, mainbuf, game, nInjected, injectCont);
					consoleSelect(&bottomScreen);
					
					if (ret == 1) 
						infoDisp(bottomScreen, 1);
					else if (ret != -1 && ret != 1) 
						errDisp(bottomScreen, ret);

					if (ret != -1) {
						while (aptMainLoop()) {
							gspWaitForVBlank();
							hidScanInput();

							if (hidKeysDown() & KEY_B) break;

							gfxFlushBuffers();
							gfxSwapBuffers();
						}
					}

					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					printf("\x1b[2J");
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}
				
				case 2 : {
					int ret = pokemonEditor(topScreen, bottomScreen, mainbuf, game, pokemonCont);
					consoleSelect(&bottomScreen);
					if (ret == 1)
						infoDisp(bottomScreen, 1);
					else if (ret != 1 && ret != 0) 
						errDisp(bottomScreen, ret);

					if (ret != 0) {
						while (aptMainLoop()) {
							gspWaitForVBlank();
							hidScanInput();

							if (hidKeysDown() & KEY_B) break;

							gfxFlushBuffers();
							gfxSwapBuffers();
						}
					}

					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 3 :  {
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/worldwide1.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 4 : {
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/worldwide2.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 5 : {
					printDistro(topScreen, bottomScreen, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/local.txt");
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 6 : {
					catchrate(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 7 : {
					psDates(topScreen, bottomScreen);
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					consoleSelect(&topScreen);
					intro(topScreen, bottomScreen, currentEntry, menuEntries);
					break;
				}

				case 8 : {
					credits(topScreen, bottomScreen);
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
	
	if (save) {
		infoDisp(bottomScreen, 2);
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		rewriteCHK(mainbuf, game);

		FSFILE_Write(mainHandle, NULL, 0, mainbuf, mainSize, FS_WRITE_FLUSH);
	}
	
	FSFILE_Close(mainHandle);
	
	if (save)
		FSUSER_ControlArchive(saveArch, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
	
	FSUSER_CloseArchive(saveArch);
	
	free(mainbuf);
	fsEnd();

    aptExit();
	gfxExit();
	return 0;
}
