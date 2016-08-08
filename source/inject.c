#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <string.h>
#include "util.h"

#define ENTRIES 14
#define BALLS 25
#define ITEM 18
#define HEAL 26
#define BERRIES 64

void injectWC6(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]) {
	if (game == 2 || game == 3) {		
		*(mainbuf + 0x1CC00 + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + 0x1CD00 + nInjected[0] * 264), (const void*)wc6buf, 264);
	}
	
	if (game == 0 || game == 1) {
		*(mainbuf + 0x1BC00 + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + 0x1BD00 + nInjected[0] * 264), (const void*)wc6buf, 264);
	}

	nInjected[0] += 1;
}

void injectLanguage(u8* mainbuf, int i) {
	const u32 value[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08};

	*(mainbuf + 0x1402D) = value[i];
}

void injectMoney(u8* mainbuf, u64 i) {
	switch (i) {
		case 0 : {
			*(mainbuf + 0x4208) = 0x00;
			*(mainbuf + 0x4209) = 0x00;
			*(mainbuf + 0x420A) = 0x00;
			break;			
		}	
		case 200000 : {
			*(mainbuf + 0x4208) = 0x40;
			*(mainbuf + 0x4209) = 0x0D;
			*(mainbuf + 0x420A) = 0x03;
			break;			
		}	
		case 1000000 : {
			*(mainbuf + 0x4208) = 0x40;
			*(mainbuf + 0x4209) = 0x42;
			*(mainbuf + 0x420A) = 0x0F;
			break;			
		}
		case 9999999 : {
			*(mainbuf + 0x4208) = 0x7F;
			*(mainbuf + 0x4209) = 0x96;
			*(mainbuf + 0x420A) = 0x98;
			break;
		}
	}
}

void injectItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[], int game) {
	u32 offset[3] = {0x400, 0xD68, 0xE68};
	if (game == 2 || game == 3) {
		offset[1] = 0xD70;
		offset[2] = 0xE70;
	}

	if (i % 2 == 0) {
		*(mainbuf + offset[type] + nInjected[type] * 4) = values[i];
		*(mainbuf + offset[type + 1] + nInjected[type] * 4) = values[i + 1];
	}
	else {
		*(mainbuf + offset[type] + nInjected[type] * 4) = values[i - 1];
		*(mainbuf + offset[type] + 1 + nInjected[type] * 4) = values[i];
	}
	
	switch (i % 2) {
		case 0 : {
			*(mainbuf + offset[type] + 2 + nInjected[type] * 4) = 0x01;
			*(mainbuf + offset[type] + 3 + nInjected[type] * 4) = 0x00;
			break;				
		}
		case 1 : {
			*(mainbuf + offset[type] + 2 + nInjected[type] * 4) = 0xE3;
			*(mainbuf + offset[type] + 3 + nInjected[type] * 4) = 0x03;
			break;				
		}
	}
	nInjected[type]++;
}

void injectBP(u8* mainbuf, int i) {
	switch (i) {
		case 0 : {
			*(mainbuf + 0x4230) = 0x00;
			*(mainbuf + 0x4231) = 0x00;
			break;			
		}
		case 9999 : {
			*(mainbuf + 0x4230) = 0x0F;
			*(mainbuf + 0x4231) = 0x27;
			break;			
		}
	}
}

void injectBadges(u8* mainbuf, int i) {
	const u32 value[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
	
	*(mainbuf + 0x420C) = value[i];
}

void injectTM(u8* mainbuf) {
	const u32 values[] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0x6A, 0x6B, 0x6C, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6};
	for (int i = 0; i < 100; i++) {
		*(mainbuf + 0xBC0 + i * 4) = values[i];
		*(mainbuf + 0xBC0 + i * 4 + 1) = 0x01;
		*(mainbuf + 0xBC0 + i * 4 + 2) = 0x01;
		*(mainbuf + 0xBC0 + i * 4 + 3) = 0x00;
	}
	for (int i = 0; i < 8; i++) {
		*(mainbuf + 0xD31 + i * 4) = 0x02;
	}
}

