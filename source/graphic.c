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

u32 logoColors[15] = {RGBA8(0, 136, 222, 255), 
					RGBA8(203, 39, 69, 255),
					RGBA8(247, 139, 50, 255),
					RGBA8(115, 118, 252, 255),
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

int logo_lookup6[7] = {0, 64, 121, 183, 250, 320, 375};
int logo_lookup4[6] = {0, 54, 116, 177, 233, 291};
int logo_lookup5[5] = {0, 53, 104, 176, 252};

int lookup[] = {0x0, 0x1, 0x2, 0x4, 0x5, 0x3};

int MAX_LENGTH_BOX_NAME = 15;

sftd_font *fontBold15, *fontBold14, *fontBold12, *fontBold11, *fontBold9; 
sf2d_texture *generations, *eventView, *iconBank, *iconEditor, *iconEvents, *iconSave, *iconSettings, *iconCredits, *mainMenuButton, *noMove, *hexIcon, *hexBG, *blueTextBox, *otaButton, *generationBG, *includeInfoButton, *hiddenPowerBG, *ballsBG, *male, *female, *naturestx, *movesBottom, *topMovesBG, *editorBar, *editorStatsBG, *subArrow, *backgroundTop, *miniBox, *plusButton, *minusButton, *balls, *typesSheet, *transferButton, *bankTop, *shinyStar, *normalBar, *LButton, *RButton, *creditsTop, *pokeball, *gameSelectorBottom1, *gameSelectorBottom2, *gameSelectorTop, *menuBar, *darkButton, *left, *lightButton, *redButton, *right, *spritesSmall, *eventMenuBottomBar, *eventMenuTopBarSelected, *eventMenuTopBar, *warningTop, *warningBottom, *boxView, *infoView, *selector, *editorBG, *plus, *minus, *back, *setting, *selectorCloning, *button, *bottomPopUp, *pokemonBufferBox, *DSBottomBG, *DSTopBG, *DSBarSelected, *DSBar, *DSEventBottom, *DSLangSelected, *DSLang, *DSEventTop, *DSNormalBarL, *DSNormalBarR, *DSSelectedBarL, *DSSelectedBarR, *item, *alternativeSpritesSmall;

AppTextCode gamesList[] = {S_GRAPHIC_GAME_SELECTOR_GAME_X, S_GRAPHIC_GAME_SELECTOR_GAME_Y, S_GRAPHIC_GAME_SELECTOR_GAME_OS, S_GRAPHIC_GAME_SELECTOR_GAME_AS, S_GRAPHIC_GAME_SELECTOR_GAME_SUN, S_GRAPHIC_GAME_SELECTOR_GAME_MOON, S_GRAPHIC_GAME_SELECTOR_GAME_DIAMOND, S_GRAPHIC_GAME_SELECTOR_GAME_PEARL, S_GRAPHIC_GAME_SELECTOR_GAME_PLATINUM, S_GRAPHIC_GAME_SELECTOR_GAME_HG, S_GRAPHIC_GAME_SELECTOR_GAME_SS, S_GRAPHIC_GAME_SELECTOR_GAME_B, S_GRAPHIC_GAME_SELECTOR_GAME_W, S_GRAPHIC_GAME_SELECTOR_GAME_B2, S_GRAPHIC_GAME_SELECTOR_GAME_W2};
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
int loadPNGInRAM_elements = 1;

void GUITextsInit() {
	freezeMsg(i18n(S_GUI_ELEMENTS_LOADING_LOCALES));
	struct i18n_files listFiles = i18n_getFilesPath();

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_ABILITIES));
	listAbilities = i18n_FileToArrayUTF32(listFiles.abilities);

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_MOVES));
	listMoves = i18n_FileToArrayUTF32(listFiles.moves);

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_SORTING_MOVES));
	ArrayUTF32_sort_starting_index(&listMoves, 1);

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_NATURES));
	listNatures = i18n_FileToArrayUTF32(listFiles.natures);

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_ITEMS));
	listItems = i18n_FileToArrayUTF32(listFiles.items);
	listBalls = i18n_FileToArrayUTF32(listFiles.balls);

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_SORTING_ITEMS));
	ArrayUTF32_sort_starting_index_with_sort_func(&listItems, 1, ArrayUTF32_sort_cmp_PKMN_Things_List);

	freezeMsgDetails(i18n(S_GUI_ELEMENTS_LOADING_LOCALES_HP));
	listHPs = i18n_FileToArrayUTF32(listFiles.hp);

	// Need loading message for forms and species
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
	fontBold15 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold12 = sftd_load_font_file("romfs:/res/Bold.ttf");
	warningTop = sfil_load_PNG_file("romfs:/res/Warning Top.png", SF2D_PLACE_RAM);
	warningBottom = sfil_load_PNG_file("romfs:/res/Warning Bottom.png", SF2D_PLACE_RAM);
	freezeMsg(i18n(S_GUI_ELEMENTS_LOADING_FILES));

	fontBold14 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold11 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold9 = sftd_load_font_file("romfs:/res/Bold.ttf");

	GUITextsInit();

	freezeMsg(i18n(S_GUI_ELEMENTS_LOADING_FONTS));
	init_font_cache();
	freezeMsg(i18n(S_GUI_ELEMENTS_LOADING_DONE));
}

sf2d_texture *loadPNGInRAM(const char* filepath) {
	wchar_t* str = malloc(sizeof(wchar_t*)*60);
	swprintf(str, 60, i18n(S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING_DETAILS), loadPNGInRAM_current_element, loadPNGInRAM_elements);
	freezeMsgDetails(str);
	loadPNGInRAM_current_element++;
	free(str);
	return sfil_load_PNG_file(filepath, SF2D_PLACE_RAM);
}

void initProgressLoadPNGInRAM(int total) {
	loadPNGInRAM_current_element = 1;
	loadPNGInRAM_elements = total;
}


int getGUIElementsI18nSpecifyTotalElements() {
	int game = game_get();
	return IS3DS ? 1 : 0;
}

void GUIElementsI18nSpecify() {
	int game = game_get();
	struct i18n_files languageSpecificFiles = i18n_getFilesPath();
	if (IS3DS) {
		typesSheet = loadPNGInRAM(languageSpecificFiles.types);
	}
}

void GUIElementsSpecify() {
	int game = game_get();
	int elements = 4;
	if (IS3DS) {
		elements += 59;
	} else {
		elements += 16;
	}

	elements += getGUIElementsI18nSpecifyTotalElements();
	initProgressLoadPNGInRAM(elements);

	freezeMsg(i18n(S_GRAPHIC_GUI_ELEMENTS_SPECIFY_LOADING));
#if CITRA
	alternativeSpritesSmall = loadPNGInRAM("romfs:/citra/PKResources/additionalassets/alternative_icons_spritesheetv3.png");
	spritesSmall = loadPNGInRAM("romfs:/citra/PKResources/additionalassets/pokemon_icons_spritesheetv3.png");
	balls = loadPNGInRAM("romfs:/citra/PKResources/additionalassets/balls_spritesheetv2.png");
#else
	alternativeSpritesSmall = loadPNGInRAM("/3ds/data/PKSM/additionalassets/alternative_icons_spritesheetv3.png");
	spritesSmall = loadPNGInRAM("/3ds/data/PKSM/additionalassets/pokemon_icons_spritesheetv3.png");
	balls = loadPNGInRAM("/3ds/data/PKSM/additionalassets/balls_spritesheetv2.png");
#endif

	if (IS3DS) {
		generations = loadPNGInRAM("romfs:/res/generations.png");
		eventView = loadPNGInRAM("romfs:/res/Event View.png");
		iconBank = loadPNGInRAM("romfs:/res/Icon Bank.png");
		iconEditor = loadPNGInRAM("romfs:/res/Icon Editor.png");
		iconEvents = loadPNGInRAM("romfs:/res/Icon Events.png");
		iconSave = loadPNGInRAM("romfs:/res/Icon Save.png");
		iconSettings = loadPNGInRAM("romfs:/res/Icon Settings.png");
		iconCredits = loadPNGInRAM("romfs:/res/Icon Credits.png");
		mainMenuButton = loadPNGInRAM("romfs:/res/Main Menu Button.png");
		
		boxView = loadPNGInRAM("romfs:/res/Box View.png");
		noMove = loadPNGInRAM("romfs:/res/No Move.png");
		back = loadPNGInRAM("romfs:/res/Back Button.png");
		editorBar = loadPNGInRAM("romfs:/res/Bottom Bar.png");
		hexIcon = loadPNGInRAM("romfs:/res/Hex Button.png");
		hexBG = loadPNGInRAM("romfs:/res/Hex BG.png");
		blueTextBox = loadPNGInRAM("romfs:/res/Blue Textbox.png");
		otaButton = loadPNGInRAM("romfs:/res/OTA Button.png");
		includeInfoButton = loadPNGInRAM("romfs:/res/Include Info.png");
		generationBG = loadPNGInRAM("romfs:/res/Generation BG.png");
		hiddenPowerBG = loadPNGInRAM("romfs:/res/Hidden Power BG.png");
		ballsBG = loadPNGInRAM("romfs:/res/BallsBG.png");
		male = loadPNGInRAM("romfs:/res/Male.png");
		female = loadPNGInRAM("romfs:/res/Female.png");
		naturestx = loadPNGInRAM("romfs:/res/Natures.png");
		movesBottom = loadPNGInRAM("romfs:/res/Moves Bottom.png");
		topMovesBG = loadPNGInRAM("romfs:/res/Top Moves.png");
		editorStatsBG = loadPNGInRAM("romfs:/res/Editor Stats.png");
		subArrow = loadPNGInRAM("romfs:/res/Sub Arrow.png");
		backgroundTop = loadPNGInRAM("romfs:/res/Background.png");
		miniBox = loadPNGInRAM("romfs:/res/Mini Box.png");
		minusButton = loadPNGInRAM("romfs:/res/Minus Button.png");
		plusButton = loadPNGInRAM("romfs:/res/Plus Button.png");
		transferButton = loadPNGInRAM("romfs:/res/Transfer Button.png");
		bankTop = loadPNGInRAM("romfs:/res/Bank Top.png");
		shinyStar = loadPNGInRAM("romfs:/res/Shiny.png");
		normalBar = loadPNGInRAM("romfs:/res/Normal Bar.png");
		RButton = loadPNGInRAM("romfs:/res/R Button.png");
		LButton = loadPNGInRAM("romfs:/res/L Button.png");
		creditsTop = loadPNGInRAM("romfs:/res/Credits Top.png");
		pokeball = loadPNGInRAM("romfs:/res/Pokeball.png");
		menuBar = loadPNGInRAM("romfs:/res/Main Menu Dark Bar.png");
		darkButton = loadPNGInRAM("romfs:/res/Dark Button.png");
		left = loadPNGInRAM("romfs:/res/Left.png");
		lightButton = loadPNGInRAM("romfs:/res/Light Button.png");
		redButton = loadPNGInRAM("romfs:/res/Red Button.png");
		right = loadPNGInRAM("romfs:/res/Right.png");
		eventMenuBottomBar = loadPNGInRAM("romfs:/res/Event Menu Bottom Bar.png");
		eventMenuTopBarSelected = loadPNGInRAM("romfs:/res/Event Menu Top Bar Selected.png");
		eventMenuTopBar = loadPNGInRAM("romfs:/res/Event Menu Top Bar.png");
		infoView = loadPNGInRAM("romfs:/res/Info View.png");
		selector = loadPNGInRAM("romfs:/res/Selector.png");
		selectorCloning = loadPNGInRAM("romfs:/res/Selector (cloning).png");
		editorBG = loadPNGInRAM("romfs:/res/Editor Bottom BG.png");
		plus = loadPNGInRAM("romfs:/res/Plus.png");
		minus = loadPNGInRAM("romfs:/res/Minus.png");
		button = loadPNGInRAM("romfs:/res/Button.png");
		setting = loadPNGInRAM("romfs:/res/Setting.png");
		bottomPopUp = loadPNGInRAM("romfs:/res/Bottom Pop-Up.png");
		pokemonBufferBox = loadPNGInRAM("romfs:/res/Pokemon Box.png");
		item = loadPNGInRAM("romfs:/res/item.png");
	} else {
		pokeball = loadPNGInRAM("romfs:/res/DS Pokeball.png");
		DSBottomBG = loadPNGInRAM("romfs:/res/Bottom BG.png");
		DSTopBG = loadPNGInRAM("romfs:/res/Top BG.png");
		DSBarSelected = loadPNGInRAM("romfs:/res/Bar Selected.png");
		DSBar = loadPNGInRAM("romfs:/res/Bar.png");
		DSEventBottom = loadPNGInRAM("romfs:/res/DS Menu Bottom BG.png");
		DSLangSelected = loadPNGInRAM("romfs:/res/Language Button Selected.png");
		DSLang = loadPNGInRAM("romfs:/res/Language Button.png");
		DSEventTop = loadPNGInRAM("romfs:/res/Event Database BG.png");
		DSNormalBarL = loadPNGInRAM("romfs:/res/Normal L.png");
		DSNormalBarR = loadPNGInRAM("romfs:/res/Normal R.png");
		DSSelectedBarL = loadPNGInRAM("romfs:/res/Selected L.png");
		DSSelectedBarR = loadPNGInRAM("romfs:/res/Selected R.png");
		
		hexBG = loadPNGInRAM("romfs:/res/Hex BG.png");
		minusButton = loadPNGInRAM("romfs:/res/Minus Button.png");
		plusButton = loadPNGInRAM("romfs:/res/Plus Button.png");
	}

	GUIElementsI18nSpecify();
}


