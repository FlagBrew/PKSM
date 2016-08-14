#include <3ds.h>
#include <string.h>
#include "pokemon.h"
#include "util.h"

#define PKMNLENGTH 232
#define PARTYPKMNLENGTH 260
#define BOXMAX 31
#define EVLENGTH 1
#define PIDLENGTH 4
#define IVLENGTH 4

#define ENTRIES 9
#define NEVS 11

const int OFFSET = 0x5400;
const int EVPOS = 0x1E;
const int PIDPOS = 0x18;
const int IVPOS = 0x74;

const u32 friendship[4] = {0, 70, 75, 255};

char *hpList[16] = {"Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel", "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark"};

const u32 evs[NEVS][6] = {
  // HP , ATK, DEF, SPE, SPA, SPD
	{  4, 252,   0, 252,   0,   0},
	{  4,   0,   0, 252, 252,   0},
	{252, 252,   4,   0,   0,   0},
	{252, 252,   0,   0,   0,   4},
	{252,   0,   4,   0, 252,   0},
	{252,   0,   0,   0, 252,   4},
	{  0, 252,   0,   4, 252,   0},
	{252,   0, 252,   0,   0,   4},
	{252,   0,   4,   0,   0, 252},
	{252,   0,   4, 252,   0,   0},
	{252,   0,   0, 252,   0,   4}
	};

u32 seedStep(const u32 seed) {
    return (seed*0x41C64E6D + 0x00006073) & 0xFFFFFFFF;
}

void shuffleArray(u8* pkmn, const u32 encryptionkey) {
    const int BLOCKLENGHT = 56;
    
    u8 seed = (((encryptionkey & 0x3E000) >> 0xD) % 24);
    
    int aloc[24] = { 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 };
    int bloc[24] = { 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 };
    int cloc[24] = { 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1 };
    int dloc[24] = { 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0 };
    int ord[4] = {aloc[seed], bloc[seed], cloc[seed], dloc[seed]};
    
    char pkmncpy[PKMNLENGTH];
    char tmp[BLOCKLENGHT];

    memcpy(&pkmncpy, pkmn, PKMNLENGTH);
    
    for (int i = 0; i < 4; i++) {
        memcpy(tmp, pkmncpy + 8 + BLOCKLENGHT * ord[i], BLOCKLENGHT);
        memcpy(pkmn + 8 + BLOCKLENGHT * i, tmp, BLOCKLENGHT);
    }
}

void decryptPkmn(u8* pkmn) {
    const int ENCRYPTIONKEYPOS = 0x0;
    const int ENCRYPTIONKEYLENGHT = 4;
    const int CRYPTEDAREAPOS = 0x08;
    
    u32 encryptionkey;
    memcpy(&encryptionkey, &pkmn[ENCRYPTIONKEYPOS], ENCRYPTIONKEYLENGHT);
    u32 seed = encryptionkey;
    
    u16 temp;
    for (int i = CRYPTEDAREAPOS; i < PKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (seedStep(seed) >> 16);
        seed = seedStep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
    
    shuffleArray(pkmn, encryptionkey);
}

int getPkmnAddress(const int boxnumber, const int indexnumber, int game) {
    int boxpos = 0;
    if (game == 0 || game == 1) {
        if (boxnumber < 31)
            boxpos = 0x27A00 - OFFSET;
        
        else boxpos = 0x19600 - OFFSET;
    }
    
    if (game == 2 || game == 3) {
        if (boxnumber < 31)
            boxpos = 0x38400 - OFFSET;
        
        else boxpos = 0x19600 - OFFSET;
    }
    const int PKMNNUM = 30;
    
    if (boxnumber < 31)
        return boxpos + (PKMNLENGTH * PKMNNUM * boxnumber) + (indexnumber * PKMNLENGTH);
    
    return boxpos + (PARTYPKMNLENGTH*indexnumber);
}

void calculatePKMNChecksum(u8* data) {
    u16 chk = 0;

    for (int i = 8; i < PKMNLENGTH; i += 2)
        chk += *(u16*)(data + i);
    
    memcpy(data + 6, &chk, 2);
}

void encryptPkmn(u8* pkmn) {
    const int ENCRYPTIONKEYPOS = 0x0;
    const int ENCRYPTIONKEYLENGHT = 4;
    const int CRYPTEDAREAPOS = 0x08;
    
    u32 encryptionkey;
    memcpy(&encryptionkey, &pkmn[ENCRYPTIONKEYPOS], ENCRYPTIONKEYLENGHT);
    u32 seed = encryptionkey;
    
    for(int i = 0; i < 11; i++)
        shuffleArray(pkmn, encryptionkey);
        
    u16 temp;
    for(int i = CRYPTEDAREAPOS; i < PKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (seedStep(seed) >> 16);
        seed = seedStep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
}

void encryptBattleSection(u8* pkmn) {
    const int ENCRYPTIONKEYPOS = 0x0;
    const int ENCRYPTIONKEYLENGHT = 4;
    
    u32 encryptionkey;
    memcpy(&encryptionkey, &pkmn[ENCRYPTIONKEYPOS], ENCRYPTIONKEYLENGHT);
    u32 seed = encryptionkey;
    
    u16 temp;
    for(int i = PKMNLENGTH; i < PARTYPKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (seedStep(seed) >> 16);
        seed = seedStep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
}

void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    memcpy(pkmn, &mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], PKMNLENGTH);
    decryptPkmn(pkmn);
}
 
void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    calculatePKMNChecksum(pkmn);
    encryptPkmn(pkmn);
	
	int length = PKMNLENGTH;
    
    if (boxnumber >= BOXMAX) {
        encryptBattleSection(pkmn);
        length = PARTYPKMNLENGTH;
    }
        
    memcpy(&mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], pkmn, length);
}

