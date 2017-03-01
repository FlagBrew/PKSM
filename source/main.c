/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#include "common.h"

#define ASSETS 5

char* path[] =    { "/3ds/data/PKSM/additionalassets/alternative_icons_spritesheetv3.png",
					"/3ds/data/PKSM/additionalassets/balls_spritesheetv2.png",
					"/3ds/data/PKSM/additionalassets/pokemon_icons_spritesheetv3.png",
					"/3ds/data/PKSM/additionalassets/types_sheetv2.png",
					"/3ds/data/PKSM/additionalassets/species_en.txt",
};
char* url[] = { "https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/alternative_icons_spritesheetv3.png",
				"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/balls_spritesheetv2.png",
				"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/pokemon_icons_spritesheetv3.png",
				"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/types_sheetv2.png",
				"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/species_en.txt",
};
		
void exitServices() {
	FXElementsExit();
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

bool initServices() {
	bool isDownloaded = false;
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
	FXElementsInit();
	GUIGameElementsInit();

	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/data", 0777);
	mkdir("sdmc:/3ds/data/PKSM", 0777);
	mkdir("sdmc:/3ds/data/PKSM/bank", 0777);
	mkdir("sdmc:/3ds/data/PKSM/backup", 0777);
	mkdir("sdmc:/3ds/data/PKSM/additionalassets", 0777);
	
	char* str = (char*)malloc(30);
	for (int i = 0; i < ASSETS; i++) {
		FILE *temp1 = fopen(path[i], "rt");
		if (temp1 == NULL) {
			fclose(temp1);
			snprintf(str, 30, "Downloading assets #%d...", i + 1);
			freezeMsg(str);
			downloadFile(url[i], path[i]);
			isDownloaded = true;
		} else
			fclose(temp1);
	}
	free(str);
	
	loadPersonal();
	
    u8 tmp[12000];
	FILE *fptr = fopen("/3ds/data/PKSM/additionalassets/species_en.txt", "rt");
	if (fptr == NULL) {
		fclose(fptr);
		return true;
	}
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	memset(tmp, 0, size);
	rewind(fptr);
	fread(tmp, size, 1, fptr);
	fclose(fptr);
	loadLines(tmp, personal.species[0], 12, size);
	
	u32 defaultSize = 150 * 30 * PKMNLENGTH;
	size = 0;
	u8 *bankbuf, *defaultBank;
	
	FILE *bank = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	if (bank == NULL) {
		freezeMsg("Creating a default bank...");
		fclose(bank);
		
		size = defaultSize;
		defaultBank = (u8*)malloc(size * sizeof(u8));
		memset(defaultBank, 0, size);
		
		FILE *new = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
		fwrite(defaultBank, 1, size, new);
		fclose(new);
		
		free(defaultBank);
	}
	
	FILE *bak = fopen("/3ds/data/PKSM/bank/bank.bak", "rt");
	if (bak) {
		fclose(bak);
		return isDownloaded;
	} else {
		fclose(bak);
		freezeMsg("Backing up bank...");
		FILE *dobak = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
		fseek(dobak, 0, SEEK_END);
		size = ftell(dobak);
		bankbuf = (u8*)malloc(size * sizeof(u8));
		
		rewind(dobak);
		fread(bankbuf, size, 1, dobak);
		fclose(dobak);
		
		FILE *new = fopen("/3ds/data/PKSM/bank/bank.bak", "wb");
		fwrite(bankbuf, 1, size, new);
		fclose(new);
		free(bankbuf);
	}
	return isDownloaded;
}

int main() {
	if (initServices()) {
		infoDisp("Restart the application!");
		exitServices();
		return 0;
	}
	
	for (int i = 0; i < ASSETS; i++) {
		if(!checkFile(path[i])) {
			infoDisp("You're missing a few assets!");
			exitServices();
			return -1;
		}
	}

	u8* mainbuf;
	u64 mainSize = 0;

	int game = 0, GBO = 0, SBO = 0;
	int currentEntry = 0;
	
	bool save = true;
	Handle mainHandle;
	FS_Archive saveArch;
	
	//X, Y, OR, AS, SUN, MOON
	const u64 ids[] = {0x0004000000055D00, 0x0004000000055E00, 0x000400000011C400, 0x000400000011C500, 0x0004000000164800, 0x0004000000175E00};
	char *gamesList[] = {"X", "Y", "OR", "AS", "S", "M", "D", "P", "PL", "HG", "SS", "B", "W", "B2", "W2"};

	while (aptMainLoop() && !(hidKeysDown() & KEY_A)) {
		hidScanInput();
		game = calcCurrentEntryOneScreen(game, 14, 4);
		
		if (hidKeysDown() & KEY_B) {
			exitServices();
			return 0;
		}
		
		gameSelectorMenu(game);
	}
	
	GUIGameElementsExit();
	freezeMsg("Loading save...");
	
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
		if (FSUSER_GetCardType(&t)) {
			infoDisp("No cartridge inserted!");
			exitServices();
			return -1;
		}
		u8 data[0x3B4];
		FSUSER_GetLegacyRomHeader(MEDIATYPE_GAME_CARD, 0LL, data);

		CardType cardType_;
		SPIGetCardType(&cardType_, (*(data + 12) == 'I') ? 1 : 0);

		mainSize = SPIGetCapacity(cardType_);
		
		if (mainSize != 524288) {
			infoDisp("Incorrect size for this game!");
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
	
	char *bakpath = (char*)malloc(100 * sizeof(char));
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);		
	snprintf(bakpath, 100, "sdmc:/3ds/data/PKSM/backup/%s_%02i%02i%02i%02i%02i%02i", gamesList[game], timeStruct->tm_year + 1900, timeStruct->tm_mon + 1, timeStruct->tm_mday, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
	mkdir(bakpath, 0777);
	chdir(bakpath);
	FILE *f = fopen("main", "wb");
	fwrite(mainbuf, 1, mainSize, f);
	fclose(f);
	free(bakpath);
	
	bool touchPressed = false;
	
	GUIElementsSpecify(game);
	if (game < 6) {
		char* mainMenu[3] = {"MANAGEMENT", "EVENTS", "CREDITS"};
		while (aptMainLoop()) {
			hidScanInput();
			touchPosition touch;
			hidTouchRead(&touch);
			currentEntry = calcCurrentEntryOneScreen(currentEntry, 2, 1);
			
			if (hidKeysDown() & KEY_START) {
				if (!confirmDisp("Save changes?"))
					save = false;
				break;
			}
			
			if (hidKeysDown() & KEY_TOUCH) {
				if (touch.px > 60 && touch.px < 260) {
					if (touch.py > 60 && touch.py < 100) { currentEntry = 0; touchPressed = true; }
					if (touch.py > 100 && touch.py < 140) { currentEntry = 1; touchPressed = true; }
					if (touch.py > 140 && touch.py < 180) { currentEntry = 2; touchPressed = true; }
				}
				
				if (touch.px > 292 && touch.px < 314 && touch.py > 194 && touch.py < 216) {
					settingsMenu(mainbuf, game);
					continue;
				}
			}

			if ((hidKeysDown() & KEY_A) || touchPressed) {
				touchPressed = false;
				switch (currentEntry) {
					case 0 : {
						int option = 0;
						char* menu[4] = {"POKEMON EDITOR", "SAVE INFO", "EXTRA STORAGE", "MASS INJECTOR"};
						while (aptMainLoop()) {
							hidScanInput();
							hidTouchRead(&touch);
							option = calcCurrentEntryOneScreen(option, 3, 1);
							
							if (hidKeysDown() & KEY_B)
								break;

							if (hidKeysDown() & KEY_TOUCH) {
								if (touch.px > 60 && touch.px < 260) {
									if (touch.py > 40 && touch.py < 80) { option = 0; touchPressed = true; }
									if (touch.py > 80 && touch.py < 120) { option = 1; touchPressed = true; }
									if (touch.py > 120 && touch.py < 160) { option = 2; touchPressed = true; }
									if (touch.py > 160 && touch.py < 200) { option = 3; touchPressed = true; }
								}
							}
							
							if ((hidKeysDown() & KEY_A) || touchPressed) {
								touchPressed = false;
								switch (option) {
									case 0 :
										pokemonEditor(mainbuf, game);
										break;
									case 1 :
										saveFileEditor(mainbuf, game);
										break;
									case 2 :
										bank(mainbuf, game);
										break;
									case 3 :
										massInjector(mainbuf, game);
										break;
								}
							}
							menu4(option, menu, 4);
						}
						break;
					}
					case 1 : {
						if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS)
							eventDatabase6(mainbuf, game);
						else if (game == GAME_SUN || game == GAME_MOON)
							eventDatabase7(mainbuf, game);
						break;
					}
					case 2 :
						printCredits();					
						break;
				}
			}
			menu3(currentEntry, mainMenu, 3, true);
		}
	} else {
		while (aptMainLoop()) {
			hidScanInput();
			touchPosition touch;
			hidTouchRead(&touch);
			currentEntry = calcCurrentEntryOneScreen(currentEntry, 2, 1);
			
			if (hidKeysDown() & KEY_START) {
				if (!confirmDisp("Save changes?"))
					save = false;
				break;
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
	
	exitServices();
	return 0;
}