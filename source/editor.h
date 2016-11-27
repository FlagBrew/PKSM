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

#pragma once

#include <3ds.h>
#include <stdio.h>

#define PGTLENGTH 260
#define PGFLENGTH 204
#define WC6LENGTH 264
#define PKMNLENGTH 232
#define BOXMAX 31
#define MOVELENGTH 2
#define EGGMOVELENGTH 2
#define EVLENGTH 1
#define PIDLENGTH 4
#define IVLENGTH 4
#define OTIDLENGTH 2
#define SOTIDLENGTH 2
#define NICKNAMELENGTH 26
#define POKEDEXNUMBERLENGTH 2
#define ABILITYLENGTH 1
#define ABILITYNUMLENGTH 1
#define NATURELENGTH 1
#define GENDERLENGTH 1
#define ITEMLENGTH 2
#define EXPPOINTLENGTH 4

struct {
	u8 pkmData[961][56];
} personal;

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
bool isEgg(u8* pkmn);
void rerollPID(u8* pkmn);

u16 getPokedexNumber(u8* pkmn);
u8 getNature(u8* pkmn);
u8 getEV(u8* pkmn, const int stat);
u8 getFriendship(u8* pkmn);
u8 getIV(u8* pkmn, const int stat);
u16 getOTID(u8* pkmn);
u16 getSOTID(u8* pkmn);
u32 getPID(u8* pkmn);
u16 getTSV(u8* pkmn);
u16 getMove(u8* pkmn, int nmove);
u16 getItem(u8* pkmn);
u8 getHPType(u8* pkmn);
u8 getLevel(u8* pkmn);
bool isInfected (u8* pkmn);
u8 getForm(u8* pkmn);
u16 getStat(u8* pkmn, const int stat);
bool getPokerus(u8* pkmn);
u8 getAbility(u8* pkmn);
char *getOT(u8* pkmn, char* dst);
char *getNickname(u8* pkmn, char* dst);
u16 getEggMove(u8 *pkmn, const int nmove);

void setNature(u8* pkmn, const u8 nature);
void setNickname(u8* pkmn, char* nick);
void setShiny(u8* pkmn, const bool shiny);
void setWC(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void setWC4(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[], int GBO);
void setLanguage(u8* mainbuf, int game, int i);
void setMoney(u8* mainbuf, int game, u64 i);
void setItem(u8* mainbuf, int type, int game);
void setBP(u8* mainbuf, int i, int game);
void setBadges(u8* mainbuf, int game, int i);
void setTM(u8* mainbuf, int game);
void setFriendship(u8* pkmn, const int value);
void setEV(u8* pkmn, u8 val, const int stat);
void setHPType(u8* pkmn, const int val);
void setIV(u8* pkmn, u8 val, const int stat);
void setPokerus(u8* pkmn);
void setLevel(u8* pkmn, int lv);
void setMove(u8* pkmn, const u16 move, const int nmove);
void setEggMove(u8* pkmn, const u16 move, const int nmove);
void setItemEditor(u8* pkmn, u16 item);

void itemEditor(u8* pkmn, int game);
void movesEditor(u8* pkmn, int game);
void saveFileEditor(u8* mainbuf, int game);
void pokemonEditor(u8* mainbuf, int game);