void refreshValues(PrintConsole topScreen, int game, u64 money[], int BP[], int langCont, int moneyCont, int ballCont, int itemCont, int healCont, int BPCont, int badgeCont, int berryCont, int nInjected[]) {
	char *language[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
	char *ballsList[BALLS] = {"Master Ball", "Ultra Ball", "Great Ball", "Poke Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Fast Ball", "Level Ball", "Lure Ball", "Heavy Ball", "Love Ball", "Friend Ball", "Moon Ball", "Sport Ball", "Park Ball"};
	char *itemList[ITEM] = {"Mental Herb", "Choice Band", "King's Rock", "Amulet Coin", "Leftovers", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Focus Sash", "Destiny Knot", "Choice Scarf", "Choice Specs", "Eviolite", "Rocky Helmet", "Air Balloon", "Weakness Policy", "Assault Vest"};
	char *healList[HEAL] = {"Full Restore", "Max Potion", "Revive", "Max Revive", "Ether", "Max Ether", "Elixir", "Max Elixir", "Sacred Ash", "HP Up   ", "Protein", "Iron   ", "Carbos", "Calcium", "Rare Candy", "PP UP  ", "Zinc   ", "PP Max ", "Health Wing", "Muscle Wing", "Resist Wing", "Genius Wing", "Clever Wing", "Swift Wing", "Pretty Wing", "Ability Capsule"};
	char *berryList[BERRIES] = {"Cheri", "Chesto", "Pecha", "Rawst", "Aspear", "Leppa", "Oran", "Persim", "Lum", "Sitrus", "Figy", "Wiki", "Mago", "Aguav", "Iapapa", "Razz", "Bluk", "Nanab", "Wepear", "Pinap", "Pomeg", "Kelpsy", "Qualot", "Hondew", "Grepa", "Tamato", "Comn", "Magost", "Rabuta", "Nomel", "Spelon", "Pamtre", "Watmel", "Durin", "Belue", "Occa", "Passho", "Wacan", "Rindo", "Yache", "Chople", "Kebia", "Shuca", "Coba", "Payapa", "Tanga", "Charti", "Kasib", "Haban", "Colbur", "Babiri", "Chilan", "Liechi", "Ganlon", "Salac", "Petaya", "Apicot", "Lansat", "Starf", "Enigma", "Micle", "Custap", "Jaboca", "Rowap"};
	consoleSelect(&topScreen);
	
	switch (game) {
		case 0 : {
			printf("\x1b[2;28H\x1b[32mX \x1b[0m");
			break;
		}
		case 1 : {
			printf("\x1b[2;28H\x1b[32mY \x1b[0m");
			break;
		}
		case 2 : {
			printf("\x1b[2;28H\x1b[32mOR\x1b[0m");
			break;
		}
		case 3 : {
			printf("\x1b[2;28H\x1b[32mAS\x1b[0m");
			break;
		}
	}
	printf("\x1b[3;28H\x1b[1;33m%s\x1b[0m", language[langCont]);
	printf("\x1b[4;28H\x1b[1;33m%llu\x1b[0m$       ", money[moneyCont]);
	printf("\x1b[5;19H%d:\x1b[5;28H\x1b[1;33m%s\x1b[0m     ", nInjected[0] + 1,  ballsList[ballCont / 2]);
	if (ballCont % 2 == 0) printf("\x1b[5;45Hx1  "); else printf("\x1b[5;45Hx995");
	printf("\x1b[6;19H%d:\x1b[6;28H\x1b[1;33m%s\x1b[0m     ", nInjected[0] + 1, itemList[itemCont / 2]);
	if (itemCont % 2 == 0) printf("\x1b[6;45Hx1  "); else printf("\x1b[6;45Hx995");
	printf("\x1b[7;19H%d:\x1b[7;28H\x1b[1;33m%s\x1b[0m     ", nInjected[1] + 1, healList[healCont / 2]);
	if (healCont % 2 == 0) printf("\x1b[7;45Hx1  "); else printf("\x1b[7;45Hx995");
	printf("\x1b[8;19H%d:\x1b[8;28H\x1b[1;33m%s Berry\x1b[0m     ", nInjected[2] + 1, berryList[berryCont / 2]);
	if (berryCont % 2 == 0) printf("\x1b[8;45Hx1  "); else printf("\x1b[8;45Hx995");
	printf("\x1b[9;28H\x1b[1;33m%d\x1b[0m    ", BP[BPCont]);
	printf("\x1b[10;28H\x1b[1;33m%d\x1b[0m badges", badgeCont);
	printf("\x1b[29;5HInjection locations: \x1b[32m%d\x1b[0m/%d | \x1b[32m%d\x1b[0m/%d | \x1b[32m%d\x1b[0m/%d  ", nInjected[0], BALLS + ITEM, nInjected[1], HEAL, nInjected[2], BERRIES);
}

int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int nInjected[]) {
	char *menuEntries[ENTRIES] = {"Game is:", "Set language to:", "Set money to:", "Set ball to slot", "Set item to slot", "Set heal to slot", "Set berry to slot", "Set Battle Points to:", "Set number of badges to:", "Set all TMs", "Set Poke Balls to max", "Set all available items to max", "Set all available heals to max", "Set all available berries to max"};

	const char *path[4] = {"/JKSV/Saves/Pokémon_X/EventAssistant/main", "/JKSV/Saves/Pokémon_Y/EventAssistant/main", "/JKSV/Saves/Pokémon_Omega_Ruby/EventAssistant/main", "/JKSV/Saves/Pokémon_Alpha_Sapphire/EventAssistant/main"};
	const char *bakPath[4] = {"/JKSV/Saves/Pokémon_X/EventAssistant/main.bak", "/JKSV/Saves/Pokémon_Y/EventAssistant/main.bak", "/JKSV/Saves/Pokémon_Omega_Ruby/EventAssistant/main.bak", "/JKSV/Saves/Pokémon_Alpha_Sapphire/EventAssistant/main.bak"};	

	u32 balls[BALLS * 2] = {0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0X0E, 0X00, 0x0F, 0x00, 0x10, 0x00, 0xEC, 0x01, 0xED, 0x01, 0xEE, 0x01, 0xEF, 0x01, 0xF0, 0x01, 0xF2, 0x01, 0xF3, 0x01, 0xF4, 0x01};
	u32 items[ITEM * 2] = {0xDB, 0x00, 0xDC, 0x00, 0xDD, 0x00, 0xDF, 0x00, 0xEA, 0x00, 0x0E, 0x01, 0x0F, 0x01, 0x10, 0x01, 0x11, 0x01, 0x13, 0x01, 0x18, 0x01, 0x1F, 0x01, 0x29, 0x01, 0x1A, 0x02, 0x1C, 0x02, 0x1D, 0x02, 0x7F, 0x02, 0x80, 0x02};
	u32 heal[HEAL * 2] = {0x17, 0x00, 0x18, 0x00, 0x1C, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2C, 0x00, 0x2D, 0x00, 0x2E, 0x00, 0x2F, 0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x35, 0x02, 0x36, 0x02, 0x37, 0x02, 0x38, 0x02, 0x39, 0x02, 0x3A, 0x02, 0x3B, 0x02, 0x85, 0x02};
	
	//fill berries
	u32 berry[BERRIES * 2];
	for (int i = 0; i < BERRIES * 2 ; i += 2) {
		berry[i] = 0x95 + i / 2;
		berry[i + 1] = 0x00;
	}

	u64 money[4] = {0, 200000, 1000000, 9999999};
	int BP[2] = {0, 9999};
	
	int langCont = 0;
	int moneyCont = 0;
	int BPCont = 0;
	int badgeCont = 0;
	int ballCont = 0;
	int itemCont = 0;
	int healCont = 0;
	int berryCont = 0;

	int currentEntry = 0;
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
	printf("\x1b[32mA\x1b[0m - Switch setting\n");
	printf("\x1b[31mSTART\x1b[0m - Start selected change\n");
	printf("----------------------------------------");
	printf("\x1b[5;0HYou need to have a \x1b[32mmain\x1b[0m located at\n\x1b[32m/JKSV/Saves/[game]/EventAssistant/main\x1b[0m.");
	printf("\n\nYou can perform one edit, then you need to reopen this function to make another one.");
	printf("\x1b[21;0H----------------------------------------");
	printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
	printf("\x1b[26;0H----------------------------------------");
	printf("\x1b[29;12HPress B to exit.");
	
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                 Save file Editor                 \x1b[0m\n");
	
	refresh(currentEntry, topScreen, menuEntries, ENTRIES);
	refreshValues(topScreen, game[0], money, BP, langCont, moneyCont, ballCont, itemCont, healCont, BPCont, badgeCont, berryCont, nInjected);	
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B) 
			break; 
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry == 0) {
				currentEntry = ENTRIES - 1;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
				refreshValues(topScreen, game[0], money, BP, langCont, moneyCont, ballCont, itemCont, healCont, BPCont, badgeCont, berryCont, nInjected);	
			}
			else if (currentEntry > 0) {
				currentEntry--;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
				refreshValues(topScreen, game[0], money, BP, langCont, moneyCont, ballCont, itemCont, healCont, BPCont, badgeCont, berryCont, nInjected);	
			}
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry == ENTRIES - 1) {
				currentEntry = 0;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
				refreshValues(topScreen, game[0], money, BP, langCont, moneyCont, ballCont, itemCont, healCont, BPCont, badgeCont, berryCont, nInjected);	
			}
			else if (currentEntry < ENTRIES - 1) {
				currentEntry++;
				refresh(currentEntry, topScreen, menuEntries, ENTRIES);
				refreshValues(topScreen, game[0], money, BP, langCont, moneyCont, ballCont, itemCont, healCont, BPCont, badgeCont, berryCont, nInjected);
			}
		}
		
		if (hidKeysDown() & KEY_A) {
			switch (currentEntry) {
				case 0 : {
					if (game[0] < 3) game[0] += 1;
					else if (game[0] == 3) game[0] = 0;
					break;
				}
				case 1 : {
					if (langCont < 6) langCont++;
					else if (langCont == 6) langCont = 0;
					break;					
				}
				case 2 : {
					if (moneyCont < 3) moneyCont++;
					else if (moneyCont == 3) moneyCont = 0;
					break;					
				}
				case 3 : {
					if (ballCont < BALLS * 2 - 1) ballCont++;
					else if (ballCont == BALLS * 2 - 1) ballCont = 0;
					break;					
				}
				case 4 : {
					if (itemCont < ITEM * 2 - 1) itemCont++;
					else if (itemCont == ITEM * 2 - 1) itemCont = 0;
					break;						
				}
				case 5 : {
					if (healCont < HEAL * 2 - 1) healCont++;
					else if (healCont == HEAL * 2 - 1) healCont = 0;
					break;						
				}
				case 6 : {
					if (berryCont < BERRIES * 2 - 1) berryCont++;
					else if (berryCont == BERRIES * 2 - 1) berryCont = 0;
					break;						
				}
				case 7 : {
					if (BPCont < 1) BPCont++;
					else if (BPCont == 1) BPCont = 0;
					break;					
				}
				case 8 : {
					if (badgeCont < 8) badgeCont++;
					else if (badgeCont == 8) badgeCont = 0;
					break;
				}
			}
			refreshValues(topScreen, game[0], money, BP, langCont, moneyCont, ballCont, itemCont, healCont, BPCont, badgeCont, berryCont, nInjected);	
		}

		if (hidKeysDown() & KEY_START) {		
			fsInit();
			
			//reading main
			FILE *fptr = fopen(path[game[0]], "rt");
			if (fptr == NULL) {
				fclose(fptr);
				fsExit();
				return -1;
			}
			fseek(fptr, 0, SEEK_END);
			u32 mainsize = ftell(fptr);
			u8* mainbuf = malloc(mainsize);
			rewind(fptr);
			fread(mainbuf, mainsize, 1, fptr);
			fclose(fptr);
			
			//doing backup
			FILE *fptr1 = fopen(bakPath[game[0]], "wb");
			fwrite(mainbuf, 1, mainsize, fptr1);
			fclose(fptr1);

			switch (currentEntry) {
				case 1 : {
					injectLanguage(mainbuf, langCont);
					break;
				}
				case 2 : {
					injectMoney(mainbuf, money[moneyCont]);
					break;
				}
				case 3 : {
					if (nInjected[0] < BALLS + ITEM) {
						injectItem(mainbuf, ballCont, balls, 0, nInjected, game[0]);
						break;
					}
					else return -1;
				}
				case 4 : {
					if (nInjected[0] < BALLS + ITEM) {
						injectItem(mainbuf, itemCont, items, 0, nInjected, game[0]);
						break;
					}
					else return -1;
				}
				case 5 : {
					if (nInjected[1] < HEAL) {
						injectItem(mainbuf, healCont, heal, 1, nInjected, game[0]);
						break;
					}
					else return -1;
				}
				case 6 : {
					if (nInjected[2] < BERRIES) {
						injectItem(mainbuf, berryCont, berry, 2, nInjected, game[0]);
						break;
					}
					else return -1;
				}
				case 7 : {
					injectBP(mainbuf, BP[BPCont]);
					break;
				}
				case 8 : {
					injectBadges(mainbuf, badgeCont);
					break;
				}
				case 9 : {
					injectTM(mainbuf);
					break;
				}
				case 10 : {
					nInjected[0] = 0;
					for (int i = 1; i < BALLS * 2; i += 2)
						injectItem(mainbuf, i, balls, 0, nInjected, game[0]);
					break;
				}
				case 11 : {
					nInjected[0] = 0;
					for (int i = 1; i < ITEM * 2; i += 2)
						injectItem(mainbuf, i, items, 0, nInjected, game[0]);
					break;
				}
				case 12 : {
					nInjected[1] = 0;
					for (int i = 1; i < HEAL * 2; i += 2)
						injectItem(mainbuf, i, heal, 1, nInjected, game[0]);
					break;
				}
				case 13 : {
					nInjected[2] = 0;
					for (int i = 1; i < BERRIES * 2; i += 2)
						injectItem(mainbuf, i, berry, 2, nInjected, game[0]);
					break;
				}
			}

			int rwCHK = rewriteCHK(mainbuf, game[0]);
			if (rwCHK != 0)
				return rwCHK;
			
			FILE *fptr2 = fopen(path[game[0]], "wb");
			fwrite(mainbuf, 1, mainsize, fptr2);
			fclose(fptr2);
			
			free(mainbuf);
			fsExit();
			return 1;
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	return 0;
}