void GUIGameElementsInit() {
	gameSelectorBottom2 = sfil_load_PNG_file("romfs:/res/LogosGen5.png", SF2D_PLACE_RAM);
	gameSelectorBottom1 = sfil_load_PNG_file("romfs:/res/LogosGen4.png", SF2D_PLACE_RAM);
	gameSelectorTop = sfil_load_PNG_file("romfs:/res/Logos.png", SF2D_PLACE_RAM);
}

void GUIGameElementsExit() {
	sf2d_free_texture(gameSelectorBottom2);
	sf2d_free_texture(gameSelectorBottom1);
	sf2d_free_texture(gameSelectorTop);
}

void GUIElementsExit() {
	sf2d_free_texture(generations);
	sf2d_free_texture(eventView);
	
	sf2d_free_texture(iconBank);
	sf2d_free_texture(iconCredits);
	sf2d_free_texture(iconEditor);
	sf2d_free_texture(iconEvents);
	sf2d_free_texture(iconSave);
	sf2d_free_texture(iconSettings);
	sf2d_free_texture(mainMenuButton);
	
	sf2d_free_texture(noMove);
	sf2d_free_texture(hexIcon);
	sf2d_free_texture(hexBG);
	sf2d_free_texture(blueTextBox);
	sf2d_free_texture(otaButton);
	sf2d_free_texture(generationBG);
	sf2d_free_texture(includeInfoButton);
	sf2d_free_texture(hiddenPowerBG);
	sf2d_free_texture(ballsBG);
	sf2d_free_texture(male);
	sf2d_free_texture(female);
	sf2d_free_texture(naturestx);
	sf2d_free_texture(movesBottom);
	sf2d_free_texture(topMovesBG);
	sf2d_free_texture(editorBar);
	sf2d_free_texture(editorStatsBG);
	sf2d_free_texture(subArrow);
	sf2d_free_texture(backgroundTop);
	sf2d_free_texture(miniBox);
	sf2d_free_texture(minusButton);
	sf2d_free_texture(plusButton);
	sf2d_free_texture(balls);
	sf2d_free_texture(transferButton);
	sf2d_free_texture(bankTop);
	sf2d_free_texture(shinyStar);
	sf2d_free_texture(normalBar);
	sf2d_free_texture(RButton);
	sf2d_free_texture(LButton);
	sf2d_free_texture(creditsTop);
	sf2d_free_texture(pokeball);
	sf2d_free_texture(alternativeSpritesSmall);
	sf2d_free_texture(item);
	sf2d_free_texture(DSEventBottom);
	sf2d_free_texture(DSLangSelected);
	sf2d_free_texture(DSLang);
	sf2d_free_texture(DSEventTop);
	sf2d_free_texture(DSNormalBarL);
	sf2d_free_texture(DSNormalBarR);
	sf2d_free_texture(DSSelectedBarL);
	sf2d_free_texture(DSSelectedBarR);
	sf2d_free_texture(DSBottomBG);
	sf2d_free_texture(DSTopBG);
	sf2d_free_texture(DSBarSelected);
	sf2d_free_texture(DSBar);
	sf2d_free_texture(bottomPopUp);
	sf2d_free_texture(pokemonBufferBox);
	sf2d_free_texture(selectorCloning);
	sf2d_free_texture(back);
	sf2d_free_texture(setting);
	sf2d_free_texture(editorBG);
	sf2d_free_texture(plus);
	sf2d_free_texture(minus);
	sf2d_free_texture(button);
	sf2d_free_texture(boxView);
	sf2d_free_texture(infoView);
	sf2d_free_texture(selector);
	sf2d_free_texture(warningTop);
	sf2d_free_texture(warningBottom);
	sf2d_free_texture(eventMenuBottomBar);
	sf2d_free_texture(eventMenuTopBarSelected);
	sf2d_free_texture(eventMenuTopBar);
	sf2d_free_texture(spritesSmall);
	sf2d_free_texture(darkButton);
	sf2d_free_texture(left);
	sf2d_free_texture(lightButton);
	sf2d_free_texture(redButton);
	sf2d_free_texture(right);
	sf2d_free_texture(menuBar);
	sftd_free_font(fontBold9);
	sftd_free_font(fontBold11);
	sftd_free_font(fontBold12);
	sftd_free_font(fontBold14);
	sftd_free_font(fontBold15);

	GUIElementsI18nExit();
}

void GUITextsExit() {
	i18n_free_ArrayUTF32(&listAbilities);
	i18n_free_ArrayUTF32(&listMoves);
	i18n_free_ArrayUTF32(&listNatures);
	i18n_free_ArrayUTF32(&listItems);
	i18n_free_ArrayUTF32(&listBalls);
	i18n_free_ArrayUTF32(&listHPs);
	i18n_free_ArrayUTF32(&listForms);
	i18n_free_ArrayUTF32(&listSpecies);
}

void GUIElementsI18nExit() {
	sf2d_free_texture(typesSheet);
	GUITextsExit();
}


void create_font_cache(sftd_font* font, unsigned int size, int total_fonts, int* num_font) {
	wchar_t str[60];
	swprintf(str, 60, i18n(S_GUI_ELEMENTS_LOADING_FONTS_CACHE_FONT), *num_font, total_fonts);
	freezeMsgDetails(str);
	sftd_draw_text(font, 0, 0, RGBA8(0, 0, 0, 0), size, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	*num_font = *num_font + 1;
}

void init_font_cache() {
	int num_font = 1;
	int total_fonts = 5;

	create_font_cache(fontBold14, 14, total_fonts, &num_font);
	create_font_cache(fontBold15, 15, total_fonts, &num_font);
	create_font_cache(fontBold12, 12, total_fonts, &num_font);
	create_font_cache(fontBold11, 11, total_fonts, &num_font);
	create_font_cache(fontBold9,  9, total_fonts, &num_font);
}

void drawFPSDebug() {
	sf2d_draw_rectangle(10, 10, 108, 20, RGBA8(0, 0, 0, 160));
	sftd_draw_textf(fontBold12, 12, 13, WHITE, 12, "FPS: %2.6f", sf2d_get_fps());
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
		if (sf2d_get_current_screen() == GFX_BOTTOM) {
			if ((lastDebugMessageTime == 0 || (currentTime - lastDebugMessageTime) < 5)) {
				sf2d_draw_rectangle(0, 0, 400, 20, RGBA8(0, 0, 0, 200));
				sftd_draw_textf(fontBold12, 2, 3, WHITE, 12, "[Debug] %s", messageDebug);
				if (lastDebugMessageTime == 0) {
					lastDebugMessageTime = currentTime;
				}
			} else {
				hasDebugMessage = false;
				lastDebugMessageTime = 0;
			}
		}
	}
}
void pksm_end_frame() {
	#if DEBUG
		drawFPSDebug();
	#endif
	#if DEBUG_I18N
		drawConsoleDebug();
	#endif
	sf2d_end_frame();
}

void printSelector(int x, int y, int width, int height) {
	sf2d_draw_rectangle(x, y, width, height, BUTTONBORD);
	sf2d_draw_rectangle(x + 1, y + 1, width - 2, height - 2, BUTTONGREY);
}

void printMenuTop() {
	sf2d_draw_rectangle(0, 0, 400, 240, MENUBLUE);
	sf2d_draw_rectangle(0, 0, 400, 25, HIGHBLUE);
}

void printMenuBottom() {
	sf2d_draw_rectangle(0, 0, 320, 240, PALEBLUE);
	sf2d_draw_rectangle(0, 219, 320, 21, MENUBLUE);
}

void infoDisp(wchar_t* message) {
	while (aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_A) break;
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(warningTop, 0, 0);
			sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, message)) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, message);
			sftd_draw_wtext(fontBold12, (400 - sftd_get_wtext_width(fontBold12, 12, i18n(S_INFORMATION_MESSAGE_PRESS_A))) / 2, 130, WHITE, 12, i18n(S_INFORMATION_MESSAGE_PRESS_A));
		pksm_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(warningBottom, 0, 0);
		pksm_end_frame();
		sf2d_swapbuffers();
	}
}

int confirmDisp(wchar_t* message) {
	while (aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_A) return 1;
		if (hidKeysDown() & KEY_B) return 0;
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(warningTop, 0, 0);
			sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, message)) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, message);
			sftd_draw_wtext(fontBold12, (400 - sftd_get_wtext_width(fontBold12, 12, i18n(S_CONFIRMATION_MESSAGE_PRESS_A_OR_B))) / 2, 130, WHITE, 12, i18n(S_CONFIRMATION_MESSAGE_PRESS_A_OR_B));
		pksm_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(warningBottom, 0, 0);
		pksm_end_frame();
		sf2d_swapbuffers();
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

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(warningTop, 0, 0);
		sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, lastMessage)) / 2, 95, WHITE, 15, lastMessage);
		sftd_draw_wtext(fontBold12, (400 - sftd_get_wtext_width(fontBold12, 12, details)) / 2, 130, WHITE, 12, details);
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(warningBottom, 0, 0);
	pksm_end_frame();
	sf2d_swapbuffers();
}

void freezeMsgDetails(wchar_t* details) { _freezeMsgWithDetails(L"", details, true); }
void freezeMsg(wchar_t* message) { _freezeMsgWithDetails(message, i18n(S_FREEZEMSG_DEFAULT_DETAILS), false); }
void freezeMsgWithDetails(wchar_t* message, wchar_t* details) { _freezeMsgWithDetails(message, details, false); }

void progressBar(wchar_t* message, u32 current, u32 sz) {
	wchar_t progress[40];
	swprintf(progress, 40, i18n(S_GRAPHIC_PROGRESSBAR_MESSAGE), current, sz);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(warningTop, 0, 0);
		sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, message)) / 2, 95, WHITE, 15, message);
		sftd_draw_wtext(fontBold12, (400 - sftd_get_wtext_width(fontBold12, 12, progress)) / 2, 130, WHITE, 12, progress);
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(warningBottom, 0, 0);
	pksm_end_frame();
	sf2d_swapbuffers();
}

void drawMenuTop() {
	printMenuTop();
	#if PKSV
	printTitle(L"PKSV");
	#else
	printTitle(L"PKSM");
	#endif
	sf2d_draw_texture(pokeball, (400 - pokeball->width) / 2 + 5, (240 - pokeball->height) / 2 + 10);
	
	sftd_draw_text(fontBold9, (398 - sftd_get_text_width(fontBold9, 9, VERSION)), 229, LIGHTBLUE, 9, VERSION);
}

void printAnimatedBG(bool isUp) {
	sf2d_draw_texture(backgroundTop, 0, 0);
	animateBG(isUp);
}

void printTitle(const wchar_t* title) {
	sftd_draw_wtext(fontBold14, (400 - sftd_get_wtext_width(fontBold14, 14, title)) / 2, 4, BLUE, 14, title);
}

void printBottomIndications(const wchar_t* message) {
	sftd_draw_wtext(fontBold9, (320 - sftd_get_wtext_width(fontBold9, 9, message)) / 2, 225, LIGHTBLUE, 9, message);
}

void gameSelectorMenu(int n) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		printMenuTop();
		sftd_draw_wtext(fontBold9, (400 - sftd_get_wtext_width(fontBold9, 9, i18n(S_GRAPHIC_GAME_SELECTOR_INFO_CART_HAS_PRIO))) / 2, 6, BLUE, 9, i18n(S_GRAPHIC_GAME_SELECTOR_INFO_CART_HAS_PRIO));
		
		for (int i = 0; i < 6; i++) {
			if (n == i) {
				sf2d_draw_texture_part_blend(gameSelectorTop, 26 + i*60 + 4, 80 + 4, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70, HIGHBLUE);
				sf2d_draw_texture_part_blend(gameSelectorTop, 26 + i*60, 80, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70, logoColors[i]);
			} else
				sf2d_draw_texture_part(gameSelectorTop, 26 + i*60, 80, logo_lookup6[i], 0, logo_lookup6[i+1] - logo_lookup6[i] - 1, 70);
		}

		sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, i18n(gamesList[n]))) / 2, 185, logoColors[n], 15, i18n(gamesList[n]));
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		
		for (int i = 6; i < 11; i++) {
			if (n == i) {
				sf2d_draw_texture_part_blend(gameSelectorBottom1, 26 + (i - 6)*56 + 4, 40 + 4, logo_lookup4[i - 6], 0, logo_lookup4[i - 5] - logo_lookup4[i - 6] - 1, 55, HIGHBLUE);
				sf2d_draw_texture_part_blend(gameSelectorBottom1, 26 + (i - 6)*56, 40, logo_lookup4[i - 6], 0, logo_lookup4[i - 5] - logo_lookup4[i - 6] - 1, 55, logoColors[i]);
			} else
				sf2d_draw_texture_part(gameSelectorBottom1, 26 + (i - 6)*56, 40, logo_lookup4[i - 6], 0, logo_lookup4[i - 5] - logo_lookup4[i - 6] - 1, 55);
		}
		
		for (int i = 11; i < 15; i++) {
			if (n == i) {
				sf2d_draw_texture_part_blend(gameSelectorBottom2, 40 + (i - 11)*60 + 4, 115 + 4, logo_lookup5[i - 11], 0, logo_lookup5[i - 10] - logo_lookup5[i - 11] - 1, 63, HIGHBLUE);
				sf2d_draw_texture_part_blend(gameSelectorBottom2, 40 + (i - 11)*60, 115, logo_lookup5[i - 11], 0, logo_lookup5[i - 10] - logo_lookup5[i - 11] - 1, 63, logoColors[i]);
			} else
				sf2d_draw_texture_part(gameSelectorBottom2, 40 + (i - 11)*60, 115, logo_lookup5[i - 11], 0, logo_lookup5[i - 10] - logo_lookup5[i - 11] - 1, 63);
		}

		printBottomIndications(i18n(S_GRAPHIC_GAME_SELECTOR_INDICATIONS));
	pksm_end_frame();
	sf2d_swapbuffers();
}

