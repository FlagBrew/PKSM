/*
* This file is part of PKSM
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
#include <sys/stat.h>
#include <time.h>
#include "spi.h"
#include "http.h"
#include "util.h"
#include "database.h"
#include "editor.h"
#include "graphic.h"
#include "bank.h"
#include "save.h"

#define GAMES 15
		
void exitServices() {
	GUIElementsExit();
	pxiDevExit();
	hidExit();
	srvExit();
	fsEnd();
	sdmcExit();
	aptExit();
	romfsExit();
	sftd_fini();
	sf2d_fini();
}

void initServices() {
	sf2d_init();
	sftd_init();
	sf2d_set_clear_color(BLACK);
	sf2d_set_vblank_wait(1);
	aptInit();
	sdmcInit();
	romfsInit();
	fsStart();
	srvInit();
	hidInit();
	pxiDevInit();
	GUIElementsInit();
	GUIGameElementsInit();

	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/data", 0777);
	mkdir("sdmc:/3ds/data/PKSM", 0777);
	mkdir("sdmc:/3ds/data/PKSM/builds", 0777);
	mkdir("sdmc:/3ds/data/PKSM/bank", 0777);
	mkdir("sdmc:/3ds/data/PKSM/backup", 0777);
	
	loadPersonal();
	
	u32 size = BANKBOXMAX * 30 * PKMNLENGTH;
	u8* bankbuf = (u8*)malloc(size * sizeof(u8));
		
	FILE *bank = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	if (bank == NULL) {
		fclose(bank);
		memset(bankbuf, 0, size);
		FILE *new = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
		fwrite(bankbuf, 1, size, new);
		fclose(new);
	}
	FILE *bak = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	fseek(bak, 0, SEEK_END);
	rewind(bak);
	fread(bankbuf, size, 1, bak);
	fclose(bak);
	FILE *new = fopen("/3ds/data/PKSM/bank/bank.bak", "wb");
	fwrite(bankbuf, 1, size, new);
	fclose(new);
	
	free(bankbuf);
}

int main() {
	initServices();

	u8* mainbuf;
	u64 mainSize = 0;

	int game = 0, GBO = 0, SBO = 0;
	int currentEntry = 0;
	
	bool save = true;
	Handle mainHandle;
	FS_Archive saveArch;
	
	//X, Y, OR, AS
	const u64 ids[6] = {0x0004000000055D00, 0x0004000000055E00, 0x000400000011C400, 0x000400000011C500, 0x0004000000164800, 0x0004000000175E00};
	char *gamesList[GAMES] = {"X", "Y", "OR", "AS", "S", "M", "D", "P", "PL", "HG", "SS", "B", "W", "W2", "B2"};

	while (aptMainLoop() && !(hidKeysDown() & KEY_A && (!(game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM)))) {
		hidScanInput();
		
		if (hidKeysDown() & KEY_B) {
			exitServices();
			return 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
			if (game == 0) game = GAMES - 1;
			else if (game > 0) game--;	
		}
		
		if (hidKeysDown() & KEY_DRIGHT) {
			if (game == GAMES - 1) game = 0;
			else if (game < GAMES - 1) game++;
		}
		
		gameSelectorMenu(game);
	}
	
	GUIGameElementsExit();
	
	if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS || game == GAME_SUN || game == GAME_MOON) {	
		if (!(openSaveArch(&saveArch, ids[game]))) {
			infoDisp("Game not found!");
			exitServices();
			return -1;
		}

		FSUSER_OpenFile(&mainHandle, saveArch, fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);		
		FSFILE_GetSize(mainHandle, &mainSize);
		
		switch(game) {
			case GAME_X : { if (mainSize != 415232) infoDisp("Incorrect size for this game!"); break; }
			case GAME_Y : { if (mainSize != 415232) infoDisp("Incorrect size for this game!"); break; }
			case GAME_OR : { if (mainSize != 483328) infoDisp("Incorrect size for this game!"); break; }
			case GAME_AS : { if (mainSize != 483328) infoDisp("Incorrect size for this game!"); break; }
			case GAME_SUN : { if (mainSize != 441856) infoDisp("Incorrect size for this game!"); break; }
			case GAME_MOON : { if (mainSize != 441856) infoDisp("Incorrect size for this game!"); break; }
			exitServices();
			return -1;
		}
		
		mainbuf = malloc(mainSize);
		FSFILE_Read(mainHandle, NULL, 0, mainbuf, mainSize);
	}
	
	else if (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS || game == GAME_B1 || game == GAME_W1 || game == GAME_B2 || game == GAME_W2) {
		FS_CardType t;
		Result res = FSUSER_GetCardType(&t);
		if (res != 0) {
			infoDisp("No cartridge inserted!");
			exitServices();
			return -1;
		}
		u8 data[0x3B4];
		res = FSUSER_GetLegacyRomHeader(MEDIATYPE_GAME_CARD, 0LL, data);

		CardType cardType_;
		res = SPIGetCardType(&cardType_, (*(data + 12) == 'I') ? 1 : 0);

		mainSize = SPIGetCapacity(cardType_);
		
		switch(game) {
			case GAME_DIAMOND  : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_PEARL    : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_PLATINUM : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_HG : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_SS : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_W1 : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_B1 : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_W2 : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			case GAME_B2 : { if (mainSize != 524288) infoDisp("Incorrect size for this game!"); break; }
			exitServices();
			return -1;
		}
		mainbuf = malloc(mainSize);
		
		TWLstoreSaveFile(mainbuf, cardType_);
		if (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS) {
			GBO = 0x40000 * getActiveGBO(mainbuf, game);
			SBO = 0x40000 * getActiveSBO(mainbuf, game);
		}
	}
	
	char *bakpath = (char*)malloc(80 * sizeof(char));
	
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);		
	snprintf(bakpath, 80, "/3ds/data/PKSM/backup/main_%s_%i-%i-%i-%02i%02i%02i", gamesList[game], timeStruct->tm_mday, timeStruct->tm_mon + 1, timeStruct->tm_year + 1900, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
	
	FILE *f = fopen(bakpath, "wb");
	fwrite(mainbuf, 1, mainSize, f);
	fclose(f);
	
	free(bakpath);
	
	bool touchPressed = false;
	
	GUIElementsSpecify(game);
	if (game < 6) {
		while (aptMainLoop() && !(hidKeysDown() & KEY_START)) {
			hidScanInput();
			touchPosition touch;
			hidTouchRead(&touch);
			
			if (hidKeysDown() & KEY_X) {
				save = false;
				break;
			}

			if (hidKeysDown() & KEY_DUP) {
				if (currentEntry == 0) currentEntry = 2;
				else if (currentEntry > 0) currentEntry--;
			}

			if (hidKeysDown() & KEY_DDOWN) {
				if (currentEntry == 2) currentEntry = 0;
				else if (currentEntry < 2) currentEntry++;
			}
			
			if (hidKeysDown() & KEY_TOUCH) {
				if (touch.px > 50 && touch.px < 270) {
					if (touch.py > 50 && touch.py < 88) { currentEntry = 0; touchPressed = true; }
					if (touch.py > 98 && touch.py < 136) { currentEntry = 1; touchPressed = true; }
					if (touch.py > 146 && touch.py < 184) { currentEntry = 2; touchPressed = true; }
				}
			}

			if ((hidKeysDown() & KEY_A) || touchPressed) {
				touchPressed = false;
				switch (currentEntry) {
					case 0 : {
						int option = 0;
						char* menu[4] = {"EVENT DATABASE", "WI-FI DISTRIBUTIONS", "CODE DISTRIBUTIONS", "LOCAL DISTRIBUTIONS"};
						while (aptMainLoop()) {
							hidScanInput();
							hidTouchRead(&touch);
							
							if (hidKeysDown() & KEY_B) break;
							
							if (hidKeysDown() & KEY_TOUCH) {
								if (touch.px > 50 && touch.px < 270) {
									if (touch.py > 40 && touch.py < 72) { option = 0; touchPressed = true; }
									if (touch.py > 82 && touch.py < 114) { option = 1; touchPressed = true; }
									if (touch.py > 124 && touch.py < 156) { option = 2; touchPressed = true; }
									if (touch.py > 166 && touch.py < 198) { option = 3; touchPressed = true; }
								}
							}
							
							if (hidKeysDown() & KEY_DUP) {
								if (option == 0) option = 3;
								else if (option > 0) option--;
							}

							if (hidKeysDown() & KEY_DDOWN) {
								if (option == 3) option = 0;
								else if (option < 3) option++;
							}
							
							if ((hidKeysDown() & KEY_A) || touchPressed) {
								touchPressed = false;
								switch (option) {
									case 0 : {
										if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS)
											eventDatabase6(mainbuf, game);
										else if (game == GAME_SUN || game == GAME_MOON)
											eventDatabase7(mainbuf, game);
										break;
									}
									case 1 : {
										printDistribution("https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/resources/worldwide1.txt");
										break;
									}
									case 2 : {
										printDistribution("https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/resources/worldwide2.txt");
										break;
									}
									case 3 : {
										printDistribution("https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/resources/local.txt");
										break;
									}
								}
							}
							
							menu4(option, menu, 4);
						}
						
						break;
					}
					case 1 : {
						int option = 0;
						char* menu[4] = {"POKEMON EDITOR", "SAVE EDITOR", "BANK", "MASS INJECTOR"};
						while (aptMainLoop()) {
							hidScanInput();
							hidTouchRead(&touch);

							if (hidKeysDown() & KEY_B) break;
							
							if (hidKeysDown() & KEY_TOUCH) {
								if (touch.px > 50 && touch.px < 270) {
									if (touch.py > 40 && touch.py < 72) { option = 0; touchPressed = true; }
									if (touch.py > 82 && touch.py < 114) { option = 1; touchPressed = true; }
									if (touch.py > 124 && touch.py < 156) { option = 2; touchPressed = true; }
									if (touch.py > 166 && touch.py < 198) { option = 3; touchPressed = true; }
								}
							}
							
							if (hidKeysDown() & KEY_DUP) {
								if (option == 0) option = 3;
								else if (option > 0) option--;
							}

							if (hidKeysDown() & KEY_DDOWN) {
								if (option == 3) option = 0;
								else if (option < 3) option++;
							}
							
							if ((hidKeysDown() & KEY_A) || touchPressed) {
								touchPressed = false;
								switch (option) {
									case 0 : {
										pokemonEditor(mainbuf, game);
										break;
									}
									case 1 : {
										saveFileEditor(mainbuf, game);
										break;
									}
									case 2 : {
										bank(mainbuf, game);
										break;
									}
									case 3 : {
										massInjector(mainbuf, game);
										break;
									}
								}
							}
							
							menu4(option, menu, 4);
						}
						break;
					}
					case 2 : {
						int option = 0;
						char* menu[4] = {"TBA", "POWERSAVES DATES", "CREDITS", "UPDATE"};
						while (aptMainLoop() && !(hidKeysDown() & KEY_B)) {
							hidScanInput();
							hidTouchRead(&touch);
							
							if (hidKeysDown() & KEY_TOUCH) {
								if (touch.px > 50 && touch.px < 270) {
									if (touch.py > 40 && touch.py < 72) { option = 0; touchPressed = true; }
									if (touch.py > 82 && touch.py < 114) { option = 1; touchPressed = true; }
									if (touch.py > 124 && touch.py < 156) { option = 2; touchPressed = true; }
									if (touch.py > 166 && touch.py < 198) { option = 3; touchPressed = true; }
								}
							}
							
							if (hidKeysDown() & KEY_DUP) {
								if (option == 0) option = 3;
								else if (option > 0) option--;
							}

							if (hidKeysDown() & KEY_DDOWN) {
								if (option == 3) option = 0;
								else if (option < 3) option++;
							}
							
							if ((hidKeysDown() & KEY_A) || touchPressed) {
								touchPressed = false;
								switch (option) {
									case 0 : {
										break;
									}
									case 1 : {
										printPSDates();
										break;
									}
									case 2 : {
										printCredits();
										break;
									}
									case 3 : {
										int temp = autoupdater();
										if (temp != 0) {
											exitServices();
											return 0;
										}
										break;
									}
								}
							}
							
							menu4(option, menu, 4);
						}
						
						break;
					}
				}
			}

			mainMenu(currentEntry);
		}
	} else {
		while (aptMainLoop() && !(hidKeysDown() & KEY_START)) {
			hidScanInput();
			touchPosition touch;
			hidTouchRead(&touch);
			
			if (hidKeysDown() & KEY_X) {
				save = false;
				break;
			}

			if (hidKeysDown() & KEY_DUP) {
				if (currentEntry == 0) currentEntry = 1;
				else if (currentEntry > 0) currentEntry--;
			}

			if (hidKeysDown() & KEY_DDOWN) {
				if (currentEntry == 1) currentEntry = 0;
				else if (currentEntry < 1) currentEntry++;
			}
			
			if (hidKeysDown() & KEY_TOUCH) {
				if (touch.px > 97 && touch.px < 222) {
					if (touch.py > 66 && touch.py < 105) { currentEntry = 0; touchPressed = true; }
					if (touch.py > 123 && touch.py < 164) { currentEntry = 1; touchPressed = true; }
				}
			}

			if ((hidKeysDown() & KEY_A) || touchPressed) {
				touchPressed = false;
				switch (currentEntry) {
					case 0 : {
						if (game == GAME_W1 || game == GAME_B1 || game == GAME_W2 || game == GAME_B2)
							eventDatabase5(mainbuf, game);
						if (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS)
							eventDatabase4(mainbuf, game, GBO, SBO);
						break;
					}
					case 1 : {
						break;
					}
				}
			}

			mainMenuDS(currentEntry);
		}
	}
	
	if (save) {
		if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS || game == GAME_SUN || game == GAME_MOON) 
			infoDisp("Save game.");
		else if (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS || game == GAME_B1 || game == GAME_W1 || game == GAME_B2 || game == GAME_W2) 
			infoDisp("Save game. It will take time...");

		if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS || game == GAME_SUN || game == GAME_MOON || game == GAME_B1 || game == GAME_W1 || game == GAME_B2 || game == GAME_W2) 
			rewriteCHK(mainbuf, game);
		else if (game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS) 
			rewriteCHK4(mainbuf, game, GBO, SBO);
	}
	
	if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS || game == GAME_SUN || game == GAME_MOON) {
		FSFILE_Write(mainHandle, NULL, 0, mainbuf, mainSize, FS_WRITE_FLUSH);
		FSFILE_Close(mainHandle);
		if (save)
			FSUSER_ControlArchive(saveArch, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
		FSUSER_CloseArchive(saveArch);
	}
	else if ((game == GAME_DIAMOND || game == GAME_PEARL || game == GAME_PLATINUM || game == GAME_HG || game == GAME_SS || game == GAME_B1 || game == GAME_W1 || game == GAME_B2 || game == GAME_W2) && save)
		TWLinjectSave(mainbuf, mainSize);
	
	free(mainbuf);
	
	if (!(save)) infoDisp("Exit without saving.");
	
	exitServices();
	return 0;
}
