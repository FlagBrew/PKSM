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

#include "graphic.h"

static void printSelector(int x, int y, int width, int height);
static void printMenuTop();
static void printMenuBottom();

u32 logoColors[17] = {
	RGBA8(0, 136, 222, 255), 
	RGBA8(203, 39, 69, 255),
	RGBA8(247, 139, 50, 255),
	RGBA8(115, 118, 252, 255),
	RGBA8(241, 148, 45, 255),
	RGBA8(75, 139, 190, 255),
	RGBA8(241, 148, 45, 255),
	RGBA8(75, 139, 190, 255),
	RGBA8(138, 207, 231, 255),
	RGBA8(195, 135, 182, 255),
	RGBA8(196, 170, 76, 255),
	RGBA8(248, 169, 64, 255),
	RGBA8(108, 131, 185, 255),
	RGBA8(227, 236, 243, 255),
	RGBA8(62, 74, 72, 255),
	RGBA8(0, 167, 215, 255),
	RGBA8(223, 105, 95, 255),
};

int logo_lookup6[9] = {0, 64, 121, 183, 250, 320, 376, 451, 527};
int logo_lookup4[6] = {0, 54, 116, 177, 233, 291};
int logo_lookup5[5] = {0, 53, 104, 176, 252};

int lookup[] = {0x0, 0x1, 0x2, 0x4, 0x5, 0x3};

int MAX_LENGTH_BOX_NAME = 15;

AppTextCode gamesList[] = {S_GRAPHIC_GAME_SELECTOR_GAME_X, S_GRAPHIC_GAME_SELECTOR_GAME_Y, S_GRAPHIC_GAME_SELECTOR_GAME_OS, S_GRAPHIC_GAME_SELECTOR_GAME_AS, S_GRAPHIC_GAME_SELECTOR_GAME_SUN, S_GRAPHIC_GAME_SELECTOR_GAME_MOON, S_GRAPHIC_GAME_SELECTOR_GAME_ULTRA_SUN, S_GRAPHIC_GAME_SELECTOR_GAME_ULTRA_MOON, S_GRAPHIC_GAME_SELECTOR_GAME_DIAMOND, S_GRAPHIC_GAME_SELECTOR_GAME_PEARL, S_GRAPHIC_GAME_SELECTOR_GAME_PLATINUM, S_GRAPHIC_GAME_SELECTOR_GAME_HG, S_GRAPHIC_GAME_SELECTOR_GAME_SS, S_GRAPHIC_GAME_SELECTOR_GAME_B, S_GRAPHIC_GAME_SELECTOR_GAME_W, S_GRAPHIC_GAME_SELECTOR_GAME_B2, S_GRAPHIC_GAME_SELECTOR_GAME_W2};
char* langs[] = { "JP", "EN", "FR", "DE", "IT", "ES", "ZH", "KO", "NL", "PT", "RU", "TW", "SD C." };

wchar_t **abilities;
wchar_t **moves;
wchar_t **movesSorted;
wchar_t **natures;

wchar_t **items;
wchar_t **itemsSorted;
wchar_t **hpList;

wchar_t **ballList;
wchar_t **formList;

ArrayUTF32 listAbilities, listNatures, listBalls, listHPs, listForms;

int loadPNGInRAM_current_element = 1;
int loadPNGInRAM_elements = 21;

void GUITextsInit() {
	struct i18n_files listFiles = i18n_getFilesPath();

	listAbilities = i18n_FileToArrayUTF32(listFiles.abilities);
	listMoves = i18n_FileToArrayUTF32(listFiles.moves);
	ArrayUTF32_sort_starting_index(&listMoves, 1);
	listNatures = i18n_FileToArrayUTF32(listFiles.natures);
	listItems = i18n_FileToArrayUTF32(listFiles.items);
	listBalls = i18n_FileToArrayUTF32(listFiles.balls);

	ArrayUTF32_sort_starting_index_with_sort_func(&listItems, 1, ArrayUTF32_sort_cmp_PKMN_Things_List);

	listHPs = i18n_FileToArrayUTF32(listFiles.hp);
	listForms = i18n_FileToArrayUTF32(listFiles.forms);
	listSpecies = i18n_FileToArrayUTF32(listFiles.species);

	abilities = listAbilities.items;
	moves = listMoves.items;
	movesSorted = listMoves.sortedItems;
	natures = listNatures.items;
	items = listItems.items;
	itemsSorted = listItems.sortedItems;
	hpList = listHPs.items;
	formList = listForms.items;
	ballList = listBalls.items;
	natures = listNatures.items;
}

void GUIElementsInit() {
	pp2d_load_texture_png(TEXTURE_WARNING_TOP, "romfs:/res/Warning Top.png");
	pp2d_load_texture_png(TEXTURE_WARNING_BOTTOM, "romfs:/res/Warning Bottom.png");
	GUITextsInit();
}

void loadPNGInRAM(u32 id, const char* filepath) {
	wchar_t str[60];
	swprintf(str, 60, i18n(S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING_DETAILS), loadPNGInRAM_current_element, loadPNGInRAM_elements);
	freezeMsgDetails(str);
	loadPNGInRAM_current_element++;
	pp2d_load_texture_png(id, filepath);
}

void GUIElementsSpecify() {
	freezeMsg(i18n(S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING));

	loadPNGInRAM(TEXTURE_ALTERNATIVE_SPRITESHEET, "/3ds/PKSM/additionalassets/altspritesheetfinal.png");
	loadPNGInRAM(TEXTURE_NORMAL_SPRITESHEET, "/3ds/PKSM/additionalassets/norspritesheetfinal.png");
	loadPNGInRAM(TEXTURE_BALLS_SPRITESHEET, "/3ds/PKSM/additionalassets/balspritesheetfinal.png");

	loadPNGInRAM(SPRITESHEET, "romfs:/res/spritesheet.png");
	loadPNGInRAM(TEXTURE_EVENT_VIEW, "romfs:/res/Event View.png");
	loadPNGInRAM(TEXTURE_BOX_VIEW, "romfs:/res/Box View.png");
	loadPNGInRAM(TEXTURE_HEX_BG, "romfs:/res/Hex BG.png");
	loadPNGInRAM(TEXTURE_GENERATION_BG, "romfs:/res/Generation BG.png");
	loadPNGInRAM(TEXTURE_HIDDEN_POWER_BG, "romfs:/res/Hidden Power BG.png");
	loadPNGInRAM(TEXTURE_BALLS_BG, "romfs:/res/BallsBG.png");
	loadPNGInRAM(TEXTURE_NATURES, "romfs:/res/Natures.png");
	loadPNGInRAM(TEXTURE_MOVES_BOTTOM, "romfs:/res/Moves Bottom.png");
	loadPNGInRAM(TEXTURE_TOP_MOVES, "romfs:/res/Top Moves.png");
	loadPNGInRAM(TEXTURE_EDITOR_STATS, "romfs:/res/Editor Stats.png");
	loadPNGInRAM(TEXTURE_BACKGROUND, "romfs:/res/Background.png");
	loadPNGInRAM(TEXTURE_BANK_TOP, "romfs:/res/Bank Top.png");
	loadPNGInRAM(TEXTURE_CREDITS, "romfs:/res/Credits Top.png");
	loadPNGInRAM(TEXTURE_INFO_VIEW, "romfs:/res/Info View.png");
	loadPNGInRAM(TEXTURE_EDITOR_BOTTOM_BG, "romfs:/res/Editor Bottom BG.png");
	loadPNGInRAM(TEXTURE_HEX_BG, "romfs:/res/Hex BG.png");

	struct i18n_files languageSpecificFiles = i18n_getFilesPath();
	loadPNGInRAM(TEXTURE_TYPES_SPRITESHEET, languageSpecificFiles.types);
}

void GUIGameElementsInit() {
	pp2d_load_texture_png(TEXTURE_LOGOS_GEN5, "romfs:/res/LogosGen5.png");
	pp2d_load_texture_png(TEXTURE_LOGOS_GEN4, "romfs:/res/LogosGen4.png");
	pp2d_load_texture_png(TEXTURE_LOGOS_3DS, "romfs:/res/Logos.png");
}

void GUIElementsExit() {
	i18n_free_ArrayUTF32(&listAbilities);
	i18n_free_ArrayUTF32(&listMoves);
	i18n_free_ArrayUTF32(&listNatures);
	i18n_free_ArrayUTF32(&listItems);
	i18n_free_ArrayUTF32(&listBalls);
	i18n_free_ArrayUTF32(&listHPs);
	i18n_free_ArrayUTF32(&listForms);
	i18n_free_ArrayUTF32(&listSpecies);
}

char messageDebug[255];
bool hasDebugMessage = false;
time_t lastDebugMessageTime = 0;
wchar_t* lastMessage;

void consoleMsg(char* message) {
	strcpy(messageDebug, message);
	hasDebugMessage = true;
}

void drawConsoleDebug() {
	if (hasDebugMessage) {
		time_t currentTime = time(NULL);
		if ((lastDebugMessageTime == 0 || (currentTime - lastDebugMessageTime) < 5)) {
			pp2d_draw_rectangle(0, 0, 400, 20, RGBA8(0, 0, 0, 200));
			pp2d_draw_textf(2, 3, FONT_SIZE_12, FONT_SIZE_12, WHITE, "[Debug] %s", messageDebug);
			if (lastDebugMessageTime == 0) {
				lastDebugMessageTime = currentTime;
			}
		} else {
			hasDebugMessage = false;
			lastDebugMessageTime = 0;
		}
	}
}

static void printSelector(int x, int y, int width, int height) {
	pp2d_draw_rectangle(x, y, width, height, BUTTONBORD);
	pp2d_draw_rectangle(x + 1, y + 1, width - 2, height - 2, BUTTONGREY);
}

static void printMenuTop() {
	pp2d_draw_rectangle(0, 0, 400, 240, MENUBLUE);
	pp2d_draw_rectangle(0, 0, 400, 25, HIGHBLUE);
}

static void printMenuBottom() {
	pp2d_draw_rectangle(0, 0, 320, 240, PALEBLUE);
	pp2d_draw_rectangle(0, 219, 320, 21, MENUBLUE);
}

void infoDisp(wchar_t* message) {
	while (aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_A) break;
		
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
			pp2d_draw_texture(TEXTURE_WARNING_TOP, 0, 0);
			pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, RGBA8(255, 255, 255, giveTransparence()), message);
			pp2d_draw_wtext_center(GFX_TOP, 130, FONT_SIZE_12, FONT_SIZE_12, WHITE, i18n(S_INFORMATION_MESSAGE_PRESS_A));
		
			pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
			pp2d_draw_texture(TEXTURE_WARNING_BOTTOM, 0, 0);
		pp2d_end_draw();
	}
}

int confirmDisp(wchar_t* message) {
	while (aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_A) return 1;
		if (hidKeysDown() & KEY_B) return 0;
		
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
			pp2d_draw_texture(TEXTURE_WARNING_TOP, 0, 0);
			pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, RGBA8(255, 255, 255, giveTransparence()), message);
			pp2d_draw_wtext_center(GFX_TOP, 130, FONT_SIZE_12, FONT_SIZE_12, WHITE, i18n(S_CONFIRMATION_MESSAGE_PRESS_A_OR_B));

			pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
			pp2d_draw_texture(TEXTURE_WARNING_BOTTOM, 0, 0);
		pp2d_end_draw();
	}
	return 0;
}

void _freezeMsgWithDetails(wchar_t* message, wchar_t* details, bool useLastMessage) {
	if (!useLastMessage) {
		free(lastMessage);
		lastMessage = malloc(wcslen(message)*sizeof(wchar_t)+1);
		wcscpy(lastMessage, message);
		lastMessage[wcslen(message)] = '\0';
	}

	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_WARNING_TOP, 0, 0);
		pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, WHITE, lastMessage);
		pp2d_draw_wtext_center(GFX_TOP, 130, FONT_SIZE_12, FONT_SIZE_12, WHITE, details);
	
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_WARNING_BOTTOM, 0, 0);
	pp2d_end_draw();
}

void freezeMsgDetails(wchar_t* details) { _freezeMsgWithDetails(L"", details, true); }
void freezeMsg(wchar_t* message) { _freezeMsgWithDetails(message, i18n(S_FREEZEMSG_DEFAULT_DETAILS), false); }
void freezeMsgWithDetails(wchar_t* message, wchar_t* details) { _freezeMsgWithDetails(message, details, false); }

void progressBar(wchar_t* message, u32 current, u32 sz) {
	wchar_t progress[40];
	swprintf(progress, 40, i18n(S_GRAPHIC_PROGRESSBAR_MESSAGE), current, sz);
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_WARNING_TOP, 0, 0);
		pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, WHITE, message);
		pp2d_draw_wtext_center(GFX_TOP, 130, FONT_SIZE_12, FONT_SIZE_12, WHITE, progress);
	
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_WARNING_BOTTOM, 0, 0);
	pp2d_end_draw();
}

