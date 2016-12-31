/* This file is part of PKSM

Copyright (C) 2016 Bernardo Giordano

>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#include <3ds.h>
#include <string.h>
#include "graphic.h"
#include "editor.h"
#include "util.h"
#include "save.h"

/* ************************ local variables ************************ */

const int SMWC7FLAGPOS = 0x65C00;
const int SMWC7POS = 0x65C00 + 0x100;
const int ORASWC6FLAGPOS = 0x1CC00;
const int ORASWC6POS = 0x1CD00;
const int XYWC6FLAGPOS = 0x1BC00;
const int XYWC6POS = 0x1BD00;
const int DPPGTFLAGPOS = 0xA6D0;
const int DPPGTPOS = 0xA7FC;
const int PTPGTFLAGPOS = 0xB4C0;
const int PTPGTPOS = 0xB5C0;
const int HGSSPGTFLAGPOS = 0x9D3C;
const int HGSSPGTPOS = 0x9E3C;
const int EONFLAGPOS = 0x319B8;
const int LANGUAGEPOS = 0x1402D;
const int MONEYPOS = 0x4208;
const int BADGEPOS = 0x420C;
const int TMSTARTPOS = 0xBC0;
const int OFFSET = 0x5400;
const int MOVEPOS = 0x5A;
const int EGGMOVEPOS = 0x6A;
const int EVPOS = 0x1E;
const int PIDPOS = 0x18;
const int IVPOS = 0x74;
const int OTIDPOS = 0x0C;
const int SOTIDPOS = 0x0E;
const int OTNAMELENGTH = 0x17;
const int OTNAMEPOS = 0xB0;
const int NICKNAMEPOS = 0x40;
const int POKEDEXNUMBERPOS = 0x08;
const int NATUREPOS = 0x1C;
const int ITEMPOS = 0x0A;
const int EXPPOINTPOS = 0x10;
const int ABILITYPOS = 0x14;
const int ABILITYNUMPOS = 0x15;
const int GENDERPOS = 0x1D;
const int POKERUS = 0x11;
const int PGFSTARTPOS = 0x1C800;
const int BWSEEDPOS = 0x1D290;

u32 expTable[100][6] = {
  {0, 0, 0, 0, 0, 0},
  {8, 15, 4, 9, 6, 10},
  {27, 52, 13, 57, 21, 33},
  {64, 122, 32, 96, 51, 80},
  {125, 237, 65, 135, 100, 156},
  {216, 406, 112, 179, 172, 270},
  {343, 637, 178, 236, 274, 428},
  {512, 942, 276, 314, 409, 640},
  {729, 1326, 393, 419, 583, 911},
  {1000, 1800, 540, 560, 800, 1250},
  {1331, 2369, 745, 742, 1064, 1663},
  {1728, 3041, 967, 973, 1382, 2160},
  {2197, 3822, 1230, 1261, 1757, 2746},
  {2744, 4719, 1591, 1612, 2195, 3430},
  {3375, 5737, 1957, 2035, 2700, 4218},
  {4096, 6881, 2457, 2535, 3276, 5120},
  {4913, 8155, 3046, 3120, 3930, 6141},
  {5832, 9564, 3732, 3798, 4665, 7290},
  {6859, 11111, 4526, 4575, 5487, 8573},
  {8000, 12800, 5440, 5460, 6400, 10000},
  {9261, 14632, 6482, 6458, 7408, 11576},
  {10648, 16610, 7666, 7577, 8518, 13310},
  {12167, 18737, 9003, 8825, 9733, 15208},
  {13824, 21012, 10506, 10208, 11059, 17280},
  {15625, 23437, 12187, 11735, 12500, 19531},
  {17576, 26012, 14060, 13411, 14060, 21970},
  {19683, 28737, 16140, 15244, 15746, 24603},
  {21952, 31610, 18439, 17242, 17561, 27440},
  {24389, 34632, 20974, 19411, 19511, 30486},
  {27000, 37800, 23760, 21760, 21600, 33750},
  {29791, 41111, 26811, 24294, 23832, 37238},
  {32768, 44564, 30146, 27021, 26214, 40960},
  {35937, 48155, 33780, 29949, 28749, 44921},
  {39304, 51881, 37731, 33084, 31443, 49130},
  {42875, 55737, 42017, 36435, 34300, 53593},
  {46656, 59719, 46656, 40007, 37324, 58320},
  {50653, 63822, 50653, 43808, 40522, 63316},
  {54872, 68041, 55969, 47846, 43897, 68590},
  {59319, 72369, 60505, 52127, 47455, 74148},
  {64000, 76800, 66560, 56660, 51200, 80000},
  {68921, 81326, 71677, 61450, 55136, 86151},
  {74088, 85942, 78533, 66505, 59270, 92610},
  {79507, 90637, 84277, 71833, 63605, 99383},
  {85184, 95406, 91998, 77440, 68147, 106480},
  {91125, 100237, 98415, 83335, 72900, 113906},
  {97336, 105122, 107069, 89523, 77868, 121670},
  {103823, 110052, 114205, 96012, 83058, 129778},
  {110592, 115015, 123863, 102810, 88473, 138240},
  {117649, 120001, 131766, 109923, 94119, 147061},
  {125000, 125000, 142500, 117360, 100000, 156250},
  {132651, 131324, 151222, 125126, 106120, 165813},
  {140608, 137795, 163105, 133229, 112486, 175760},
  {148877, 144410, 172697, 141677, 119101, 186096},
  {157464, 151165, 185807, 150476, 125971, 196830},
  {166375, 158056, 196322, 159635, 133100, 207968},
  {175616, 165079, 210739, 169159, 140492, 219520},
  {185193, 172229, 222231, 179056, 148154, 231491},
  {195112, 179503, 238036, 189334, 156089, 243890},
  {205379, 186894, 250562, 199999, 164303, 256723},
  {216000, 194400, 267840, 211060, 172800, 270000},
  {226981, 202013, 281456, 222522, 181584, 283726},
  {238328, 209728, 300293, 234393, 190662, 297910},
  {250047, 217540, 315059, 246681, 200037, 312558},
  {262144, 225443, 335544, 259392, 209715, 327680},
  {274625, 233431, 351520, 272535, 219700, 343281},
  {287496, 241496, 373744, 286115, 229996, 359370},
  {300763, 249633, 390991, 300140, 240610, 375953},
  {314432, 257834, 415050, 314618, 251545, 393040},
  {328509, 267406, 433631, 329555, 262807, 410636},
  {343000, 276458, 459620, 344960, 274400, 428750},
  {357911, 286328, 479600, 360838, 286328, 447388},
  {373248, 296358, 507617, 377197, 298598, 466560},
  {389017, 305767, 529063, 394045, 311213, 486271},
  {405224, 316074, 559209, 411388, 324179, 506530},
  {421875, 326531, 582187, 429235, 337500, 527343},
  {438976, 336255, 614566, 447591, 351180, 548720},
  {456533, 346965, 639146, 466464, 365226, 570666},
  {474552, 357812, 673863, 485862, 379641, 593190},
  {493039, 367807, 700115, 505791, 394431, 616298},
  {512000, 378880, 737280, 526260, 409600, 640000},
  {531441, 390077, 765275, 547274, 425152, 664301},
  {551368, 400293, 804997, 568841, 441094, 689210},
  {571787, 411686, 834809, 590969, 457429, 714733},
  {592704, 423190, 877201, 613664, 474163, 740880},
  {614125, 433572, 908905, 636935, 491300, 767656},
  {636056, 445239, 954084, 660787, 508844, 795070},
  {658503, 457001, 987754, 685228, 526802, 823128},
  {681472, 467489, 1035837, 710266, 545177, 851840},
  {704969, 479378, 1071552, 735907, 563975, 881211},
  {729000, 491346, 1122660, 762160, 583200, 911250},
  {753571, 501878, 1160499, 789030, 602856, 941963},
  {778688, 513934, 1214753, 816525, 622950, 973360},
  {804357, 526049, 1254796, 844653, 643485, 1005446},
  {830584, 536557, 1312322, 873420, 664467, 1038230},
  {857375, 548720, 1354652, 902835, 685900, 1071718},
  {884736, 560922, 1415577, 932903, 707788, 1105920},
  {912673, 571333, 1460276, 963632, 730138, 1140841},
  {941192, 583539, 1524731, 995030, 752953, 1176490},
  {970299, 591882, 1571884, 1027103, 776239, 1212873},
  {1000000, 600000, 1640000, 1059860, 800000, 1250000}
};

/* ************************ utilities ************************ */

u32 seedStep(const u32 seed) { return (seed * 0x41C64E6D + 0x00006073) & 0xFFFFFFFF; }
u32 LCRNG(u32 seed) { return seed * 0x41C64E6D + 0x00006073; }

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
    if (game == GAME_X || game == GAME_Y) {
		if (boxnumber < 33)
			boxpos = 0x22600;
		else
			boxpos = 0x14200;
	} else if (game == GAME_OR || game == GAME_AS) {
		if (boxnumber < 33)
			boxpos = 0x33000;
		else
			boxpos = 0x14200;
	} else if (game == GAME_SUN || game == GAME_MOON) {
		if (boxnumber < 33) 
			boxpos = 0x04E00;
		else 
			boxpos = 0x01400;
	}

	if (boxnumber < 33)
		return boxpos + (PKMNLENGTH * 30 * boxnumber) + (indexnumber * PKMNLENGTH);

	return boxpos + indexnumber * 260;
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

