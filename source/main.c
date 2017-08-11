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

#define ASSETS 11

char* path[] = { 
	"/3ds/data/PKSM/additionalassets/alternative_icons_spritesheetv3.png",
	"/3ds/data/PKSM/additionalassets/balls_spritesheetv2.png",
	"/3ds/data/PKSM/additionalassets/pokemon_icons_spritesheetv3.png",
	"/3ds/data/PKSM/additionalassets/i18n/de/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/en/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/es/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/fr/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/it/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/jp/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/ko/types.png",
	"/3ds/data/PKSM/additionalassets/i18n/zh/types.png",
};

char* url[] = {
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/alternative_icons_spritesheetv3.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/balls_spritesheetv2.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/pokemon_icons_spritesheetv3.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_de.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_en.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_es.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_fr.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_it.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_jp.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_ko.png",
	"https://raw.githubusercontent.com/dsoldier/PKResources/master/additionalassets/i18n/types_zh.png",
};

char* LANG_PREFIX[] = { "jp", "en", "fr", "de", "it", "es", "zh", "ko", "nl", "pt", "ru", "tw" };
		
void exitServices() {
	GUIElementsExit();
	pxiDevExit();
	hidExit();
	srvExit();
	fsEnd();
	i18n_exit();
	sdmcExit();
	aptExit();
	romfsExit();
	screen_exit();
	gfxExit();
	cfguExit();
}