void drawMenuTop() {
	printMenuTop();
	printTitle(L"PKSM");
	pksm_draw_texture(TEXTURE_POKEBALL, 171, 95);
	
	char version[10];
	sprintf(version, "v%d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_MICRO);
	pp2d_draw_text(398 - pp2d_get_text_width(version, FONT_SIZE_9, FONT_SIZE_9), 229, FONT_SIZE_9, FONT_SIZE_9, LIGHTBLUE, version);
}

void printAnimatedBG(bool isUp) {
	pp2d_draw_texture(TEXTURE_BACKGROUND, 0, 0);
	animateBG(isUp);
}

void printTitle(const wchar_t* title) {
	pp2d_draw_wtext_center(GFX_TOP, 4, FONT_SIZE_14, FONT_SIZE_14, BLUE, title);
}

void printBottomIndications(const wchar_t* message) {
	pp2d_draw_wtext_center(GFX_BOTTOM, 225, FONT_SIZE_9, FONT_SIZE_9, LIGHTBLUE, message);
}

void gameSelectorMenu(int n) {
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		printMenuTop();
		pp2d_draw_wtext_center(GFX_TOP, 6, FONT_SIZE_9, FONT_SIZE_9, BLUE, i18n(S_GRAPHIC_GAME_SELECTOR_INFO_CART_HAS_PRIO));
		
		for (int i = 0; i < 4; i++) {
			if (n == i) {
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_3DS, 44 + i*85 + 4, 55 + 4, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70, HIGHBLUE);
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_3DS, 44 + i*85, 55, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70, logoColors[i]);
			} else
				pp2d_draw_texture_part(TEXTURE_LOGOS_3DS, 44 + i*85, 55, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70);
		}
		
		for (int i = 4; i < 8; i++) {
			if (n == i) {
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_3DS, 60 + (i - 4)*70 + 4, 120 + 4, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70, HIGHBLUE);
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_3DS, 60 + (i - 4)*70, 120, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70, logoColors[i]);
			} else
				pp2d_draw_texture_part(TEXTURE_LOGOS_3DS, 60 + (i - 4)*70, 120, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70);
		}

		pp2d_draw_wtext_center(GFX_TOP, 200, FONT_SIZE_18, FONT_SIZE_18, logoColors[n], i18n(gamesList[n]));
	
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		
		for (int i = 8; i < 13; i++) {
			if (n == i) {
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_GEN4, 26 + (i - 8)*56 + 4, 40 + 4, logo_lookup4[i - 8], 0, logo_lookup4[i - 7] - logo_lookup4[i - 8] - 1, 56, HIGHBLUE);
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_GEN4, 26 + (i - 8)*56, 40, logo_lookup4[i - 8], 0, logo_lookup4[i - 7] - logo_lookup4[i - 8] - 1, 56, logoColors[i]);
			} else
				pp2d_draw_texture_part(TEXTURE_LOGOS_GEN4, 26 + (i - 8)*56, 40, logo_lookup4[i - 8], 0, logo_lookup4[i - 7] - logo_lookup4[i - 8] - 1, 56);
		}
		
		for (int i = 13; i < 17; i++) {
			if (n == i) {
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_GEN5, 40 + (i - 13)*60 + 4, 115 + 4, logo_lookup5[i - 13], 0, logo_lookup5[i - 12] - logo_lookup5[i - 13] - 1, 60, HIGHBLUE);
				pp2d_draw_texture_part_blend(TEXTURE_LOGOS_GEN5, 40 + (i - 13)*60, 115, logo_lookup5[i - 13], 0, logo_lookup5[i - 12] - logo_lookup5[i - 13] - 1, 60, logoColors[i]);
			} else
				pp2d_draw_texture_part(TEXTURE_LOGOS_GEN5, 40 + (i - 13)*60, 115, logo_lookup5[i - 13], 0, logo_lookup5[i - 12] - logo_lookup5[i - 13] - 1, 60);
		}

		printBottomIndications(i18n(S_GRAPHIC_GAME_SELECTOR_INDICATIONS));
	pp2d_end_draw();
}

void menu(int menu[]) {
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		drawMenuTop();

		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();		
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				pksm_draw_texture(TEXTURE_MAIN_MENU_BUTTON, 15 + j*150, 20 + i*63);
				pp2d_draw_wtext(15 + j*150 + 52 + (84 - pp2d_get_wtext_width(i18n(menu[i*2+j]), FONT_SIZE_15, FONT_SIZE_15)) / 2, 17 + i*63 + 19, FONT_SIZE_15, FONT_SIZE_15, LIGHTBLUE, i18n(menu[i*2+j]));
				switch (i*2+j) {
					case 0: 
						pksm_draw_texture(TEXTURE_ICON_BANK, 25, 27);
						break;
					case 1: 
						pksm_draw_texture(TEXTURE_ICON_EDITOR, 175, 28);
						break;
					case 2: 
						pksm_draw_texture(TEXTURE_ICON_EVENTS, 25, 93);
						break;
					case 3: 
						pksm_draw_texture(TEXTURE_ICON_SAVE, 175, 91);
						break;
					case 4: 
						pksm_draw_texture(TEXTURE_ICON_SETTINGS, 25, 157);
						break;
					case 5: 
						pksm_draw_texture(TEXTURE_ICON_CREDITS, 175, 160);
						break;
				}
			}
		}
		printBottomIndications(i18n(S_MAIN_MENU_INDICATION_EXIT));
	pp2d_end_draw();
}

void printCredits() {
	static const char* credits = "Naxann and Anty-Lemon for various contributions\nKaphotics and SciresM for PKHeX and memecrypto\nJ-K-D for direct save import/export\nAstronautlevel for QR code support\nArchitDate for serveLegality\nSlownic and zaksabeast for servepkx\nSlashcash for PCHex++\nTuxSH for TWLSaveTool\nProjectPokemon.org for most of the wondercards\nSimona for being my best supporter\nAll the countless translators who worked on the localization\nAll the contributors on Github\nEveryone supporting the development\n\nwww.github.com/BernardoGiordano/PKSM";
	
	while (aptMainLoop() && !(hidKeysUp() & KEY_B)) {
		hidScanInput();
		
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
			printMenuTop();
			printTitle(i18n(S_GRAPHIC_CREDITS_TITLE));
			pp2d_draw_texture(TEXTURE_CREDITS, 0, 45);
			pp2d_draw_text(18, 79, FONT_SIZE_15, FONT_SIZE_15, LIGHTBLUE, "Bernardo Giordano");
			pp2d_draw_text(64, 174, FONT_SIZE_15, FONT_SIZE_15,  LIGHTBLUE, "dsoldier for the complete GUI design");

			pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
			printMenuBottom();
			pp2d_draw_text(20, 30, FONT_SIZE_9, FONT_SIZE_9, LIGHTBLUE, credits);
			printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
		pp2d_end_draw();
	}
}

void printEventList(char *database[], int currentEntry, int page, int spriteArray[]) {
	int pk, y = 41;
	char pages[24]; // 24 is way too much but kills a gcc warning
	sprintf(pages, "%d/%d", page + 1, fill_get_index()/10);
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		printMenuTop();
		printTitle(i18n(S_GRAPHIC_DB6_TITLE));
		
		for (int i = 0; i < 5; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				pksm_draw_texture(TEXTURE_EVENT_MENU_TOP_BAR_SELECTED, 18, y);
			else
				pksm_draw_texture(TEXTURE_EVENT_MENU_TOP_BAR_NORMAL, 18, y);
			
			if (pk != -1) {
				pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0), 34 * (pk % 30), 30 * (pk / 30), 34, 30);
			}
			
			if (pp2d_get_text_width(database[page*10 + i], FONT_SIZE_9, FONT_SIZE_9) <= 140)
				pp2d_draw_text(54, y + 12, FONT_SIZE_9, FONT_SIZE_9, (i == currentEntry) ? HIGHBLUE : YELLOW, database[page * 10 + i]);
			else {
				pp2d_draw_text_wrap(54, y + 3, FONT_SIZE_9, FONT_SIZE_9, (i == currentEntry) ? HIGHBLUE : YELLOW, 140, database[page * 10 + i]);
			}
			y += 37;
		}

		y = 41;
		for (int i = 5; i < 10; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				pksm_draw_texture(TEXTURE_EVENT_MENU_TOP_BAR_SELECTED, 200, y);
			else
				pksm_draw_texture(TEXTURE_EVENT_MENU_TOP_BAR_NORMAL, 200, y);
			
			if (pk != -1) {
				pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, 202, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0), 34 * (pk % 30), 30 * (pk / 30), 34, 30);
			}
			
			if (pp2d_get_text_width(database[page*10 + i], FONT_SIZE_9, FONT_SIZE_9) <= 140)
				pp2d_draw_text(235, y + 14, FONT_SIZE_9, FONT_SIZE_9, (i == currentEntry) ? HIGHBLUE : YELLOW, database[page * 10 + i]);
			else {
				pp2d_draw_text_wrap(235, y + 3, FONT_SIZE_9, FONT_SIZE_9, (i == currentEntry) ? HIGHBLUE : YELLOW, 140, database[page * 10 + i]);
			}
			y += 37;
		}
		
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		pksm_draw_texture(TEXTURE_EVENT_MENU_BOTTOM_BAR, 65, 45);
		pksm_draw_texture(TEXTURE_L_BUTTON, 83, 52);
		pksm_draw_texture(TEXTURE_R_BUTTON, 221, 52);
		pp2d_draw_text_center(GFX_BOTTOM, 52, FONT_SIZE_12, FONT_SIZE_12, WHITE, pages);
		printBottomIndications(i18n(S_GRAPHIC_DB6_INDICATIONS));
	pp2d_end_draw();
}

void printEventInjector(u8* previewbuf, int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected, bool ota) {
	static const char *languages[] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR", "CHS", "CHT"};
	char cont[3];
	snprintf(cont, 3, "%d", nInjected + 1);
	
	const int total = game_isgen7() ? 9 : 7;
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		printAnimatedBG(true);
		
		if (game_is3DS())
		{
			pksm_draw_texture(TEXTURE_OTA_BUTTON, 360, 2);
		}
		
		if (sprite != -1 && (game_is3DS() || game_isgen5()))
		{
			wcxInfoViewer(previewbuf);
			pp2d_draw_texture_part_scale(TEXTURE_NORMAL_SPRITESHEET, 282, 41 - movementOffsetLong(6), 34 * (wc_get_species(previewbuf) % 30), 30 * (wc_get_species(previewbuf) / 30), 34, 30, 2, 2);
		}
		else
		{
			pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, BLACK, L"Detailed infos not available");
			pp2d_draw_wtext_center(GFX_TOP, 115, FONT_SIZE_15, FONT_SIZE_15, BLACK, L"for this generation. Sorry!");
		}
		
		if (ota)
		{
			pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, 220));
			pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, RGBA8(255, 255, 255, giveTransparence()), i18n(S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT));
			pp2d_draw_wtext_center(GFX_TOP, 130, FONT_SIZE_12, FONT_SIZE_12, WHITE, i18n(S_GRAPHIC_PKVIEWER_OTA_INDICATIONS));
		}

		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();

		if (getN(i) > 1)
		{
			pp2d_draw_text_center(GFX_BOTTOM, 18, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, "\uE004+\uE005 \uE01E. L/R to switch multiple wondercards.");
		}
		else
		{
			pp2d_draw_text_center(GFX_BOTTOM, 18, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, "\uE004+\uE005 \uE01E");
		}
		
		pp2d_draw_wtext(16, 50, FONT_SIZE_14, FONT_SIZE_14, LIGHTBLUE, i18n(S_GRAPHIC_DB_LANGUAGES));
		
		for (int t = 0; t < total; t++)
		{
			int x = 0, y = 0;
			switch (t)
			{
				case 0 : { x = 114; y = 50; break; }
				case 1 : { x = 153; y = 50; break; }
				case 2 : { x = 192; y = 50; break; }
				case 3 : { x = 231; y = 50; break; }
				case 4 : { x = 270; y = 50; break; }
				case 5 : { x = 153; y = 74; break; }
				case 6 : { x = 192; y = 74; break; }
				case 7 : { x = 231; y = 74; break; }
				case 8 : { x = 270; y = 74; break; }
			}
			if (langVett[t])
			{
				pksm_draw_texture(t == langSelected ? TEXTURE_RED_BUTTON : TEXTURE_DARK_BUTTON, x, y);
				pp2d_draw_text(x + (36 - pp2d_get_text_width(languages[t], FONT_SIZE_14, FONT_SIZE_14)) / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, t == langSelected ? DARKBLUE : YELLOW, languages[t]);
			}
			else
			{
				pksm_draw_texture(TEXTURE_LIGHT_BUTTON, x, y);
				pp2d_draw_text(x + (36 - pp2d_get_text_width(languages[t], FONT_SIZE_14, FONT_SIZE_14)) / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, DARKBLUE, languages[t]);
			}
		}

		if (game_is3DS())
		{
			pksm_draw_texture(overwrite ? TEXTURE_RED_BUTTON : TEXTURE_DARK_BUTTON, 231, 110);
			pksm_draw_texture(overwrite ? TEXTURE_DARK_BUTTON : TEXTURE_RED_BUTTON, 270, 110);
			pksm_draw_texture(adapt ? TEXTURE_RED_BUTTON : TEXTURE_DARK_BUTTON, 231, 138);
			pksm_draw_texture(adapt ? TEXTURE_DARK_BUTTON : TEXTURE_RED_BUTTON, 270, 138);
			pksm_draw_texture(TEXTURE_DARK_BUTTON, 251, 168);	
			
			pp2d_draw_wtext(16, 112, FONT_SIZE_14, FONT_SIZE_14, LIGHTBLUE, i18n(S_GRAPHIC_DB_OVERWRITE_WC));
			pp2d_draw_wtext(16, 140, FONT_SIZE_14, FONT_SIZE_14, LIGHTBLUE, i18n(S_GRAPHIC_DB_ADAPT_LANGUAGE_WC));
			pp2d_draw_wtext(16, 170, FONT_SIZE_14, FONT_SIZE_14, LIGHTBLUE, i18n(S_GRAPHIC_DB_INJECT_WC_SLOT));
			
			pp2d_draw_wtext(231 + (36 - pp2d_get_wtext_width(i18n(S_YES), FONT_SIZE_12, FONT_SIZE_12)) / 2, 113, FONT_SIZE_12, FONT_SIZE_12, overwrite ? DARKBLUE : YELLOW, i18n(S_YES));
			pp2d_draw_wtext(270 + (36 - pp2d_get_wtext_width(i18n(S_NO), FONT_SIZE_12, FONT_SIZE_12)) / 2, 113, FONT_SIZE_12, FONT_SIZE_12, !overwrite ? DARKBLUE : YELLOW, i18n(S_NO));
			pp2d_draw_wtext(231 + (36 - pp2d_get_wtext_width(i18n(S_YES), FONT_SIZE_12, FONT_SIZE_12)) / 2, 141, FONT_SIZE_12, FONT_SIZE_12, adapt ? DARKBLUE : YELLOW, i18n(S_YES));
			pp2d_draw_wtext(270 + (36 - pp2d_get_wtext_width(i18n(S_NO), FONT_SIZE_12, FONT_SIZE_12)) / 2, 141, FONT_SIZE_12, FONT_SIZE_12, !adapt ? DARKBLUE : YELLOW, i18n(S_NO));
			pp2d_draw_text(251 + (36 - pp2d_get_text_width(cont, FONT_SIZE_12, FONT_SIZE_12)) / 2, 171, FONT_SIZE_12, FONT_SIZE_12, YELLOW, cont);			
		}
		
		if (ota)
		{
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtextf(10, 225, FONT_SIZE_9, FONT_SIZE_9, WHITE, i18n(S_HTTP_SERVER_RUNNING), socket_get_ip());
		} 
		else
		{
			printBottomIndications(i18n(S_GRAPHIC_DB_INDICATIONS_INJECT));
		}

	pp2d_end_draw();
}