void menu(int menu[]) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop();
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();		
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				sf2d_draw_texture(mainMenuButton, 15 + j*150, 20 + i*63);
				sftd_draw_wtext(fontBold15, 15 + j*150 + 52 + (84 - sftd_get_wtext_width(fontBold15, 15, i18n(menu[i*2+j]))) / 2, 17 + i*63 + (mainMenuButton->height - 15) / 2, LIGHTBLUE, 15, i18n(menu[i*2+j]));
				switch (i*2+j) {
					case 0: 
						sf2d_draw_texture(iconBank, 25, 18 + (53 - iconBank->height)/2);
						break;
					case 1: 
						sf2d_draw_texture(iconEditor, 175, 18 + (53 - iconEditor->height)/2);
						break;
					case 2: 
						sf2d_draw_texture(iconEvents, 25, 81 + (53 - iconEvents->height)/2);
						break;
					case 3: 
						sf2d_draw_texture(iconSave, 175, 81 + (53 - iconSave->height)/2);
						break;
					case 4: 
						sf2d_draw_texture(iconSettings, 25, 144 + (53 - iconSettings->height)/2);
						break;
					case 5: 
						sf2d_draw_texture(iconCredits, 175, 144 + (53 - iconCredits->height)/2);
						break;
				}
			}
		}
		printBottomIndications(i18n(S_MAIN_MENU_INDICATION_EXIT));
	pksm_end_frame();
	sf2d_swapbuffers();
}

void mainMenuDS(int currentEntry) {
	wchar_t* menu[] = { L"Events", L"Save Info", L"Other"};
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(DSTopBG, 0, 0);
		sf2d_draw_texture(pokeball, (400 - pokeball->width) / 2, (240 - pokeball->height) / 2 + 3);
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(DSBottomBG, 0, 0);
		for (int i = 0; i < 3; i++) {
			sf2d_draw_texture((i == currentEntry) ? DSBarSelected : DSBar, 0, 33 + i*56);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, menu[i])) / 2, (i == currentEntry) ? 43 + i*56 : 45 + i*56, (i == currentEntry) ? WHITE : DS, 14, menu[i]);
		}
		sftd_draw_wtext(fontBold9, (320 - sftd_get_wtext_width(fontBold9, 9, i18n(S_GRAPHIC_MENUDS_INDICATIONS))) / 2, 224, RED, 9, i18n(S_GRAPHIC_MENUDS_INDICATIONS));
	pksm_end_frame();
	sf2d_swapbuffers();
}

void printCredits() {
	u8 buf[1500];
	memset(buf, 0, 1500);
	loadFile(buf, "romfs:/res/credits.txt");
	
	while (aptMainLoop() && !(hidKeysUp() & KEY_B)) {
		hidScanInput();
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			printMenuTop();
			printTitle(i18n(S_GRAPHIC_CREDITS_TITLE));
			sf2d_draw_texture_part(creditsTop, 16, 104, 0, 22, 181, 46);
			sf2d_draw_texture_part(creditsTop, 36, 104, 0, 22, 181, 46);
			sf2d_draw_texture(creditsTop, 0, 45);
			sftd_draw_text(fontBold15, 18, 79, LIGHTBLUE, 15, "Bernardo Giordano");
			sftd_draw_text(fontBold15, 34, 117, LIGHTBLUE, 15, "Naxann, Anty-Lemon");
			sftd_draw_text(fontBold15, 64, 174, LIGHTBLUE, 15, "dsoldier for the complete GUI design");
		pksm_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			printMenuBottom();
			sftd_draw_text(fontBold9, 20, 30, LIGHTBLUE, 9, (char*)buf);
			printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
		pksm_end_frame();
		sf2d_swapbuffers();
	}
}

void printDatabase6(char *database[], int currentEntry, int page, int spriteArray[]) {
	int pk, y = 41;
	char pages[24];
	sprintf(pages, "%d/%d", page + 1, fill_get_index()/10);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		printMenuTop();
		printTitle(i18n(S_GRAPHIC_DB6_TITLE));
		
		for (int i = 0; i < 5; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 18, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 18, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0), 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 54, y + 12, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 54, y + 3, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, 148, database[page * 10 + i]);
			
			y += 37;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 200, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 200, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 202, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0), 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 235, y + 14, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 235, y + 3, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, 148, database[page * 10 + i]);
			
			y += 37;
		}
	pksm_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		sf2d_draw_texture(eventMenuBottomBar, (320 - eventMenuBottomBar->width) / 2, 45);
		sf2d_draw_texture(LButton, 83, 52);
		sf2d_draw_texture(RButton, 221, 52);
		sftd_draw_text(fontBold12, (320 - sftd_get_text_width(fontBold12, 12, pages)) / 2, 52, WHITE, 12, pages);
		printBottomIndications(i18n(S_GRAPHIC_DB6_INDICATIONS));
	pksm_end_frame();
	sf2d_swapbuffers();
}

void printDatabaseListDS(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]) {
	int pk, y = 41;
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(DSEventTop, 0, 0);
		
		for (int i = 0; i < 5; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(DSSelectedBarL, 18, y);
			else
				sf2d_draw_texture(DSNormalBarL, 18, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 22, y + 2, 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 55, y + 14, WHITE, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 55, y + 3, WHITE, 9, 148, database[page * 10 + i]);
			
			y += DSSelectedBarL->height;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(DSSelectedBarR, 200, y);
			else
				sf2d_draw_texture(DSNormalBarR, 200, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 204, y + 2, 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 235, y + 14, WHITE, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 235, y + 3, WHITE, 9, 148, database[page * 10 + i]);
			
			y += DSSelectedBarR->height;
		}
	pksm_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(DSEventBottom, 0, 0);
		sftd_draw_wtext(fontBold9, (320 - sftd_get_wtext_width(fontBold9, 9, isSelected ? i18n(S_GRAPHIC_DB_INDICATIONS_INJECT) : i18n(S_GRAPHIC_DB_INDICATIONS_SELECT))) / 2, 222, RGBA8(255, 255, 255, 130), 9, isSelected ? i18n(S_GRAPHIC_DB_INDICATIONS_INJECT) : i18n(S_GRAPHIC_DB_INDICATIONS_SELECT));
		
		if (isSelected) {
			char *languages[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};			
			
			for (int t = 0; t < 7; t++) {
				int x = 0, y = 178;
				switch (t) {
					case 0 : { x = 25; break; }
					case 1 : { x = 63; break; }
					case 2 : { x = 101; break; }
					case 3 : { x = 139; break; }
					case 4 : { x = 177; break; }
					case 5 : { x = 215; break; }
					case 6 : { x = 253; break; }
				}
				
				sf2d_draw_texture(DSLang, x, y);
				if (langVett[t]) {
					if (t == langSelected) sf2d_draw_texture(DSLangSelected, x, y);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[t])) / 2, y + 4, WHITE, 12, languages[t]);
				}
				else {
					if (t == langSelected) sf2d_draw_texture(DSLangSelected, x, y);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[t])) / 2, y + 4, RGBA8(255, 255, 255, 100), 12, languages[t]);
				}
			}
		}
	
	pksm_end_frame();
	sf2d_swapbuffers();
}

void printDB7(u8* previewbuf, int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected, bool ota) {
	int game = game_get();
	
	char *languages[] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR", "CHS", "CHT"};
	char cont[3];
	snprintf(cont, 3, "%d", nInjected + 1);
	
	int total = ISGEN7 ? 9 : 7;
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		wcxInfoViewer(previewbuf);
#if PKSV
#else
		sf2d_draw_texture(otaButton, 360, 2);
#endif
		
		if (sprite != -1) {
			sf2d_draw_texture_part_scale(spritesSmall, 282, 41 - movementOffsetLong(6), 40 * (wcx_get_species(previewbuf) % 25) + 4, 30 * (wcx_get_species(previewbuf) / 25), 34, 30, 2, 2);
		}
		
		if (ota) {
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, 220));
			sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, i18n(S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT))) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, i18n(S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT));
			sftd_draw_wtext(fontBold12, (400 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKVIEWER_OTA_INDICATIONS))) / 2, 130, WHITE, 12, i18n(S_GRAPHIC_PKVIEWER_OTA_INDICATIONS));
		}
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();

		if (getN(i) > 1)
			sftd_draw_text(fontBold11, (320 - sftd_get_text_width(fontBold11, 11, "Press L to switch multiple wondercards.")) / 2, 19, LIGHTBLUE, 11, "Press L/R to switch multiple wondercards.");
		
		sftd_draw_wtext(fontBold14, 16, 50, LIGHTBLUE, 14, i18n(S_GRAPHIC_DB_LANGUAGES));
		sftd_draw_wtext(fontBold14, 16, 112, LIGHTBLUE, 14, i18n(S_GRAPHIC_DB_OVERWRITE_WC));
		sftd_draw_wtext(fontBold14, 16, 140, LIGHTBLUE, 14, i18n(S_GRAPHIC_DB_ADAPT_LANGUAGE_WC));
		sftd_draw_wtext(fontBold14, 16, 170, LIGHTBLUE, 14, i18n(S_GRAPHIC_DB_INJECT_WC_SLOT));
		
		for (int t = 0; t < total; t++) {
			int x = 0, y = 0;
			switch (t) {
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
			if (langVett[t]) {
				if (t == langSelected) {
					sf2d_draw_texture(redButton, x, y);
					sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, DARKBLUE, 14, languages[t]);
				} else {
					sf2d_draw_texture(darkButton, x, y);
					sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, YELLOW, 14, languages[t]);
				}
			}
			else {
				sf2d_draw_texture(lightButton, x, y);
				sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, DARKBLUE, 14, languages[t]);
			}
		}
			
		if (overwrite) {
			sf2d_draw_texture(redButton, 231, 110);
			sf2d_draw_texture(darkButton, 270, 110);
		}
		else {
			sf2d_draw_texture(darkButton, 231, 110);
			sf2d_draw_texture(redButton, 270, 110);			
		}
		
		if (adapt) {
			sf2d_draw_texture(redButton, 231, 138);
			sf2d_draw_texture(darkButton, 270, 138);
		}
		else {
			sf2d_draw_texture(darkButton, 231, 138);
			sf2d_draw_texture(redButton, 270, 138);
		}
		
		sf2d_draw_texture(darkButton, 251, 168);	
		
		sftd_draw_wtext(fontBold12, 231 + (36 - sftd_get_wtext_width(fontBold12, 12, i18n(S_YES))) / 2, 113, (overwrite) ? DARKBLUE : YELLOW, 12, i18n(S_YES));
		sftd_draw_wtext(fontBold12, 270 + (36 - sftd_get_wtext_width(fontBold12, 12, i18n(S_NO))) / 2, 113, (!overwrite) ? DARKBLUE : YELLOW, 12, i18n(S_NO));
		sftd_draw_wtext(fontBold12, 231 + (36 - sftd_get_wtext_width(fontBold12, 12, i18n(S_YES))) / 2, 141, (adapt) ? DARKBLUE : YELLOW, 12, i18n(S_YES));
		sftd_draw_wtext(fontBold12, 270 + (36 - sftd_get_wtext_width(fontBold12, 12, i18n(S_NO))) / 2, 141, (!adapt) ? DARKBLUE : YELLOW, 12, i18n(S_NO));
		sftd_draw_text(fontBold12, 251 + (36 - sftd_get_text_width(fontBold12, 12, cont)) / 2, 171, YELLOW, 12, cont);
		
		if (ota) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtextf(fontBold9, 10, 225, WHITE, 9, i18n(S_HTTP_SERVER_RUNNING), socket_get_ip());
		} else
			printBottomIndications(i18n(S_GRAPHIC_DB_INDICATIONS_INJECT));
		
		pksm_end_frame();
	sf2d_swapbuffers();
}

void printEditor(u8* mainbuf, u64 size, int currentEntry, int page) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(hexBG, 0, 0);
		for (int rows = 0; rows < 15; rows++) {
			for (int columns = 0; columns < 16; columns++) {
				int byte = rows*16 + columns;
				if (currentEntry == byte)
					printSelector(columns*25, rows*15, 24, 14);
				sftd_draw_textf(fontBold11, 4 + 25*columns, 15*rows, (saveSectors[byte + 240*page][0]) ? WHITE : DS, 11, "%02hhX", mainbuf[byte + 240*page]);
				
				if (byte + 240*page == size) break;
			}
		}
		//sftd_draw_wtextf(fontBold11, 4, 225, LIGHTBLUE, 11, L"%ls", descriptions[additional1]);
	pksm_end_frame();
		
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		if (saveSectors[currentEntry + 240*page][0] && !(saveSectors[currentEntry + 240*page][1])) {
			sf2d_draw_texture(minusButton, 244, 30);
			sf2d_draw_texture(plusButton, 267, 30);
		}
		
		sftd_draw_wtextf(fontBold14, (155 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE))), 30, LIGHTBLUE, 14, i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE));
		sftd_draw_textf(fontBold14, 165, 30, WHITE, 14, "0x%05X", currentEntry + 240*page);
		
		printSaveEditorInfo(mainbuf, page*240 + currentEntry);
	pksm_end_frame();
	sf2d_swapbuffers();		
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
	if (t) {
		t -= 1;
		sf2d_draw_texture_part(alternativeSpritesSmall, x, y, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30); 
	} else {
		if (pkx_get_species(pkmn) < 0 || pkx_get_species(pkmn) > 821)
			sf2d_draw_texture_part(spritesSmall, x, y, 0, 0, 34, 30);
		else
			sf2d_draw_texture_part(spritesSmall, x, y, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30);
	}
	if (pkx_is_egg(pkmn))
		sf2d_draw_texture_part(spritesSmall, x + 6, y + 6, 40 * (EGGSPRITEPOS % 25) + 4, 30 * (EGGSPRITEPOS / 25), 34, 30);
	if (pkx_get_item(pkmn))
		sf2d_draw_texture(item, x + 3, y + 21);
}