void refreshPokemon(PrintConsole topScreen, int game, int pokemonCont[]) {
	consoleSelect(&topScreen);

    switch (game) {
        case 0 : {
			printf("\x1b[2;23HX \x1b[0m");
			break;
        }
        case 1 : {
			printf("\x1b[2;23HY \x1b[0m");
			break;
        }
        case 2 : {
			printf("\x1b[2;23HOR\x1b[0m");
			break;
        }
        case 3 : {
			printf("\x1b[2;23HAS\x1b[0m");
			break;
        }
    }
	
	printf("\x1b[3;23H\x1b[33m%d\x1b[0m ", pokemonCont[1] + 1);
	printf("\x1b[4;23H\x1b[33m%d\x1b[0m ", pokemonCont[2] + 1);
	printf("\x1b[5;23H\x1b[33m%lu\x1b[0m  ", friendship[pokemonCont[3]]);
	if (pokemonCont[0] != 5)
		printf("\x1b[7;23H%lu  \x1b[7;27H%lu  \x1b[7;31H%lu  \x1b[7;35H%lu  \x1b[7;39H%lu  \x1b[7;43H%lu  ", evs[pokemonCont[4]][0], evs[pokemonCont[4]][1], evs[pokemonCont[4]][2], evs[pokemonCont[4]][4], evs[pokemonCont[4]][5], evs[pokemonCont[4]][3]);
	else if (pokemonCont[0] == 5)
		printf("\x1b[33m\x1b[7;23H%lu  \x1b[7;27H%lu  \x1b[7;31H%lu  \x1b[7;35H%lu  \x1b[7;39H%lu  \x1b[7;43H%lu\x1b[0m  ", evs[pokemonCont[4]][0], evs[pokemonCont[4]][1], evs[pokemonCont[4]][2], evs[pokemonCont[4]][4], evs[pokemonCont[4]][5], evs[pokemonCont[4]][3]);

	printf("\x1b[9;23H\x1b[33m%s\x1b[0m    ", hpList[pokemonCont[5]]);
}

void setFriendship(u8* pkmn, u32 value) {
	switch (value) {
		case 0 : {
			*(pkmn + 0xA2) = 0x00;
			break;
		}
		case 70 : {
			*(pkmn + 0xA2) = 0x46;
			break;
		}
		case 75 : {
			*(pkmn + 0xA2) = 0x4B;
			break;
		}
		case 255 : {
			*(pkmn + 0xA2) = 0xFF;
			break;
		}
	}
}

void setEV(u8* pkmn, u8 val, const int stat) {
    memcpy(&pkmn[EVPOS+(EVLENGTH*stat)], &val, EVLENGTH);
}