u16 getAlternativeSprite(u8* pkmn, int game) {
	u8 form = pkx_get_form(pkmn);
	if (form) {
		FormData *forms = pkx_get_legal_form_data(pkx_get_species(pkmn), game);
		int spritenum = forms->spriteNum;
		if (spritenum > 0 && form >= forms->min && form <= forms->max)
			spritenum += form - (forms->min > 0 ? forms->min : 1);
		free(forms);
		return spritenum;
	}
	return 0;
}

void printElement(u8* pkmn, int game, u16 n, int x, int y) {
	u16 t = getAlternativeSprite(pkmn, game);
	if (t && !(n == 745 && pkx_get_form(pkmn) == 2)) { // handles dusk lycanroc sprite to return the standard one
		t -= 1;
		pp2d_draw_texture_part(TEXTURE_ALTERNATIVE_SPRITESHEET, x, y, 34 * (t % 6), 30 * (t / 6), 34, 30);  
	} else {
		if (pkx_get_species(pkmn) > ofs.totalSpecies)
			pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, x, y, 0, 0, 34, 30);
		else
			pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, x, y, 34 * (n % 30), 30 * (n / 30), 34, 30);
	}
	if (pkx_is_egg(pkmn))
		pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, x + 6, y + 6, 34 * (EGGSPRITEPOS % 30), 30 * (EGGSPRITEPOS / 30), 34, 30);
	if (pkx_get_item(pkmn))
		pksm_draw_texture(TEXTURE_ITEM, x + 3, y + 21);
}

void printElementBlend(u8* pkmn, int game, u16 n, int x, int y, u32 color) {
	u16 t = getAlternativeSprite(pkmn, game);
	if (t && !(n == 745 && pkx_get_form(pkmn) == 2)) { // handles dusk lycanroc sprite to return the standard one
		t -= 1;
		pp2d_draw_texture_part_blend(TEXTURE_ALTERNATIVE_SPRITESHEET, x, y, 34 * (t % 6), 30 * (t / 6), 34, 30, color); 
	} else {
		if (pkx_get_species(pkmn) > ofs.totalSpecies)
			pp2d_draw_texture_part_blend(TEXTURE_NORMAL_SPRITESHEET, x, y, 0, 0, 34, 30, color);
		else
			pp2d_draw_texture_part_blend(TEXTURE_NORMAL_SPRITESHEET, x, y, 34 * (n % 30), 30 * (n / 30), 34, 30, color);
	}
	if (pkx_is_egg(pkmn))
		pp2d_draw_texture_part_blend(TEXTURE_NORMAL_SPRITESHEET, x + 6, y + 6, 34 * (EGGSPRITEPOS % 30), 30 * (EGGSPRITEPOS / 30), 34, 30, color);
	if (pkx_get_item(pkmn))
		pksm_draw_texture_blend(TEXTURE_ITEM, x + 3, y + 21, color);
}