void printElementBlend(u8* pkmn, int game, u16 n, int x, int y, u32 color) {
	u16 t = getAlternativeSprite(pkmn, game);
	if (t) {
		t -= 1;
		sf2d_draw_texture_part_blend(alternativeSpritesSmall, x, y, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30, color); 
	} else {
		if (pkx_get_species(pkmn) < 0 || pkx_get_species(pkmn) > 821)
			sf2d_draw_texture_part_blend(spritesSmall, x, y, 0, 0, 34, 30, color);
		else
			sf2d_draw_texture_part_blend(spritesSmall, x, y, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30, color);
	}
	if (pkx_is_egg(pkmn))
		sf2d_draw_texture_part_blend(spritesSmall, x + 6, y + 6, 40 * (EGGSPRITEPOS % 25) + 4, 30 * (EGGSPRITEPOS / 25), 34, 30, color);
	if (pkx_get_item(pkmn))
		sf2d_draw_texture_blend(item, x + 3, y + 21, color);
}

void printGeneration(u8* pkmn, int x, int y) {
	u8 version = pkx_get_version(pkmn);
	switch (version) {
		case 1: // sapphire
		case 2: // ruby
		case 3: // emerald
		case 4: // fire red
		case 5: // leaf green
		case 15: // colosseum/XD
			sf2d_draw_texture_part(generations, x, y, 20, 0, 10, 10);
			break;
		case 10: // diamond
		case 11: // pearl
		case 12: // platinum
		case 7: // heart gold
		case 8: // soul silver
			sf2d_draw_texture_part(generations, x, y, 30, 0, 10, 10);
			break;		
		case 20: // white
		case 21: // black
		case 22: // white2
		case 23: // black2
			sf2d_draw_texture_part(generations, x, y, 40, 0, 10, 10);
			break;
		case 24: // x
		case 25: // y
		case 26: // as
		case 27: // or
			sf2d_draw_texture_part(generations, x, y, 50, 0, 10, 10);
			break;
		case 30: // sun
		case 31: // moon
		case 32: // us
		case 33: // um
			sf2d_draw_texture_part(generations, x, y, 60, 0, 10, 10);
			break;
		case 34: // go
			sf2d_draw_texture_part(generations, x, y, 10, 0, 10, 10);
			break;
		case 35: // rd
		case 36: // gn
		case 37: // bu
		case 38: // yw
		case 39: // gd
		case 40: // sv
			sf2d_draw_texture_part(generations, x, y, 0, 0, 10, 10);
			break;
		default:
			break;
	}
}

void infoViewer(u8* pkmn) {
	int y_desc = 29;
	wchar_t* entries[] = {i18n(S_GRAPHIC_INFOVIEWER_NICKNAME), i18n(S_GRAPHIC_INFOVIEWER_OT), i18n(S_GRAPHIC_INFOVIEWER_POKERUS), i18n(S_GRAPHIC_INFOVIEWER_NATURE), i18n(S_GRAPHIC_INFOVIEWER_ABILITY), i18n(S_GRAPHIC_INFOVIEWER_ITEM), i18n(S_GRAPHIC_INFOVIEWER_ESVTSV), i18n(S_GRAPHIC_INFOVIEWER_TIDSID), i18n(S_GRAPHIC_INFOVIEWER_HTOT_FRIENDSHIP), i18n(S_GRAPHIC_INFOVIEWER_HTOT_HIDDEN_POWER)};
	wchar_t* values[] =  {i18n(S_GRAPHIC_INFOVIEWER_VALUE_HP), i18n(S_GRAPHIC_INFOVIEWER_VALUE_ATTACK), i18n(S_GRAPHIC_INFOVIEWER_VALUE_DEFENSE), i18n(S_GRAPHIC_INFOVIEWER_VALUE_SP_ATK), i18n(S_GRAPHIC_INFOVIEWER_VALUE_SP_DEF), i18n(S_GRAPHIC_INFOVIEWER_VALUE_SPEED)};
	
	printAnimatedBG(true);
	sf2d_draw_texture(infoView, 0, 2);

	sf2d_draw_texture((pkx_get_move(pkmn, 0)) ? normalBar : noMove, 252, 155);
	sf2d_draw_texture((pkx_get_move(pkmn, 1)) ? normalBar : noMove, 252, 176);
	sf2d_draw_texture((pkx_get_move(pkmn, 2)) ? normalBar : noMove, 252, 197);
	sf2d_draw_texture((pkx_get_move(pkmn, 3)) ? normalBar : noMove, 252, 218);
	
	sftd_draw_wtext(fontBold12, 251, 138, WHITE, 12, i18n(S_GRAPHIC_INFOVIEWER_MOVES));
	for (int i = 0; i < 10; i++) {
		if (i == 8 && pkx_is_egg(pkmn))
			sftd_draw_wtext(fontBold12, 2, y_desc, BLUE, 12, i18n(S_GRAPHIC_INFOVIEWER_EGG_CYCLE));
		else
			sftd_draw_wtext(fontBold12, 2, y_desc, BLUE, 12, entries[i]);
		y_desc += 20;
		if (i == 2) y_desc += 5;
		if (i == 5) y_desc += 6;
	}
	
	y_desc = 8;
	for (int i = 0; i < 6; i++) {
		sftd_draw_wtext(fontBold12, 225, y_desc, LIGHTBLUE, 12, values[i]);
		y_desc += 20;
	}
	
	if (pkx_get_species(pkmn) > 0 && pkx_get_species(pkmn) < 822) {
		
		sf2d_draw_texture_part(balls, -2, -5, 32 * (pkx_get_ball(pkmn) % 8), 32 * (pkx_get_ball(pkmn) / 8), 32, 32);
		sftd_draw_wtext(fontBold12, 30, 6, WHITE, 12, listSpecies.items[pkx_get_species(pkmn)]);
		
		printGeneration(pkmn, 134, 8);
		
		if (pkx_get_gender(pkmn) == 0)
			sf2d_draw_texture(male, 146, 7);
		else if (pkx_get_gender(pkmn) == 1)
			sf2d_draw_texture(female, 148, 7);
		
		wchar_t* level = (wchar_t*)malloc(8 * sizeof(wchar_t));
		swprintf(level, 8, i18n(S_GRAPHIC_INFOVIEWER_LV), pkx_get_level(pkmn));
		sftd_draw_wtext(fontBold12, 160, 6, WHITE, 12, level);
		free(level);
		
		u32 nick[NICKNAMELENGTH*2];
		memset(nick, 0, NICKNAMELENGTH*2*sizeof(u32));
		pkx_get_nickname(pkmn, nick);
		sftd_draw_wtext(fontBold12, 215 - (sftd_get_wtext_width(fontBold12, 12, (wchar_t*)nick)), 29, WHITE, 12, (wchar_t*)nick);
		
		u32 ot_name[NICKNAMELENGTH*2];
		memset(ot_name, 0, NICKNAMELENGTH*2*sizeof(u32));
		pkx_get_ot(pkmn, ot_name);
		sftd_draw_wtext(fontBold12, 215 - (sftd_get_wtext_width(fontBold12, 12, (wchar_t*)ot_name)), 49, WHITE, 12, (wchar_t*)ot_name);
		
		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, (pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO))), 69, WHITE, 12, pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO));
		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, natures[pkx_get_nature(pkmn)]), 94, WHITE, 12, natures[pkx_get_nature(pkmn)]);
		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, abilities[pkx_get_ability(pkmn)]), 114, WHITE, 12, abilities[pkx_get_ability(pkmn)]);
		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, items[pkx_get_item(pkmn)]), 134, WHITE, 12, items[pkx_get_item(pkmn)]);
		
		if (pkx_is_shiny(pkmn))
			sf2d_draw_texture(shinyStar, 205, 9);
		
		char* friendship = (char*)malloc(11 * sizeof(char));
		if (pkx_is_egg(pkmn))
			snprintf(friendship, 11, "%u", pkx_get_ot_friendship(pkmn));
		else
			snprintf(friendship, 11, "%u / %u", pkx_get_ht_friendship(pkmn), pkx_get_ot_friendship(pkmn));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, friendship), 200, WHITE, 12, friendship);
		free(friendship);
		
		char* otid = (char*)malloc(18 * sizeof(char));
		snprintf(otid, 18, "%u / %u", pkx_get_psv(pkmn), pkx_get_tsv(pkmn));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, otid), 160, WHITE, 12, otid);
		snprintf(otid, 18, "%u / %u", pkx_get_tid(pkmn), pkx_get_sid(pkmn));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, otid), 180, WHITE, 12, otid);
		free(otid);

		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, hpList[pkx_get_hp_type(pkmn)]), 220, WHITE, 12, hpList[pkx_get_hp_type(pkmn)]);
		
		int max = sftd_get_text_width(fontBold12, 12, "252");		
		int y_moves = 159;
		for (int i = 0; i < 4; i++) {
			if (pkx_get_move(pkmn, i))
				sftd_draw_wtext(fontBold12, 396 - sftd_get_wtext_width(fontBold12, 12, moves[pkx_get_move(pkmn, i)]), y_moves, WHITE, 12, moves[pkx_get_move(pkmn, i)]);
			y_moves += 21;
		}
		
		char tmp[6];
		for (int i = 0; i < 6; i++) {
			sprintf(tmp, "%d", pkx_get_iv(pkmn, lookup[i]));
			sftd_draw_text(fontBold12, 289 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 8 + i * 20, WHITE, 12, tmp);
			sprintf(tmp, "%d", pkx_get_ev(pkmn, lookup[i]));
			sftd_draw_text(fontBold12, 328 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 8 + i * 20, WHITE, 12, tmp);
			sprintf(tmp, "%d", pkx_get_stat(pkmn, lookup[i]));
			sftd_draw_text(fontBold12, 369 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 8 + i * 20, WHITE, 12, tmp);
		}
	}
}

void wcxInfoViewer(u8* buf) {
	int game = game_get();
	
	int y_desc = 29;
	wchar_t* entries[] = {L"Species", L"OT", L"TID/SID", L"Held Item", L"Game", L"Met Date"};

	printAnimatedBG(true);
	sf2d_draw_texture(eventView, 0, 2);

	sf2d_draw_texture((wcx_get_move(buf, 0)) ? normalBar : noMove, 252, 155);
	sf2d_draw_texture((wcx_get_move(buf, 1)) ? normalBar : noMove, 252, 176);
	sf2d_draw_texture((wcx_get_move(buf, 2)) ? normalBar : noMove, 252, 197);
	sf2d_draw_texture((wcx_get_move(buf, 3)) ? normalBar : noMove, 252, 218);
	
	sftd_draw_wtext(fontBold12, 251, 138, WHITE, 12, i18n(S_GRAPHIC_INFOVIEWER_MOVES));
	for (int i = 0; i < 6; i++) {
		sftd_draw_wtext(fontBold12, 2, y_desc, BLUE, 12, entries[i]);
		y_desc += 20;
		if (i == 2) y_desc += 6;
		if (i == 3) y_desc -= 1;
		if (i == 5) y_desc += 6;
	}
	
	wchar_t *version;
	if (wcx_get_origin_game(buf) != 0) {
		switch (wcx_get_origin_game(buf)) {
			case 24:
				version = L"X"; break;
			case 25:
				version = L"Y"; break;
			case 26:
				version = L"Alpha Sapphire"; break;
			case 27:
				version = L"Omega Ruby"; break;
			case 30:
				version = L"Sun"; break;
			case 31:
				version = L"Moon"; break;
			default: 
				version = L"WIP";
		}
	} else {
		switch (game) {
			case GAME_X:
			case GAME_Y:
				version = L"X/Y compatible"; break;
			case GAME_OR:
			case GAME_AS:
				version = L"OR/AS compatible"; break;
			case GAME_SUN:
			case GAME_MOON:
				version = L"Sun/Moon compatible"; break;
			default:
				version = L"WIP"; break;
		}
	}
	
	sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, version), 115, WHITE, 12, version);
	
	u32 title[72];
	memset(title, 0, 72*sizeof(u32));
	wcx_get_title(buf, title);
	sftd_draw_wtext(fontBold12, !wcx_is_pokemon(buf) ? 4 : 30, 6, WHITE, 12, (wchar_t*)title);
	
	u32 date[12];
	memset(date, 0, 12*sizeof(u32));
	swprintf((wchar_t*)date, 12, L"%u/%u/%u", wcx_get_year(buf), wcx_get_month(buf), wcx_get_day(buf));
	sftd_draw_wtext(fontBold12, 215 - (sftd_get_wtext_width(fontBold12, 12, (wchar_t*)date)), 134, WHITE, 12, (wchar_t*)date);
	
	if (wcx_is_pokemon(buf)) {
		sf2d_draw_texture_part(balls, -2, -5, 32 * (wcx_get_ball(buf) % 8), 32 * (wcx_get_ball(buf) / 8), 32, 32);
		
		if (wcx_get_gender(buf) == 0)
			sf2d_draw_texture(male, 288, 7);
		else if (wcx_get_gender(buf) == 1)
			sf2d_draw_texture(female, 290, 7);
		
		wchar_t* level = (wchar_t*)malloc(8 * sizeof(wchar_t));
		swprintf(level, 8, i18n(S_GRAPHIC_INFOVIEWER_LV), wcx_get_level(buf));
		sftd_draw_wtext(fontBold12, 302, 6, WHITE, 12, level);
		free(level);
		
		u32 ot_name[NICKNAMELENGTH*2];
		memset(ot_name, 0, NICKNAMELENGTH*2*sizeof(u32));
		wcx_get_ot(buf, ot_name);
		sftd_draw_wtext(fontBold12, 215 - (sftd_get_wtext_width(fontBold12, 12, (wchar_t*)ot_name)), 49, WHITE, 12, (wchar_t*)ot_name);
		
		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, items[wcx_get_held_item(buf)]), 94, WHITE, 12, items[wcx_get_held_item(buf)]);
		
		if (wcx_is_shiny(buf)) {
			sf2d_draw_texture(shinyStar, 206, 32);
			sftd_draw_wtext(fontBold12, 202 - sftd_get_wtext_width(fontBold12, 12, listSpecies.items[wcx_get_species(buf)]), 29, WHITE, 12, listSpecies.items[wcx_get_species(buf)]);
		} else {
			sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, listSpecies.items[wcx_get_species(buf)]), 29, WHITE, 12, listSpecies.items[wcx_get_species(buf)]);
		}
		
		char* otid = (char*)malloc(18 * sizeof(char));
		snprintf(otid, 18, "%u / %u", wcx_get_tid(buf), wcx_get_sid(buf));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, otid), 69, WHITE, 12, otid);
		free(otid);
	
		int y_moves = 159;
		for (int i = 0; i < 4; i++) {
			if (wcx_get_move(buf, i))
				sftd_draw_wtext(fontBold12, 396 - sftd_get_wtext_width(fontBold12, 12, moves[wcx_get_move(buf, i)]), y_moves, WHITE, 12, moves[wcx_get_move(buf, i)]);
			y_moves += 21;
		}	
	} else if (wcx_is_item(buf)) {
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "None"), 29, WHITE, 12, "None");
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "None"), 49, WHITE, 12, "None");
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "None"), 69, WHITE, 12, "None");
		sftd_draw_wtext(fontBold12, 215 - sftd_get_wtext_width(fontBold12, 12, items[wcx_get_item(buf)]), 94, WHITE, 12, items[wcx_get_item(buf)]);
	}  else if (wcx_is_bp(buf)) {
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "None"), 29, WHITE, 12, "None");
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "None"), 49, WHITE, 12, "None");
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "None"), 69, WHITE, 12, "None");
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, "Battle Points"), 94, WHITE, 12, "Battle Points");
	}
}

