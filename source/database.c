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
#include <unistd.h>
#include "database.h"
#include "pokemon.h"
#include "util.h"
#include "fill.h"

#define ENTRIES 4
#define DELAY 30

#define MAXPAGES 75
#define RIGHE 27

char *overwritechar[2] = {"DISABLED", "ENABLED "};
char *adaptchar[2] = {"NO ", "YES"};

int printDB(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int i, int nInjected[], int game, int overwrite[]) {	
	char *language[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
	
    int langCont = 0;
    int adapt = 0;

    consoleSelect(&bottomScreen);
    printf("\x1b[2J");
    printf("----------------------------------------");
	printf("\x1b[32mSELECT\x1b[0m change language");
	printf("\n\x1b[32mX\x1b[0m      overwrite wondercards");
	printf("\n\x1b[32mY\x1b[0m      adapt save to language");
	printf("\n\x1b[31mSTART\x1b[0m  inject wc6 in slot");
	
	printf("\x1b[1;32H%s", language[langCont]);
	printf("\x1b[2;32H%s", overwritechar[overwrite[0]]);
	printf("\x1b[3;32H%s", adaptchar[adapt]);
	printf("\x1b[4;32H%d ", nInjected[0] + 1);	
    printf("\x1b[5;0H----------------------------------------");

    printf("\x1b[21;0H----------------------------------------");
    printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this. This is a new way to inject WC6\nand I need time to perfect it.");
    printf("\x1b[27;0H----------------------------------------");
    printf("\x1b[29;11HPress B to return.");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    printf("\x1b[0;0HScanning server for available languages...");
    printf("\x1b[1;0HLanguages available: ");

    gfxFlushBuffers();
    gfxSwapBuffers();

    char *testpath = (char*)malloc(30 * sizeof(char));
	
    for (int j = 0; j < 7; j++) {
        switch (j) {
            case 0 : {
				snprintf(testpath, 30, "romfs:/wc6/jpn/%d.wc6", i);
				break;
            } 
            case 1 : {
				snprintf(testpath, 30, "romfs:/wc6/eng/%d.wc6", i);
				break;
            }
            case 2 : {
				snprintf(testpath, 30, "romfs:/wc6/fre/%d.wc6", i);
				break;
            }
            case 3 : {
				snprintf(testpath, 30, "romfs:/wc6/ita/%d.wc6", i);
				break;
            }
            case 4 : {
				snprintf(testpath, 30, "romfs:/wc6/ger/%d.wc6", i);
				break;
            }
            case 5 : {
				snprintf(testpath, 30, "romfs:/wc6/spa/%d.wc6", i);
				break;
            }
            case 6 : {
				snprintf(testpath, 30, "romfs:/wc6/kor/%d.wc6", i);
				break;
            }
        }
		FILE* f = fopen(testpath, "r");
		if (f) {
            printf("%s ", language[j]);
		   	fclose(f);
		}

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
	char *descpath = (char*)malloc(30 * sizeof(char));
	snprintf(descpath, 30, "romfs:/database/%d.txt", i);
	
	printf("\n\n");
	printfile(descpath);
	
	free(descpath);
	free(testpath);

    consoleSelect(&bottomScreen);

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        if (hidKeysDown() & KEY_B)
            break;

        if (hidKeysDown() & KEY_Y) {
            if (adapt == 0) 
				adapt = 1;
            else if (adapt == 1) 
				adapt = 0;

			printf("\x1b[3;32H%s", adaptchar[adapt]);
        }

        if (hidKeysDown() & KEY_X) {
            if (overwrite[0] == 0) 
				overwrite[0] = 1;
            else if (overwrite[0] == 1) {
                overwrite[0] = 0;
                nInjected[0] = 0;
            }

			printf("\x1b[2;32H%s", overwritechar[overwrite[0]]);
        }

        if (hidKeysDown() & KEY_SELECT) {
            if (langCont < 6) 
				langCont++;
            else if (langCont == 6) 
				langCont = 0;

			printf("\x1b[1;32H%s", language[langCont]);
        }

        if (hidKeysDown() & KEY_START) {
			if (nInjected[0] >= 24)
				nInjected[0] = 0;

			if (game < 2 && i == 2048)
				return 12;

			char *wc6path = (char*)malloc(30 * sizeof(char));

			switch (langCont) {
				case 0 : {
					snprintf(wc6path, 30, "romfs:/wc6/jpn/%d.wc6", i);
					break;
				} 
				case 1 : {
					snprintf(wc6path, 30, "romfs:/wc6/eng/%d.wc6", i);
					break;
				}
				case 2 : {
					snprintf(wc6path, 30, "romfs:/wc6/fre/%d.wc6", i);
					break;
				}
				case 3 : {
					snprintf(wc6path, 30, "romfs:/wc6/ita/%d.wc6", i);
					break;
				}
				case 4 : {
					snprintf(wc6path, 30, "romfs:/wc6/ger/%d.wc6", i);
					break;
				}
				case 5 : {
					snprintf(wc6path, 30, "romfs:/wc6/spa/%d.wc6", i);
					break;
				}
				case 6 : {
					snprintf(wc6path, 30, "romfs:/wc6/kor/%d.wc6", i);
					break;
				}
			}
			
			FILE *fptr = fopen(wc6path, "rt");
			if (fptr == NULL) 
				return 15;
			fseek(fptr, 0, SEEK_END);
			u32 contentsize = ftell(fptr);
			u8 *wc6buf = (u8*)malloc(contentsize);
			if (wc6buf == NULL) 
				return 8;
			rewind(fptr);
			fread(wc6buf, contentsize, 1, fptr);
			fclose(fptr);

			//finding first free location
			if (overwrite[0] == 0) {
				if (game == 0 || game == 1) {
					for (int t = 0; t < 23; t++)
						if (*(mainbuf + 0x1BD00 + t * 264) == 0x00) {
							nInjected[0] = t;
							break;
						}
				} else if (game == 2 || game == 3) {
					for (int t = 0; t < 23; t++)
						if (*(mainbuf + 0x1CD00 + t * 264) == 0x00) {
							nInjected[0] = t;
							break;
						}
				}
			}

			if (adapt == 1)
				setLanguage(mainbuf, langCont);

			setWC(mainbuf, wc6buf, game, i, nInjected);

			free(wc6path);
			free(wc6buf);
			return 0;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    return -1;
}

void eventDatabase(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game) {
	char *database[RIGHE * (MAXPAGES + 1)];
	
	filldatabase(database);
	
	int t_frame = 1;
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
	printf("\x1b[29;8HTouch or press B to exit");
	
	consoleSelect(&topScreen);		
	printf("\x1b[2J");
	printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
	
	refreshDB(currentEntry, topScreen, database, RIGHE, page);

	consoleSelect(&topScreen);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B || hidKeysDown() & KEY_TOUCH)	
			break;
		
		if ((hidKeysDown() & KEY_R) ^ (hidKeysHeld() & KEY_R && t_frame % DELAY == 1)) {
			if (page < MAXPAGES) page++;
			else if (page == MAXPAGES) page = 0;
			consoleSelect(&topScreen);	
			printf("\x1b[2J");
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}
		
		if ((hidKeysDown() & KEY_L) ^ (hidKeysHeld() & KEY_L && t_frame % DELAY == 1)) {
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
			Result ret = printDB(topScreen, bottomScreen, mainbuf, (currentEntry + page * RIGHE), nInjected, game, overwrite);
			consoleSelect(&bottomScreen);
			printf("\x1b[2J");
			printf("----------------------------------------");
			printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
			printf("\x1b[32mL/R\x1b[0m - Switch page\n");
			printf("\x1b[32mA\x1b[0m - Open/close entry\n");
			printf("----------------------------------------");
			
			if (ret == 0) infoDisp(bottomScreen, (int)ret, BOTTOM);
			if (ret != -1 && ret != 0)
				errDisp(bottomScreen, (int)ret, BOTTOM);

			consoleSelect(&bottomScreen);
			printf("\x1b[29;12HPress B to exit.");			
			consoleSelect(&topScreen);
			printf("\x1b[2J");	
			printf("\x1b[47;30mPage: \x1b[47;34m%d\x1b[47;30m of \x1b[47;34m%d\x1b[47;30m - from \x1b[47;34m%d\x1b[47;30m to \x1b[47;34m%d\x1b[47;30m                      \x1b[0m\x1b[1;0H      ", page + 1, MAXPAGES + 1, page * 27, (page + 1) * 27 - 1);
			refreshDB(currentEntry, topScreen, database, RIGHE, page);
		}

		t_frame++;
		if (t_frame > 5000) 
			t_frame = 1;		
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
}

void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *path, int page) {
    consoleSelect (&bottomScreen);
    printf("\x1b[7;0HPage %d/7 ", page);
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    printfile(path);
}

void psDates(PrintConsole topScreen, PrintConsole bottomScreen) {
	int i = 0;
	char *path = (char*)malloc(100 * sizeof(char));
	snprintf(path, 40, "romfs:/psdates/hacked%d.txt", i + 1);
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("Source:\n\x1b[32m/r/pokemontrades/wiki/hackedevents\x1b[0m\n");
	printf("----------------------------------------");
	printf("\n\x1b[32mL/R\x1b[0m - Switch page");
	printf("\x1b[29;8HTouch or press B to exit");
	
	printPSdates(topScreen, bottomScreen, path, i + 1);

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B || hidKeysDown() & KEY_TOUCH)	
			break;	
		
		if (hidKeysDown() & KEY_R) {
			if (i < 6) i++;
			else if (i == 6) i = 0;
			snprintf(path, 40, "romfs:/psdates/hacked%d.txt", i + 1);
			printPSdates(topScreen, bottomScreen, path, i + 1);
		}

		if (hidKeysDown() & KEY_L) {
			if (i > 0) i--;
			else if (i == 0) i = 6;
			snprintf(path, 40, "romfs:/psdates/hacked%d.txt", i + 1);
			printPSdates(topScreen, bottomScreen, path, i + 1);
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	
	free(path);
}

int massInjecter(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game) {
	int cont = 0;
	char *menuEntries[ENTRIES] = {"XD collection", "Colosseum collection", "10ANNIV collection", "Wolfe Glick Top1 team Worlds2016"};
	
    consoleSelect(&bottomScreen);
    printf("\x1b[2J");
    printf("----------------------------------------");
	printf("\x1b[31mSTART\x1b[0m  inject selected entry");	
    printf("\x1b[2;0H----------------------------------------");

	printf("\x1b[18;0HThis will \x1b[31mOVERWRITE\x1b[0m the first N boxes ofyour pcdata.");
    printf("\x1b[21;0H----------------------------------------");
    printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
    printf("\x1b[26;0H----------------------------------------");
	printf("\x1b[29;8HTouch or press B to exit");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
	printf("\x1b[47;1;34m                  Mass Injecter                   \x1b[0m\n");	
	refresh(cont, topScreen, menuEntries, ENTRIES);

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        if (hidKeysDown() & KEY_B || hidKeysDown() & KEY_TOUCH)
            break;

		if (hidKeysDown() & KEY_DUP) {
			if (cont == 0)
				cont = ENTRIES - 1;
			else if (cont > 0) 
				cont--;

			refresh(cont, topScreen, menuEntries, ENTRIES);
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (cont == ENTRIES - 1)
				cont = 0;
			else if (cont < ENTRIES - 1) 
				cont++;

			refresh(cont, topScreen, menuEntries, ENTRIES);
		}

        if (hidKeysDown() & KEY_START) {
			switch (cont) {
				case 0 : {
					char *path[3] = {"romfs:/misc/xd/1.bin", "romfs:/misc/xd/2.bin", "romfs:/misc/xd/3.bin"};
					
					consoleSelect(&topScreen);
					printf("\x1b[15;%dH\x1b[31mSTART\x1b[0m: \x1b[33m%d\x1b[0m boxes will be replaced", 8, 3);
					while (aptMainLoop()) {
						gspWaitForVBlank();
						hidScanInput();
						
						if (hidKeysDown() & KEY_B)
							return 0;

						if (hidKeysDown() & KEY_START)
							break;

						gfxFlushBuffers();
						gfxSwapBuffers();
					}
					
					int res = setBoxBin(bottomScreen, mainbuf, game, 3, 30, path);
					if (res != 1) 
						return res;
					break;
				}
				case 1 : {
					char *path[2] = {"romfs:/misc/colosseum/1.bin", "romfs:/misc/colosseum/2.bin"};

					consoleSelect(&topScreen);
					printf("\x1b[15;%dH\x1b[31mSTART\x1b[0m: \x1b[33m%d\x1b[0m boxes will be replaced", 8, 2);
					while (aptMainLoop()) {
						gspWaitForVBlank();
						hidScanInput();
						
						if (hidKeysDown() & KEY_B)
							return 0;

						if (hidKeysDown() & KEY_START)
							break;

						gfxFlushBuffers();
						gfxSwapBuffers();
					}					
					
					int res = setBoxBin(bottomScreen, mainbuf, game, 2, 30, path);
					if (res != 1) 
						return res;
					break;
				}
				case 2 : {
					char *path[1] = {"romfs:/misc/10anni.bin"};

					consoleSelect(&topScreen);
					printf("\x1b[15;%dH\x1b[31mSTART\x1b[0m: \x1b[33m%d\x1b[0m box will be replaced", 9, 1);
					while (aptMainLoop()) {
						gspWaitForVBlank();
						hidScanInput();
						
						if (hidKeysDown() & KEY_B)
							return 0;

						if (hidKeysDown() & KEY_START)
							break;

						gfxFlushBuffers();
						gfxSwapBuffers();
					}					
					
					int res = setBoxBin(bottomScreen, mainbuf, game, 1, 30, path);
					if (res != 1) 
						return res;
					break;
				}
			}
			return 1;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    return 0;
}