void infoViewer(u8* pkmn) {
	int y_desc = 29;
	wchar_t* entries[] = {i18n(S_GRAPHIC_INFOVIEWER_NICKNAME), i18n(S_GRAPHIC_INFOVIEWER_OT), i18n(S_GRAPHIC_INFOVIEWER_POKERUS), i18n(S_GRAPHIC_INFOVIEWER_NATURE), i18n(S_GRAPHIC_INFOVIEWER_ABILITY), i18n(S_GRAPHIC_INFOVIEWER_ITEM), i18n(S_GRAPHIC_INFOVIEWER_ESVTSV), i18n(S_GRAPHIC_INFOVIEWER_TIDSID), i18n(S_GRAPHIC_INFOVIEWER_HTOT_FRIENDSHIP), i18n(S_GRAPHIC_INFOVIEWER_HTOT_HIDDEN_POWER)};
	wchar_t* values[] =  {i18n(S_GRAPHIC_INFOVIEWER_VALUE_HP), i18n(S_GRAPHIC_INFOVIEWER_VALUE_ATTACK), i18n(S_GRAPHIC_INFOVIEWER_VALUE_DEFENSE), i18n(S_GRAPHIC_INFOVIEWER_VALUE_SP_ATK), i18n(S_GRAPHIC_INFOVIEWER_VALUE_SP_DEF), i18n(S_GRAPHIC_INFOVIEWER_VALUE_SPEED)};
	
	printAnimatedBG(true);
	pp2d_draw_texture(TEXTURE_INFO_VIEW, 0, 2);

	pksm_draw_texture((pkx_get_move(pkmn, 0)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 155);
	pksm_draw_texture((pkx_get_move(pkmn, 1)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 176);
	pksm_draw_texture((pkx_get_move(pkmn, 2)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 197);
	pksm_draw_texture((pkx_get_move(pkmn, 3)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 218);
	
	pp2d_draw_wtext(251, 138, FONT_SIZE_12, FONT_SIZE_12, WHITE, i18n(S_GRAPHIC_INFOVIEWER_MOVES));
	for (int i = 0; i < 10; i++) {
		if (i == 8 && pkx_is_egg(pkmn))
			pp2d_draw_wtext(2, y_desc, FONT_SIZE_12, FONT_SIZE_12, BLUE, i18n(S_GRAPHIC_INFOVIEWER_EGG_CYCLE));
		else
			pp2d_draw_wtext(2, y_desc, FONT_SIZE_12, FONT_SIZE_12, BLUE, entries[i]);
		y_desc += 20;
		if (i == 2) y_desc += 5;
		if (i == 5) y_desc += 6;
	}
	
	y_desc = 8;
	for (int i = 0; i < 6; i++) {
		pp2d_draw_wtext(225, y_desc, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, values[i]);
		y_desc += 20;
	}
	
	if (pkx_get_species(pkmn) > 0 && pkx_get_species(pkmn) <= ofs.totalSpecies) {
		pp2d_draw_texture_part(TEXTURE_BALLS_SPRITESHEET, -2, -5, 32 * (pkx_get_ball(pkmn) % 8), 32 * (pkx_get_ball(pkmn) / 8), 32, 32);
		pp2d_draw_wtext(30, 6, FONT_SIZE_12, FONT_SIZE_12, WHITE, listSpecies.items[pkx_get_species(pkmn)]);
		
		printGeneration(pkmn, 134, 8);
		
		if (pkx_get_gender(pkmn) == 0)
			pksm_draw_texture(TEXTURE_MALE, 146, 7);
		else if (pkx_get_gender(pkmn) == 1)
			pksm_draw_texture(TEXTURE_FEMALE, 148, 7);
		
		wchar_t level[8];
		swprintf(level, 8, i18n(S_GRAPHIC_INFOVIEWER_LV), pkx_get_level(pkmn));
		pp2d_draw_wtext(160, 6, FONT_SIZE_12, FONT_SIZE_12, WHITE, level);
		
		u32 nick[NICKNAMELENGTH*2] = {0};
		pkx_get_nickname(pkmn, nick);
		pp2d_draw_wtext(215 - pp2d_get_wtext_width((wchar_t*)nick, FONT_SIZE_12, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)nick);
		
		u32 ot_name[NICKNAMELENGTH*2] = {0};
		pkx_get_ot(pkmn, ot_name);
		pp2d_draw_wtext(215 - pp2d_get_wtext_width((wchar_t*)ot_name, FONT_SIZE_12, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)ot_name);
		
		pp2d_draw_wtext(215 - pp2d_get_wtext_width((pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO)), FONT_SIZE_12, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, WHITE, pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO));
		pp2d_draw_wtext(215 - pp2d_get_wtext_width(natures[pkx_get_nature(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, WHITE, natures[pkx_get_nature(pkmn)]);
		pp2d_draw_wtext(215 - pp2d_get_wtext_width(abilities[pkx_get_ability(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 114, FONT_SIZE_12, FONT_SIZE_12, WHITE, abilities[pkx_get_ability(pkmn)]);
		pp2d_draw_wtext(215 - pp2d_get_wtext_width(items[pkx_get_item(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 134, FONT_SIZE_12, FONT_SIZE_12, WHITE, items[pkx_get_item(pkmn)]);
		
		if (pkx_is_shiny(pkmn))
			pksm_draw_texture(TEXTURE_SHINY, 205, 9);
		
		char friendship[11];
		if (pkx_is_egg(pkmn))
			snprintf(friendship, 11, "%u", pkx_get_ot_friendship(pkmn));
		else
			snprintf(friendship, 11, "%u / %u", pkx_get_ht_friendship(pkmn), pkx_get_ot_friendship(pkmn));
		pp2d_draw_text(215 - pp2d_get_text_width(friendship, FONT_SIZE_12, FONT_SIZE_12), 200, FONT_SIZE_12, FONT_SIZE_12, WHITE, friendship);
		
		char otid[18];
		snprintf(otid, 18, "%u / %u", pkx_get_psv(pkmn), pkx_get_tsv(pkmn));
		pp2d_draw_text(215 - pp2d_get_text_width(otid, FONT_SIZE_12, FONT_SIZE_12), 160, FONT_SIZE_12, FONT_SIZE_12, WHITE, otid);
		snprintf(otid, 18, "%u / %u", pkx_get_tid(pkmn), pkx_get_sid(pkmn));
		pp2d_draw_text(215 - pp2d_get_text_width(otid, FONT_SIZE_12, FONT_SIZE_12), 180, FONT_SIZE_12, FONT_SIZE_12, WHITE, otid);

		pp2d_draw_wtext(215 - pp2d_get_wtext_width(hpList[pkx_get_hp_type(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 220, FONT_SIZE_12, FONT_SIZE_12, WHITE, hpList[pkx_get_hp_type(pkmn)]);
		
		int max = pp2d_get_text_width("252", FONT_SIZE_12, FONT_SIZE_12);		
		int y_moves = 159;
		for (int i = 0; i < 4; i++) {
			if (pkx_get_move(pkmn, i))
				pp2d_draw_wtext(396 - pp2d_get_wtext_width(moves[pkx_get_move(pkmn, i)], FONT_SIZE_12, FONT_SIZE_12), y_moves, FONT_SIZE_12, FONT_SIZE_12, WHITE, moves[pkx_get_move(pkmn, i)]);
			y_moves += 21;
		}
		
		char tmp[6];
		for (int i = 0; i < 6; i++) {
			sprintf(tmp, "%d", pkx_get_iv(pkmn, lookup[i]));
			pp2d_draw_text(289 + (max - pp2d_get_text_width(tmp, FONT_SIZE_12, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, WHITE, tmp);
			sprintf(tmp, "%d", pkx_get_ev(pkmn, lookup[i]));
			pp2d_draw_text(328 + (max - pp2d_get_text_width(tmp, FONT_SIZE_12, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, WHITE, tmp);
			sprintf(tmp, "%d", pkx_get_stat(pkmn, lookup[i]));
			pp2d_draw_text(369 + (max - pp2d_get_text_width(tmp, FONT_SIZE_12, FONT_SIZE_12)) / 2, 8 + i * 20, FONT_SIZE_12, FONT_SIZE_12, WHITE, tmp);
		}
	}
}

void wcxInfoViewer(u8* buf) {
	int y_desc = 29;
	wchar_t* entries[] = {L"Species", L"OT", L"TID/SID", L"Held Item", L"Game", L"Met Date"};

	pp2d_draw_texture(TEXTURE_EVENT_VIEW, 0, 2);

	pksm_draw_texture((wc_get_move(buf, 0)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 155);
	pksm_draw_texture((wc_get_move(buf, 1)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 176);
	pksm_draw_texture((wc_get_move(buf, 2)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 197);
	pksm_draw_texture((wc_get_move(buf, 3)) ? TEXTURE_NORMAL_BAR : TEXTURE_NO_MOVE, 252, 218);
	
	pp2d_draw_wtext(251, 138, FONT_SIZE_12, FONT_SIZE_12, WHITE, i18n(S_GRAPHIC_INFOVIEWER_MOVES));
	for (int i = 0; i < 6; i++) {
		pp2d_draw_wtext(2, y_desc, FONT_SIZE_12, FONT_SIZE_12, BLUE, entries[i]);
		y_desc += 20;
		if (i == 2) y_desc += 6;
		if (i == 3) y_desc -= 1;
		if (i == 5) y_desc += 6;
	}
	
	wchar_t *version = L"WIP";
	pp2d_draw_wtext(215 - pp2d_get_wtext_width(version, FONT_SIZE_12, FONT_SIZE_12), 115, FONT_SIZE_12, FONT_SIZE_12, WHITE, version);
	
	u32 title[0x4A] = {0};
	wc_get_title(buf, title);
	pp2d_draw_wtext(!wc_is_pokemon(buf) ? 4 : 30, 6, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)title);
	
	u32 date[12] = {0};
	swprintf((wchar_t*)date, 12, L"%u/%u/%u", wc_get_year(buf), wc_get_month(buf), wc_get_day(buf));
	pp2d_draw_wtext(215 - (pp2d_get_wtext_width((wchar_t*)date, FONT_SIZE_12, FONT_SIZE_12)), 134, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)date);
	
	if (wc_is_pokemon(buf)) {
		pp2d_draw_texture_part(TEXTURE_BALLS_SPRITESHEET, -2, -5, 32 * (wc_get_ball(buf) % 8), 32 * (wc_get_ball(buf) / 8), 32, 32);
		
		if (wc_get_gender(buf) == 0)
			pksm_draw_texture(TEXTURE_MALE, 288, 7);
		else if (wc_get_gender(buf) == 1)
			pksm_draw_texture(TEXTURE_FEMALE, 290, 7);
		
		wchar_t level[8];
		swprintf(level, 8, i18n(S_GRAPHIC_INFOVIEWER_LV), wc_get_level(buf));
		pp2d_draw_wtext(302, 6, FONT_SIZE_12, FONT_SIZE_12, WHITE, level);
		
		u32 ot_name[0x4A] = {0};
		wc_get_ot(buf, ot_name);
		pp2d_draw_wtext(215 - pp2d_get_wtext_width((wchar_t*)ot_name, FONT_SIZE_12, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)ot_name);
		
		pp2d_draw_wtext(215 - pp2d_get_wtext_width(items[wc_get_held_item(buf)], FONT_SIZE_12, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, WHITE, items[wc_get_held_item(buf)]);
		
		if (wc_is_shiny(buf)) {
			pksm_draw_texture(TEXTURE_SHINY, 206, 32);
			pp2d_draw_wtext(202 - pp2d_get_wtext_width(listSpecies.items[wc_get_species(buf)], FONT_SIZE_12, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, WHITE, listSpecies.items[wc_get_species(buf)]);
		} else {
			pp2d_draw_wtext(215 - pp2d_get_wtext_width(listSpecies.items[wc_get_species(buf)], FONT_SIZE_12, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, WHITE, listSpecies.items[wc_get_species(buf)]);
		}
		
		char otid[18];
		snprintf(otid, 18, "%u / %u", wc_get_tid(buf), wc_get_sid(buf));
		pp2d_draw_text(215 - pp2d_get_text_width(otid, FONT_SIZE_12, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, WHITE, otid);
	
		int y_moves = 159;
		for (int i = 0; i < 4; i++) {
			if (wc_get_move(buf, i))
				pp2d_draw_wtext(396 - pp2d_get_wtext_width(moves[wc_get_move(buf, i)], FONT_SIZE_12, FONT_SIZE_12), y_moves, FONT_SIZE_12, FONT_SIZE_12, WHITE, moves[wc_get_move(buf, i)]);
			y_moves += 21;
		}	
	} else if (wc_is_item(buf)) {
		pp2d_draw_text(215 - pp2d_get_text_width("None", FONT_SIZE_12, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, WHITE, "None");
		pp2d_draw_text(215 - pp2d_get_text_width("None", FONT_SIZE_12, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, WHITE, "None");
		pp2d_draw_text(215 - pp2d_get_text_width("None", FONT_SIZE_12, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, WHITE, "None");
		pp2d_draw_wtext(215 - pp2d_get_wtext_width(items[wc_get_item(buf)], FONT_SIZE_12, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, WHITE, items[wc_get_item(buf)]);
	}  else if (wcx_is_bp(buf)) {
		pp2d_draw_text(215 - pp2d_get_text_width("None", FONT_SIZE_12, FONT_SIZE_12), 29, FONT_SIZE_12, FONT_SIZE_12, WHITE, "None");
		pp2d_draw_text(215 - pp2d_get_text_width("None", FONT_SIZE_12, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, WHITE, "None");
		pp2d_draw_text(215 - pp2d_get_text_width("None", FONT_SIZE_12, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, WHITE, "None");
		pp2d_draw_text(215 - pp2d_get_text_width("Battle Points", FONT_SIZE_12, FONT_SIZE_12), 94, FONT_SIZE_12, FONT_SIZE_12, WHITE, "Battle Points");
	}
}

void printDexViewer(u8* mainbuf, int currentEntry, int page, int seen, int caught) {
	char temp[12];
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_GENERATION_BG, 0, 0);
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 8; j++) {
				if (currentEntry == i * 8 + j)
					printSelector(j*49 + j, i*47 + i, 49, 47);
				
				int entry = i*8+j + 40*page;
				if ((entry) < ofs.maxSpecies) {
					if (getCaught(mainbuf, entry + 1))
						pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, 7 + 49 * j + j, 2 + 47 * i + i, 34 * ((entry + 1) % 30), 30 * ((entry + 1) / 30), 34, 30);
					else if (getSeen(mainbuf, entry + 1)) {
						pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, 7 + 49 * j + j, 2 + 47 * i + i, 34 * ((entry + 1) % 30), 30 * ((entry + 1) / 30), 34, 30);
						pp2d_draw_texture_part_blend(TEXTURE_NORMAL_SPRITESHEET, 7 + 49 * j + j, 2 + 47 * i + i, 34 * ((entry + 1) % 30), 30 * ((entry + 1) / 30), 34, 30, RGBA8(0,0,0,160));
					} else {
						pp2d_draw_texture_part_blend(TEXTURE_NORMAL_SPRITESHEET, 7 + 49 * j + j, 2 + 47 * i + i, 34 * ((entry + 1) % 30), 30 * ((entry + 1) / 30), 34, 30, RGBA8(0,0,0,255));
					}
					sprintf(temp, "%d", entry + 1);
					pp2d_draw_text(49 * j + (49 - pp2d_get_text_width(temp, FONT_SIZE_9, FONT_SIZE_9)) / 2 + j, 34 + i * 47 + i, FONT_SIZE_9, FONT_SIZE_9, getCaught(mainbuf, entry + 1) ? WHITE : DS, temp);
				}
			}
		}

		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		pp2d_draw_wtextf(16, 32, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKBANK_SEEN), seen);
		pp2d_draw_wtextf(16, 48, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKBANK_CAUGHT), caught);
		printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
	pp2d_end_draw();
}

void printPKViewer(u8* mainbuf, u8* tmp, bool isTeam, int currentEntry, int menuEntry, int box, int mode, int additional1, int additional2) {
	int game = game_get();
	
	wchar_t* menuEntries[] = {i18n(S_GRAPHIC_PKVIEWER_MENU_EDIT), i18n(S_GRAPHIC_PKVIEWER_MENU_CLONE), i18n(S_GRAPHIC_PKVIEWER_MENU_RELEASE), i18n(S_GRAPHIC_PKVIEWER_MENU_GENERATE), i18n(S_GRAPHIC_PKVIEWER_MENU_EXIT)};
	int x;

	wchar_t page[MAX_LENGTH_BOX_NAME + 1];
	swprintf(page, MAX_LENGTH_BOX_NAME+1, i18n(S_GRAPHIC_PKVIEWER_BOX), box + 1);
	
	u8 pkmn[PKMNLENGTH];
	pkx_get(mainbuf, isTeam ? 33 : box, currentEntry, pkmn);
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		if (mode == ED_GENERATE) {
			char temp[4];
			pp2d_draw_texture(TEXTURE_GENERATION_BG, 0, 0);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 8; j++) {
					if (additional1 == i * 8 + j)
						printSelector(j*49 + j, i*47 + i, 49, 47);
					
					if ((i*8+j + 40*additional2) < ofs.totalSpecies) {
						pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, 7 + 49 * j + j, 2 + 47 * i + i, 34 * ((40 * additional2 + i * 8 + j + 1) % 30), 30 * ((40 * additional2 + i * 8 + j + 1) / 30), 34, 30);
						snprintf(temp, 4, "%d", 40 * additional2 + i * 8 + j + 1);
						pp2d_draw_text(49 * j + (49 - pp2d_get_text_width(temp, FONT_SIZE_9, FONT_SIZE_9)) / 2 + j, 34 + i * 47 + i, FONT_SIZE_9, FONT_SIZE_9, WHITE, temp);
					}
				}
			}
		} else
			infoViewer(pkmn);
		
		if (mode == ED_OTA) {
			pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, 220));
			pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, RGBA8(255, 255, 255, giveTransparence()), i18n(S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT));
			pp2d_draw_wtext_center(GFX_TOP, 130, FONT_SIZE_12, FONT_SIZE_12, WHITE, i18n(S_GRAPHIC_PKVIEWER_OTA_INDICATIONS));
			if (!socket_is_legality_address_set())
				pp2d_draw_wtext_center(GFX_TOP, 222, FONT_SIZE_11, FONT_SIZE_11, WHITE, i18n(S_GRAPHIC_PKVIEWER_OTA_SET_ADDRESS));
			else
				pp2d_draw_wtext_center(GFX_TOP, 222, FONT_SIZE_11, FONT_SIZE_11, WHITE, i18n(S_GRAPHIC_PKVIEWER_OTA_CHECK_LEGALITY));
		}

		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_BOX_VIEW, 0, 0);
		pksm_draw_texture(TEXTURE_BOTTOM_BAR, 0, 210);
		pp2d_draw_wtext(12 + (178 - pp2d_get_wtext_width(page, FONT_SIZE_12, FONT_SIZE_12)) / 2, 19, FONT_SIZE_12, FONT_SIZE_12, WHITE, page);
		pksm_draw_texture(TEXTURE_LEFT, 7, 17);
		pksm_draw_texture(TEXTURE_RIGHT, 185, 17);
		pksm_draw_texture(TEXTURE_OTA_BUTTON, 240, 211);
		pksm_draw_texture(TEXTURE_BACK_BUTTON, 283, 211);
		pp2d_draw_text(241, 7, FONT_SIZE_12, FONT_SIZE_12, WHITE, "\uE004+\uE005 \uE01E");
		
		int y = 45;
		int pointer[2] = {0, 0};
		for (int i = 0; i < 5; i++) {
			x = 4;
			for (int j = 0; j < 6; j++) {
				pkx_get(mainbuf, box, i * 6 + j, pkmn);
				u16 n = pkx_get_species(pkmn);
				if (n > 0 && n <= ofs.totalSpecies)
					printElement(pkmn, game, n, x, y);

				if ((currentEntry == (i * 6 + j)) && !isTeam) {
					pointer[0] = x + 18;
					pointer[1] = y - 8;
				}
				x += 34;
			}
			y += 30;
		}
		
		for (int i = 0; i < 3; i++) {
			pksm_draw_texture(TEXTURE_POKEMON_BOX, 214, 40 + i * 45);
			pksm_draw_texture(TEXTURE_POKEMON_BOX, 266, 60 + i * 45);
		}
		
		y = 45;
		for (int i = 0; i < 3; i++) {
			x = 222;
			for (int j = 0; j < 2; j++) {
				pkx_get(mainbuf, 33, i * 2 + j, pkmn);
				u16 n = pkx_get_species(pkmn);
				if (n)
					printElement(pkmn, game, n, x, (j == 1) ? y + 20 : y);

				if ((currentEntry == (i * 2 + j)) && isTeam) {
					pointer[0] = x + 18;
					pointer[1] = y - 8 + ((j == 1) ? 20 : 0);
				}
				x += 52;
			}
			y += 45;
		}
		if (mode != ED_CLONE)
			pksm_draw_texture(TEXTURE_SELECTOR_NORMAL, pointer[0], pointer[1] - ((mode == ED_STANDARD) ? movementOffsetSlow(3) : 0));
		else {
			int ofs = movementOffsetSlow(3);
			pksm_draw_texture(TEXTURE_SELECTOR_NORMAL, pointer[0], pointer[1] - ofs);	
			pksm_draw_texture_blend(TEXTURE_SELECTOR_NORMAL, pointer[0], pointer[1] - ofs, RGBA8(0, 0, 255, 100));			
			pksm_draw_texture(TEXTURE_BOTTOM_POPUP, 1, 214);
			pp2d_draw_wtext(8, 220, FONT_SIZE_11, FONT_SIZE_11, WHITE, i18n(S_GRAPHIC_PKVIEWER_CLONE_INDICATIONS));
		}
		
		if (mode == ED_MENU) {
			pkx_get(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn);

			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pksm_draw_texture(TEXTURE_BOTTOM_POPUP, 1, 214);

			if (pkx_get_ball(pkmn) != CHERISH_BALL && !isTeam) {
				pksm_draw_texture(TEXTURE_INCLUDE_INFO, 242, 5);
			}
			pp2d_draw_wtextf(8, 220, FONT_SIZE_11, FONT_SIZE_11, WHITE, i18n(S_GRAPHIC_PKVIEWER_MENU_POKEMON_SELECTED), listSpecies.items[pkx_get_species(tmp)]);
			for (int i = 0; i < 5; i++) {
				pksm_draw_texture(TEXTURE_BUTTON, 208, 42 + i * 27 + i*4);
				if (isTeam && (i == 0 || i == 2 || i == 3)) {
					pksm_draw_texture_blend(TEXTURE_BUTTON, 208, 42 + i * 27 + i*4, RGBA8(0, 0, 0, 100));
				}
				pp2d_draw_wtext(208 + (109 - pp2d_get_wtext_width(menuEntries[i], FONT_SIZE_12, FONT_SIZE_12)) / 2, 49 + i * 27 + i*4, FONT_SIZE_12, FONT_SIZE_12, BLACK, menuEntries[i]);
				if (i == menuEntry)
					pksm_draw_texture(TEXTURE_SUB_ARROW, 203 - movementOffsetSlow(3), 46 + i * 27 + i*4);
			}
		} else if (mode == ED_GENERATE) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtext_center(GFX_BOTTOM, 105, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKVIEWER_GENERATE_INDICATIONS));
		} else if (mode == ED_OTA) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtextf(10, 220, FONT_SIZE_9, FONT_SIZE_9, WHITE, i18n(S_HTTP_SERVER_RUNNING), socket_get_ip());
		} else if (mode != ED_CLONE) {
			if (mode == ED_STANDARD)
				pp2d_draw_wtextf(10, 220, FONT_SIZE_9, FONT_SIZE_9, BARTEXT, i18n(S_GRAPHIC_PKVIEWER_TIDSIDTSV), getSaveTID(mainbuf), getSaveSID(mainbuf), getSaveTSV(mainbuf));
			else
				pp2d_draw_wtextf(10, 220, FONT_SIZE_9, FONT_SIZE_9, BARTEXT, i18n(S_GRAPHIC_PKVIEWER_SEED), getSaveSeed(mainbuf, 3), getSaveSeed(mainbuf, 2), getSaveSeed(mainbuf, 1), getSaveSeed(mainbuf, 0));
		}
	pp2d_end_draw();
}

void printPKEditor(u8* pkmn, int additional1, int additional2, int additional3, int mode, wchar_t* descriptions[]) {
	int game = game_get();
	
	int max = pp2d_get_text_width("252", FONT_SIZE_12, FONT_SIZE_12);
	wchar_t* entries[] = {i18n(S_GRAPHIC_PKEDITOR_LEVEL), i18n(S_GRAPHIC_PKEDITOR_NATURE), i18n(S_GRAPHIC_PKEDITOR_ABILITY), i18n(S_GRAPHIC_PKEDITOR_ITEM), i18n(S_GRAPHIC_PKEDITOR_SHINY), i18n(S_GRAPHIC_PKEDITOR_POKERUS), i18n(S_GRAPHIC_PKEDITOR_OT), i18n(S_GRAPHIC_PKEDITOR_NICKNAME), i18n(S_GRAPHIC_PKEDITOR_FRIENDSHIP)};
	wchar_t* options[] = {i18n(S_GRAPHIC_PKEDITOR_MENU_STATS), i18n(S_GRAPHIC_PKEDITOR_MENU_MOVES), i18n(S_GRAPHIC_PKEDITOR_MENU_SAVE)};
	
	wchar_t* values[6] = {i18n(S_GRAPHIC_PKEDITOR_STATS_HP), i18n(S_GRAPHIC_PKEDITOR_STATS_ATTACK), i18n(S_GRAPHIC_PKEDITOR_STATS_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_STATS_SP_ATTACK), i18n(S_GRAPHIC_PKEDITOR_STATS_SP_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_STATS_SPEED)};
	u16 n = pkx_get_species(pkmn);
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		pp2d_set_screen_color(GFX_TOP, BACKGROUND_BLACK);
		if (mode == ED_BASE || mode == ED_STATS) {
			pp2d_draw_wtext_center(GFX_TOP, 95, FONT_SIZE_15, FONT_SIZE_15, RGBA8(255, 255, 255, giveTransparence()), i18n(S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_1));
			pp2d_draw_wtext_center(GFX_TOP, 115, FONT_SIZE_15, FONT_SIZE_15, RGBA8(255, 255, 255, giveTransparence()), i18n(S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_2));
		} else if (mode == ED_ITEMS) {
			int entry;
			int y = 0;
			pp2d_draw_texture(TEXTURE_TOP_MOVES, 0, 0);
			for (int i = 0; i < 20; i++) {
				entry = 40 * additional2 + i;
				if (i == additional1)
					printSelector(2, y, 198, 11);
				else if (i + 20 == additional1) 
					printSelector(200, y, 198, 11);
				if (entry <= ofs.totalItems)
				{
					pp2d_draw_wtextf(2, y, FONT_SIZE_9, FONT_SIZE_9, WHITE, L"%d - %ls", entry, itemsSorted[entry]);
				}
				if (entry + 20 <= ofs.totalItems)
				{				
					pp2d_draw_wtextf(200, y, FONT_SIZE_9, FONT_SIZE_9, WHITE, L"%d - %ls", entry + 20, itemsSorted[entry + 20]);
				}
				y += 12;
			}	
		} else if (mode == ED_MOVES) {
			int entry;
			int y = 0;
			pp2d_draw_texture(TEXTURE_TOP_MOVES, 0, 0);
			for (int i = 0; i < 20; i++) {
				entry = 40 * additional2 + i;
				if (i == additional1) 
					printSelector(2, y, 198, 11);
				else if (i + 20 == additional1) 
					printSelector(200, y, 198, 11);
				if (entry <= ofs.totalMoves)
				{
					pp2d_draw_wtextf(2, y, FONT_SIZE_9, FONT_SIZE_9, WHITE, L"%d - %ls", entry, movesSorted[entry]);
				}
				if (entry + 20 <= ofs.totalMoves)
				{
					pp2d_draw_wtextf(200, y, FONT_SIZE_9, FONT_SIZE_9, WHITE, L"%d - %ls", entry + 20, movesSorted[entry + 20]);
				}
				y += 12;
			}
		} else if (mode == ED_NATURES) {
			wchar_t* hor[] = {i18n(S_GRAPHIC_PKEDITOR_NATURE_NEUTRAL), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_SPEED), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_SP_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_SP_DEFENSE)};
			wchar_t* ver[] = {i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_SPEED), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_SP_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_SP_DEFENSE)};
			
			pp2d_draw_texture(TEXTURE_NATURES, 0, 0);
			for (int i = 0; i < 6; i++)
				pp2d_draw_wtext(66 * i + (66 - pp2d_get_wtext_width(hor[i], FONT_SIZE_12, FONT_SIZE_12)) / 2, 13, FONT_SIZE_12, FONT_SIZE_12, (i == 0) ? YELLOW : BLUE, hor[i]);
			for (int i = 0; i < 5; i++)
				pp2d_draw_wtext((66 - pp2d_get_wtext_width(ver[i], FONT_SIZE_12, FONT_SIZE_12)) / 2, 53 + i * 40, FONT_SIZE_12, FONT_SIZE_12, BLUE, ver[i]);
			
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					if (additional1 == i * 5 + j)
						printSelector(66*j + 66 + j, 40*i + 40, 66, 39);
					pp2d_draw_wtext(66 + 66 * j + (66 - pp2d_get_wtext_width(natures[i * 5 + j], FONT_SIZE_12, FONT_SIZE_12)) / 2 + j, 53 + i * 40, FONT_SIZE_12, FONT_SIZE_12, (i == j) ? YELLOW : WHITE, natures[i * 5 + j]);
				}
			}
		} else if (mode == ED_BALLS) {
			pp2d_draw_texture(TEXTURE_BALLS_BG, 0, 0);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 6; j++) {
					if (additional1 == i * 6 + j)
						printSelector(66*j + j, 47*i + i, 66, 47);
					pp2d_draw_texture_part(TEXTURE_BALLS_SPRITESHEET, 17 + 66 * j + j, 2 + i * 47, 32 * ((i * 6 + j + 1) % 8), 32 * ((i * 6 + j + 1) / 8), 32, 32);
					pp2d_draw_wtext(66 * j + (66 - pp2d_get_wtext_width(ballList[i * 6 + j], FONT_SIZE_9, FONT_SIZE_9)) / 2 + j, 30 + i * 47 + i, FONT_SIZE_9, FONT_SIZE_9, WHITE, ballList[i * 6 + j]);
				}
			}
		} else if (mode == ED_HIDDENPOWER) {
			pp2d_draw_texture(TEXTURE_HIDDEN_POWER_BG, 0, 0);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (additional1 == i * 4 + j)
						printSelector(99*j + j, 59*i + i, 99, 59);
					pp2d_draw_texture_part(TEXTURE_TYPES_SPRITESHEET, 24 + 99 * j + j, 20 + 59 * i + i, 50 * (i * 4 + j + 1), 0, 50, 18); 
				}
			}
		} else if (mode == ED_FORMS) {
			FormData *forms = pkx_get_legal_form_data((u16)additional2, game);
			int numforms = forms->max - forms->min + 1;
			
			int rows, columns, width, height;
			if (numforms <= 16) {
				columns = 4; rows = 4;
				width = 99; height = 59;
				pp2d_draw_texture(TEXTURE_HIDDEN_POWER_BG, 0, 0);
			} else {
				columns = 6; rows = 5;
				width = 66; height = 47;
				pp2d_draw_texture(TEXTURE_BALLS_BG, 0, 0);
			}

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					if (additional1 == i * columns + j)
						printSelector(j*width + j, i*height + i, width, height);

					int form = i * columns + j;
					if (form < numforms) {
						wchar_t *str = formList[forms->stringNum + form];
						if (forms->min > 0)
							form++;
						if (form == 0 || forms->spriteNum == 0)
							pp2d_draw_texture_part(TEXTURE_NORMAL_SPRITESHEET, (width - 34) / 2 + width * j + j, 2 * (height - 44) / 3 + i * height, 34 * (additional2 % 30), 30 * (additional2 / 30), 34, 30);
						else {
							int sprite = forms->spriteNum + form - 2;
							pp2d_draw_texture_part(TEXTURE_ALTERNATIVE_SPRITESHEET, (width - 34) / 2 + width * j + j, 2 * (height - 44) / 3 + i * height + i, 34 * (sprite % 6), 30 * (sprite / 6), 34, 30);
						}
						pp2d_draw_wtext(width * j + (width - pp2d_get_wtext_width(str, FONT_SIZE_9, FONT_SIZE_9)) / 2 + j, (height * 5) / 7 + i * height + i, FONT_SIZE_9, FONT_SIZE_9, WHITE, str);
					}
				}
			}
		} else if (mode == ED_HEX) {
			pp2d_draw_texture(TEXTURE_HEX_BG, 0, 0);
			if (hax)
				pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(255, 0, 0, 100));
			for (int rows = 0; rows < 15; rows++) {
				for (int columns = 0; columns < 16; columns++) {
					int byte = rows*16 + columns;
					if (additional1 == byte)
						printSelector(columns*25, rows*15, 24, 14);
					pp2d_draw_textf(4 + 25*columns, 15*rows, FONT_SIZE_11, FONT_SIZE_11, (sector[byte][0]) ? WHITE : DS, "%02hhX", pkmn[byte]);
					
					if (byte == ofs.pkmnLength - 1) break;
				}
			}
			pp2d_draw_wtextf(4, 225, FONT_SIZE_11, FONT_SIZE_11, LIGHTBLUE, L"%ls", descriptions[additional1]);
		}
	
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		if (mode != ED_HEX)
			printAnimatedBG(false);
		if (mode == ED_BASE || mode == ED_ITEMS || mode == ED_NATURES || mode == ED_BALLS || mode == ED_FORMS) {
			pp2d_draw_texture(TEXTURE_EDITOR_BOTTOM_BG, 0, 1);
			pksm_draw_texture(TEXTURE_BOTTOM_BAR, 0, 210);
			pksm_draw_texture(TEXTURE_HEX_BUTTON, 290, 1);
			
			pp2d_draw_wtext(27, 4, FONT_SIZE_12, FONT_SIZE_12, WHITE, listSpecies.items[n]);
			pp2d_draw_texture_part(TEXTURE_BALLS_SPRITESHEET, -2, -6, 32 * (pkx_get_ball(pkmn) % 8), 32 * (pkx_get_ball(pkmn) / 8), 32, 32);
			
			u16 t = getAlternativeSprite(pkmn, game);
			int ofs = movementOffsetSlow(3);
			if (t) {
				t -= 1;
				pp2d_draw_texture_part_scale_blend(TEXTURE_ALTERNATIVE_SPRITESHEET, 232, 32 - ofs, 34 * (t % 6), 30 * (t / 6), 34, 30, 2, 2, RGBA8(0x0, 0x0, 0x0, 100)); 
				pp2d_draw_texture_part_scale(TEXTURE_ALTERNATIVE_SPRITESHEET, 227, 27 - ofs, 34 * (t % 6), 30 * (t / 6), 34, 30, 2, 2);
			} else {
				pp2d_draw_texture_part_scale_blend(TEXTURE_NORMAL_SPRITESHEET, 232, 32 - ofs, 34 * (n % 30), 30 * (n / 30), 34, 30, 2, 2, RGBA8(0x0, 0x0, 0x0, 100));
				pp2d_draw_texture_part_scale(TEXTURE_NORMAL_SPRITESHEET, 227, 27 - ofs, 34 * (n % 30), 30 * (n / 30), 34, 30, 2, 2);
			}
			
			if (pkx_get_gender(pkmn) == 0)
				pksm_draw_texture(TEXTURE_MALE, 159, 6);
			else if (pkx_get_gender(pkmn) == 1)
				pksm_draw_texture(TEXTURE_FEMALE, 161, 6);
			
			for (int i = 0; i < 3; i++) {
				pksm_draw_texture(TEXTURE_BUTTON, 206, 110 + i * 27 + i*4);
				pp2d_draw_wtext(206 + (109 - pp2d_get_wtext_width(options[i], FONT_SIZE_12, FONT_SIZE_12)) / 2, 117 + i * 27 + i*4, FONT_SIZE_12, FONT_SIZE_12, BLACK, options[i]);
			}
			
			for (int i = 0; i < 9; i++)
				if (i == 8 && pkx_is_egg(pkmn))
					pp2d_draw_wtext(2, 29 + i * 20, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_EGG_CYCLE));
				else
					pp2d_draw_wtext(2, 29 + i * 20, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, entries[i]);

			for (int i = 0; i < 7; i++)
				pksm_draw_texture(TEXTURE_SETTING, 180, 51 + i * 20);

			pksm_draw_texture(TEXTURE_MINUS, 180 - max - 19, 29);
			pksm_draw_texture(TEXTURE_PLUS, 180, 29);
			pksm_draw_texture(TEXTURE_MINUS, 180 - max - 19, 189);
			pksm_draw_texture(TEXTURE_PLUS, 180, 189);

			char level[4];
			snprintf(level, 4, "%u", pkx_get_level(pkmn));
			pp2d_draw_text(180 - max - 3 + (max - pp2d_get_text_width(level, FONT_SIZE_12, FONT_SIZE_12)) / 2, 29, FONT_SIZE_12, FONT_SIZE_12, WHITE, level);
				
			pp2d_draw_wtext(178 - pp2d_get_wtext_width(natures[pkx_get_nature(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 49, FONT_SIZE_12, FONT_SIZE_12, WHITE, natures[pkx_get_nature(pkmn)]);
			pp2d_draw_wtext(178 - pp2d_get_wtext_width(abilities[pkx_get_ability(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 69, FONT_SIZE_12, FONT_SIZE_12, WHITE, abilities[pkx_get_ability(pkmn)]);
			pp2d_draw_wtext(178 - pp2d_get_wtext_width(items[pkx_get_item(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 89, FONT_SIZE_12, FONT_SIZE_12, WHITE, items[pkx_get_item(pkmn)]);
			pp2d_draw_wtext(178 - pp2d_get_wtext_width(pkx_is_shiny(pkmn) ? i18n(S_YES) : i18n(S_NO), FONT_SIZE_12, FONT_SIZE_12), 109, FONT_SIZE_12, FONT_SIZE_12, WHITE, pkx_is_shiny(pkmn) ? i18n(S_YES) : i18n(S_NO));
			pp2d_draw_wtext(178 - pp2d_get_wtext_width(pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO), FONT_SIZE_12, FONT_SIZE_12), 129, FONT_SIZE_12, FONT_SIZE_12, WHITE, pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO));
			
			char friendship[4];
			if (pkx_is_egg(pkmn))
				snprintf(friendship, 4, "%u", pkx_get_ot_friendship(pkmn));
			else
				snprintf(friendship, 4, "%u", pkx_get_friendship(pkmn));
			pp2d_draw_text(180 - max - 3 + (max - pp2d_get_text_width(friendship, FONT_SIZE_12, FONT_SIZE_12)) / 2, 189, FONT_SIZE_12, FONT_SIZE_12, WHITE, friendship);
			
			u32 nick[NICKNAMELENGTH*2] = {0};
			pkx_get_nickname(pkmn, nick);
			pp2d_draw_wtext(178 - pp2d_get_wtext_width((wchar_t*)nick, FONT_SIZE_12, FONT_SIZE_12), 169, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)nick);

			u32 ot_name[NICKNAMELENGTH*2] = {0};
			pkx_get_ot(pkmn, ot_name);
			pp2d_draw_wtext(178 - pp2d_get_wtext_width((wchar_t*)ot_name, FONT_SIZE_12, FONT_SIZE_12), 149, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)ot_name);
		}
		if (mode == ED_STATS || mode == ED_HIDDENPOWER) {
			pp2d_draw_texture(TEXTURE_EDITOR_STATS, 0, 1);
			pksm_draw_texture(TEXTURE_BOTTOM_BAR, 0, 210);
			pksm_draw_texture(TEXTURE_SETTING, 291, 175);
			pp2d_draw_wtext(2, 28, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_LBL_STATS));
			pp2d_draw_wtext(118, 28, FONT_SIZE_12, FONT_SIZE_12, DARKBLUE, i18n(S_GRAPHIC_PKEDITOR_LBL_IV));
			pp2d_draw_wtext(197, 28, FONT_SIZE_12, FONT_SIZE_12, DARKBLUE, i18n(S_GRAPHIC_PKEDITOR_LBL_EV));
			pp2d_draw_wtext(256, 28, FONT_SIZE_12, FONT_SIZE_12, DARKBLUE, i18n(S_GRAPHIC_PKEDITOR_LBL_TOTAL));
			pp2d_draw_wtext(2, 173, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_HIDDEN_POWER));
			
			pp2d_draw_wtext(27, 4, FONT_SIZE_12, FONT_SIZE_12, WHITE, listSpecies.items[n]);
			pp2d_draw_texture_part(TEXTURE_BALLS_SPRITESHEET, -2, -6, 32 * (pkx_get_ball(pkmn) % 8), 32 * (pkx_get_ball(pkmn) / 8), 32, 32);
			
			if (pkx_get_gender(pkmn) == 0)
				pksm_draw_texture(TEXTURE_MALE, 159, 6);
			else if (pkx_get_gender(pkmn) == 1)
				pksm_draw_texture(TEXTURE_FEMALE, 161, 6);
			
			for (int i = 0; i < 6; i++)
				pp2d_draw_wtext(2, 49 + i * 20, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, values[i]);

			char tmp[6];
			for (int i = 0; i < 6; i++) {
				sprintf(tmp, "%d", pkx_get_iv(pkmn, lookup[i]));
				pp2d_draw_text(112 + (max - pp2d_get_text_width(tmp, FONT_SIZE_12, FONT_SIZE_12)) / 2, 49 + i * 20, FONT_SIZE_12, FONT_SIZE_12, WHITE, tmp);
				sprintf(tmp, "%d", pkx_get_ev(pkmn, lookup[i]));
				pp2d_draw_text(192 + (max - pp2d_get_text_width(tmp, FONT_SIZE_12, FONT_SIZE_12)) / 2, 49 + i * 20, FONT_SIZE_12, FONT_SIZE_12, WHITE, tmp);
				sprintf(tmp, "%d", pkx_get_stat(pkmn, lookup[i]));
				pp2d_draw_text(263 + (max - pp2d_get_text_width(tmp, FONT_SIZE_12, FONT_SIZE_12)) / 2, 49 + i * 20, FONT_SIZE_12, FONT_SIZE_12, WHITE, tmp);
			}
			
			pp2d_draw_wtext(288 - pp2d_get_wtext_width(hpList[pkx_get_hp_type(pkmn)], FONT_SIZE_12, FONT_SIZE_12), 173, FONT_SIZE_12, FONT_SIZE_12, WHITE, hpList[pkx_get_hp_type(pkmn)]);

			for (int i = 0; i < 6; i++) {
				pksm_draw_texture(TEXTURE_MINUS, 96, 49 + i * 20);
				pksm_draw_texture(TEXTURE_PLUS, 139, 49 + i * 20);
				pksm_draw_texture(TEXTURE_MINUS, 177, 49 + i * 20);
				pksm_draw_texture(TEXTURE_PLUS, 218, 49 + i * 20);
			}
		}
		if (mode == ED_MOVES) {
			pp2d_draw_texture(TEXTURE_MOVES_BOTTOM, 0, 1);
			pp2d_draw_texture(TEXTURE_MOVES_BOTTOM, 0, 105);
			pksm_draw_texture(TEXTURE_BOTTOM_BAR, 0, 210);
			pp2d_draw_wtext(2, 5, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_MOVES));
			pp2d_draw_wtext(2, 110, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_RELEARN_MOVES));
			
			for (int i = 0; i < 4; i++) {
				pp2d_draw_wtext(2, 28 + i * 20, FONT_SIZE_12, FONT_SIZE_12, (i == additional3) ? YELLOW : WHITE, moves[pkx_get_move(pkmn, i)]);
				pp2d_draw_wtext(2, 132 + i * 20, FONT_SIZE_12, FONT_SIZE_12, (i == additional3 - 4) ? YELLOW: WHITE, moves[pkx_get_egg_move(pkmn, i)]);
				if (i == additional3)
					pksm_draw_texture(TEXTURE_SUB_ARROW_FLIPPED, 190 - movementOffsetSlow(3), 26 + i*20);
				else if (i == additional3 - 4)
					pksm_draw_texture(TEXTURE_SUB_ARROW_FLIPPED, 190 - movementOffsetSlow(3), 130 + i*20);
			}

		}
		if (mode == ED_HEX) {
			printMenuBottom();
			pksm_draw_texture(TEXTURE_BLUE_TEXT_BOX, 165, 28);
			if (sector[additional1][0] && !(sector[additional1][1])) {
				pksm_draw_texture(TEXTURE_MINUS_BUTTON, 224, 31);
				pksm_draw_texture(TEXTURE_PLUS_BUTTON, 247, 31);
			}
			if (hax)
				pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(255, 0, 0, 100));
			
			pp2d_draw_wtextf(155 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE), FONT_SIZE_14, FONT_SIZE_14), 30, FONT_SIZE_14, FONT_SIZE_14, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE));
			pp2d_draw_textf(171, 30, FONT_SIZE_14, FONT_SIZE_14, WHITE, "0x%02hhX", additional1);
			
			printfHexEditorInfo(pkmn, additional1);
			printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
		}

		if (mode != ED_HEX)
			pksm_draw_texture(TEXTURE_BACK_BUTTON, 283, 211);

		// apply masks
		if (mode == ED_ITEMS) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtext_center(GFX_BOTTOM, 105, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKEDITOR_ITEM_INDICATION));
		} else if (mode == ED_NATURES) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtext_center(GFX_BOTTOM, 105, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKEDITOR_NATURE_INDICATION));
		} else if (mode == ED_BALLS) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtext_center(GFX_BOTTOM, 105, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKEDITOR_BALL_INDICATION));
		} else if (mode == ED_HIDDENPOWER) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtext_center(GFX_BOTTOM, 105, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKEDITOR_HP_INDICATION));
		} else if (mode == ED_FORMS) {
			pp2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			pp2d_draw_wtext_center(GFX_BOTTOM, 105, FONT_SIZE_14, FONT_SIZE_14, WHITE, i18n(S_GRAPHIC_PKEDITOR_FORM_INDICATION));
		}
	pp2d_end_draw();
}