void encryptBattleSection(u8* mainbuf, u8* pkmn, int game, int currentEntry) {
	int boxpos = 0;
    if (game == GAME_X || game == GAME_Y)
		boxpos = 0x14200;
	else if (game == GAME_OR || game == GAME_AS)
		boxpos = 0x14200;
	else if (game == GAME_SUN || game == GAME_MOON) 
		boxpos = 0x01400;
	
    const int ENCRYPTIONKEYPOS = 0x0;
    const int ENCRYPTIONKEYLENGHT = 4;
    
    u32 encryptionkey;
    memcpy(&encryptionkey, &pkmn[ENCRYPTIONKEYPOS], ENCRYPTIONKEYLENGHT);
    u32 seed = encryptionkey;
    
    u16 temp;
    for (int i = 232; i < 260; i += 2) {
        memcpy(&temp, &mainbuf[boxpos + 260 * currentEntry + i], 2);
        temp ^= (seedStep(seed) >> 16);
        seed = seedStep(seed);
        memcpy(&mainbuf[boxpos + 260 * currentEntry + i], &temp, 2);
    }
}

void fillBattleSection(u8* mainbuf, u8* pkmn, int game, int currentEntry) {
	int boxpos = 0;
    if (game == GAME_X || game == GAME_Y)
		boxpos = 0x14200;
	else if (game == GAME_OR || game == GAME_AS)
		boxpos = 0x14200;
	else if (game == GAME_SUN || game == GAME_MOON) 
		boxpos = 0x01400;
		
    u8 battle = 0;
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xE8], &battle, 1);
    
    u8 level = getLevel(pkmn);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xEC], &level, 1);
    
    u16 currenthp = getStat(pkmn, 0);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xF0], &currenthp, 2);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xF2], &currenthp, 2);
    
    u16 atk = getStat(pkmn, 1);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xF4], &atk, 2);
    
    u16 def = getStat(pkmn, 2);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xF6], &def, 2);
    
    u16 spe = getStat(pkmn, 3);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xF8], &spe, 2);
    
    u16 satk = getStat(pkmn, 4);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xFA], &satk, 2);
    
    u16 sdef = getStat(pkmn, 5);
    memcpy(&mainbuf[boxpos + 260 * currentEntry + 0xFC], &sdef, 2);
}

void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    memcpy(pkmn, &mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], PKMNLENGTH);
    decryptPkmn(pkmn);
}

void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    calculatePKMNChecksum(pkmn);
	if (boxnumber == 33) {
		fillBattleSection(mainbuf, pkmn, game, indexnumber);
		encryptBattleSection(mainbuf, pkmn, game, indexnumber);
	}
    encryptPkmn(pkmn);

    memcpy(&mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], pkmn, PKMNLENGTH);
}

bool isShiny(u8* pkmn) {
    u16 trainersv = (getOTID(pkmn) ^ getSOTID(pkmn)) >> 4;
    u16 pkmnv = ((getPID(pkmn) >> 16) ^ (getPID(pkmn) & 0xFFFF)) >> 4;

    if (trainersv == pkmnv) return true;
    else return false;
}

void rerollPID(u8* pkmn) {
    srand(getPID(pkmn));
    u32 pidbuffer = rand();

    memcpy(&pkmn[PIDPOS], &pidbuffer, PIDLENGTH);
}

bool isEgg(u8* pkmn) {
    u32 eggbuffer;
    memcpy(&eggbuffer, &pkmn[IVPOS], IVLENGTH);
    eggbuffer = eggbuffer >> 30;
    eggbuffer = eggbuffer & 0x1;
    if (eggbuffer == 1) return true;
    else return false;
}

/* ************************ get ************************ */

u8 getGender(u8* pkmn) { return ((*(u8*)(pkmn + 0x1D)) >> 1) & 0x3; }
void setGender(u8* pkmn, u8 val) { pkmn[0x1D] = (u8)((pkmn[0x1D] & ~0x06) | (val << 1)); }
u8 getLanguage(u8* pkmn) { return *(u8*)(pkmn + 0xE3); }
u8 getAbility(u8* pkmn) { return *(u8*)(pkmn + 0x14); }
u8 getForm(u8* pkmn) { return ((*(u8*)(pkmn + 0x1D)) >> 3); }
u16 getItem(u8* pkmn) { return *(u16*)(pkmn + 0x0A); }
void setBall(u8* pkmn, u8 val) { pkmn[0xDC] = val; }
u8 getHPType(u8* pkmn) { return 15 * ((getIV(pkmn, 0)& 1) + 2 * (getIV(pkmn, 1) & 1) + 4 * (getIV(pkmn, 2) & 1) + 8 * (getIV(pkmn, 3) & 1) + 16 * (getIV(pkmn, 4) & 1) + 32 * (getIV(pkmn, 5) & 1)) / 63; }

u16 getEggMove(u8 *pkmn, const int nmove) { 
    u16 eggmovebuffer[4];
    memcpy(&eggmovebuffer, &pkmn[EGGMOVEPOS], EGGMOVELENGTH*4);
    return eggmovebuffer[nmove];
}

u16 getStat(u8* pkmn, const int stat) {
	u8 vett[] = {0x0, 0x1, 0x2, 0x4, 0x5, 0x3};
    u16 tempspecies = getPokedexNumber(pkmn);
    if (getForm(pkmn))
		memcpy(&tempspecies, &personal.pkmData[getPokedexNumber(pkmn)][0x1C], 2);

	u16 final;
    u8 mult = 10;
    u8 basestat = personal.pkmData[tempspecies][vett[stat]];

    if (stat == 0) final = 10 + ((2 * basestat) + getIV(pkmn, stat) + getEV(pkmn, stat) / 4 + 100) * getLevel(pkmn) / 100;
    else           final = 5 + (2 * basestat + getIV(pkmn, stat) + getEV(pkmn, stat) / 4) * getLevel(pkmn) / 100;

    if (getNature(pkmn) / 5 + 1 == stat) mult++;
    if (getNature(pkmn) % 5 + 1 == stat) mult--;

    final = final * mult / 10;
    return final;
}

bool isInfected (u8* pkmn) {
	u8 pkrs;
	memcpy(&pkrs, &pkmn[0x2B], 1);
	pkrs = pkrs >> 4;
	return pkrs > 0;
}

char *getOT(u8* pkmn, char* dst) {
	u16 src[OTNAMELENGTH];
	memcpy(src, &pkmn[OTNAMEPOS], OTNAMELENGTH);
	
	int cnt = 0;
	while (src[cnt] && cnt < 24) {
		dst[cnt] = src[cnt];
		cnt += 1;
	}
	dst[cnt] = 0;
	return dst;
}

char *getNickname(u8* pkmn, char* dst) {
	u16 src[NICKNAMELENGTH];
	memcpy(src, &pkmn[NICKNAMEPOS], NICKNAMELENGTH);
	
	int cnt = 0;
	while (src[cnt] && cnt < NICKNAMELENGTH) {
		dst[cnt] = src[cnt];
		cnt += 1;
	}
	dst[cnt] = 0;
	return dst;
}

u8 getLevel(u8* pkmn) {
	u32 exp;
	u8 xpType = personal.pkmData[getPokedexNumber(pkmn)][0x15];
	u8 iterLevel = 1;
	memcpy(&exp, &pkmn[EXPPOINTPOS], EXPPOINTLENGTH);

	while (iterLevel < 100 && exp >= expTable[iterLevel][xpType]) iterLevel++;
	return iterLevel;
}

u16 getMove(u8* pkmn, int nmove) {
    u16 movebuffer[4];
    memcpy(&movebuffer, &pkmn[MOVEPOS], MOVELENGTH*4);
    return movebuffer[nmove];
}

u16 getOTID(u8* pkmn) {
    u16 otidbuffer;
    memcpy(&otidbuffer, &pkmn[OTIDPOS], OTIDLENGTH);
    return otidbuffer;
}

u16 getSOTID(u8* pkmn) {
    u16 sotidbuffer;
    memcpy(&sotidbuffer, &pkmn[SOTIDPOS], SOTIDLENGTH);
    return sotidbuffer;
}

u32 getPID(u8* pkmn) {
    u32 pidbuffer;
    memcpy(&pidbuffer, &pkmn[PIDPOS], PIDLENGTH);
    return pidbuffer;
}

u16 getTSV(u8* pkmn) {
	u16 TID = getOTID(pkmn);
	u16 SID = getSOTID(pkmn);
	return (TID ^ SID) >> 4;
}

u16 getPokedexNumber(u8* pkmn) {
    u16 pokedexnumber;
    memcpy(&pokedexnumber, &pkmn[POKEDEXNUMBERPOS], POKEDEXNUMBERLENGTH);
    return pokedexnumber;
}

u8 getFriendship(u8* pkmn) {
    u8 friendship;
    memcpy(&friendship, &pkmn[0xA2], 1);
    return friendship;
}

u8 getNature(u8* pkmn) {
    u8 nature;
    memcpy(&nature, &pkmn[NATUREPOS], NATURELENGTH);
    return nature;
}

u8 getEV(u8* pkmn, const int stat) {
    u8 evbuffer[6];
    memcpy(evbuffer, &pkmn[EVPOS], EVLENGTH * 6);
    return evbuffer[stat];
}

u8 getIV(u8* pkmn, const int stat) {
    u32 buffer;
    u8 toreturn;

    memcpy(&buffer, &pkmn[IVPOS], IVLENGTH);
    buffer = buffer >> 5 * stat;
    buffer = buffer & 0x1F;
    memcpy(&toreturn, &buffer, 1);
    return toreturn;
}

bool getPokerus(u8* pkmn) {
	u8 pkrs;
	memcpy(&pkrs, &pkmn[0x2B], 1);
	return pkrs;
}