void printDexViewer(u8* mainbuf, int currentEntry, int page, int seen, int caught) {
	char temp[12];
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(generationBG, 0, 0);
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 8; j++) {
				if (currentEntry == i * 8 + j)
					printSelector(j*49 + j, i*47 + i, 49, 47);
				
				int entry = i*8+j + 40*page;
				if ((entry) < 802) {
					if (getCaught(mainbuf, entry + 1))
						sf2d_draw_texture_part(spritesSmall, 7 + 49 * j + j, 2 + 47 * i + i, 40 * ((entry + 1) % 25) + 4, 30 * ((entry + 1) / 25), 34, 30);
					else if (getSeen(mainbuf, entry + 1)) {
						sf2d_draw_texture_part(spritesSmall, 7 + 49 * j + j, 2 + 47 * i + i, 40 * ((entry + 1) % 25) + 4, 30 * ((entry + 1) / 25), 34, 30);
						sf2d_draw_texture_part_blend(spritesSmall, 7 + 49 * j + j, 2 + 47 * i + i, 40 * ((entry + 1) % 25) + 4, 30 * ((entry + 1) / 25), 34, 30, RGBA8(0,0,0,160));
					} else
						sf2d_draw_texture_part_blend(spritesSmall, 7 + 49 * j + j, 2 + 47 * i + i, 40 * ((entry + 1) % 25) + 4, 30 * ((entry + 1) / 25), 34, 30, RGBA8(0,0,0,255));
					sprintf(temp, "%d", entry + 1);
					sftd_draw_text(fontBold9, 49 * j + (49 - sftd_get_text_width(fontBold9, 9, temp)) / 2 + j, 34 + i * 47 + i, getCaught(mainbuf, entry + 1) ? WHITE : DS, 9, temp);
				}
			}
		}
	pksm_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();
		sftd_draw_wtextf(fontBold14, 16, 32, WHITE, 14, i18n(S_GRAPHIC_PKBANK_SEEN), seen);
		sftd_draw_wtextf(fontBold14, 16, 48, WHITE, 14, i18n(S_GRAPHIC_PKBANK_CAUGHT), caught);
		printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
	pksm_end_frame();
	sf2d_swapbuffers();	
}

void printPKViewer(u8* mainbuf, u8* tmp, bool isTeam, int currentEntry, int menuEntry, int box, int mode, int additional1, int additional2) {
	int game = game_get();
	
	wchar_t* menuEntries[] = {i18n(S_GRAPHIC_PKVIEWER_MENU_EDIT), i18n(S_GRAPHIC_PKVIEWER_MENU_CLONE), i18n(S_GRAPHIC_PKVIEWER_MENU_RELEASE), i18n(S_GRAPHIC_PKVIEWER_MENU_GENERATE), i18n(S_GRAPHIC_PKVIEWER_MENU_EXIT)};
	int x;

	wchar_t* page = (wchar_t*)malloc((MAX_LENGTH_BOX_NAME+1) * sizeof(wchar_t));
	swprintf(page, MAX_LENGTH_BOX_NAME+1, i18n(S_GRAPHIC_PKVIEWER_BOX), box + 1);
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	pkx_get(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		if (mode == ED_GENERATE) {
			char* temp = (char*)malloc(4);
			sf2d_draw_texture(generationBG, 0, 0);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 8; j++) {
					if (additional1 == i * 8 + j)
						printSelector(j*49 + j, i*47 + i, 49, 47);
					
					if ((i*8+j + 40*additional2) < 802) {
						sf2d_draw_texture_part(spritesSmall, 7 + 49 * j + j, 2 + 47 * i + i, 40 * ((40 * additional2 + i * 8 + j + 1) % 25) + 4, 30 * ((40 * additional2 + i * 8 + j + 1) / 25), 34, 30);
						snprintf(temp, 4, "%d", 40 * additional2 + i * 8 + j + 1);
						sftd_draw_text(fontBold9, 49 * j + (49 - sftd_get_text_width(fontBold9, 9, temp)) / 2 + j, 34 + i * 47 + i, WHITE, 9, temp);
					}
				}
			}
			free(temp);
		} else
			infoViewer(pkmn);
		
		if (mode == ED_OTA) {
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, 220));
			sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, i18n(S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT))) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, i18n(S_GRAPHIC_PKVIEWER_OTA_LAUNCH_CLIENT));
			sftd_draw_wtext(fontBold12, (400 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKVIEWER_OTA_INDICATIONS))) / 2, 130, WHITE, 12, i18n(S_GRAPHIC_PKVIEWER_OTA_INDICATIONS));
			if (!socket_is_legality_address_set())
				sftd_draw_wtext(fontBold11, (400 - sftd_get_wtext_width(fontBold11, 11, i18n(S_GRAPHIC_PKVIEWER_OTA_SET_ADDRESS))) / 2, 222, WHITE, 11, i18n(S_GRAPHIC_PKVIEWER_OTA_SET_ADDRESS));
			else
				sftd_draw_wtext(fontBold11, (400 - sftd_get_wtext_width(fontBold11, 11, i18n(S_GRAPHIC_PKVIEWER_OTA_CHECK_LEGALITY))) / 2, 222, WHITE, 11, i18n(S_GRAPHIC_PKVIEWER_OTA_CHECK_LEGALITY));
		}
	pksm_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(boxView, 0, 0);
		sf2d_draw_texture(editorBar, 0, 210);
		sftd_draw_wtext(fontBold12, 12 + (178 - sftd_get_wtext_width(fontBold12, 12, page)) / 2, 19, WHITE, 12, page);
		sf2d_draw_texture(left, 7, 17);
		sf2d_draw_texture(right, 185, 17);
#if PKSV
#else
		sf2d_draw_texture(otaButton, 240, 211);
#endif
		sf2d_draw_texture(back, 283, 211);
		sftd_draw_wtext(fontBold12, 247, 7, WHITE, 12, i18n(S_GRAPHIC_PKVIEWER_TEAM));
		
		int y = 45;
		int pointer[2] = {0, 0};
		for (int i = 0; i < 5; i++) {
			x = 4;
			for (int j = 0; j < 6; j++) {
				pkx_get(mainbuf, box, i * 6 + j, pkmn);
				u16 n = pkx_get_species(pkmn);
				if (n > 0 && n < 822)
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
			sf2d_draw_texture(pokemonBufferBox, 214, 40 + i * 45);
			sf2d_draw_texture(pokemonBufferBox, 266, 60 + i * 45);
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
			sf2d_draw_texture(selector, pointer[0], pointer[1] - ((mode == ED_STANDARD) ? movementOffsetSlow(3) : 0));
		else {
			sf2d_draw_texture(selectorCloning, pointer[0], pointer[1] - movementOffsetSlow(3));			
			sf2d_draw_texture(bottomPopUp, 1, 214);
			sftd_draw_wtext(fontBold11, 8, 220, WHITE, 11, i18n(S_GRAPHIC_PKVIEWER_CLONE_INDICATIONS));
		}
		
		if (mode == ED_MENU) {
			pkx_get(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn);

			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sf2d_draw_texture(bottomPopUp, 1, 214);

			if (pkx_get_ball(pkmn) != CHERISH_BALL && !isTeam) {
				sf2d_draw_texture(includeInfoButton, 242, 5);
			}
			sftd_draw_wtextf(fontBold11, 8, 220, WHITE, 11, i18n(S_GRAPHIC_PKVIEWER_MENU_POKEMON_SELECTED), listSpecies.items[pkx_get_species(tmp)]);
			for (int i = 0; i < 5; i++) {
				sf2d_draw_texture(button, 208, 42 + i * 27 + i*4);
				if (isTeam && (i == 0 || i == 2 || i == 3))
					sf2d_draw_texture_blend(button, 208, 42 + i * 27 + i*4, RGBA8(0, 0, 0, 100));
				sftd_draw_wtext(fontBold12, 208 + (109 - sftd_get_wtext_width(fontBold12, 12, menuEntries[i])) / 2, 49 + i * 27 + i*4, BLACK, 12, menuEntries[i]);
				if (i == menuEntry)
					sf2d_draw_texture(subArrow, 203 - movementOffsetSlow(3), 46 + i * 27 + i*4);
			}
		} else if (mode == ED_GENERATE) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKVIEWER_GENERATE_INDICATIONS))) / 2, 105, WHITE, 14, i18n(S_GRAPHIC_PKVIEWER_GENERATE_INDICATIONS));
		} else if (mode == ED_OTA) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtextf(fontBold9, 10, 220, WHITE, 9, i18n(S_HTTP_SERVER_RUNNING), socket_get_ip());
		} else if (mode != ED_CLONE) {
			if (mode == ED_STANDARD)
				sftd_draw_wtextf(fontBold9, 10, 220, BARTEXT, 9, i18n(S_GRAPHIC_PKVIEWER_TIDSIDTSV), getSaveTID(mainbuf), getSaveSID(mainbuf), getSaveTSV(mainbuf));
			else
				sftd_draw_wtextf(fontBold9, 10, 220, BARTEXT, 9, i18n(S_GRAPHIC_PKVIEWER_SEED), getSaveSeed(mainbuf, 3), getSaveSeed(mainbuf, 2), getSaveSeed(mainbuf, 1), getSaveSeed(mainbuf, 0));
		}
	pksm_end_frame();
	sf2d_swapbuffers();
	
	free(pkmn);
	free(page);
}