void printPKBank(u8* bankbuf, u8* mainbuf, u8* wirelessBuffer, u8* pkmnbuf, int currentEntry, int saveBox, int bankBox, bool isBufferized, bool isSeen, bool isWirelessActivated) {
	int game = game_get();
	
	int x, y;
	int pointer[2] = {0, 0};
	wchar_t page[MAX_LENGTH_BOX_NAME+1];
	
	u8 pkmn[PKMNLENGTH];
	if (currentEntry < 30)
		memcpy(pkmn, &bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
	else if (isWirelessActivated)
		memcpy(pkmn, &wirelessBuffer[saveBox * 30 * PKMNLENGTH + (currentEntry - 30) * PKMNLENGTH], PKMNLENGTH);
	else
		pkx_get(mainbuf, saveBox, currentEntry - 30, pkmn);
	
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		if (isSeen) {
			infoViewer(pkmnbuf);
		} else {
			printAnimatedBG(true);
			pp2d_draw_texture(TEXTURE_BANK_TOP, 34, 5);
			swprintf(page, MAX_LENGTH_BOX_NAME+1, i18n(S_GRAPHIC_PKBANK_BANK_TITLE), bankBox + 1);
			pp2d_draw_wtext(55 + (178 - pp2d_get_wtext_width(page, FONT_SIZE_12, FONT_SIZE_12)) / 2, 9, FONT_SIZE_12, FONT_SIZE_12, WHITE, page);

			if (pkx_get_species(pkmn) > 0 && pkx_get_species(pkmn) <= ofs.totalSpecies) {
				u16 tempspecies = pkx_get_form_species_number(pkmn);
				u8 type1 = 0, type2 = 0;

				if (tempspecies == 493 || tempspecies == 773) {
					type1 = pkx_get_form(pkmn);
					if (game_isgen4() && type1 > 9)
						type1--;
					type2 = type1;
				} else {
					type1 = personal.pkmData[tempspecies][0x6];
					type2 = personal.pkmData[tempspecies][0x7];
				}
				
				pp2d_draw_texture_part(TEXTURE_TYPES_SPRITESHEET, 273, 120, 50 * type1, 0, 50, 18); 
				if (type1 != type2)
					pp2d_draw_texture_part(TEXTURE_TYPES_SPRITESHEET, 325, 120, 50 * type2, 0, 50, 18); 
				
				u32 nick[NICKNAMELENGTH*2] = {0};
				pkx_get_nickname(pkmn, nick);
				pp2d_draw_wtext(273, 69, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)nick);
				
				wchar_t level[8];
				swprintf(level, 8, i18n(S_GRAPHIC_PKBANK_LV_PKMN), pkx_get_level(pkmn));
				float width = pp2d_get_wtext_width(level, FONT_SIZE_12, FONT_SIZE_12);
				pp2d_draw_wtext(372 - width, 86, FONT_SIZE_12, FONT_SIZE_12, WHITE, level);
				
				if (pkx_get_gender(pkmn) == 0)
					pksm_draw_texture(TEXTURE_MALE, 358 - width, 86);
				else if (pkx_get_gender(pkmn) == 1)
					pksm_draw_texture(TEXTURE_FEMALE, 360 - width, 86);
				if (pkx_is_shiny(pkmn))
					pksm_draw_texture(TEXTURE_SHINY, 360 - width - 14, 88);
				
				u32 ot_name[NICKNAMELENGTH*2] = {0};
				pkx_get_ot(pkmn, ot_name);
				pp2d_draw_wtext(273, 146, FONT_SIZE_12, FONT_SIZE_12, WHITE, (wchar_t*)ot_name);

				wchar_t otid[12];
				swprintf(otid, 12, i18n(S_GRAPHIC_PKBANK_OTID_PKMN), pkx_get_tid(pkmn));
				pp2d_draw_wtext(372 - pp2d_get_wtext_width(otid, FONT_SIZE_12, FONT_SIZE_12), 163, FONT_SIZE_12, FONT_SIZE_12, WHITE, otid);
				
				pp2d_draw_wtext(273, 104, FONT_SIZE_12, FONT_SIZE_12, WHITE, listSpecies.items[pkx_get_species(pkmn)]);
			}
			
			y = 45;
			for (int i = 0; i < 5; i++) {
				x = 44;
				for (int j = 0; j < 6; j++) {
					memcpy(pkmn, &bankbuf[bankBox * 30 * PKMNLENGTH + (i * 6 + j) * PKMNLENGTH], PKMNLENGTH);
					u16 n = pkx_get_species(pkmn);
					if (n)
						printElement(pkmn, GAME_SUN, n, x, y);

					if (currentEntry == (i * 6 + j)) {
						pointer[0] = x + 18;
						pointer[1] = y - 8;
					}
					x += 34;
				}
				y += 30;
			}
			
			if (currentEntry < 30) {
				u16 n = pkx_get_species(pkmnbuf);
				if (n) {
					printElementBlend(pkmnbuf, GAME_SUN, n, pointer[0] - 14, pointer[1] + 8, RGBA8(0x0, 0x0, 0x0, 100));
					printElement(pkmnbuf, GAME_SUN, n, pointer[0] - 18, pointer[1] + 3);
				}
				pksm_draw_texture(TEXTURE_SELECTOR_NORMAL, pointer[0], pointer[1] - 2 - ((!isBufferized) ? movementOffsetSlow(3) : 0));
			}
		}

		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_BOX_VIEW, 0, 0);
		pksm_draw_texture(TEXTURE_BOTTOM_BAR, 0, 210);
		swprintf(page, MAX_LENGTH_BOX_NAME+1, isWirelessActivated ? L"Wireless %d" : i18n(S_GRAPHIC_PKBANK_SAVED_BOX_TITLE), saveBox + 1);
		pp2d_draw_wtext(12 + (178 - pp2d_get_wtext_width(page, FONT_SIZE_12, FONT_SIZE_12)) / 2, 19, FONT_SIZE_12, FONT_SIZE_12, WHITE, page);
		pksm_draw_texture(TEXTURE_LEFT, 7, 17);
		pksm_draw_texture(TEXTURE_RIGHT, 185, 17);
		pksm_draw_texture(TEXTURE_TRANSFER_BUTTON, 242, 5);
		pksm_draw_texture(TEXTURE_BACK_BUTTON, 283, 211);
		pksm_draw_texture(TEXTURE_BUTTON, 208, 43);
		pksm_draw_texture(TEXTURE_BUTTON, 208, 70);
		pksm_draw_texture(TEXTURE_BUTTON, 208, 97);
		pksm_draw_texture(TEXTURE_BUTTON, 208, 153);
		pksm_draw_texture(TEXTURE_BUTTON, 208, 180);
		pp2d_draw_wtext(208 + (109 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKBANK_MENU_VIEW), FONT_SIZE_12, FONT_SIZE_12)) / 2, 50, FONT_SIZE_12, FONT_SIZE_12, BLACK, i18n(S_GRAPHIC_PKBANK_MENU_VIEW));
		pp2d_draw_wtext(208 + (109 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKBANK_MENU_CLEARBOX), FONT_SIZE_12, FONT_SIZE_12)) / 2, 77, FONT_SIZE_12, FONT_SIZE_12, BLACK, i18n(S_GRAPHIC_PKBANK_MENU_CLEARBOX));
		pp2d_draw_wtext(208 + (109 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKBANK_MENU_RELEASE), FONT_SIZE_12, FONT_SIZE_12)) / 2, 104, FONT_SIZE_12, FONT_SIZE_12, BLACK, i18n(S_GRAPHIC_PKBANK_MENU_RELEASE));
		pp2d_draw_wtext(208 + (109 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKBANK_MENU_DEX), FONT_SIZE_12, FONT_SIZE_12)) / 2, 160, FONT_SIZE_12, FONT_SIZE_12, BLACK, i18n(S_GRAPHIC_PKBANK_MENU_DEX));
		pp2d_draw_wtext(208 + (109 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKBANK_MENU_DUMP), FONT_SIZE_12, FONT_SIZE_12)) / 2, 187, FONT_SIZE_12, FONT_SIZE_12, BLACK, i18n(S_GRAPHIC_PKBANK_MENU_DUMP));
		pksm_draw_texture(TEXTURE_OTA_BUTTON, 240, 211);
		
		y = 45;
		for (int i = 0; i < 5; i++) {
			x = 4;
			for (int j = 0; j < 6; j++) {
				if (isWirelessActivated)
					memcpy(pkmn, wirelessBuffer + saveBox*30*PKMNLENGTH + (i*6+j)*PKMNLENGTH, PKMNLENGTH);
				else
					pkx_get(mainbuf, saveBox, i*6+j, pkmn);
				u16 n = pkx_get_species(pkmn);
				if (n)
					printElement(pkmn, game, n, x, y);

				if ((currentEntry - 30) == (i * 6 + j)) {
					pointer[0] = x + 18;
					pointer[1] = y - 8;
				}
				x += 34;
			}
			y += 30;
		}
		
		if (currentEntry > 29) {
			if (!isSeen) {
				u16 n = pkx_get_species(pkmnbuf);
				if (n) {
					printElementBlend(pkmnbuf, GAME_SUN, n, pointer[0] - 14, pointer[1] + 8, RGBA8(0x0, 0x0, 0x0, 100));
					printElement(pkmnbuf, GAME_SUN, n, pointer[0] - 18, pointer[1] + 3);
				}
				pksm_draw_texture(TEXTURE_SELECTOR_NORMAL, pointer[0], pointer[1] - 2 - ((!isBufferized) ? movementOffsetSlow(3) : 0));
			} else
				pksm_draw_texture(TEXTURE_SELECTOR_NORMAL, pointer[0], pointer[1] - 2);
		}
		
		if (isSeen)
			pp2d_draw_rectangle(0, -30, 320, 240, MASKBLACK);
		
		if (bank_getIsInternetWorking())
			pp2d_draw_wtextf(10, 220, FONT_SIZE_9, FONT_SIZE_9, WHITE, i18n(S_HTTP_SERVER_RUNNING), socket_get_ip());
	pp2d_end_draw();
}