u32 getMoney(u8* mainbuf, int game) {
	u32 money;
	memcpy(&money, &mainbuf[(game < 4) ? 0x4208 : 0x4004], sizeof(u32));
	return money;
}

u16 getBP(u8* mainbuf, int game) {
	u16 bp;
	memcpy(&bp, &mainbuf[(game == GAME_X || game == GAME_Y) ? 0x423C : (game == GAME_OR || game == GAME_AS) ? 0x4230 : 0x411D], sizeof(u16));
	return bp;
}

u8 getBall(u8* pkmn) {
    u8 ballbuffer;
    memcpy(&ballbuffer, &pkmn[0xDC], 1);
    
    return ballbuffer;
}

/* ************************ set ************************ */

void setItemEditor(u8* pkmn, u16 item) {
    memcpy(&pkmn[ITEMPOS], &item, ITEMLENGTH);
}

void setAbility(u8* pkmn, const u8 ability) {
    u16 tempspecies = getPokedexNumber(pkmn);
    if (getForm(pkmn))
		memcpy(&tempspecies, &personal.pkmData[getPokedexNumber(pkmn)][0x1C], 2);
	
	u8 abilitynum = 0;
	if (ability == 0)      abilitynum = 1;
	else if (ability == 1) abilitynum = 2;
	else                   abilitynum = 4;
	
	memcpy(&pkmn[ABILITYNUMPOS], &abilitynum, ABILITYNUMLENGTH);
	memcpy(&pkmn[ABILITYPOS], &personal.pkmData[tempspecies][0x18 + ability], ABILITYLENGTH);
}

void setMove(u8* pkmn, const u16 move, const int nmove) {
    memcpy(&pkmn[MOVEPOS + (MOVELENGTH * nmove)], &move, MOVELENGTH);
}

void setEggMove(u8* pkmn, const u16 move, const int nmove) {
    memcpy(&pkmn[EGGMOVEPOS + (EGGMOVELENGTH * nmove)], &move, EGGMOVELENGTH);
}

void setNickname(u8* pkmn, char* nick) {
    u8 toinsert[NICKNAMELENGTH];
	memset(toinsert, 0, NICKNAMELENGTH);

    for (u16 i = 0, nicklen = strlen(nick); i < nicklen; i++)
        toinsert[i * 2] = *(nick + i);

    memcpy(&pkmn[NICKNAMEPOS], toinsert, NICKNAMELENGTH);
}

void setOT(u8* pkmn, char* nick) {
    u8 toinsert[NICKNAMELENGTH];
	memset(toinsert, 0, NICKNAMELENGTH);

    for (u16 i = 0, nicklen = strlen(nick); i < nicklen; i++)
        toinsert[i * 2] = *(nick + i);

    memcpy(&pkmn[OTNAMEPOS], toinsert, NICKNAMELENGTH);
}

void setNature(u8* pkmn, const u8 nature) {
    memcpy(&pkmn[NATUREPOS], &nature, NATURELENGTH);
}

void setFriendship(u8* pkmn, const int val) {
	memcpy(&pkmn[0xA2], &val, 1);
	memcpy(&pkmn[0xCA], &val, 1);
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

void setHPType(u8* pkmn, const int val) {
    u8 ivstat[6];
    for (int i = 0; i < 6; i++)
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

    for (int i = 0; i < 6; i++)
         ivstat[i] = (ivstat[i] & 0x1E) + hpivs[val][i];

    for (int i = 0; i < 6; i++)
        setIV(pkmn, ivstat[i], i);
}

void setShiny(u8* pkmn, const bool shiny) {
	while(isShiny(pkmn) != shiny)
		rerollPID(pkmn);
}

void setLevel(u8* pkmn, int lv) {
    u32 towrite = expTable[lv - 1][personal.pkmData[getPokedexNumber(pkmn)][0x15]];
    memcpy(&pkmn[EXPPOINTPOS], &towrite, EXPPOINTLENGTH);
}

void setWC(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[]) {
	if (game == GAME_X || game == GAME_Y) {
		*(mainbuf + XYWC6FLAGPOS + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + XYWC6POS + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);
	}

	if (game == GAME_OR || game == GAME_AS) {
		*(mainbuf + ORASWC6FLAGPOS + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + ORASWC6POS + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);

		if (i == 2048) {
			*(mainbuf + EONFLAGPOS)     = 0xC2;
			*(mainbuf + EONFLAGPOS + 1) = 0x73;
			*(mainbuf + EONFLAGPOS + 2) = 0x5D;
			*(mainbuf + EONFLAGPOS + 3) = 0x22;
		}
	}

	if (game == GAME_SUN || game == GAME_MOON) {
		*(mainbuf + SMWC7FLAGPOS + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + SMWC7POS + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);
	}

	if (game == GAME_B1 || game == GAME_W1 || game == GAME_B2 || game == GAME_W2) {
		u32 seed;
		memcpy(&seed, &mainbuf[BWSEEDPOS], sizeof(u32));

		//decrypt
		u16 temp;
		for (int i = 0; i < 0xA90; i += 2) {
			memcpy(&temp, &mainbuf[PGFSTARTPOS + i], 2);
			temp ^= (LCRNG(seed) >> 16);
			seed = LCRNG(seed);
			memcpy(&mainbuf[PGFSTARTPOS + i], &temp, 2);
		}

		*(mainbuf + PGFSTARTPOS + i / 8) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + 0x1C900 + nInjected[0] * PGFLENGTH), (const void*)wcbuf, PGFLENGTH);

		//encrypt
		memcpy(&seed, &mainbuf[BWSEEDPOS], sizeof(u32));
		for (int i = 0; i < 0xA90; i += 2) {
			memcpy(&temp, &mainbuf[PGFSTARTPOS + i], 2);
			temp ^= (LCRNG(seed) >> 16);
			seed = LCRNG(seed);
			memcpy(&mainbuf[PGFSTARTPOS + i], &temp, 2);
		}
	}

	nInjected[0] += 1;
	if (game < 4) {
		if (nInjected[0] >= 24)
			nInjected[0] = 0;
	} else if (nInjected[0] >= 48)
		nInjected[0] = 0;
}

void setWC4(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[], int GBO) {
	if (game == GAME_HG || game == GAME_SS) {
		*(mainbuf + HGSSPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + HGSSPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}
	else if (game == GAME_PLATINUM) {
		*(mainbuf + PTPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + PTPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}
	else if (game == GAME_DIAMOND || game == GAME_PEARL) {
		*(mainbuf + DPPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + DPPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}

	nInjected[0] += 1;
	if (nInjected[0] >= 8)
		nInjected[0] = 0;
}

void setLanguage(u8* mainbuf, int game, int i) {
	u8 langValues[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x09, 0x0A};
	memcpy(&mainbuf[(game < 4) ? 0x1402D : 0x1235], &langValues[i], sizeof(u8));
}

void setMoney(u8* mainbuf, int game, u32 money) {
	memcpy(&mainbuf[(game < 4) ? 0x4208 : 0x4004], &money, sizeof(u32));
}

void setItem(u8* mainbuf, int type, int game) {
	if (game == GAME_OR || game == GAME_AS) {
		char* paths[] = {"romfs:/misc/oras/base.bin", "romfs:/misc/oras/heals.bin", "romfs:/misc/oras/berries.bin"};
		u32 offset[] = { 0x400, 0xD72, 0xE70 };
		injectFromFile(mainbuf, paths[type], offset[type]);
	} else if (game == GAME_X || game == GAME_Y) {
		char* paths[] = {"romfs:/misc/xy/base.bin", "romfs:/misc/xy/heals.bin", "romfs:/misc/xy/berries.bin"};
		u32 offset[] = { 0x400, 0xD6A, 0xE68 };
		injectFromFile(mainbuf, paths[type], offset[type]);
	} else if (game == GAME_SUN || game == GAME_MOON) {

	}
}

void setBP(u8* mainbuf, int i, int game) {
	const u32 offset[] = {0x423C, 0x423D, 0x4230, 0x4231, 0x411C, 0x411D};
	int type = 0;

	if (game == GAME_OR || game == GAME_AS)
		type = 2;
	else if (game == GAME_SUN || game == GAME_MOON)
		type = 4;

	switch (i) {
		case 0 : {
			*(mainbuf + offset[type])     = 0x00;
			*(mainbuf + offset[type + 1]) = 0x00;
			break;
		}
		case 9999 : {
			*(mainbuf + offset[type])     = 0x0F;
			*(mainbuf + offset[type + 1]) = 0x27;
			break;
		}
	}
}

void setPokerus(u8* pkmn) {
	*(pkmn + 0x2B) = POKERUS;
}

void setTM(u8* mainbuf, int game) {
	if (game == GAME_OR || game == GAME_AS)
		injectFromFile(mainbuf, "romfs:/misc/oras/tm.bin", 0xBC0);
	else if (game == GAME_X || game == GAME_Y)
		injectFromFile(mainbuf, "romfs:/misc/xy/tm.bin", 0xBC0);
	else if (game == GAME_SUN || game == GAME_MOON)
		injectFromFile(mainbuf, "romfs:/misc/sm/tm.bin", 0x0);
}

void saveFileEditor(u8* mainbuf, int game) {
	int currentEntry = 0;
	int langCont = 0;

	while(aptMainLoop()) {
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)
			break;

		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) 
				currentEntry--;
			else if (currentEntry == 0) 
				currentEntry = 9;
		}

		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) 
				currentEntry++;
			else if (currentEntry == 9) 
				currentEntry = 0;
		}

		if (hidKeysDown() & KEY_DLEFT) {
			if (currentEntry >= 5) 
				currentEntry -= 5;
		}

		if (hidKeysDown() & KEY_DRIGHT) {
			if (currentEntry <= 4) 
				currentEntry += 5;
		}

		if (hidKeysDown() & KEY_A) {
			switch (currentEntry) {
				case 0 : {
					int langMax = (game < 4) ? 6 : 8;
					if (langCont < langMax) langCont++;
					else if (langCont == langMax) langCont = 0;
					break;
				}
			}
		}

		if (hidKeysDown() & KEY_START) {
			switch (currentEntry) {
				case 0 : {
					setLanguage(mainbuf, game, langCont);
					infoDisp("Language set successfully!");
					break;
				}
				case 1 : {
					if (game < 4) {
						setItem(mainbuf, 1, game);
						infoDisp("Heals injected successfully!");
					} else
						infoDisp("Not currently available for SM");
					break;
				}
				case 2 : {
					if (game < 4) {
						setItem(mainbuf, 0, game);
						infoDisp("Items injected successfully!");
					} else
						infoDisp("Not currently available for SM");
					break;
				}
				case 3 : {
					if (game < 4) {
						setItem(mainbuf, 2, game);
						infoDisp("Berries set successfully!");
					} else
						infoDisp("Not currently available for SM");
					break;
				}
				case 4 : {
					if (game < 4) {
						setTM(mainbuf, game);
						infoDisp("TMs set successfully!");
					} else
						infoDisp("Not currently available for SM");
					break;
				}
				case 5 : {
					int start = 0;
					int wcmax = (game < 4) ? 24 : 48;
					if (game == GAME_X || game == GAME_Y)
						start = XYWC6FLAGPOS;
					else if (game == GAME_OR || game == GAME_AS)
						start = ORASWC6FLAGPOS;
					else if (game == GAME_SUN || game == GAME_MOON)
						start = SMWC7FLAGPOS;

					for (int i = 0; i < (0x100 + wcmax * WC6LENGTH); i++)
						*(mainbuf + start + i) = 0x00;
					infoDisp("Mistery Gift box cleaned!");
					break;
				}
			}
		}

		printEditor(mainbuf, game, currentEntry, langCont);
	}
}