void printPKEditor(u8* pkmn, int additional1, int additional2, int additional3, int mode, wchar_t* descriptions[]) {
	int game = game_get();
	
	int max = sftd_get_text_width(fontBold12, 12, "252");
	wchar_t* entries[] = {i18n(S_GRAPHIC_PKEDITOR_LEVEL), i18n(S_GRAPHIC_PKEDITOR_NATURE), i18n(S_GRAPHIC_PKEDITOR_ABILITY), i18n(S_GRAPHIC_PKEDITOR_ITEM), i18n(S_GRAPHIC_PKEDITOR_SHINY), i18n(S_GRAPHIC_PKEDITOR_POKERUS), i18n(S_GRAPHIC_PKEDITOR_OT), i18n(S_GRAPHIC_PKEDITOR_NICKNAME), i18n(S_GRAPHIC_PKEDITOR_FRIENDSHIP)};
	wchar_t* options[] = {i18n(S_GRAPHIC_PKEDITOR_MENU_STATS), i18n(S_GRAPHIC_PKEDITOR_MENU_MOVES), i18n(S_GRAPHIC_PKEDITOR_MENU_SAVE)};
	
	wchar_t* values[6] = {i18n(S_GRAPHIC_PKEDITOR_STATS_HP), i18n(S_GRAPHIC_PKEDITOR_STATS_ATTACK), i18n(S_GRAPHIC_PKEDITOR_STATS_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_STATS_SP_ATTACK), i18n(S_GRAPHIC_PKEDITOR_STATS_SP_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_STATS_SPEED)};
	u16 n = pkx_get_species(pkmn);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	if (mode == ED_BASE || mode == ED_STATS) {
		sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, i18n(S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_1))) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, i18n(S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_1));
		sftd_draw_wtext(fontBold15, (400 - sftd_get_wtext_width(fontBold15, 15, i18n(S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_2))) / 2, 115, RGBA8(255, 255, 255, giveTransparence()), 15, i18n(S_GRAPHIC_PKEDITOR_BASE_STATS_INDICATIONS_2));
	} else if (mode == ED_ITEMS) {
		int entry;
		int y = 0;
		sf2d_draw_texture(topMovesBG, 0, 0);
		for (int i = 0; i < 20; i++) {
			entry = 40 * additional2 + i;
			if (i == additional1)
				printSelector(2, y, 198, 11);
			else if (i + 20 == additional1) 
				printSelector(200, y, 198, 11);
			sftd_draw_wtextf(fontBold9, 2, y, WHITE, 9, L"%d - %ls", entry, itemsSorted[entry]);
			sftd_draw_wtextf(fontBold9, 200, y, WHITE, 9, L"%d - %ls", entry + 20, itemsSorted[entry + 20]);
			y += 12;
		}	
	} else if (mode == ED_MOVES) {

		int entry;
		int y = 0;
		sf2d_draw_texture(topMovesBG, 0, 0);
		for (int i = 0; i < 20; i++) {
			entry = 40 * additional2 + i;
			if (i == additional1) 
				printSelector(2, y, 198, 11);
			else if (i + 20 == additional1) 
				printSelector(200, y, 198, 11);
			sftd_draw_wtextf(fontBold9, 2, y, WHITE, 9, L"%d - %ls", entry, movesSorted[entry]);
			sftd_draw_wtextf(fontBold9, 200, y, WHITE, 9, L"%d - %ls", entry + 20, movesSorted[entry + 20]);
			y += 12;
		}
	} else if (mode == ED_NATURES) {
		wchar_t* hor[] = {i18n(S_GRAPHIC_PKEDITOR_NATURE_NEUTRAL), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_SPEED), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_SP_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_MIN_SP_DEFENSE)};
		wchar_t* ver[] = {i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_DEFENSE), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_SPEED), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_SP_ATTACK), i18n(S_GRAPHIC_PKEDITOR_NATURE_PLUS_SP_DEFENSE)};
		
		sf2d_draw_texture(naturestx, 0, 0);
		for (int i = 0; i < 6; i++)
			sftd_draw_wtext(fontBold12, 66 * i + (66 - sftd_get_wtext_width(fontBold12, 12, hor[i])) / 2, 13, (i == 0) ? YELLOW : BLUE, 12, hor[i]);
		for (int i = 0; i < 5; i++)
			sftd_draw_wtext(fontBold12, (66 - sftd_get_wtext_width(fontBold12, 12, ver[i])) / 2, 53 + i * 40, BLUE, 12, ver[i]);
		
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (additional1 == i * 5 + j)
					printSelector(66*j + 66 + j, 40*i + 40, 66, 39);
				sftd_draw_wtext(fontBold12, 66 + 66 * j + (66 - sftd_get_wtext_width(fontBold12, 12, natures[i * 5 + j])) / 2 + j, 53 + i * 40, (i == j) ? YELLOW : WHITE, 12, natures[i * 5 + j]);
			}
		}
	} else if (mode == ED_BALLS) {
		sf2d_draw_texture(ballsBG, 0, 0);
		
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				if (additional1 == i * 6 + j)
					printSelector(66*j + j, 47*i + i, 66, 47);
				sf2d_draw_texture_part(balls, 17 + 66 * j + j, 2 + i * 47, 32 * ((i * 6 + j + 1) % 8), 32 * ((i * 6 + j + 1) / 8), 32, 32);
				sftd_draw_wtext(fontBold9, 66 * j + (66 - sftd_get_wtext_width(fontBold9, 9, ballList[i * 6 + j])) / 2 + j, 30 + i * 47 + i, WHITE, 9, ballList[i * 6 + j]);
			}
		}
	} else if (mode == ED_HIDDENPOWER) {
		sf2d_draw_texture(hiddenPowerBG, 0, 0);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (additional1 == i * 4 + j)
					printSelector(99*j + j, 59*i + i, 99, 59);
				sf2d_draw_texture_part(typesSheet, 24 + 99 * j + j, 20 + 59 * i + i, 50 * (i * 4 + j + 1), 0, 50, 18); 
			}
		}
	} else if (mode == ED_FORMS) {
		FormData *forms = pkx_get_legal_form_data((u16)additional2, game);
		int numforms = forms->max - forms->min + 1;
		
		int rows, columns, width, height;
		if (numforms <= 16) {
			columns = 4; rows = 4;
			width = 99; height = 59;
			sf2d_draw_texture(hiddenPowerBG, 0, 0);
		} else {
			columns = 6; rows = 5;
			width = 66; height = 47;
			sf2d_draw_texture(ballsBG, 0, 0);
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
						sf2d_draw_texture_part(spritesSmall, (width - 34) / 2 + width * j + j, 2 * (height - 44) / 3 + i * height, 40 * (additional2 % 25) + 4, 30 * (additional2 / 25), 34, 30);
					else {
						int sprite = forms->spriteNum + form - 2;
						sf2d_draw_texture_part(alternativeSpritesSmall, (width - 34) / 2 + width * j + j, 2 * (height - 44) / 3 + i * height + i, 40 * (sprite % 6) + 4, 30 * (sprite / 6), 34, 30);
					}
					sftd_draw_wtext(fontBold9, width * j + (width - sftd_get_wtext_width(fontBold9, 9, str)) / 2 + j, (height * 5) / 7 + i * height + i, WHITE, 9, str);
				}
			}
		}
	} else if (mode == ED_HEX) {
		sf2d_draw_texture(hexBG, 0, 0);
		if (hax)
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(255, 0, 0, 100));
		for (int rows = 0; rows < 15; rows++) {
			for (int columns = 0; columns < 16; columns++) {
				int byte = rows*16 + columns;
				if (additional1 == byte)
					printSelector(columns*25, rows*15, 24, 14);
				sftd_draw_textf(fontBold11, 4 + 25*columns, 15*rows, (sector[byte][0]) ? WHITE : DS, 11, "%02hhX", pkmn[byte]);
				
				if (byte == 231) break;
			}
		}
		sftd_draw_wtextf(fontBold11, 4, 225, LIGHTBLUE, 11, L"%ls", descriptions[additional1]);
	}
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (mode != ED_HEX)
			printAnimatedBG(false);
		if (mode == ED_BASE || mode == ED_ITEMS || mode == ED_NATURES || mode == ED_BALLS || mode == ED_FORMS) {
			sf2d_draw_texture(editorBG, 0, 1);
			sf2d_draw_texture(editorBar, 0, 210);
			sf2d_draw_texture(hexIcon, 290, 1);
			
			sftd_draw_wtext(fontBold12, 27, 4, WHITE, 12, listSpecies.items[n]);
			sf2d_draw_texture_part(balls, -2, -6, 32 * (pkx_get_ball(pkmn) % 8), 32 * (pkx_get_ball(pkmn) / 8), 32, 32);
			
			u16 t = getAlternativeSprite(pkmn, game);
			int ofs = movementOffsetSlow(3);
			if (t) {
				t -= 1;
				sf2d_draw_texture_part_scale_blend(alternativeSpritesSmall, 232, 32 - ofs, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30, 2, 2, RGBA8(0x0, 0x0, 0x0, 100)); 
				sf2d_draw_texture_part_scale(alternativeSpritesSmall, 227, 27 - ofs, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30, 2, 2);
			} else {
				sf2d_draw_texture_part_scale_blend(spritesSmall, 232, 32 - ofs, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30, 2, 2, RGBA8(0x0, 0x0, 0x0, 100));
				sf2d_draw_texture_part_scale(spritesSmall, 227, 27 - ofs, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30, 2, 2);
			}
			
			if (pkx_get_gender(pkmn) == 0)
				sf2d_draw_texture(male, 159, 6);
			else if (pkx_get_gender(pkmn) == 1)
				sf2d_draw_texture(female, 161, 6);
			
			for (int i = 0; i < 3; i++) {
				sf2d_draw_texture(button, 206, 110 + i * 27 + i*4);
				sftd_draw_wtext(fontBold12, 206 + (109 - sftd_get_wtext_width(fontBold12, 12, options[i])) / 2, 117 + i * 27 + i*4, BLACK, 12, options[i]);
			}
			
			for (int i = 0; i < 9; i++)
				if (i == 8 && pkx_is_egg(pkmn))
					sftd_draw_wtext(fontBold12, 2, 29 + i * 20, LIGHTBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_EGG_CYCLE));
				else
					sftd_draw_wtext(fontBold12, 2, 29 + i * 20, LIGHTBLUE, 12, entries[i]);

			for (int i = 0; i < 7; i++)
				sf2d_draw_texture(setting, 180, 51 + i * 20);

			sf2d_draw_texture(minus, 180 - max - 6 - minus->width, 29);
			sf2d_draw_texture(plus, 180, 29);
			sf2d_draw_texture(minus, 180 - max - 6 - minus->width, 189);
			sf2d_draw_texture(plus, 180, 189);

			char* level = (char*)malloc(4 * sizeof(char));
			snprintf(level, 4, "%u", pkx_get_level(pkmn));
			sftd_draw_text(fontBold12, 180 - max - 3 + (max - sftd_get_text_width(fontBold12, 12, level)) / 2, 29, WHITE, 12, level);
			free(level);
				
			sftd_draw_wtext(fontBold12, 178 - sftd_get_wtext_width(fontBold12, 12, natures[pkx_get_nature(pkmn)]), 49, WHITE, 12, natures[pkx_get_nature(pkmn)]);
			sftd_draw_wtext(fontBold12, 178 - sftd_get_wtext_width(fontBold12, 12, abilities[pkx_get_ability(pkmn)]), 69, WHITE, 12, abilities[pkx_get_ability(pkmn)]);
			sftd_draw_wtext(fontBold12, 178 - sftd_get_wtext_width(fontBold12, 12, items[pkx_get_item(pkmn)]), 89, WHITE, 12, items[pkx_get_item(pkmn)]);
			sftd_draw_wtext(fontBold12, 178 - sftd_get_wtext_width(fontBold12, 12, pkx_is_shiny(pkmn) ? i18n(S_YES) : i18n(S_NO)), 109, WHITE, 12, pkx_is_shiny(pkmn) ? i18n(S_YES) : i18n(S_NO));
			sftd_draw_wtext(fontBold12, 178 - sftd_get_wtext_width(fontBold12, 12, pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO)), 129, WHITE, 12, pkx_get_pokerus(pkmn) ? i18n(S_YES) : i18n(S_NO));
			
			char* friendship = (char*)malloc(4 * sizeof(char));
			if (pkx_is_egg(pkmn))
				snprintf(friendship, 4, "%u", pkx_get_ot_friendship(pkmn));
			else
				snprintf(friendship, 4, "%u", pkx_get_friendship(pkmn));
			sftd_draw_text(fontBold12, 180 - max - 3 + (max - sftd_get_text_width(fontBold12, 12, friendship)) / 2, 189, WHITE, 12, friendship);
			free(friendship);
			
			u32 nick[NICKNAMELENGTH*2];
			memset(nick, 0, NICKNAMELENGTH*2*sizeof(u32));
			pkx_get_nickname(pkmn, nick);
			sftd_draw_wtext(fontBold12, 178 - (sftd_get_wtext_width(fontBold12, 12, (wchar_t*)nick)), 169, WHITE, 12, (wchar_t*)nick);

			u32 ot_name[NICKNAMELENGTH*2];
			memset(ot_name, 0, NICKNAMELENGTH*2*sizeof(u32));
			pkx_get_ot(pkmn, ot_name);
			sftd_draw_wtext(fontBold12, 178 - (sftd_get_wtext_width(fontBold12, 12, (wchar_t*)ot_name)), 149, WHITE, 12, (wchar_t*)ot_name);
		}
		if (mode == ED_STATS || mode == ED_HIDDENPOWER) {
			sf2d_draw_texture(editorStatsBG, 0, 1);
			sf2d_draw_texture(editorBar, 0, 210);
			sf2d_draw_texture(setting, 291, 175);
			sftd_draw_wtext(fontBold12, 2, 28, LIGHTBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_LBL_STATS));
			sftd_draw_wtext(fontBold12, 118, 28, DARKBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_LBL_IV));
			sftd_draw_wtext(fontBold12, 197, 28, DARKBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_LBL_EV));
			sftd_draw_wtext(fontBold12, 256, 28, DARKBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_LBL_TOTAL));
			sftd_draw_wtext(fontBold12, 2, 173, LIGHTBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_HIDDEN_POWER));
			
			sftd_draw_wtext(fontBold12, 27, 4, WHITE, 12, listSpecies.items[n]);
			sf2d_draw_texture_part(balls, -2, -6, 32 * (pkx_get_ball(pkmn) % 8), 32 * (pkx_get_ball(pkmn) / 8), 32, 32);
			
			if (pkx_get_gender(pkmn) == 0)
				sf2d_draw_texture(male, 159, 6);
			else if (pkx_get_gender(pkmn) == 1)
				sf2d_draw_texture(female, 161, 6);
			
			for (int i = 0; i < 6; i++)
				sftd_draw_wtext(fontBold12, 2, 49 + i * 20, LIGHTBLUE, 12, values[i]);

			char tmp[6];
			for (int i = 0; i < 6; i++) {
				sprintf(tmp, "%d", pkx_get_iv(pkmn, lookup[i]));
				sftd_draw_text(fontBold12, 112 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 49 + i * 20, WHITE, 12, tmp);
				sprintf(tmp, "%d", pkx_get_ev(pkmn, lookup[i]));
				sftd_draw_text(fontBold12, 192 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 49 + i * 20, WHITE, 12, tmp);
				sprintf(tmp, "%d", pkx_get_stat(pkmn, lookup[i]));
				sftd_draw_text(fontBold12, 263 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 49 + i * 20, WHITE, 12, tmp);
			}
			
			sftd_draw_wtext(fontBold12, 288 - sftd_get_wtext_width(fontBold12, 12, hpList[pkx_get_hp_type(pkmn)]), 173, WHITE, 12, hpList[pkx_get_hp_type(pkmn)]);

			for (int i = 0; i < 6; i++) {
				sf2d_draw_texture(minus, 96, 49 + i * 20);
				sf2d_draw_texture(plus, 139, 49 + i * 20);
				sf2d_draw_texture(minus, 177, 49 + i * 20);
				sf2d_draw_texture(plus, 218, 49 + i * 20);
			}
		}
		if (mode == ED_MOVES) {

			sf2d_draw_texture(movesBottom, 0, 1);
			sf2d_draw_texture(movesBottom, 0, 2 + movesBottom->height);
			sf2d_draw_texture(editorBar, 0, 210);
			sftd_draw_wtext(fontBold12, 2, 5, LIGHTBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_MOVES));
			sftd_draw_wtext(fontBold12, 2, 110, LIGHTBLUE, 12, i18n(S_GRAPHIC_PKEDITOR_RELEARN_MOVES));
			
			for (int i = 0; i < 4; i++) {
				sftd_draw_wtext(fontBold12, 2, 28 + i * 20, (i == additional3) ? YELLOW : WHITE, 12, moves[pkx_get_move(pkmn, i)]);
				sftd_draw_wtext(fontBold12, 2, 132 + i * 20, (i == additional3 - 4) ? YELLOW: WHITE, 12, moves[pkx_get_egg_move(pkmn, i)]);
				if (i == additional3)
					sf2d_draw_texture_rotate(subArrow, 198 - movementOffsetSlow(3), 33 + i * 20, 3.1415f);
				else if (i == additional3 - 4)
					sf2d_draw_texture_rotate(subArrow, 198 - movementOffsetSlow(3), 137 + i * 20, 3.1415f);
			}

		}
		if (mode == ED_HEX) {
			printMenuBottom();
			sf2d_draw_texture(blueTextBox, 165, 28);
			if (sector[additional1][0] && !(sector[additional1][1])) {
				sf2d_draw_texture(minusButton, 224, 31);
				sf2d_draw_texture(plusButton, 247, 31);
			}
			if (hax)
				sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(255, 0, 0, 100));
			
			sftd_draw_wtextf(fontBold14, (155 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE))), 30, LIGHTBLUE, 14, i18n(S_GRAPHIC_PKEDITOR_SELECTED_BYTE));
			sftd_draw_textf(fontBold14, 171, 30, WHITE, 14, "0x%02hhX", additional1);
			
			printfHexEditorInfo(pkmn, additional1);
			printBottomIndications(i18n(S_GRAPHIC_CREDITS_INDICATIONS));
		}

		if (mode != ED_HEX)
			sf2d_draw_texture(back, 283, 211);
		
		// apply masks
		if (mode == ED_ITEMS) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_ITEM_INDICATION))) / 2, 105, WHITE, 14, i18n(S_GRAPHIC_PKEDITOR_ITEM_INDICATION));
		} else if (mode == ED_NATURES) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_NATURE_INDICATION))) / 2, 105, WHITE, 14, i18n(S_GRAPHIC_PKEDITOR_NATURE_INDICATION));
		} else if (mode == ED_BALLS) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_BALL_INDICATION))) / 2, 105, WHITE, 14, i18n(S_GRAPHIC_PKEDITOR_BALL_INDICATION));
		} else if (mode == ED_HIDDENPOWER) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_HP_INDICATION))) / 2, 105, WHITE, 14, i18n(S_GRAPHIC_PKEDITOR_HP_INDICATION));
		} else if (mode == ED_FORMS) {
			sf2d_draw_rectangle(0, 0, 320, 240, MASKBLACK);
			sftd_draw_wtext(fontBold14, (320 - sftd_get_wtext_width(fontBold14, 14, i18n(S_GRAPHIC_PKEDITOR_FORM_INDICATION))) / 2, 105, WHITE, 14, i18n(S_GRAPHIC_PKEDITOR_FORM_INDICATION));
		}
		
	pksm_end_frame();
	sf2d_swapbuffers();
}