void printSettings(u8* config_buf, int currentEntry, int configSize, wchar_t* descriptions[]) {
	pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		pp2d_draw_texture(TEXTURE_HEX_BG, 0, 0);
		
		for (int rows = 0; rows < 3; rows++)
		{
			for (int columns = 0; columns < 16; columns++)
			{
				int byte = rows*16 + columns;
				if (currentEntry == byte)
					printSelector(columns*25, rows*15, 24, 14);
				pp2d_draw_textf(4 + 25*columns, 15*rows, FONT_SIZE_11, FONT_SIZE_11, WHITE, "%02hhX", config_buf[byte]);
				
				if (byte == configSize - 1) break;
			}
		}
		
		pp2d_draw_wtextf(4, 225, FONT_SIZE_11, FONT_SIZE_11, LIGHTBLUE, L"%ls", descriptions[currentEntry]);

		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		pksm_draw_texture(TEXTURE_BLUE_TEXT_BOX, 165, 28);
		pksm_draw_texture(TEXTURE_MINUS_BUTTON, 224, 31);
		pksm_draw_texture(TEXTURE_PLUS_BUTTON, 247, 31);		
		pp2d_draw_wtextf(155 - pp2d_get_wtext_width(i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE), FONT_SIZE_14, FONT_SIZE_14), 30, FONT_SIZE_14, FONT_SIZE_14, LIGHTBLUE, i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE));
		pp2d_draw_textf(171, 30, FONT_SIZE_14, FONT_SIZE_14, WHITE, "0x%02hhX", currentEntry);
		
		printfConfigEditorInfo(currentEntry);
		printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
	pp2d_end_draw();
}

