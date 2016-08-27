/*
* This file is part of EventAssistant
* Copyright (C) 2016 Bernardo Giordano
*
* This software is provided 'as-is', 
* without any express or implied warranty. 
* In no event will the authors be held liable for any damages 
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* and to alter it and redistribute it freely, 
* subject to the following restrictions:
*
* 1) The origin of this software must not be misrepresented; 
* 2) You must not claim that you wrote the original software. 
*
* Altered source versions must be plainly marked as such, 
* and must not be misrepresented as being the original software.
*/

#include <3ds.h>
#include <stdio.h>

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

u32 seedStep(const u32 seed);
void shuffleArray(u8* pkmn, const u32 encryptionkey);
void decryptPkmn(u8* pkmn);
int getPkmnAddress(const int boxnumber, const int indexnumber, int game);
void calculatePKMNChecksum(u8* data);
void encryptPkmn(u8* pkmn);
void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game);
void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game);
u16 getPokedexNumber(u8* pkmn);
void setNickname(u8* pkmn, char* nick);
u8 getNature(u8* pkmn);
void setNature(u8* pkmn, const u8 nature);

void refreshPokemon(PrintConsole topScreen, u8* mainbuf, int pokemonCont[], int game);
u8 getFriendship(u8* pkmn);
void setFriendship(u8* pkmn, const int value);
void setEV(u8* pkmn, u8 val, const int stat);
u8 getEV(u8* pkmn, const int stat);
void setIV(u8* pkmn, u8 val, const int stat);
u8 getIV(u8* pkmn, const int stat);
void setHPType(u8* pkmn, const int val);
u16 getOTID(u8* pkmn);
u16 getSOTID(u8* pkmn);
u32 getPID(u8* pkmn);
bool isShiny(u8* pkmn);
void rerollPID(u8* pkmn);
void setShiny(u8* pkmn, const bool shiny);

int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game, int pokemonCont[]);