bool initServices() {
	bool isDownloaded = false;
	cfguInit();
	aptInit();
	sdmcInit();
	romfsInit();
	
	mkdir("sdmc:/3ds", 777);
	mkdir("sdmc:/3ds/data", 777);
	mkdir("sdmc:/3ds/data/PKSM", 777);
	mkdir("sdmc:/3ds/data/PKSM/bank", 777);
	mkdir("sdmc:/3ds/data/PKSM/dump", 777);
	mkdir("sdmc:/3ds/data/PKSM/backup", 777);
	mkdir("sdmc:/3ds/data/PKSM/additionalassets", 777);
	mkdir("sdmc:/3ds/data/PKSM/additionalassets/i18n", 777);
	
	char i18npath[80];
	for (unsigned int i = 0; i < 11; i++) {
		snprintf(i18npath, 80, "sdmc:/3ds/data/PKSM/additionalassets/i18n/%s", LANG_PREFIX[i]);
		mkdir(i18npath, 0777);
	}
	
	i18n_init();
	fsStart();
	srvInit();
	hidInit();
	pxiDevInit();

	gfxInitDefault();
	screen_init();
	GUIElementsInit();
	GUIGameElementsInit();

#if CITRA
#else	
	wchar_t str[60];
	for (int i = 0; i < ASSETS; i++) {
		FILE *temp1 = fopen(path[i], "rt");
		if (temp1 == NULL) {
			fclose(temp1);
			swprintf(str, 60, i18n(S_DOWNLOADING_ASSETS), i + 1);
			freezeMsg(str);
			if (downloadFile(url[i], path[i]) != 0) {
				infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
			}
			isDownloaded = true;
		} else
			fclose(temp1);
	}
#endif
	
	loadPersonal();
	
	u32 defaultSize = 150 * 30 * PKMNLENGTH;
	u32 size = 0;
	u8 *bankbuf, *defaultBank;
	
	FILE *bank = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
	if (bank == NULL) {
		freezeMsg(i18n(S_MAIN_CREATING_DEFAULT_BANK));
		fclose(bank);
		
		size = defaultSize;
		defaultBank = (u8*)malloc(size);
		memset(defaultBank, 0, size);
		
		FILE *new = fopen("/3ds/data/PKSM/bank/bank.bin", "wb");
		fwrite(defaultBank, 1, size, new);
		fclose(new);
		
		free(defaultBank);
	}
	
	FILE *bak = fopen("/3ds/data/PKSM/bank/bank.bak", "rt");
	if (bak) {
		fclose(bak);
	} else {
		fclose(bak);
		freezeMsg(i18n(S_MAIN_BACKING_UP_BANK));
		FILE *dobak = fopen("/3ds/data/PKSM/bank/bank.bin", "rt");
		fseek(dobak, 0, SEEK_END);
		size = ftell(dobak);
		bankbuf = (u8*)malloc(size);
		
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
		infoDisp(i18n(S_MAIN_RESTART_APP));
		exitServices();
		return 0;
	}
	
#if CITRA
#else
	for (int i = 0; i < ASSETS; i++) {
		if (!checkFile(path[i])) {
			infoDisp(i18n(S_MAIN_MISSING_ASSETS));
			exitServices();
			return -1;
		}
	}
#endif

	u8* mainbuf;
	u64 mainSize = 0;
	
	int currentEntry = 0;
	bool save = true;
	
#if CITRA
#else
	Handle mainHandle;
	FS_Archive saveArch;
	//X, Y, OR, AS, SUN, MOON
	const u64 ids[] = {0x0004000000055D00, 0x0004000000055E00, 0x000400000011C400, 0x000400000011C500, 0x0004000000164800, 0x0004000000175E00};
#endif

	const char *gamesList[] = {"X", "Y", "OR", "AS", "S", "M", "D", "P", "PL", "HG", "SS", "B", "W", "B2", "W2"};

	while (aptMainLoop() && !(hidKeysDown() & KEY_A)) {
		hidScanInput();
		game_set(calcCurrentEntryOneScreen(game_get(), 14, 4));
		
		if (hidKeysDown() & KEY_B) {
			exitServices();
			return 0;
		}
		
		gameSelectorMenu(game_get());
	}
	
	int game = game_get();
	
	freezeMsg(i18n(S_MAIN_LOADING_SAVE));
#if CITRA
	char savepath[100];
	sprintf(savepath, "romfs:/citra/saves/%s.bin", gamesList[game]);
	FILE *saveptr = fopen(savepath, "rt");
	if (!saveptr) {
		infoDisp(L"Missing file!");
		exitServices();
		return -1;
	}
	fseek(saveptr, 0, SEEK_END);
	mainSize = ftell(saveptr);
	mainbuf = (u8*)malloc(mainSize);
	memset(mainbuf, 0, mainSize);
	rewind(saveptr);
	fread(mainbuf, mainSize, 1, saveptr);
	fclose(saveptr);
	
	if (game_isgen4()) {
		save_set_GBO(mainbuf);
		save_set_SBO(mainbuf);
	}
#else
	if (game_is3DS()) {
		if (!(openSaveArch(&saveArch, ids[game]))) {
			infoDisp(i18n(S_MAIN_GAME_NOT_FOUND));
			exitServices();
			return -1;
		}

		FSUSER_OpenFile(&mainHandle, saveArch, fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);		
		FSFILE_GetSize(mainHandle, &mainSize);
		
		switch(game) {
			case GAME_X : { if (mainSize != 415232)    infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE)); break; }
			case GAME_Y : { if (mainSize != 415232)    infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE)); break; }
			case GAME_OR : { if (mainSize != 483328)   infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE)); break; }
			case GAME_AS : { if (mainSize != 483328)   infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE)); break; }
			case GAME_SUN : { if (mainSize != 441856)  infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE)); break; }
			case GAME_MOON : { if (mainSize != 441856) infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE)); break; }
			exitServices();
			return -1;
		}
		
		mainbuf = malloc(mainSize);
		FSFILE_Read(mainHandle, NULL, 0, mainbuf, mainSize);
	}
	else if (game_isDS()) {
		FS_CardType t;
		if (FSUSER_GetCardType(&t)) {
			infoDisp(i18n(S_MAIN_NO_CARTRIDGE));
			exitServices();
			return -1;
		}
		
		u8 data[0x3B4];
		FSUSER_GetLegacyRomHeader(MEDIATYPE_GAME_CARD, 0LL, data);

		CardType cardType_;
		SPIGetCardType(&cardType_, (*(u8*)(data + 12) == 'I') ? 1 : 0);

		mainSize = SPIGetCapacity(cardType_);
		if (mainSize != 524288) {
			infoDisp(i18n(S_MAIN_INCORRECT_SAVE_SIZE));
			exitServices();
			return -1;
		}
		
		mainbuf = malloc(mainSize);
		
		TWLstoreSaveFile(mainbuf, cardType_);
		if (game_isgen4()) {
			save_set_GBO(mainbuf);
			save_set_SBO(mainbuf);
		}
	}