void printPKBank(u8* bankbuf, u8* mainbuf, u8* wirelessBuffer, u8* pkmnbuf, int currentEntry, int saveBox, int bankBox, bool isBufferized, bool isSeen, bool isWirelessActivated) {
	int game = game_get();
	
	int x, y;
	int pointer[2] = {0, 0};
	wchar_t* page = (wchar_t*)malloc((MAX_LENGTH_BOX_NAME+1) * sizeof(wchar_t));
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	if (currentEntry < 30)
		memcpy(pkmn, &bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
	else if (isWirelessActivated)
		memcpy(pkmn, &wirelessBuffer[saveBox * 30 * PKMNLENGTH + (currentEntry - 30) * PKMNLENGTH], PKMNLENGTH);
	else
		pkx_get(mainbuf, saveBox, currentEntry - 30, pkmn);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		if (isSeen) {
			infoViewer(pkmnbuf);
		} else {
			printAnimatedBG(true);
			sf2d_draw_texture(bankTop, 34, 5);
			swprintf(page, MAX_LENGTH_BOX_NAME+1, i18n(S_GRAPHIC_PKBANK_BANK_TITLE), bankBox + 1);
			sftd_draw_wtext(fontBold12, 55 + (178 - sftd_get_wtext_width(fontBold12, 12, page)) / 2, 9, WHITE, 12, page);

			if (pkx_get_species(pkmn) > 0 && pkx_get_species(pkmn) < 822) {
				u16 tempspecies = pkx_get_form_species_number(pkmn);
				u8 type1 = 0, type2 = 0;

				if (tempspecies == 493 || tempspecies == 773) {
					type1 = pkx_get_form(pkmn);
					if (ISGEN4 && type1 > 9)
						type1--;
					type2 = type1;
				} else {
					type1 = personal.pkmData[tempspecies][0x6];
					type2 = personal.pkmData[tempspecies][0x7];
				}
				
				sf2d_draw_texture_part(typesSheet, 273, 120, 50 * type1, 0, 50, 18); 
				if (type1 != type2)
					sf2d_draw_texture_part(typesSheet, 325, 120, 50 * type2, 0, 50, 18); 
				
				u32 nick[NICKNAMELENGTH*2];
				memset(nick, 0, NICKNAMELENGTH*2*sizeof(u32));
				pkx_get_nickname(pkmn, nick);
				sftd_draw_wtext(fontBold12, 273, 69, WHITE, 12, (wchar_t*)nick);
				
				wchar_t* level = (wchar_t*)malloc(8 * sizeof(wchar_t));
				swprintf(level, 8, i18n(S_GRAPHIC_PKBANK_LV_PKMN), pkx_get_level(pkmn));
				sftd_draw_wtext(fontBold12, 372 - sftd_get_wtext_width(fontBold12, 12, level), 86, WHITE, 12, level);
				
				if (pkx_get_gender(pkmn) == 0)
					sf2d_draw_texture(male, 358 - sftd_get_wtext_width(fontBold12, 12, level), 86);
				else if (pkx_get_gender(pkmn) == 1)
					sf2d_draw_texture(female, 360 - sftd_get_wtext_width(fontBold12, 12, level), 86);
				if (pkx_is_shiny(pkmn))
					sf2d_draw_texture(shinyStar, 360 - sftd_get_wtext_width(fontBold12, 12, level) - 14, 88);
				
				free(level);
				
				u32 ot_name[NICKNAMELENGTH*2];
				memset(ot_name, 0, NICKNAMELENGTH*2*sizeof(u32));
				pkx_get_ot(pkmn, ot_name);
				sftd_draw_wtext(fontBold12, 273, 146, WHITE, 12, (wchar_t*)ot_name);

				wchar_t* otid = (wchar_t*)malloc(12 * sizeof(wchar_t));
				swprintf(otid, 12, i18n(S_GRAPHIC_PKBANK_OTID_PKMN), pkx_get_tid(pkmn));
				sftd_draw_wtext(fontBold12, 372 - sftd_get_wtext_width(fontBold12, 12, otid), 163, WHITE, 12, otid);
				free(otid);
				
				sftd_draw_wtext(fontBold12, 273, 104, WHITE, 12, listSpecies.items[pkx_get_species(pkmn)]);
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
				if (n) printElementBlend(pkmnbuf, GAME_SUN, n, pointer[0] - 14, pointer[1] + 8, RGBA8(0x0, 0x0, 0x0, 100));
				if (n) printElement(pkmnbuf, GAME_SUN, n, pointer[0] - 18, pointer[1] + 3);
				sf2d_draw_texture(selector, pointer[0], pointer[1] - 2 - ((!isBufferized) ? movementOffsetSlow(3) : 0));
			}
		}
	pksm_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(boxView, 0, 0);
		sf2d_draw_texture(editorBar, 0, 210);
		swprintf(page, MAX_LENGTH_BOX_NAME+1, isWirelessActivated ? L"Wireless %d" : i18n(S_GRAPHIC_PKBANK_SAVED_BOX_TITLE), saveBox + 1);
		sftd_draw_wtext(fontBold12, 12 + (178 - sftd_get_wtext_width(fontBold12, 12, page)) / 2, 19, WHITE, 12, page);
		sf2d_draw_texture(left, 7, 17);
		sf2d_draw_texture(right, 185, 17);
		sf2d_draw_texture(transferButton, 242, 5);
		sf2d_draw_texture(back, 283, 211);
		sf2d_draw_texture(button, 208, 43);
		sf2d_draw_texture(button, 208, 70);
		sf2d_draw_texture(button, 208, 97);
		sf2d_draw_texture(button, 208, 153);
		sf2d_draw_texture(button, 208, 180);
		sftd_draw_wtext(fontBold12, 208 + (109 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKBANK_MENU_VIEW))) / 2, 50, BLACK, 12, i18n(S_GRAPHIC_PKBANK_MENU_VIEW));
		sftd_draw_wtext(fontBold12, 208 + (109 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKBANK_MENU_CLEARBOX))) / 2, 77, BLACK, 12, i18n(S_GRAPHIC_PKBANK_MENU_CLEARBOX));
		sftd_draw_wtext(fontBold12, 208 + (109 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKBANK_MENU_RELEASE))) / 2, 104, BLACK, 12, i18n(S_GRAPHIC_PKBANK_MENU_RELEASE));
		sftd_draw_wtext(fontBold12, 208 + (109 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKBANK_MENU_DEX))) / 2, 160, BLACK, 12, i18n(S_GRAPHIC_PKBANK_MENU_DEX));
		sftd_draw_wtext(fontBold12, 208 + (109 - sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_PKBANK_MENU_DUMP))) / 2, 187, BLACK, 12, i18n(S_GRAPHIC_PKBANK_MENU_DUMP));
#if PKSV
#else
		sf2d_draw_texture(otaButton, 240, 211);
#endif
		
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
				if (n) printElementBlend(pkmnbuf, GAME_SUN, n, pointer[0] - 14, pointer[1] + 8, RGBA8(0x0, 0x0, 0x0, 100));
				if (n) printElement(pkmnbuf, GAME_SUN, n, pointer[0] - 18, pointer[1] + 3);
				sf2d_draw_texture(selector, pointer[0], pointer[1] - 2 - ((!isBufferized) ? movementOffsetSlow(3) : 0));
			} else
				sf2d_draw_texture(selector, pointer[0], pointer[1] - 2);
		}
		
		if (isSeen)
			sf2d_draw_rectangle(0, -30, 320, 240, MASKBLACK);
		
		if (bank_getIsInternetWorking())
			sftd_draw_wtextf(fontBold9, 10, 220, WHITE, 9, i18n(S_HTTP_SERVER_RUNNING), socket_get_ip());
			
	pksm_end_frame();
	sf2d_swapbuffers();
	
	free(pkmn);
	free(page);
}

void printSettings(int box, int language) {
	wchar_t *menu[] = {i18n(S_GRAPHIC_SETTINGS_BANK_SIZE), i18n(S_GRAPHIC_SETTINGS_BACKUP_SAVE), i18n(S_GRAPHIC_SETTINGS_BACKUP_BANK)};
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop();
	pksm_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printMenuBottom();

		for (int i = 0; i < 3; i++) {
			sf2d_draw_texture(menuBar, (320 - menuBar->width) / 2, 60 + i * (menuBar->height));
			sftd_draw_wtext(fontBold15, i == 0 ? 69 : (320 - sftd_get_wtext_width(fontBold15, 15, menu[i])) / 2, 58 + (menuBar->height - 18) / 2 + i * (menuBar->height), DARKBLUE, 15, menu[i]);
		}

#if PKSV
#else
		sf2d_draw_texture(otaButton, 280, 220);
#endif
		sf2d_draw_texture(miniBox, 189, 64);
		sf2d_draw_texture(minusButton, 169, 65);
		sf2d_draw_texture(plusButton, 228, 65);
		sf2d_draw_texture(miniBox, 281, 191);
		sftd_draw_text(fontBold11, 281 + (36 - (sftd_get_text_width(fontBold11, 11, langs[language]))) / 2, 195, DARKBLUE, 11, langs[language]);
		
		char size[5];
		snprintf(size, 5, "%d", box);
		sftd_draw_text(fontBold11, 189 + (36 - (sftd_get_text_width(fontBold11, 11, size))) / 2, 68, WHITE, 11, size);
		printBottomIndications(i18n(S_GRAPHIC_SETTINGS_INDICATION));
	pksm_end_frame();
	sf2d_swapbuffers();
}