void pokemonEditor(u8* mainbuf, int game) {
	int lookup[] = {0, 1, 2, 4, 5, 3};
	
	bool isTeam = false;
	bool speedy = false;
	int box = 0;
	int currentEntry = 0;
	int menuEntry = 0;
	int boxmax = (game < 4) ? 30 : 31;
	int ability = 2;
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));

	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);

		if (hidKeysDown() & KEY_B) 
			break;

		if (hidKeysDown() & KEY_R) {
			if (box < boxmax) 
				box++;
			else if (box == boxmax) 
				box = 0;
		}

		if (hidKeysDown() & KEY_L) {
			if (box > 0) 
				box--;
			else if (box == 0) 
				box = boxmax;
		}

 		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 210 && touch.px < 320 && touch.py > 0 && touch.py < 210) {
				currentEntry = 0;
				isTeam = true;
			}
			if (touch.px > 0 && touch.px < 210 && touch.py > 0 && touch.py < 210) {
				currentEntry = 0;
				isTeam = false;
			}
			
			if (touch.px > 7 && touch.px < 23 && touch.py > 17 && touch.py < 37) {
				if (box > 0) 
					box--;
				else if (box == 0) 
					box = boxmax;
			}
			if (touch.px > 185 && touch.px < 201 && touch.py > 17 && touch.py < 37) {
				if (box < boxmax) 
					box++;
				else if (box == boxmax) 
					box = 0;
			}
			if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
				break;
		} 

		if (hidKeysDown() & KEY_DRIGHT && !isTeam) {
			if (currentEntry < 29) 
				currentEntry++;
			else if (currentEntry == 29) 
				currentEntry = 0;
		}

		if (hidKeysDown() & KEY_DLEFT && !isTeam) {
			if (currentEntry > 0) 
				currentEntry--;
			else if (currentEntry == 0) 
				currentEntry = 29;
		}

		if (hidKeysDown() & KEY_DUP) {
			if (!isTeam) {
				if (currentEntry >= 6) 
					currentEntry -= 6;
			} else if (currentEntry > 0)
				currentEntry--;
		}

		if (hidKeysDown() & KEY_DDOWN) {
			if (!isTeam) {
				if (currentEntry <= 23) 
					currentEntry += 6;
			} else if (currentEntry < 5)
				currentEntry++;
		}
		
 		if (hidKeysHeld() & KEY_TOUCH) {
			if (!isTeam) {
				if (touch.px > 4 && touch.px < 38 && touch.py > 45 && touch.py < 75) currentEntry = 0;
				if (touch.px > 38 && touch.px < 72 && touch.py > 45 && touch.py < 75) currentEntry = 1;
				if (touch.px > 72 && touch.px < 106 && touch.py > 45 && touch.py < 75) currentEntry = 2;
				if (touch.px > 106 && touch.px < 140 && touch.py > 45 && touch.py < 75) currentEntry = 3;
				if (touch.px > 140 && touch.px < 174 && touch.py > 45 && touch.py < 75) currentEntry = 4;
				if (touch.px > 174 && touch.px < 208 && touch.py > 45 && touch.py < 75) currentEntry = 5;
				if (touch.px > 4 && touch.px < 38 && touch.py > 75 && touch.py < 105) currentEntry = 6;
				if (touch.px > 38 && touch.px < 72 && touch.py > 75 && touch.py < 105) currentEntry = 7;
				if (touch.px > 72 && touch.px < 106 && touch.py > 75 && touch.py < 105) currentEntry = 8;
				if (touch.px > 106 && touch.px < 140 && touch.py > 75 && touch.py < 105) currentEntry = 9;
				if (touch.px > 140 && touch.px < 174 && touch.py > 75 && touch.py < 105) currentEntry = 10;
				if (touch.px > 174 && touch.px < 208 && touch.py > 75 && touch.py < 105) currentEntry = 11;
				if (touch.px > 4 && touch.px < 38 && touch.py > 105 && touch.py < 135) currentEntry = 12;
				if (touch.px > 38 && touch.px < 72 && touch.py > 105 && touch.py < 135) currentEntry = 13;
				if (touch.px > 72 && touch.px < 106 && touch.py > 105 && touch.py < 135) currentEntry = 14;
				if (touch.px > 106 && touch.px < 140 && touch.py > 105 && touch.py < 135) currentEntry = 15;
				if (touch.px > 140 && touch.px < 174 && touch.py > 105 && touch.py < 135) currentEntry = 16;
				if (touch.px > 174 && touch.px < 208 && touch.py > 105 && touch.py < 135) currentEntry = 17;
				if (touch.px > 4 && touch.px < 38 && touch.py > 135 && touch.py < 165) currentEntry = 18;
				if (touch.px > 38 && touch.px < 72 && touch.py > 135 && touch.py < 165) currentEntry = 19;
				if (touch.px > 72 && touch.px < 106 && touch.py > 135 && touch.py < 165) currentEntry = 20;
				if (touch.px > 106 && touch.px < 140 && touch.py > 135 && touch.py < 165) currentEntry = 21;
				if (touch.px > 140 && touch.px < 174 && touch.py > 135 && touch.py < 165) currentEntry = 22;
				if (touch.px > 174 && touch.px < 208 && touch.py > 135 && touch.py < 165) currentEntry = 23;
				if (touch.px > 4 && touch.px < 38 && touch.py > 165 && touch.py < 195) currentEntry = 24;
				if (touch.px > 38 && touch.px < 72 && touch.py > 165 && touch.py < 195) currentEntry = 25;
				if (touch.px > 72 && touch.px < 106 && touch.py > 165 && touch.py < 195) currentEntry = 26;
				if (touch.px > 106 && touch.px < 140 && touch.py > 165 && touch.py < 195) currentEntry = 27;
				if (touch.px > 140 && touch.px < 174 && touch.py > 165 && touch.py < 195) currentEntry = 28;
				if (touch.px > 174 && touch.px < 208 && touch.py > 165 && touch.py < 195) currentEntry = 29;
			} else {
				if (touch.px > 214 && touch.px < 265 && touch.py > 40 && touch.py < 85) currentEntry = 0;
				if (touch.px > 266 && touch.px < 317 && touch.py > 60 && touch.py < 105) currentEntry = 1;
				if (touch.px > 214 && touch.px < 265 && touch.py > 85 && touch.py < 130) currentEntry = 2;
				if (touch.px > 266 && touch.px < 317 && touch.py > 105 && touch.py < 150) currentEntry = 3;
				if (touch.px > 214 && touch.px < 265 && touch.py > 130 && touch.py < 175) currentEntry = 4;
				if (touch.px > 266 && touch.px < 317 && touch.py > 150 && touch.py < 195) currentEntry = 5;				
			}
		}
		
		if (hidKeysDown() & KEY_A) {
			getPkmn(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
			bool operationDone = false;

			while (aptMainLoop() && getPokedexNumber(pkmn) && !operationDone) {
				hidScanInput();
				hidTouchRead(&touch);
				
				printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_MENU, speedy, 0);
				
				if (hidKeysDown() & KEY_B)
					break;
				
				if (hidKeysDown() & KEY_DUP) {
					if (menuEntry > 0)
						menuEntry--;
				}

				if (hidKeysDown() & KEY_DDOWN) {
					if (menuEntry < 4) 
						menuEntry++;
				}
				
				if (hidKeysHeld() & KEY_TOUCH) {
					if (touch.px > 208 && touch.px < 317) {
						if (touch.py > 42 && touch.py < 69) menuEntry = 0;
						if (touch.py > 69 && touch.py < 96) menuEntry = 1;
						if (touch.py > 96 && touch.py < 123) menuEntry = 2;
						if (touch.py > 123 && touch.py < 150) menuEntry = 3;
						if (touch.py > 150 && touch.py < 177) menuEntry = 4;
					}
				}
				
				if (hidKeysDown() & KEY_A) {
					switch(menuEntry) {
						case 0 : {
							while(aptMainLoop() && !operationDone) {
								hidScanInput();
								touchPosition touch;
								hidTouchRead(&touch);

								if (hidKeysDown() & KEY_B) {
									operationDone = true;
									break;
								}

								if (hidKeysDown() & KEY_L)
									speedy = false;

								if (hidKeysDown() & KEY_R)
									speedy = true;
								
								if (hidKeysDown() & KEY_TOUCH) {
									if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
										break;
			
									if (touch.px > 206 && touch.px < 317 && touch.py > 116 && touch.py < 143) {
										while(aptMainLoop()) {
											hidScanInput();
											touchPosition touch;
											hidTouchRead(&touch);

											if (hidKeysDown() & KEY_B)
												break;

											if (hidKeysDown() & KEY_L)
												speedy = false;

											if (hidKeysDown() & KEY_R)
												speedy = true;
											
											if (hidKeysDown() & KEY_TOUCH) {
												if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
													break;
												if (touch.px > 291 && touch.px < 316 && touch.py > 175 && touch.py < 187) {
													int hpEntry = 0;
													while(aptMainLoop()) {
														hidScanInput();
														
														if (hidKeysDown() & KEY_B)
															break;

														if (hidKeysDown() & KEY_DRIGHT)
															if (hpEntry < 15)
																hpEntry++;
														
														if (hidKeysDown() & KEY_DLEFT)
															if (hpEntry > 0) 
																hpEntry--;
														
														if (hidKeysDown() & KEY_DUP)
															if (hpEntry >= 4) 
																hpEntry -= 4;
														
														if (hidKeysDown() & KEY_DDOWN)
															if (hpEntry <= 11)
																hpEntry += 4;
															
														if (hidKeysDown() & KEY_A)
															setHPType(pkmn, (u8)hpEntry);
														
														printPKEditor(pkmn, game, speedy, hpEntry, 0, 0, ED_HIDDENPOWER);
													}
												}												
											}
											
											if (hidKeysDown() & KEY_TOUCH && !(speedy)) {
												for (int i = 0; i < 6; i++) {
													if (touch.px > 96 && touch.px < 109 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														if (getIV(pkmn, lookup[i]) > 0)
															setIV(pkmn, getIV(pkmn, lookup[i]) - 1, lookup[i]);
													if (touch.px > 139 && touch.px < 152 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														if (getIV(pkmn, lookup[i]) < 31)
															setIV(pkmn, getIV(pkmn, lookup[i]) + 1, lookup[i]);
													if (touch.px > 177 && touch.px < 190 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														if (getEV(pkmn, lookup[i]) > 0)
															setEV(pkmn, getEV(pkmn, lookup[i]) - 1, lookup[i]);
													if (touch.px > 218 && touch.px < 231 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20) {
														if (getEV(pkmn, lookup[i]) < 252) {
															int tot = 0;
															for (int i = 0; i < 6; i++)
																tot += getEV(pkmn, i);
															if (tot < 508)
																setEV(pkmn, getEV(pkmn, lookup[i]) + 1, lookup[i]);
														}
													}
												}
											}
											
											else if (hidKeysHeld() & KEY_TOUCH && speedy) {
												for (int i = 0; i < 6; i++) {
													if (touch.px > 96 && touch.px < 109 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														if (getIV(pkmn, lookup[i]) > 0)
															setIV(pkmn, getIV(pkmn, lookup[i]) - 1, lookup[i]);
													if (touch.px > 139 && touch.px < 152 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														if (getIV(pkmn, lookup[i]) < 31)
															setIV(pkmn, getIV(pkmn, lookup[i]) + 1, lookup[i]);
													if (touch.px > 177 && touch.px < 190 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														if (getEV(pkmn, lookup[i]) > 0)
															setEV(pkmn, getEV(pkmn, lookup[i]) - 1, lookup[i]);
													if (touch.px > 218 && touch.px < 231 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20) {
														if (getEV(pkmn, lookup[i]) < 252) {
															int tot = 0;
															for (int i = 0; i < 6; i++)
																tot += getEV(pkmn, i);
															if (tot < 508)
																setEV(pkmn, getEV(pkmn, lookup[i]) + 1, lookup[i]);
														}
													}
												}
											}
											
											printPKEditor(pkmn, game, speedy, 0, 0, 0, ED_STATS);
										}
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 51 && touch.py < 63) {
										int natureEntry = 0;
										while(aptMainLoop()) {
											hidScanInput();
											
											if (hidKeysDown() & KEY_B)
												break;

											if (hidKeysDown() & KEY_DRIGHT)
												if (natureEntry < 24) 
													natureEntry++;
											
											if (hidKeysDown() & KEY_DLEFT)
												if (natureEntry > 0) 
													natureEntry--;
											
											if (hidKeysDown() & KEY_DUP)
												if (natureEntry >= 5) 
													natureEntry -= 5;
											
											if (hidKeysDown() & KEY_DDOWN)
												if (natureEntry <= 19) 
													natureEntry += 5;
												
											if (hidKeysDown() & KEY_A)
												setNature(pkmn, natureEntry);
											
											printPKEditor(pkmn, game, speedy, natureEntry, 0, 0, ED_NATURES);
										}
									}
									
									if (touch.px > 0 && touch.px < 26 && touch.py > 0 && touch.py < 20) {
										int ballEntry = 0;
										while(aptMainLoop()) {
											hidScanInput();
											
											if (hidKeysDown() & KEY_B)
												break;

											if (hidKeysDown() & KEY_DRIGHT)
												if (ballEntry < 25) 
													ballEntry++;
											
											if (hidKeysDown() & KEY_DLEFT)
												if (ballEntry > 0) 
													ballEntry--;
											
											if (hidKeysDown() & KEY_DUP)
												if (ballEntry >= 6) 
													ballEntry -= 6;
											
											if (hidKeysDown() & KEY_DDOWN)
												if (ballEntry <= 19)
													ballEntry += 6;
												
											if (hidKeysDown() & KEY_A)
												setBall(pkmn, (u8)ballEntry + 1);
											
											printPKEditor(pkmn, game, speedy, ballEntry, 0, 0, ED_BALLS);
										}
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 71 && touch.py < 83) {
										if (ability < 2) 
											ability++;
										else if (ability == 2) 
											ability = 0;
										setAbility(pkmn, ability);
									}
									
									if (touch.px > 156 && touch.px < 174 && touch.py > 0 && touch.py < 20) {
										if (getGender(pkmn) != 2) 
											setGender(pkmn, ((getGender(pkmn) == 0) ? 1 : 0));
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 111 && touch.py < 123) {
										if (isShiny(pkmn))
											setShiny(pkmn, false);
										else 
											setShiny(pkmn, true);
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 131 && touch.py < 143) {
										if (!(getPokerus(pkmn)))
											setPokerus(pkmn);
										else 
											*(pkmn + 0x2B) = 0x00;
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 171 && touch.py < 183) {
										static SwkbdState swkbd;
										static char buf[60];

										SwkbdButton button = SWKBD_BUTTON_NONE;
										swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, (NICKNAMELENGTH / 2) - 1);
										swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancel", false);
										swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Set", true);
										swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_DIGITS | SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 2);
										swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
										swkbdSetHintText(&swkbd, "Enter your nickname");
										button = swkbdInputText(&swkbd, buf, sizeof(buf));

										static char nick[NICKNAMELENGTH];
										for (int i = 0; i < NICKNAMELENGTH; i++)
											nick[i] = 0;
										memcpy(nick, buf, NICKNAMELENGTH);
										nick[NICKNAMELENGTH - 1] = '\0';

										if (button != SWKBD_BUTTON_NONE)
											setNickname(pkmn, nick);
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 151 && touch.py < 163) {
										static SwkbdState swkbd;
										static char buf[60];

										SwkbdButton button = SWKBD_BUTTON_NONE;
										swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, (NICKNAMELENGTH / 2) - 1);
										swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancel", false);
										swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Set", true);
										swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_DIGITS | SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 2);
										swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
										swkbdSetHintText(&swkbd, "Enter your OT name");
										button = swkbdInputText(&swkbd, buf, sizeof(buf));

										static char nick[NICKNAMELENGTH];
										for (int i = 0; i < NICKNAMELENGTH; i++)
											nick[i] = 0;
										memcpy(nick, buf, NICKNAMELENGTH);
										nick[NICKNAMELENGTH - 1] = '\0';

										if (button != SWKBD_BUTTON_NONE)
											setOT(pkmn, nick);
									}
									
									if (touch.px > 206 && touch.px < 315 && touch.py > 170 && touch.py < 197) {
										setPkmn(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
										operationDone = true;
										break;
									}
									
									if (touch.px > 206 && touch.px < 317 && touch.py > 143 && touch.py < 170) {
										int movesSorted[] = {0, 71, 709, 51, 151, 628, 629, 491, 512, 367, 332, 177, 495, 97, 314, 403, 624, 625, 502, 133, 677, 246, 453, 392, 401, 292, 312, 597, 274, 372, 310, 454, 213, 396, 62, 694, 475, 419, 608, 661, 140, 112, 226, 690, 251, 562, 187, 516, 117, 20, 44, 654, 655, 307, 299, 59, 335, 644, 645, 551, 34, 550, 125, 198, 155, 586, 340, 413, 622, 623, 280, 362, 693, 145, 61, 450, 405, 339, 523, 418, 331, 682, 347, 293, 445, 658, 606, 268, 451, 204, 448, 498, 509, 128, 691, 499, 370, 489, 4, 590, 109, 93, 132, 632, 633, 160, 176, 383, 687, 638, 639, 322, 538, 178, 68, 343, 152, 578, 238, 440, 242, 306, 462, 174, 15, 399, 464, 663, 605, 455, 111, 432, 194, 197, 652, 653, 591, 91, 50, 574, 435, 291, 146, 353, 458, 24, 3, 104, 38, 434, 620, 225, 337, 349, 692, 406, 82, 407, 525, 409, 577, 138, 65, 529, 530, 223, 414, 89, 497, 598, 121, 604, 582, 486, 527, 373, 52, 227, 283, 203, 412, 494, 284, 153, 326, 702, 245, 263, 587, 584, 252, 313, 206, 297, 364, 185, 565, 552, 515, 126, 424, 680, 519, 7, 83, 660, 90, 175, 481, 488, 172, 53, 394, 148, 430, 260, 705, 374, 666, 579, 19, 560, 411, 116, 264, 266, 395, 193, 571, 492, 553, 573, 338, 524, 218, 31, 210, 154, 559, 558, 248, 380, 544, 674, 703, 601, 202, 416, 646, 647, 549, 137, 447, 520, 320, 580, 356, 45, 74, 288, 470, 385, 698, 12, 441, 16, 360, 258, 359, 603, 106, 114, 543, 457, 29, 215, 377, 456, 505, 361, 531, 391, 535, 257, 484, 270, 506, 237, 667, 136, 610, 607, 468, 30, 32, 532, 336, 542, 308, 56, 642, 643, 63, 158, 304, 621, 593, 95, 301, 58, 554, 423, 665, 8, 420, 556, 333, 196, 286, 510, 517, 640, 641, 611, 275, 689, 569, 334, 442, 231, 449, 26, 2, 134, 588, 282, 616, 673, 387, 436, 348, 437, 536, 670, 141, 73, 43, 122, 617, 113, 710, 199, 142, 67, 490, 381, 461, 679, 295, 183, 277, 478, 345, 463, 443, 393, 602, 222, 696, 561, 382, 212, 96, 72, 25, 5, 224, 262, 368, 232, 319, 309, 118, 208, 102, 170, 107, 357, 243, 119, 429, 54, 296, 581, 585, 714, 236, 234, 426, 341, 300, 189, 330, 718, 595, 417, 363, 267, 717, 302, 636, 637, 539, 101, 400, 171, 568, 609, 613, 697, 190, 316, 466, 618, 200, 315, 220, 570, 575, 6, 371, 64, 195, 572, 80, 566, 42, 589, 583, 365, 305, 139, 398, 77, 40, 342, 676, 1, 600, 181, 408, 471, 384, 379, 681, 438, 612, 619, 217, 711, 182, 60, 244, 94, 706, 678, 354, 427, 375, 473, 540, 149, 701, 386, 685, 228, 511, 98, 501, 483, 99, 476, 240, 229, 75, 534, 13, 105, 278, 115, 513, 287, 547, 156, 514, 216, 686, 279, 179, 46, 459, 350, 431, 397, 157, 249, 88, 317, 439, 272, 27, 205, 355, 563, 496, 221, 533, 219, 28, 328, 201, 634, 635, 503, 184, 10, 103, 545, 290, 548, 402, 465, 69, 120, 247, 708, 421, 467, 325, 425, 159, 648, 649, 329, 504, 704, 508, 351, 659, 324, 318, 493, 47, 695, 166, 285, 130, 143, 507, 327, 303, 21, 163, 79, 214, 124, 188, 482, 479, 684, 265, 123, 108, 555, 289, 173, 487, 135, 76, 669, 49, 699, 460, 209, 664, 712, 683, 169, 131, 191, 596, 662, 255, 180, 150, 147, 671, 446, 592, 537, 211, 564, 254, 700, 23, 707, 444, 500, 480, 70, 668, 81, 165, 522, 78, 66, 164, 650, 651, 389, 241, 713, 162, 276, 48, 626, 627, 57, 207, 256, 186, 230, 129, 415, 14, 485, 235, 33, 294, 541, 39, 366, 36, 269, 715, 546, 630, 631, 298, 477, 100, 168, 614, 615, 37, 675, 87, 422, 9, 84, 86, 85, 321, 576, 259, 92, 390, 672, 144, 161, 271, 433, 567, 167, 688, 376, 41, 656, 657, 239, 369, 253, 557, 410, 599, 474, 11, 22, 233, 521, 344, 358, 55, 518, 352, 594, 346, 323, 127, 311, 250, 18, 469, 528, 261, 17, 273, 110, 472, 452, 526, 388, 35, 378, 404, 281, 192, 428, 716, 719};
										int moveEntry = 0;
										int entryBottom = 0;
										int page = 0, maxpages = 18;
										
										while (aptMainLoop()) {
											hidScanInput();
											touchPosition touch;
											hidTouchRead(&touch);
											
											if (hidKeysDown() & KEY_B) break;
											
											if (hidKeysDown() & KEY_L) {
												if (page > 0) page--;
												else if (page == 0) page = maxpages - 1;
											}
											
											if (hidKeysDown() & KEY_R) {
												if (page < maxpages - 1) page++;
												else if (page == maxpages - 1) page = 0;
											}
											
											if (hidKeysDown() & KEY_DUP) {
												if (moveEntry > 0) moveEntry--;
												else if (moveEntry == 0) moveEntry = 39;
											}
											
											if (hidKeysDown() & KEY_DDOWN) {
												if (moveEntry < 39) moveEntry++;
												else if (moveEntry == 39) moveEntry = 0;
											}
											
											if (hidKeysDown() & KEY_DLEFT) {
												if (moveEntry <= 19)	{
													page--;
													if (page < 0) 
														page = maxpages - 1;
												}
												else if (moveEntry >= 20) moveEntry -= 20;
											}
											
											if (hidKeysDown() & KEY_DRIGHT) {
												if (moveEntry <= 19) moveEntry += 20;
												else if (moveEntry >= 20) {
													page++;
													if (page > maxpages - 1)
														page = 0;
												}
											}
											
											if (hidKeysHeld() & KEY_TOUCH) {
												if (touch.px > 0 && touch.px < 198 && touch.py > 25 && touch.py < 45)   entryBottom = 0;
												if (touch.px > 0 && touch.px < 198 && touch.py > 45 && touch.py < 65)   entryBottom = 1;
												if (touch.px > 0 && touch.px < 198 && touch.py > 65 && touch.py < 85)   entryBottom = 2;
												if (touch.px > 0 && touch.px < 198 && touch.py > 85 && touch.py < 105)  entryBottom = 3;
												if (touch.px > 0 && touch.px < 198 && touch.py > 129 && touch.py < 149) entryBottom = 4;
												if (touch.px > 0 && touch.px < 198 && touch.py > 149 && touch.py < 169) entryBottom = 5;
												if (touch.px > 0 && touch.px < 198 && touch.py > 169 && touch.py < 189) entryBottom = 6;
												if (touch.px > 0 && touch.px < 198 && touch.py > 189 && touch.py < 209) entryBottom = 7;
												
												if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
													break;
											}
											
											if (hidKeysDown() & KEY_A) {
												if (entryBottom < 4)
													setMove(pkmn, movesSorted[moveEntry + page * 40], entryBottom);
												else
													setEggMove(pkmn, movesSorted[moveEntry + page * 40], entryBottom - 4);
											}
											
											printPKEditor(pkmn, game, speedy, moveEntry, page, entryBottom, ED_MOVES);
										}										
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 90 && touch.py < 102) {
										int itemsSorted[] = {0, 645, 611, 674, 677, 545, 719, 135, 846, 703, 672, 667, 162, 541, 679, 803, 831, 755, 655, 658, 223, 18, 205, 468, 742, 104, 153, 640, 757, 21, 455, 199, 580, 668, 476, 723, 851, 770, 183, 43, 470, 852, 87, 581, 89, 296, 450, 713, 544, 491, 241, 68, 240, 281, 661, 664, 486, 472, 65, 535, 261, 73, 165, 654, 795, 145, 148, 144, 146, 147, 213, 558, 909, 787, 818, 118, 19, 49, 767, 48, 475, 591, 546, 249, 660, 678, 195, 149, 16, 150, 200, 119, 220, 287, 297, 189, 100, 224, 474, 569, 192, 444, 198, 635, 583, 698, 739, 740, 457, 175, 460, 461, 462, 572, 210, 96, 285, 562, 919, 617, 791, 822, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 109, 798, 826, 227, 226, 280, 721, 735, 738, 764, 56, 592, 615, 699, 7, 628, 629, 102, 116, 471, 311, 250, 561, 918, 235, 579, 790, 821, 312, 576, 636, 637, 324, 182, 13, 108, 305, 805, 833, 547, 322, 550, 915, 881, 779, 810, 700, 40, 34, 35, 208, 536, 860, 775, 726, 78, 38, 229, 538, 216, 268, 428, 793, 824, 715, 920, 435, 492, 138, 844, 553, 904, 782, 813, 159, 548, 912, 82, 777, 808, 842, 303, 273, 298, 539, 64, 556, 905, 785, 816, 230, 275, 841, 30, 497, 27, 316, 23, 440, 756, 202, 683, 657, 502, 656, 568, 560, 910, 789, 820, 763, 728, 796, 446, 98, 466, 623, 624, 625, 551, 914, 780, 811, 884, 3, 488, 263, 75, 137, 173, 286, 112, 555, 907, 784, 815, 95, 634, 55, 676, 197, 238, 14, 36, 565, 93, 284, 495, 101, 680, 420, 421, 422, 423, 424, 425, 737, 641, 714, 172, 94, 702, 666, 45, 25, 163, 552, 20, 917, 849, 302, 781, 812, 282, 799, 827, 141, 308, 697, 47, 278, 313, 612, 613, 211, 532, 710, 433, 675, 196, 190, 687, 170, 773, 730, 731, 732, 733, 221, 279, 206, 684, 685, 42, 255, 85, 234, 32, 705, 154, 725, 493, 574, 201, 270, 236, 269, 616, 142, 533, 712, 429, 768, 479, 496, 673, 319, 231, 256, 157, 648, 708, 453, 494, 136, 11, 718, 481, 215, 458, 741, 323, 242, 161, 176, 706, 682, 688, 802, 830, 1, 681, 41, 39, 24, 77, 29, 301, 627, 665, 747, 750, 744, 716, 766, 746, 717, 745, 696, 748, 749, 454, 219, 758, 233, 257, 729, 751, 771, 772, 774, 277, 806, 834, 662, 663, 209, 307, 239, 883, 33, 498, 858, 81, 266, 566, 484, 243, 166, 8, 6, 246, 178, 564, 776, 807, 92, 452, 184, 314, 111, 103, 439, 54, 445, 155, 631, 110, 743, 437, 180, 22, 459, 500, 480, 575, 185, 193, 88, 582, 151, 630, 156, 204, 501, 762, 794, 825, 835, 836, 168, 489, 855, 262, 671, 644, 633, 573, 449, 432, 245, 554, 906, 783, 814, 4, 63, 651, 431, 577, 724, 169, 17, 293, 292, 290, 289, 271, 291, 695, 294, 53, 51, 12, 571, 800, 828, 537, 765, 642, 843, 578, 880, 321, 46, 557, 916, 882, 786, 817, 320, 856, 171, 15, 217, 274, 177, 504, 483, 106, 50, 152, 326, 327, 164, 325, 485, 542, 441, 67, 853, 534, 478, 260, 72, 588, 584, 590, 586, 585, 589, 587, 9, 79, 143, 614, 567, 638, 37, 28, 652, 850, 187, 543, 559, 315, 908, 788, 819, 540, 643, 99, 318, 686, 212, 139, 430, 456, 736, 754, 647, 44, 5, 650, 711, 203, 769, 727, 753, 670, 232, 254, 436, 434, 467, 464, 709, 244, 759, 295, 253, 632, 107, 70, 71, 117, 191, 251, 222, 482, 158, 105, 306, 760, 473, 228, 283, 804, 832, 649, 31, 237, 722, 218, 225, 845, 247, 179, 299, 310, 499, 448, 689, 477, 97, 91, 90, 207, 563, 911, 792, 823, 761, 259, 288, 309, 463, 734, 704, 451, 857, 80, 26, 76, 447, 653, 752, 134, 570, 174, 194, 801, 829, 879, 140, 258, 83, 503, 10, 86, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 694, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 618, 619, 620, 690, 691, 692, 693, 701, 442, 272, 304, 707, 248, 669, 2, 469, 252, 659, 465, 443, 186, 720, 549, 913, 84, 778, 809, 181, 317, 639, 167, 646, 490, 69, 214, 265, 160, 267, 438, 60, 598, 604, 610, 57, 597, 603, 609, 58, 596, 602, 608, 61, 594, 600, 606, 62, 595, 601, 607, 59, 593, 599, 605, 621, 626, 188, 487, 66, 854, 264, 74, 797, 300, 52, 276, 847, 113, 114, 115, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 426, 427, 622, 837, 838, 839, 840, 848, 859, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 903};	
										int itemEntry = 0;
										int page = 0, maxpages = 23;
										
										while (aptMainLoop()) {
											hidScanInput();
											touchPosition touch;
											hidTouchRead(&touch);
											
											if (hidKeysDown() & KEY_TOUCH && touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
												break;
											
											if (hidKeysDown() & KEY_B) break;
											
											if (hidKeysDown() & KEY_L) {
												if (page > 0) page--;
												else if (page == 0) page = maxpages - 1;
											}
											
											if (hidKeysDown() & KEY_R) {
												if (page < maxpages - 1) page++;
												else if (page == maxpages - 1) page = 0;
											}
											
											if (hidKeysDown() & KEY_DUP) {
												if (itemEntry > 0) itemEntry--;
												else if (itemEntry == 0) itemEntry = 39;
											}
											
											if (hidKeysDown() & KEY_DDOWN) {
												if (itemEntry < 39) itemEntry++;
												else if (itemEntry == 39) itemEntry = 0;
											}
											
											if (hidKeysDown() & KEY_DLEFT) {
												if (itemEntry <= 19)	{
													page--;
													if (page < 0) 
														page = maxpages - 1;
												}
												else if (itemEntry >= 20) itemEntry -= 20;
											}
											
											if (hidKeysDown() & KEY_DRIGHT) {
												if (itemEntry <= 19) itemEntry += 20;
												else if (itemEntry >= 20) {
													page++;
													if (page > maxpages - 1)
														page = 0;
												}
											}
											
											if (hidKeysDown() & KEY_A)
												setItemEditor(pkmn, itemsSorted[itemEntry + page * 40]);
											
											printPKEditor(pkmn, game, speedy, itemEntry, page, 0, ED_ITEMS);
										}
									}
								}
								
								if ((hidKeysDown() & KEY_TOUCH) && !(speedy)) {
									if (touch.px > 180 && touch.px < 193 && touch.py > 29 && touch.py < 42) {
										if (getLevel(pkmn) < 100)
											setLevel(pkmn, getLevel(pkmn) + 1);
										else if (getLevel(pkmn) == 100)
											setLevel(pkmn, 1);
									}
									
									if (touch.px > 137 && touch.px < 150 && touch.py > 29 && touch.py < 42) {
										if (getLevel(pkmn) > 1)
											setLevel(pkmn, getLevel(pkmn) - 1);
										else if (getLevel(pkmn) == 1)
											setLevel(pkmn, 100);
									}
									
									if (touch.px > 137 && touch.px < 150 && touch.py > 189 && touch.py < 202) {
										if (getFriendship(pkmn) > 0)
											setFriendship(pkmn, getFriendship(pkmn) - 1);
									}

									if (touch.px > 180&& touch.px < 193 && touch.py > 189 && touch.py < 202) {
										if (getFriendship(pkmn) < 255)
											setFriendship(pkmn, getFriendship(pkmn) + 1);
									}
								}

								if ((hidKeysHeld() & KEY_TOUCH) && speedy) {
									if (touch.px > 180 && touch.px < 193 && touch.py > 29 && touch.py < 42) {
										if (getLevel(pkmn) < 100)
											setLevel(pkmn, getLevel(pkmn) + 1);
										else if (getLevel(pkmn) == 100)
											setLevel(pkmn, 1);
									}
									
									if (touch.px > 137 && touch.px < 150 && touch.py > 29 && touch.py < 42) {
										if (getLevel(pkmn) > 1)
											setLevel(pkmn, getLevel(pkmn) - 1);
										else if (getLevel(pkmn) == 1)
											setLevel(pkmn, 100);
									}
									
									if (touch.px > 137 && touch.px < 150 && touch.py > 189 && touch.py < 202) {
										if (getFriendship(pkmn) > 0)
											setFriendship(pkmn, getFriendship(pkmn) - 1);
									}

									if (touch.px > 180&& touch.px < 193 && touch.py > 189 && touch.py < 202) {
										if (getFriendship(pkmn) < 255)
											setFriendship(pkmn, getFriendship(pkmn) + 1);
									}
								}								
								
								printPKEditor(pkmn, game, speedy, 0, 0, 0, ED_BASE);
							}
							break;
						}
						case 1 : {
							isTeam = false;
							currentEntry = 0;
							while(aptMainLoop() && !operationDone) {
								hidScanInput();
								touchPosition touch;
								hidTouchRead(&touch);

								if (hidKeysDown() & KEY_B) 
									break;

								if (hidKeysDown() & KEY_R) {
									if (box < boxmax) 
										box++;
									else if (box == boxmax) 
										box = 0;
								}

								if (hidKeysDown() & KEY_L) {
									if (box > 0) 
										box--;
									else if (box == 0) 
										box = boxmax;
								}

								if (hidKeysDown() & KEY_TOUCH) {
									if (touch.px > 210 && touch.px < 320 && touch.py > 0 && touch.py < 210) {
										currentEntry = 0;
										isTeam = true;
									}
									if (touch.px > 0 && touch.px < 210 && touch.py > 0 && touch.py < 210) {
										currentEntry = 0;
										isTeam = false;
									}
									
									if (touch.px > 7 && touch.px < 23 && touch.py > 17 && touch.py < 37) {
										if (box > 0) 
											box--;
										else if (box == 0) 
											box = boxmax;
									}
									if (touch.px > 185 && touch.px < 201 && touch.py > 17 && touch.py < 37) {
										if (box < boxmax) 
											box++;
										else if (box == boxmax) 
											box = 0;
									}
									if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
										break;
								} 

								if (hidKeysDown() & KEY_DRIGHT && !isTeam) {
									if (currentEntry < 29) 
										currentEntry++;
									else if (currentEntry == 29) 
										currentEntry = 0;
								}

								if (hidKeysDown() & KEY_DLEFT && !isTeam) {
									if (currentEntry > 0) 
										currentEntry--;
									else if (currentEntry == 0) 
										currentEntry = 29;
								}

								if (hidKeysDown() & KEY_DUP) {
									if (!isTeam) {
										if (currentEntry >= 6) 
											currentEntry -= 6;
									} else if (currentEntry > 0)
										currentEntry--;
								}

								if (hidKeysDown() & KEY_DDOWN) {
									if (!isTeam) {
										if (currentEntry <= 23) 
											currentEntry += 6;
									} else if (currentEntry < 5)
										currentEntry++;
								}
			
								if (hidKeysHeld() & KEY_TOUCH) {
									if (!isTeam) {
										if (touch.px > 4 && touch.px < 38 && touch.py > 45 && touch.py < 75) currentEntry = 0;
										if (touch.px > 38 && touch.px < 72 && touch.py > 45 && touch.py < 75) currentEntry = 1;
										if (touch.px > 72 && touch.px < 106 && touch.py > 45 && touch.py < 75) currentEntry = 2;
										if (touch.px > 106 && touch.px < 140 && touch.py > 45 && touch.py < 75) currentEntry = 3;
										if (touch.px > 140 && touch.px < 174 && touch.py > 45 && touch.py < 75) currentEntry = 4;
										if (touch.px > 174 && touch.px < 208 && touch.py > 45 && touch.py < 75) currentEntry = 5;
										if (touch.px > 4 && touch.px < 38 && touch.py > 75 && touch.py < 105) currentEntry = 6;
										if (touch.px > 38 && touch.px < 72 && touch.py > 75 && touch.py < 105) currentEntry = 7;
										if (touch.px > 72 && touch.px < 106 && touch.py > 75 && touch.py < 105) currentEntry = 8;
										if (touch.px > 106 && touch.px < 140 && touch.py > 75 && touch.py < 105) currentEntry = 9;
										if (touch.px > 140 && touch.px < 174 && touch.py > 75 && touch.py < 105) currentEntry = 10;
										if (touch.px > 174 && touch.px < 208 && touch.py > 75 && touch.py < 105) currentEntry = 11;
										if (touch.px > 4 && touch.px < 38 && touch.py > 105 && touch.py < 135) currentEntry = 12;
										if (touch.px > 38 && touch.px < 72 && touch.py > 105 && touch.py < 135) currentEntry = 13;
										if (touch.px > 72 && touch.px < 106 && touch.py > 105 && touch.py < 135) currentEntry = 14;
										if (touch.px > 106 && touch.px < 140 && touch.py > 105 && touch.py < 135) currentEntry = 15;
										if (touch.px > 140 && touch.px < 174 && touch.py > 105 && touch.py < 135) currentEntry = 16;
										if (touch.px > 174 && touch.px < 208 && touch.py > 105 && touch.py < 135) currentEntry = 17;
										if (touch.px > 4 && touch.px < 38 && touch.py > 135 && touch.py < 165) currentEntry = 18;
										if (touch.px > 38 && touch.px < 72 && touch.py > 135 && touch.py < 165) currentEntry = 19;
										if (touch.px > 72 && touch.px < 106 && touch.py > 135 && touch.py < 165) currentEntry = 20;
										if (touch.px > 106 && touch.px < 140 && touch.py > 135 && touch.py < 165) currentEntry = 21;
										if (touch.px > 140 && touch.px < 174 && touch.py > 135 && touch.py < 165) currentEntry = 22;
										if (touch.px > 174 && touch.px < 208 && touch.py > 135 && touch.py < 165) currentEntry = 23;
										if (touch.px > 4 && touch.px < 38 && touch.py > 165 && touch.py < 195) currentEntry = 24;
										if (touch.px > 38 && touch.px < 72 && touch.py > 165 && touch.py < 195) currentEntry = 25;
										if (touch.px > 72 && touch.px < 106 && touch.py > 165 && touch.py < 195) currentEntry = 26;
										if (touch.px > 106 && touch.px < 140 && touch.py > 165 && touch.py < 195) currentEntry = 27;
										if (touch.px > 140 && touch.px < 174 && touch.py > 165 && touch.py < 195) currentEntry = 28;
										if (touch.px > 174 && touch.px < 208 && touch.py > 165 && touch.py < 195) currentEntry = 29;
									} else {
										if (touch.px > 214 && touch.px < 265 && touch.py > 40 && touch.py < 85) currentEntry = 0;
										if (touch.px > 266 && touch.px < 317 && touch.py > 60 && touch.py < 105) currentEntry = 1;
										if (touch.px > 214 && touch.px < 265 && touch.py > 85 && touch.py < 130) currentEntry = 2;
										if (touch.px > 266 && touch.px < 317 && touch.py > 105 && touch.py < 150) currentEntry = 3;
										if (touch.px > 214 && touch.px < 265 && touch.py > 130 && touch.py < 175) currentEntry = 4;
										if (touch.px > 266 && touch.px < 317 && touch.py > 150 && touch.py < 195) currentEntry = 5;				
									}
								}
								if (hidKeysDown() & KEY_A) {
									setPkmn(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
									operationDone = true;
									break;
								}

								printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_CLONE, speedy, 0);
							}
							break;
						}
						case 2 : {
							if (isTeam) {
								infoDisp("You can't release a team member!");
								break;
							}
							memset(pkmn, 0, PKMNLENGTH);
							setPkmn(mainbuf, box, currentEntry, pkmn, game);
							infoDisp("Changes applied!");
							operationDone = true;
							break;
						}
						case 3 : {
							int index = (int)getPokedexNumber(pkmn);
							while(aptMainLoop() && !operationDone) {
								hidScanInput();
								touchPosition touch;
								hidTouchRead(&touch);

								if (hidKeysDown() & KEY_B) 
									break;
								
								if (hidKeysDown() & KEY_L)
									speedy = false;
								
								if (hidKeysDown() & KEY_R)
									speedy = true;

								if (hidKeysDown() & KEY_TOUCH && !speedy) {
									if (touch.px > 48 && touch.px < 64 && touch.py > 91 && touch.py < 210)
										if (index > 1)
											index--;
										
									if (touch.px > 145 && touch.px < 161 && touch.py > 91 && touch.py < 210)
										if (index < ((game < 4) ? 721 : 802))
											index++;
								}
								
								if (hidKeysHeld() & KEY_TOUCH && speedy) {
									if (touch.px > 48 && touch.px < 64 && touch.py > 91 && touch.py < 210)
										if (index > 1)
											index--;
										
									if (touch.px > 145 && touch.px < 161 && touch.py > 91 && touch.py < 210)
										if (index < ((game < 4) ? 721 : 802))
											index++;
								}

								if (hidKeysDown() & KEY_A) {
									FILE *fptr = fopen("romfs:/misc/living.bin", "rt");
									fseek(fptr, 0, SEEK_END);
									u32 size = ftell(fptr);
									u8* livingbuf = (u8*)malloc(size);
									memset(livingbuf, 0, size);
									rewind(fptr);
									fread(livingbuf, size, 1, fptr);
									fclose(fptr);
									memcpy(&mainbuf[getPkmnAddress((isTeam)? 33 : box, currentEntry, game)], &livingbuf[(index - 1) * 232], 232);
									free(livingbuf);
									operationDone = true;
									break;
								}

								printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_GENERATE, speedy, index);
							}
							break;
						}
					}
				}
				
				if (hidKeysDown() & KEY_A && menuEntry == 4)
					break;
			}
			if (!getPokedexNumber(pkmn)) {
				int index = 1;
				while(aptMainLoop() && !operationDone) {
					hidScanInput();
					touchPosition touch;
					hidTouchRead(&touch);

					if (hidKeysDown() & KEY_B) 
						break;
					
					if (hidKeysDown() & KEY_L)
						speedy = false;
					
					if (hidKeysDown() & KEY_R)
						speedy = true;

					if (hidKeysDown() & KEY_TOUCH && !speedy) {
						if (touch.px > 48 && touch.px < 64 && touch.py > 91 && touch.py < 210)
							if (index > 1)
								index--;
							
						if (touch.px > 145 && touch.px < 161 && touch.py > 91 && touch.py < 210)
							if (index < ((game < 4) ? 721 : 802))
								index++;
					}
					
					if (hidKeysHeld() & KEY_TOUCH && speedy) {
						if (touch.px > 48 && touch.px < 64 && touch.py > 91 && touch.py < 210)
							if (index > 1)
								index--;
							
						if (touch.px > 145 && touch.px < 161 && touch.py > 91 && touch.py < 210)
							if (index < ((game < 4) ? 721 : 802))
								index++;
					}

					if (hidKeysDown() & KEY_A) {
						FILE *fptr = fopen("romfs:/misc/living.bin", "rt");
						fseek(fptr, 0, SEEK_END);
						u32 size = ftell(fptr);
						u8* livingbuf = (u8*)malloc(size);
						memset(livingbuf, 0, size);
						rewind(fptr);
						fread(livingbuf, size, 1, fptr);
						fclose(fptr);
						memcpy(&mainbuf[getPkmnAddress((isTeam)? 33 : box, currentEntry, game)], &livingbuf[(index - 1) * 232], 232);
						free(livingbuf);
						operationDone = true;
						break;
					}

					printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_GENERATE, speedy, index);
				}
			}
		}
		printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_STANDARD, speedy, 0);
	}
	free(pkmn);
}