void setIV(u8* pkmn, u8 val, const int stat) {
	u32 nval = val;
	u32 mask = 0xFFFFFFFF;
	mask ^= 0x1F << (5 * stat);

	u32 buffer;
	memcpy(&buffer, &pkmn[IVPOS], IVLENGTH);

	buffer &= mask;
	buffer ^= ((nval & 0x1F) << (5 * stat));
	memcpy(&pkmn[IVPOS], &buffer, IVLENGTH);
}

u8 getIV(u8* pkmn, const int stat) {
    u32 buffer;
    u8 toreturn;
    
    memcpy(&buffer, &pkmn[IVPOS], IVLENGTH);
    buffer = buffer >> 5*stat;
    buffer = buffer & 0x1F;
    memcpy(&toreturn, &buffer, 1);
    
    return toreturn;
}

void setHPType(u8* pkmn, const int val) {
    u8 ivstat[6];
    for(int i = 0; i < 6; i++)
        ivstat[i] = getIV(pkmn, i);
    
    u8 hpivs[16][6] = {
        { 1, 1, 0, 0, 0, 0 }, // Fighting
        { 0, 0, 0, 1, 0, 0 }, // Flying
        { 1, 1, 0, 1, 0, 0 }, // Poison
        { 1, 1, 1, 1, 0, 0 }, // Ground
        { 1, 1, 0, 0, 1, 0 }, // Rock
        { 1, 0, 0, 1, 1, 0 }, // Bug
        { 1, 0, 1, 1, 1, 0 }, // Ghost
        { 1, 1, 1, 1, 1, 0 }, // Steel
        { 1, 0, 1, 0, 0, 1 }, // Fire
        { 1, 0, 0, 1, 0, 1 }, // Water
        { 1, 0, 1, 1, 0, 1 }, // Grass
        { 1, 1, 1, 1, 0, 1 }, // Electric
        { 1, 0, 1, 0, 1, 1 }, // Psychic
        { 1, 0, 0, 1, 1, 1 }, // Ice
        { 1, 0, 1, 1, 1, 1 }, // Dragon
        { 1, 1, 1, 1, 1, 1 }, // Dark
    };
    
    for(int i = 0; i < 6; i++)
         ivstat[i] = (ivstat[i] & 0x1E) + hpivs[val][i];
    
    for(int i = 0; i < 6; i++)
        setIV(pkmn, ivstat[i], i);
}

u32 getPID(u8* pkmn) {
    u32 pidbuffer;
    memcpy(&pidbuffer, &pkmn[PIDPOS], PIDLENGTH);
    
    return pidbuffer;
}

void rerollPID(u8* pkmn) {
    srand(getPID(pkmn));
    u32 pidbuffer = rand();
    
    memcpy(&pkmn[PIDPOS], &pidbuffer, PIDLENGTH);
}

void setShiny(u8* pkmn) {
    rerollPID(pkmn);
}

