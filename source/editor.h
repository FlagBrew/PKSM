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

#pragma once
#include "common.h"

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

#define ED_STANDARD 0
#define ED_MENU 1
#define ED_CLONE 2
#define ED_GENERATE 3
#define ED_OTA 4
#define ED_SEED 5

#define ED_BASE 0
#define ED_STATS 1
#define ED_MOVES 2
#define ED_ITEMS 3
#define ED_NATURES 4
#define ED_BALLS 5
#define ED_HIDDENPOWER 6
#define ED_FORMS 7
#define ED_HEX 8

#define CHERISH_BALL 16

struct {
	u8 pkmData[961][15];
} personal;

typedef struct FormData {
	int spriteNum;
	int stringNum;
	int min;
	int max;
	bool editable;
} FormData;

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
bool isNicknameF(u8* pkmn);
void rerollPID(u8* pkmn);
void encryptBattleSection(u8* mainbuf, u8* pkmn, int game, int currentEntry);
void fillBattleSection(u8* mainbuf, u8* pkmn, int game, int currentEntry);
bool isBattleBoxed(u8* mainbuf, int game, int box, int slot);
bool checkHTLegality(u8* mainbuf, u8* pkmn, int game);

u8 getOTGender(u8* pkmn);
u8 getHPType(u8* pkmn);
u8 getGender(u8* pkmn);
u8 getLanguage(u8* pkmn);
u16 getPokedexNumber(u8* pkmn);
u8 getNature(u8* pkmn);
u8 getEV(u8* pkmn, const int stat);
u8 getFriendship(u8* pkmn);
u8 getHTFriendship(u8* pkmn);
u8 getOTFriendship(u8* pkmn);
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
u16 getFormSpeciesNumber(u8* pkmn);
FormData *getLegalFormData(u16 species, int game);
u16 getStat(u8* pkmn, const int stat);
bool getPokerus(u8* pkmn);
u8 getAbility(u8* pkmn);
u8 getAbilityNum(u8* pkmn);
u32 *getOT(u8* pkmn, u32* dst);
u32 *getHTName(u8* pkmn, u32* dst);
u32 *getNickname(u8* pkmn, u32* dst);
u16 getEggMove(u8 *pkmn, const int nmove);
u8 getBall(u8* pkmn);
u16 getPSV(u8* pkmn);
u32 *getSaveOT(u8* mainbuf, int game, u32* dst);
u8 getSaveGender(u8* mainbuf, int game);
u16 getSaveTID(u8* mainbuf, int game);
u16 getSaveSID(u8* mainbuf, int game);
u16 getSaveTSV(u8* mainbuf, int game);
u32 getSaveSeed(u8* mainbuf, int game, int index);
u8 getSaveLanguage(u8* mainbuf, int game);

u32 getMoney(u8* mainbuf, int game);
u16 getBP(u8* mainbuf, int game);

void setGender(u8* pkmn, u8 val);
void setForm(u8* pkmn, u8 val);
void setHT(u8* pkmn, char* nick);
void setHTGender(u8* pkmn, const u8 gender);
void setNature(u8* pkmn, const u8 nature);
void setNicknameZ(u8* pkmn, char* nick, int dst);
void setShiny(u8* pkmn, const bool shiny);
void setWC(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void setWC4(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[], int GBO);
void setSaveLanguage(u8* mainbuf, int game, int i);
void setFriendship(u8* pkmn, const int value);
void setHTFriendship(u8* pkmn, const int value);
void setOTFriendship(u8* pkmn, const int value);
void setEV(u8* pkmn, u8 val, const int stat);
void setHPType(u8* pkmn, const int val);
void setIV(u8* pkmn, u8 val, const int stat);
void setPokerus(u8* pkmn);
void setLevel(u8* pkmn, int lv);
void setMove(u8* pkmn, const u16 move, const int nmove);
void setEggMove(u8* pkmn, const u16 move, const int nmove);
void setItemEditor(u8* pkmn, u16 item);
void setBall(u8* pkmn, u8 val);
void setSID(u8* pkmn, u16 sid);
void setTID(u8* pkmn, u16 tid);
void setOTGender(u8* pkmn, u8 val);

void setRibbons(u8* pkmn, int ribcat, int ribnumber, bool value);
bool getRibbons(u8* pkmn, int ribcat, int ribnumber);

void setHTi(u8* pkmn, int htnumber, bool value);
bool getHTi(u8* pkmn, int htnumber);
void setFlag(u8* pkmn, int flgaddr, int flgshift, bool value);

void parseSaveHexEditor(u8* mainbuf, int game, int byte);

void saveFileEditor(u8* mainbuf, int game, u64 size);
void pokemonEditor(u8* mainbuf, int game);