#endif
	
	char bakpath[100];
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);		
	snprintf(bakpath, 100, "sdmc:/3ds/data/PKSM/backup/%s_%02i%02i%02i%02i%02i%02i", gamesList[game], timeStruct->tm_year + 1900, timeStruct->tm_mon + 1, timeStruct->tm_mday, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
	mkdir(bakpath, 777);
	chdir(bakpath);
	FILE *f = fopen("main", "wb");
	fwrite(mainbuf, 1, mainSize, f);
	fclose(f);
	
	bool touchPressed = false;
	
	GUIElementsSpecify();

	if (game_is3DS()) {
		int mainMenu[] = {S_MAIN_MENU_EXTRA_STORAGE, S_MAIN_MENU_EDITOR, S_MAIN_MENU_EVENTS, S_MAIN_MENU_SAVE_INFO, S_MAIN_MENU_SETTINGS, S_MAIN_MENU_CREDITS};
		while (aptMainLoop()) {
			hidScanInput();
			touchPosition touch;
			hidTouchRead(&touch);
			
			if (hidKeysDown() & KEY_START) {
				if (!confirmDisp(i18n(S_MAIN_Q_SAVE_CHANGES)))
					save = false;
				break;
			}
			
			if (hidKeysDown() & KEY_TOUCH) {
				if (touch.px > 15 && touch.px < 155 && touch.py > 20 && touch.py < 73) {
					bank(mainbuf);
				}

				if (touch.px > 165 && touch.px < 305 && touch.py > 20 && touch.py < 73) {
					pokemonEditor(mainbuf);
				}

				if (touch.px > 15 && touch.px < 155 && touch.py > 83 && touch.py < 136) {
					eventDatabase7(mainbuf);
				}

				if (touch.px > 165 && touch.px < 305 && touch.py > 83 && touch.py < 136) {
					infoDisp(L"Work in progress!");
					//saveFileEditor(mainbuf, mainSize);
				}
				
				if (touch.px > 15 && touch.px < 155 && touch.py > 146 && touch.py < 199) {
					settingsMenu(mainbuf);
					continue;
				}
				
				if (touch.px > 165 && touch.px < 305 && touch.py > 146 && touch.py < 199) {
					printCredits();
				}
			}

			menu(mainMenu);
		}
	} else {
		while (aptMainLoop()) {
			hidScanInput();
			touchPosition touch;
			hidTouchRead(&touch);
			currentEntry = calcCurrentEntryOneScreen(currentEntry, 2, 1);
			
			if (hidKeysDown() & KEY_START) {
				if (!confirmDisp(i18n(S_MAIN_Q_SAVE_CHANGES)))
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
						eventDatabase5(mainbuf);
						break;
					}
					case 1 : {
						infoDisp(L"Work in progress!");
						//saveFileEditor(mainbuf, mainSize);
						break;
					}
				}
			}

			mainMenuDS(currentEntry);
		}
	}
	
	if (save) {
		rewriteCHK(mainbuf);
	}

#if CITRA
#else
	if (game_is3DS()) {
		FSFILE_Write(mainHandle, NULL, 0, mainbuf, mainSize, FS_WRITE_FLUSH);
		FSFILE_Close(mainHandle);
		if (save)
			FSUSER_ControlArchive(saveArch, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
		FSUSER_CloseArchive(saveArch);
	}
	else if (game_isDS() && save)
		TWLinjectSave(mainbuf, mainSize);
#endif

	free(mainbuf);

#if CITRA
#elif ROSALINA_3DSX
#else
	if (!isHBL() && game_isgen7() && confirmDisp(i18n(S_LAUNCH_GAME))) {
		i18n_exit();
		screen_exit();
		
		APT_PrepareToDoApplicationJump(0, ids[game], getLoadedFromCart() ? MEDIATYPE_GAME_CARD : MEDIATYPE_SD);
		u8 hmac[0x20] = {0};
		APT_DoApplicationJump(NULL, 0, hmac);		
	}
#endif
	
	exitServices();
	return 0;
}