void printSaveEditorInfo(u8* mainbuf, int byte) {
	/*int y = 70, x = 8;
	u32 string[NICKNAMELENGTH*2];
	memset(string, 0, NICKNAMELENGTH*2);

	if (game == GAME_SUN || game == GAME_MOON) {
		if (byte >= SAVE_SM_ITEM && byte < SAVE_SM_ITEM_SIZE) {

		}
	}*/
}

void printfHexEditorInfo(u8* pkmn, int byte) {
	int y = 70, x = 8, xribbon = 90;
	u32 string[NICKNAMELENGTH*2];
	memset(string, 0, NICKNAMELENGTH*2*sizeof(u32));
	
	switch (byte) {
		case 0x08 :
		case 0x09 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_SPECIES), pkx_get_species(pkmn), listSpecies.items[pkx_get_species(pkmn)]);
			break;
		case 0x0A :
		case 0x0B :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_ITEM), pkx_get_item(pkmn), items[pkx_get_item(pkmn)]);
			break;
		case 0x0C :
		case 0x0D :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_TID), pkx_get_tid(pkmn));
			sftd_draw_textf(fontBold12, x, y + 16, LIGHTBLUE, 12, "Gen7 TID: %d", ((unsigned int)(pkx_get_tid(pkmn) + pkx_get_sid(pkmn)*65536) % 1000000));
			break;
		case 0x0E :
		case 0x0F :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_SID), pkx_get_sid(pkmn));
			sftd_draw_textf(fontBold12, x, y + 16, LIGHTBLUE, 12, "Gen7 TID: %d", ((unsigned int)(pkx_get_tid(pkmn) + pkx_get_sid(pkmn)*65536) % 1000000));
			break;
		case 0x14 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ABILITY), abilities[pkx_get_ability(pkmn)]);
			break;
		case 0x1C :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_NATURE), natures[pkx_get_nature(pkmn)]);
			break;
		case 0x1D : {
			wchar_t* entries[] = { i18n(S_GRAPHIC_HEXEDITOR_FATEFUL_ENCOUNTER_FLAG) };
			int i = sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_HEXEDITOR_GENDER));
			sftd_draw_wtext(fontBold12, xribbon + 27, y, LIGHTBLUE, 12, entries[0]);
			sf2d_draw_rectangle(xribbon, y, 13, 13, (pkmn[byte] & 1) ? BUTTONGREEN : BUTTONRED);
			sftd_draw_wtext(fontBold12, x, y + 17, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_GENDER));
			if (pkx_get_gender(pkmn) == 0)
				sftd_draw_wtext(fontBold12, x + i, y + 17, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_GENDER_MALE));
			else if (pkx_get_gender(pkmn) == 1)
				sftd_draw_wtext(fontBold12, x + i, y + 17, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_GENDER_FEMALE));
			else
				sftd_draw_wtext(fontBold12, x + i, y + 17, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_GENDER_GENDERLESS));
			sftd_draw_wtextf(fontBold12, x, y + 17 * 2, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ALTERNATIVE_FORM), pkmn[byte] >> 3);
			break;
		}
		case 0x1E :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EV_HP), pkmn[byte]);
			break;
		case 0x1F : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EV_ATK), pkmn[byte]);
			break;
		case 0x20 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EV_DEF), pkmn[byte]);
			break;	
		case 0x21 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EV_SPE), pkmn[byte]);
			break;
		case 0x22 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EV_SPA), pkmn[byte]);
			break;	
		case 0x23 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EV_SPD), pkmn[byte]);
			break;	
		case 0x24 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_COOL), pkmn[byte]);
			break;	
		case 0x25 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_BEAUTY), pkmn[byte]);
			break;	
		case 0x26 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_CUTE), pkmn[byte]);
			break;	
		case 0x27 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_SMART), pkmn[byte]);
			break;	
		case 0x28 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_TOUGH), pkmn[byte]);
			break;	
		case 0x29 : 
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_CONTEST_VALUE_SHEEN), pkmn[byte]);
			break;
		case 0x30 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_KALOS, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_G3_HOENN, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_SINNOH, S_GRAPHIC_HEXEDITOR_RIBBON_BEST_FRIENDS, S_GRAPHIC_HEXEDITOR_RIBBON_TRAINING,S_GRAPHIC_HEXEDITOR_RIBBON_SKILLFUL_BATTLER, S_GRAPHIC_HEXEDITOR_RIBBON_BATTLER_EXPERT, S_GRAPHIC_HEXEDITOR_RIBBON_EFFORT };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 0, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x31 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_ALERT, S_GRAPHIC_HEXEDITOR_RIBBON_SHOCK, S_GRAPHIC_HEXEDITOR_RIBBON_DOWNCAST, S_GRAPHIC_HEXEDITOR_RIBBON_CARELESS, S_GRAPHIC_HEXEDITOR_RIBBON_RELAX, S_GRAPHIC_HEXEDITOR_RIBBON_SNOOZE, S_GRAPHIC_HEXEDITOR_RIBBON_SMILE, S_GRAPHIC_HEXEDITOR_RIBBON_GORGEOUS };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 1, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x32 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_ROYAL, S_GRAPHIC_HEXEDITOR_RIBBON_GORGEOUS_ROYAL, S_GRAPHIC_HEXEDITOR_RIBBON_ARTIST, S_GRAPHIC_HEXEDITOR_RIBBON_FOOTPRINT, S_GRAPHIC_HEXEDITOR_RIBBON_RECORD, S_GRAPHIC_HEXEDITOR_RIBBON_LEGEND, S_GRAPHIC_HEXEDITOR_RIBBON_COUNTRY, S_GRAPHIC_HEXEDITOR_RIBBON_NATIONAL };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 2, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x33 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_EARTH, S_GRAPHIC_HEXEDITOR_RIBBON_WORLD, S_GRAPHIC_HEXEDITOR_RIBBON_CLASSIC, S_GRAPHIC_HEXEDITOR_RIBBON_PREMIER, S_GRAPHIC_HEXEDITOR_RIBBON_EVENT, S_GRAPHIC_HEXEDITOR_RIBBON_BIRTHDAY, S_GRAPHIC_HEXEDITOR_RIBBON_SPECIAL, S_GRAPHIC_HEXEDITOR_RIBBON_SOUVENIR };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 3, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x34 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_WISHING, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_BATTLE, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_REGIONAL, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_NATIONAL, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_WORLD, S_GRAPHIC_HEXEDITOR_RIBBON_38, S_GRAPHIC_HEXEDITOR_RIBBON_39, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_G6_HOENN };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 4, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x35 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_CONTEST_STAR, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_COOLNESS, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_BEAUTY, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_CUTENESS, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_CLEVERNESS, S_GRAPHIC_HEXEDITOR_RIBBON_MASTER_TOUGHNESS, S_GRAPHIC_HEXEDITOR_RIBBON_CHAMPION_ALOLA, S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_ROYALE };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 5, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
		case 0x36 : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_TREE_GREAT, S_GRAPHIC_HEXEDITOR_RIBBON_BATTLE_TREE_MASTER, S_GRAPHIC_HEXEDITOR_RIBBON_51, S_GRAPHIC_HEXEDITOR_RIBBON_52, S_GRAPHIC_HEXEDITOR_RIBBON_53, S_GRAPHIC_HEXEDITOR_RIBBON_54, S_GRAPHIC_HEXEDITOR_RIBBON_55, S_GRAPHIC_HEXEDITOR_RIBBON_56 };
			for (int i = 0; i < 8; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17*i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17*i, 13, 13, (pkx_get_ribbons(pkmn, 6, i)) ? BUTTONGREEN : BUTTONRED);
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
			sftd_draw_wtext(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_NICKNAME));
			sftd_draw_wtext(fontBold12, x + sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_HEXEDITOR_NICKNAME)), y, LIGHTBLUE, 12, (wchar_t*)string);
			break;
		case 0x5A :
		case 0x5B :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE1), pkx_get_move(pkmn, 0), moves[pkx_get_move(pkmn, 0)]);
			break;
		case 0x5C :
		case 0x5D :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE2), pkx_get_move(pkmn, 1), moves[pkx_get_move(pkmn, 1)]);
			break;
		case 0x5E :
		case 0x5F :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE3), pkx_get_move(pkmn, 2), moves[pkx_get_move(pkmn, 2)]);
			break;
		case 0x60 :
		case 0x61 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE4), pkx_get_move(pkmn, 3), moves[pkx_get_move(pkmn, 3)]);
			break;
		case 0x62 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE1_PP), pkmn[byte]);
			break;
		case 0x63 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE2_PP), pkmn[byte]);
			break;
		case 0x64 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE3_PP), pkmn[byte]);
			break;
		case 0x65 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE4_PP), pkmn[byte]);
			break;
		case 0x66 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE1_PPUP), pkmn[byte]);
			break;
		case 0x67 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE2_PPUP), pkmn[byte]);
			break;
		case 0x68 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE3_PPUP), pkmn[byte]);
			break;
		case 0x69 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MOVE4_PPUP), pkmn[byte]);
			break;
		case 0x6A :
		case 0x6B :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE1), pkx_get_egg_move(pkmn, 0), moves[pkx_get_egg_move(pkmn, 0)]);
			break;
		case 0x6C :
		case 0x6D :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE2), pkx_get_egg_move(pkmn, 1), moves[pkx_get_egg_move(pkmn, 1)]);
			break;
		case 0x6E :
		case 0x6F :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE3), pkx_get_egg_move(pkmn, 2), moves[pkx_get_egg_move(pkmn, 2)]);
			break;
		case 0x70 :
		case 0x71 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_RELEARN_MOVE4), pkx_get_egg_move(pkmn, 3), moves[pkx_get_egg_move(pkmn, 3)]);
			break;
		case 0x77: {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_IS_NICKNAMED, S_GRAPHIC_HEXEDITOR_IS_EGG };
			sftd_draw_wtext(fontBold12, xribbon + 27, y, LIGHTBLUE, 12, i18n(entries[0]));
			sf2d_draw_rectangle(xribbon, y, 13, 13, (pkx_get_nickname_flag(pkmn)) ? BUTTONGREEN : BUTTONRED);
			sftd_draw_wtext(fontBold12, xribbon + 27, y + 17, LIGHTBLUE, 12, i18n(entries[1]));
			sf2d_draw_rectangle(xribbon, y + 17, 13, 13, (pkx_is_egg(pkmn)) ? BUTTONGREEN : BUTTONRED);
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
			sftd_draw_wtext(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_NAME));
			sftd_draw_wtext(fontBold12, x + sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_NAME)), y, LIGHTBLUE, 12, (wchar_t*)string);
			break;
		case 0xA2 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_FRIENDSHIP), pkmn[byte]);
			break;
		case 0xA3 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_AFFECTION), pkmn[byte]);
			break;
		case 0xA4 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_INTENSITY), pkmn[byte]);
			break;
		case 0xA5 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_MEMORY), pkmn[byte]);
			break;	
		case 0xA6 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_HELD_TRAINER_FEELING), pkmn[byte]);
			break;
		case 0xAE :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_FULLNESS), pkmn[byte]);
			break;	
		case 0xAF :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ENJOYMENT), pkmn[byte]);
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
			sftd_draw_wtext(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_NAME));
			sftd_draw_wtext(fontBold12, x + sftd_get_wtext_width(fontBold12, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_NAME)), y, LIGHTBLUE, 12, (wchar_t*)string);
			break;
		case 0xCA :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_FRIENDSHIP), pkmn[byte]);
			break;
		case 0xCB :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_AFFECTION), pkmn[byte]);
			break;
		case 0xCC :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_INTENSITY), pkmn[byte]);
			break;
		case 0xCD :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_MEMORY), pkmn[byte]);
			break;	
		case 0xD0 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_FEELING), pkmn[byte]);
			break;
		case 0xD1 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EGG_YEAR), 2000 + pkmn[byte]);
			break;
		case 0xD2 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EGG_MONTH), pkmn[byte]);
			break;
		case 0xD3 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_EGG_DAY), pkmn[byte]);
			break;
		case 0xD4 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MET_YEAR), 2000 + pkmn[byte]);
			break;
		case 0xD5 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MET_MONTH), pkmn[byte]);
			break;
		case 0xD6 :
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MET_DAY), pkmn[byte]);
			break;
		case 0xDD : {
			sftd_draw_wtextf(fontBold12, x, y, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_MET_LEVEL), (pkmn[byte] & 0x7f));
			sftd_draw_wtextf(fontBold12, xribbon + 27, y + 17, LIGHTBLUE, 12, i18n(S_GRAPHIC_HEXEDITOR_ORIGINAL_TRAINER_GENDER));
			if (pkx_get_ot_gender(pkmn) == 0)
				sf2d_draw_texture(male, xribbon + 10, y + 17 + 2);
			else if (pkx_get_ot_gender(pkmn) == 1)
				sf2d_draw_texture(female, xribbon + 10 + 2, y + 17 + 2);
			break;
		}
		case 0xDE : {
			AppTextCode entries[] = { S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_HP, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_ATK, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_DEF, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPATK, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPDEF, S_GRAPHIC_HEXEDITOR_HYPER_TRAINED_SPEED };
			for (int i = 0; i < 6; i++) {
				sftd_draw_wtext(fontBold12, xribbon + 27, y + 17 * i, LIGHTBLUE, 12, i18n(entries[i]));
				sf2d_draw_rectangle(xribbon, y + 17 * i, 13, 13, (pkx_get_hti(pkmn, i)) ? BUTTONGREEN : BUTTONRED);
			}
			break;
		}
	}
}