void printfConfigEditorInfo(int currentEntry)
{
	int y = 70, x = 8;
	switch (currentEntry) {
		case 0x00:
			pp2d_draw_text(50, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, "0x00: JP\n0x01: EN\n0x02: FR\n0x03: DE\n0x04: IT\n0x05: ES");
			pp2d_draw_text(200, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, "0x06: ZH\n0x07: KO\n0x08: NL\n0x09: PT\n0x0A: RU\n0x0B: TW");
			break;
		case 0x02:
		case 0x03:
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, L"Storage size: %d", PKSM_Configuration.storageSize);
			break;
		case 0x04:
		case 0x05:
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, L"Default TID: %d", PKSM_Configuration.defaultTID);
			break;
		case 0x06:
		case 0x07:
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, L"Default SID: %d", PKSM_Configuration.defaultSID);
			break;
		case 0x20:
			break;
	}
}

void printfHexEditorInfo(u8* pkmn, int byte) {
	int y = 70, x = 8, xribbon = 90;
	u32 string[NICKNAMELENGTH*2];
	memset(string, 0, NICKNAMELENGTH*2*sizeof(u32));
	
	switch (byte) {
		case 0x08 :
		case 0x09 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_SPECIES), pkx_get_species(pkmn), listSpecies.items[pkx_get_species(pkmn)]);
			break;
		case 0x0A :
		case 0x0B :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_ITEM), pkx_get_item(pkmn), items[pkx_get_item(pkmn)]);
			break;
		case 0x0C :
		case 0x0D :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_TID), pkx_get_tid(pkmn));
			pp2d_draw_textf(x, y + 16, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, "Gen7 TID: %d", ((unsigned int)(pkx_get_tid(pkmn) + pkx_get_sid(pkmn)*65536) % 1000000));
			break;
		case 0x0E :
		case 0x0F :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_SID), pkx_get_sid(pkmn));
			pp2d_draw_textf(x, y + 16, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, "Gen7 TID: %d", ((unsigned int)(pkx_get_tid(pkmn) + pkx_get_sid(pkmn)*65536) % 1000000));
			break;
		case 0x14 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ABILITY), abilities[pkx_get_ability(pkmn)]);
			break;
		case 0x1C :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_NATURE), natures[pkx_get_nature(pkmn)]);
			break;
		case 0x1D : {
			wchar_t* entries[] = { i18n(S_GRAPHIC_HEXEDITOR_FATEFUL_ENCOUNTER_FLAG) };
			int i = pp2d_get_wtext_width(i18n(S_GRAPHIC_HEXEDITOR_GENDER), FONT_SIZE_12, FONT_SIZE_12);
			pp2d_draw_wtext(xribbon + 27, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, entries[0]);
			pp2d_draw_rectangle(xribbon, y, 13, 13, (pkmn[byte] & 1) ? BUTTONGREEN : BUTTONRED);
			pp2d_draw_wtext(x, y + 17, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_GENDER));
			if (pkx_get_gender(pkmn) == 0)
				pp2d_draw_wtext(x + i, y + 17, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_GENDER_MALE));
			else if (pkx_get_gender(pkmn) == 1)
				pp2d_draw_wtext(x + i, y + 17, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_GENDER_FEMALE));
			else
				pp2d_draw_wtext(x + i, y + 17, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_GENDER_GENDERLESS));
			pp2d_draw_wtextf(x, y + 17 * 2, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ALTERNATIVE_FORM), pkmn[byte] >> 3);
			break;
		}
		case 0x1E :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EV_HP), pkmn[byte]);
			break;
		case 0x1F : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EV_ATK), pkmn[byte]);
			break;
		case 0x20 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EV_DEF), pkmn[byte]);
			break;	
		case 0x21 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EV_SPE), pkmn[byte]);
			break;
		case 0x22 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EV_SPA), pkmn[byte]);
			break;	
		case 0x23 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EV_SPD), pkmn[byte]);
			break;	
		case 0x24 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_COOL), pkmn[byte]);
			break;	
		case 0x25 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_BEAUTY), pkmn[byte]);
			break;	
		case 0x26 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_CUTE), pkmn[byte]);
			break;	
		case 0x27 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_SMART), pkmn[byte]);
			break;	
		case 0x28 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_TOUGH), pkmn[byte]);
			break;	
		case 0x29 : 
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_SHEEN), pkmn[byte]);
			break;
		case 0x30 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_KALOS, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_G3_HOENN, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_SINNOH, S_GRAPHIC_HEXEDITOR_RIBBON_BEST_FRIENDS, S_GRAPHIC_HEXEDITOR_RIBBON_TRAINING,S_GRAPHIC_HEXEDITOR_RIBBON_SKILLFUL_BATTLER, S_GRAPHIC_HEXEDITOR_RIBBON_BATTLER_EXPERT, S_GRAPHIC_HEXEDITOR_RIBBON_EFFORT };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 0, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x31 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_ALERT, S_GRAPHIC_HEXEDITOR_RIBBON_SHOCK, S_GRAPHIC_HEXEDITOR_RIBBON_DOWNCAST, S_GRAPHIC_HEXEDITOR_RIBBON_CARELESS, S_GRAPHIC_HEXEDITOR_RIBBON_RELAX, S_GRAPHIC_HEXEDITOR_RIBBON_SNOOZE, S_GRAPHIC_HEXEDITOR_RIBBON_SMILE, S_GRAPHIC_HEXEDITOR_RIBBON_GORGEOUS };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 1, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x32 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_ROYAL, S_GRAPHIC_HEXEDITOR_RIBBON_GORGEOUS_ROYAL, S_GRAPHIC_HEXEDITOR_RIBBON_ARTIST, S_GRAPHIC_HEXEDITOR_RIBBON_FOOTPRINT, S_GRAPHIC_HEXEDITOR_RIBBON_RECORD, S_GRAPHIC_HEXEDITOR_RIBBON_LEGEND, S_GRAPHIC_HEXEDITOR_RIBBON_COUNTRY, S_GRAPHIC_HEXEDITOR_RIBBON_NATIONAL };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 2, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x33 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_EARTH, S_GRAPHIC_HEXEDITOR_RIBBON_WORLD, S_GRAPHIC_HEXEDITOR_RIBBON_CLASSIC, S_GRAPHIC_HEXEDITOR_RIBBON_PREMIER, S_GRAPHIC_HEXEDITOR_RIBBON_EVENT, S_GRAPHIC_HEXEDITOR_RIBBON_BIRTHDAY, S_GRAPHIC_HEXEDITOR_RIBBON_SPECIAL, S_GRAPHIC_HEXEDITOR_RIBBON_SOUVENIR };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 3, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x34 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_WISHING, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_BATTLE, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_REGIONAL, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_NATIONAL, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_WORLD, S_GRAPHIC_HEXEDITOR_RIBBON_38, S_GRAPHIC_HEXEDITOR_RIBBON_39, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_G6_HOENN };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 4, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x35 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_CONTEST_STAR, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_COOLNESS, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_BEAUTY, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_CUTENESS, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_CLEVERNESS, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_TOUGHNESS, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_ALOLA, S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_ROYALE };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 5, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x36 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_TREE_GREAT, S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_TREE_MASTER, S_GRAPHIC_HEXEDITOR_RIBBON_51, S_GRAPHIC_HEXEDITOR_RIBBON_52, S_GRAPHIC_HEXEDITOR_RIBBON_53, S_GRAPHIC_HEXEDITOR_RIBBON_54, S_GRAPHIC_HEXEDITOR_RIBBON_55, S_GRAPHIC_HEXEDITOR_RIBBON_56 };
			for (int i = 0; i < 8; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17*i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 6, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x40 :
		case 0x41 :
		case 0x42 :
		case 0x43 :
		case 0x44 :
		case 0x45 :
		case 0x46 :
		case 0x47 :
		case 0x48 :
		case 0x49 :
		case 0x4A :
		case 0x4B :
		case 0x4C :
		case 0x4D :
		case 0x4E :
		case 0x4F :
		case 0x50 :
		case 0x51 :
		case 0x52 :
		case 0x53 :
		case 0x54 :
		case 0x55 :
		case 0x56 :
		case 0x57 :
			pkx_get_nickname(pkmn, string);
			pp2d_draw_wtext(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_NICKNAME));
			pp2d_draw_wtext(x + pp2d_get_wtext_width(i18n(S_GRAPHIC_HEXEDITOR_NICKNAME), FONT_SIZE_12, FONT_SIZE_12), y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, (wchar_t*)string);
			break;
		case 0x5A :
		case 0x5B :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE1), pkx_get_move(pkmn, 0), moves[pkx_get_move(pkmn, 0)]);
			break;
		case 0x5C :
		case 0x5D :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE2), pkx_get_move(pkmn, 1), moves[pkx_get_move(pkmn, 1)]);
			break;
		case 0x5E :
		case 0x5F :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE3), pkx_get_move(pkmn, 2), moves[pkx_get_move(pkmn, 2)]);
			break;
		case 0x60 :
		case 0x61 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE4), pkx_get_move(pkmn, 3), moves[pkx_get_move(pkmn, 3)]);
			break;
		case 0x62 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE1_PP), pkmn[byte]);
			break;
		case 0x63 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE2_PP), pkmn[byte]);
			break;
		case 0x64 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE3_PP), pkmn[byte]);
			break;
		case 0x65 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE4_PP), pkmn[byte]);
			break;
		case 0x66 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE1_PPUP), pkmn[byte]);
			break;
		case 0x67 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE2_PPUP), pkmn[byte]);
			break;
		case 0x68 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE3_PPUP), pkmn[byte]);
			break;
		case 0x69 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MOVE4_PPUP), pkmn[byte]);
			break;
		case 0x6A :
		case 0x6B :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE1), pkx_get_egg_move(pkmn, 0), moves[pkx_get_egg_move(pkmn, 0)]);
			break;
		case 0x6C :
		case 0x6D :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE2), pkx_get_egg_move(pkmn, 1), moves[pkx_get_egg_move(pkmn, 1)]);
			break;
		case 0x6E :
		case 0x6F :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE3), pkx_get_egg_move(pkmn, 2), moves[pkx_get_egg_move(pkmn, 2)]);
			break;
		case 0x70 :
		case 0x71 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE4), pkx_get_egg_move(pkmn, 3), moves[pkx_get_egg_move(pkmn, 3)]);
			break;
		case 0x77: {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_IS_NICKNAMED, S_GRAPHIC_HEXEDITOR_IS_EGG };
			pp2d_draw_wtext(xribbon + 27, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[0]));
			pp2d_draw_rectangle(xribbon, y, 13, 13, (pkx_get_nickname_flag(pkmn)) ? BUTTONGREEN : BUTTONRED);
			pp2d_draw_wtext(xribbon + 27, y + 17, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[1]));
			pp2d_draw_rectangle(xribbon, y + 17, 13, 13, (pkx_is_egg(pkmn)) ? BUTTONGREEN : BUTTONRED);
			break;
		}
		case 0x78 :
		case 0x79 :
		case 0x7A :
		case 0x7B :
		case 0x7C :
		case 0x7D :
		case 0x7E :
		case 0x7F :
		case 0x80 :
		case 0x81 :
		case 0x82 :
		case 0x83 :
		case 0x84 :
		case 0x85 :
		case 0x86 :
		case 0x87 :
		case 0x88 :
		case 0x89 :
		case 0x8A :
		case 0x8B :	
		case 0x8C :
		case 0x8D :
		case 0x8E :
		case 0x8F :	
			pkx_get_ht(pkmn, string);
			pp2d_draw_wtext(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_NAME));
			pp2d_draw_wtext(x + pp2d_get_wtext_width(i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_NAME), FONT_SIZE_12, FONT_SIZE_12), y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, (wchar_t*)string);
			break;
		case 0xA2 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_FRIENDSHIP), pkmn[byte]);
			break;
		case 0xA3 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_AFFECTION), pkmn[byte]);
			break;
		case 0xA4 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_INTENSITY), pkmn[byte]);
			break;
		case 0xA5 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_MEMORY), pkmn[byte]);
			break;	
		case 0xA6 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_FEELING), pkmn[byte]);
			break;
		case 0xAE :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_FULLNESS), pkmn[byte]);
			break;	
		case 0xAF :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ENJOYMENT), pkmn[byte]);
			break;
		case 0xB0 :
		case 0xB1 :
		case 0xB2 :
		case 0xB3 :
		case 0xB4 :
		case 0xB5 :
		case 0xB6 :
		case 0xB7 :
		case 0xB8 :
		case 0xB9 :
		case 0xBA :
		case 0xBB :
		case 0xBC :
		case 0xBD :
		case 0xBE :
		case 0xBF :
		case 0xC0 :
		case 0xC1 :
		case 0xC2 :
		case 0xC3 :
		case 0xC4 :
		case 0xC5 :
		case 0xC6 :
		case 0xC7 :
			pkx_get_ot(pkmn, string);
			pp2d_draw_wtext(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_NAME));
			pp2d_draw_wtext(x + pp2d_get_wtext_width(i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_NAME), FONT_SIZE_12, FONT_SIZE_12), y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, (wchar_t*)string);
			break;
		case 0xCA :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_FRIENDSHIP), pkmn[byte]);
			break;
		case 0xCB :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_AFFECTION), pkmn[byte]);
			break;
		case 0xCC :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_INTENSITY), pkmn[byte]);
			break;
		case 0xCD :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_MEMORY), pkmn[byte]);
			break;	
		case 0xD0 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_FEELING), pkmn[byte]);
			break;
		case 0xD1 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EGG_YEAR), 2000 + pkmn[byte]);
			break;
		case 0xD2 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EGG_MONTH), pkmn[byte]);
			break;
		case 0xD3 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_EGG_DAY), pkmn[byte]);
			break;
		case 0xD4 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MET_YEAR), 2000 + pkmn[byte]);
			break;
		case 0xD5 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MET_MONTH), pkmn[byte]);
			break;
		case 0xD6 :
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MET_DAY), pkmn[byte]);
			break;
		case 0xDD : {
			pp2d_draw_wtextf(x, y, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_MET_LEVEL), (pkmn[byte] & 0x7f));
			pp2d_draw_wtextf(xribbon + 27, y + 17, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_GENDER));
			if (pkx_get_ot_gender(pkmn) == 0)
				pksm_draw_texture(TEXTURE_MALE, xribbon + 10, y + 17 + 2);
			else if (pkx_get_ot_gender(pkmn) == 1)
				pksm_draw_texture(TEXTURE_FEMALE, xribbon + 10 + 2, y + 17 + 2);
			break;
		}
		case 0xDE : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_HP, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_ATK, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_DEF, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPATK, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPDEF, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPEED };
			for (int i = 0; i < 6; i++) {
				pp2d_draw_wtext(xribbon + 27, y + 17 * i, FONT_SIZE_12, FONT_SIZE_12, LIGHTBLUE, i18n(entries[i]));
				pp2d_draw_rectangle(xribbon, y + 17 * i, 13, 13, (pkx_get_hti(pkmn, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
	}
}
