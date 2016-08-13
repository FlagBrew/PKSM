#include <3ds.h>
#include <string.h>
#include "pokemon.h"
#include "util.h"

#define PKMNLENGTH 232
#define PARTYPKMNLENGTH 260
#define BOXMAX 31

#define OFFSET 0x5400

#define ENTRIES 2

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
    
    if (game == 3 || game == 4) {
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
    memcpy((void*)pkmn, (const void*)(mainbuf + getPkmnAddress(boxnumber, indexnumber, game)), PKMNLENGTH);
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
        
    memcpy((void*)(mainbuf + getPkmnAddress(boxnumber, indexnumber, game)), (const void*)pkmn, length);
}

void refreshPokemon(PrintConsole topScreen, int game, int pokemonCont[]) {
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
}

void setFriendship(u8* pkmn, u32 value) {
	switch (value) {
		case 0 : {
			*(pkmn + 0xA2) = 0x00;
			break;
		}
		case 255 : {
			*(pkmn + 0xA2) = 0xFF;
			break;
		}
	}
	
}

int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int pokemonCont[]) {
	int boxnumber = 0;
	int indexnumber = 0;
	
	char *menuEntries[ENTRIES] = {"Game is:", "Set friendship to 255"};

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
			}
			refreshPokemon(topScreen, game[0], pokemonCont);
		}
		if (hidKeysDown() & KEY_START && pokemonCont[0] != 0) {
			
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
					case 1 : {
						getPkmn(mainbuf, boxnumber, indexnumber, pkmn, game[0]);
						setFriendship(pkmn, 255);
						setPkmn(mainbuf, boxnumber, indexnumber, pkmn, game[0]);
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
