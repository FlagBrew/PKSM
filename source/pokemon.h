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

#include <3ds.h>
#include <stdio.h>

#define PGTLENGTH 260
#define PGFLENGTH 204
#define WC6LENGTH 264
#define PKMNLENGTH 232
#define BOXMAX 31
#define EVLENGTH 1
#define PIDLENGTH 4
#define IVLENGTH 4
#define OTIDLENGTH 2
#define SOTIDLENGTH 2
#define NICKNAMELENGTH 26
#define POKEDEXNUMBERLENGTH 2
#define NATURELENGTH 1

int getActiveGBO(u8* mainbuf, int game);
int getActiveSBO(u8* mainbuf, int game);
u32 CHKOffset(u32 i, int game);
u32 CHKLength(u32 i, int game);
u16 ccitt16(u8* data, u32 len);
u32 BWCHKOff(u32 i, int game);
u32 BWCHKMirr(u32 i, int game);
void rewriteCHK(u8 *mainbuf, int game);
void rewriteCHK4(u8 *mainbuf, int game, int GBO, int SBO);
u32 seedStep(const u32 seed);
u32 LCRNG(u32 seed);
void shuffleArray(u8* pkmn, const u32 encryptionkey);
void decryptPkmn(u8* pkmn);
int getPkmnAddress(const int boxnumber, const int indexnumber, int game);
void calculatePKMNChecksum(u8* data);
void encryptPkmn(u8* pkmn);
void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game);
void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game);
bool isShiny(u8* pkmn);
void rerollPID(u8* pkmn);
void findFreeLocationWC(u8 *mainbuf, int game, int nInjected[]);

u16 getPokedexNumber(u8* pkmn);
u8 getNature(u8* pkmn);
u8 getEV(u8* pkmn, const int stat);
u8 getFriendship(u8* pkmn);
u8 getIV(u8* pkmn, const int stat);
u16 getOTID(u8* pkmn);
u16 getSOTID(u8* pkmn);
u32 getPID(u8* pkmn);
u16 getTSV(u8* pkmn);

void setNature(u8* pkmn, const u8 nature);
void setNickname(u8* pkmn, char* nick);
void setShiny(u8* pkmn, const bool shiny);
void setWC(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void setWC4(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[], int GBO);
void setLanguage(u8* mainbuf, int i);
void setMoney(u8* mainbuf, u64 i);
void setItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[], int game);
void setBP(u8* mainbuf, int i, int game);
void setBadges(u8* mainbuf, int i);
void setTM(u8* mainbuf, int game);
void setFriendship(u8* pkmn, const int value);
void setEV(u8* pkmn, u8 val, const int stat);
void setHPType(u8* pkmn, const int val);
void setIV(u8* pkmn, u8 val, const int stat);
int setBoxBin(PrintConsole topScreen, u8* mainbuf, int game, int NBOXES, int N, char* urls[]);

void refreshPK(PrintConsole topScreen, u8* mainbuf, int pokemonCont[], int game);
void refreshItem(PrintConsole topScreen, int injectCont[], int nInjected[]);
int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8* mainbuf, int game, int nInjected[], int cont[]);
int PKEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game, int cont[]);