int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int pokemonCont[]) {
	char *menuEntries[ENTRIES] = {"Game is:", "Select box (1-31):", "Select index (1-30):", "Set f.ship to:", "Set EVs:             HPs|ATK|DEF|SPA|SPD|SPE  ", " ", "Set all IVs to max", "Set Hidden Power:", "Set shiny"};
	
	//X, Y, OR, AS
	const u64 ids[4] = {0x0004000000055D00, 0x0004000000055E00, 0x000400000011C400, 0x000400000011C500};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
	printf("\x1b[32mA\x1b[0m - Switch setting\n");
	printf("\x1b[1;31mSTART\x1b[0m - Start selected change\n");
	printf("----------------------------------------");
	printf("\x1b[17;0HYou can perform one edit, then you need to reopen this function to make another one.");
	printf("\x1b[21;0H----------------------------------------");
	printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
	printf("\x1b[26;0H----------------------------------------");
	printf("\x1b[29;12HPress B to exit.");
	
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");
	
	refresh(pokemonCont[0], topScreen, menuEntries, ENTRIES);
	refreshPokemon(topScreen, game[0], pokemonCont);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B) 
			break; 
		
		if (hidKeysDown() & KEY_DUP) {
			if (pokemonCont[0] == 0) {
				pokemonCont[0] = ENTRIES - 1;
				refresh(pokemonCont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, game[0], pokemonCont);
			}
			else if (pokemonCont[0] > 0) {
				pokemonCont[0]--;
				refresh(pokemonCont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, game[0], pokemonCont);
			}
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (pokemonCont[0] == ENTRIES - 1) {
				pokemonCont[0] = 0;
				refresh(pokemonCont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, game[0], pokemonCont);	
			}
			else if (pokemonCont[0] < ENTRIES - 1) {
				pokemonCont[0]++;
				refresh(pokemonCont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, game[0], pokemonCont);
			}
		}
		
		if (hidKeysDown() & KEY_A) {
			switch (pokemonCont[0]) {
				case 0 : {
					if (game[0] < 3) game[0] += 1;
					else if (game[0] == 3) game[0] = 0;
					break;
				}
				case 1 : {
					if (pokemonCont[1] < BOXMAX - 1) pokemonCont[1] += 1;
					else if (pokemonCont[1] == BOXMAX - 1) pokemonCont[1] = 0;
					break;
				}
				case 2 : {
					if (pokemonCont[2] < 29) pokemonCont[2] += 1;
					else if (pokemonCont[2] == 29) pokemonCont[2] = 0;
					break;
				}
				case 3 : {
					if (pokemonCont[3] < 3) pokemonCont[3] += 1;
					else if (pokemonCont[3] == 3) pokemonCont[3] = 0;
					break;
				}
				case 5 : {
					if (pokemonCont[4] < NEVS - 1) pokemonCont[4] += 1;
					else if (pokemonCont[4] == NEVS - 1) pokemonCont[4] = 0;
					break;
				}
				case 7 : {
					if (pokemonCont[5] < 15) pokemonCont[5] += 1;
					else if (pokemonCont[5] == 15) pokemonCont[5] = 0;
					break;
				}
			}
			refreshPokemon(topScreen, game[0], pokemonCont);
		}
		if (hidKeysDown() & KEY_START && pokemonCont[0] != 0 && pokemonCont[0] != 1 && pokemonCont[0] != 2 && pokemonCont[0] != 5) {
			fsStart();
			FS_Archive saveArch;
			if(openSaveArch(&saveArch, ids[game[0]])) {
				u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
				
				//Open main
				Handle mainHandle;
				FSUSER_OpenFile(&mainHandle, saveArch, fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);
			
				//Get size 
				u64 mainSize;
				FSFILE_GetSize(mainHandle, &mainSize);
				
				switch(game[0]) {
					case 0 : {
						if (mainSize != 415232)
							return -13;
						break;
					}
					case 1 : {
						if (mainSize != 415232)
							return -13;
						break;
					}
					case 2 : {
						if (mainSize != 483328)
							return -13;
						break;
					}
					case 3 : {
						if (mainSize != 483328)
							return -13;
						break;
					}
				}
				
				//allocate mainbuf
				u8* mainbuf = malloc(mainSize);
				
				//Read main 
				FSFILE_Read(mainHandle, NULL, 0, mainbuf, mainSize);
				
				switch (pokemonCont[0]) {
					case 3 : {
						getPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						setFriendship(pkmn, friendship[pokemonCont[3]]);
						setPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						break;
					}
					case 4 : {
						getPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						for (int i = 0; i < 6; i++) 
							setEV(pkmn, evs[pokemonCont[4]][i], i);
						setPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						break;
					}
					case 6 : {
						getPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						for (int i = 0; i < 6; i++) 
							setIV(pkmn, 31, i);
						setPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						break;
					}
					case 7 : {
						getPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						setHPType(pkmn, pokemonCont[5]);
						setPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						break;
					}
					case 8 : {
						getPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						setShiny(pkmn);
						setPkmn(mainbuf, pokemonCont[1], pokemonCont[2], pkmn, game[0]);
						break;
					}
				}
				
				int rwCHK = rewriteCHK(mainbuf, game[0]);
				if (rwCHK != 0)
					return rwCHK;
				
				FSFILE_Write(mainHandle, NULL, 0, mainbuf, mainSize, FS_WRITE_FLUSH);
				FSFILE_Close(mainHandle);
				
				FSUSER_ControlArchive(saveArch, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
				FSUSER_CloseArchive(saveArch);
				
				free(mainbuf);
				free(pkmn);
				fsEnd();
				return 1;
			} else
				return -1;
		}
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	return 0;
}
