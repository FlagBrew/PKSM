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

#include "pkx.h"

u32 pkx_seedstep(const u32 seed) { 
	return seed * 0x41C64E6D + 0x00006073; 
}

u32 pkx_lcrng(u32 seed) { 
	return seed * 0x41C64E6D + 0x00006073; 
}

u32 pkx_get_save_address(const int boxnumber, const int indexnumber, const int game) {
    int boxpos = 0;
	
    if (ISXY)
		boxpos = boxnumber < 33 ? 0x22600 : 0x14200;
	else if (ISORAS)
		boxpos = boxnumber < 33 ? 0x33000 : 0x14200;
	else if (ISSUMO)
		boxpos = boxnumber < 33 ? 0x04E00 : 0x01400;

	if (boxnumber < 33)
		return boxpos + (PKMNLENGTH * 30 * boxnumber) + (indexnumber * PKMNLENGTH);

	return boxpos + indexnumber * 260;
}

void pkx_calculate_checksum(u8* data) {
    u16 chk = 0;

    for (int i = 8; i < PKMNLENGTH; i += 2)
        chk += *(u16*)(data + i);

    memcpy(data + 6, &chk, 2);
}

void pkx_shuffle_array(u8* pkmn, const u32 encryptionkey) {
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

void pkx_decrypt(u8* pkmn) {
    u32 encryptionkey = *(u32*)(pkmn);
    u32 seed = encryptionkey;

    u16 temp;
    for (int i = 0x08; i < PKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (pkx_seedstep(seed) >> 16);
        seed = pkx_seedstep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }

    pkx_shuffle_array(pkmn, encryptionkey);
}

void pkx_encrypt(u8* pkmn) {
    u32 encryptionkey = *(u32*)(pkmn);
    u32 seed = encryptionkey;

    for(int i = 0; i < 11; i++)
        pkx_shuffle_array(pkmn, encryptionkey);

    u16 temp;
    for(int i = 0x08; i < PKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (pkx_seedstep(seed) >> 16);
        seed = pkx_seedstep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
}

void pkx_get(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, const int game) {
    memcpy(pkmn, &mainbuf[pkx_get_save_address(boxnumber, indexnumber, game)], PKMNLENGTH);
    pkx_decrypt(pkmn);
}

void pkx_set(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, const int game) {
	u8 latestHandlers[10];
	char ot_name[NICKNAMELENGTH];
	char save_name[NICKNAMELENGTH];
	char ht_name[NICKNAMELENGTH];
	memset(ht_name, 0, NICKNAMELENGTH);

	memcpy(latestHandlers, &pkmn[0x94], 10);
	memcpy(ot_name, &pkmn[0xB0], NICKNAMELENGTH);
	memcpy(save_name, &mainbuf[ISGEN6 ? 0x14048 : 0x1238], NICKNAMELENGTH);
	
	if ((getSaveTID(mainbuf, game) == getOTID(pkmn)) && (getSaveSID(mainbuf, game) == getSOTID(pkmn)) && !memcmp(ot_name, save_name, NICKNAMELENGTH) && !memcmp(latestHandlers, ht_name, 10)) { //you're the first owner
		setHT(pkmn, ht_name);
		setHTGender(pkmn, 0);
	} else {
		setHT(pkmn, save_name);
		setHTGender(pkmn, getSaveGender(mainbuf, game));
	}

    pkx_calculate_checksum(pkmn);
    pkx_encrypt(pkmn);

    memcpy(&mainbuf[pkx_get_save_address(boxnumber, indexnumber, game)], pkmn, PKMNLENGTH);
}

u8 pkx_get_HT(u8* pkmn) { 
	return *(u8*)(pkmn + 0xDE); 
}

u8 pkx_get_gender(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0x1D)) >> 1) & 0x3; 
}

u8 pkx_get_language(u8* pkmn) { 
	return *(u8*)(pkmn + 0xE3); 
}

u8 pkx_get_ability(u8* pkmn) { 
	return *(u8*)(pkmn + 0x14); 
}

u8 pkx_get_ability_number(u8* pkmn) { 
	return *(u8*)(pkmn + 0x15); 
}

u8 pkx_get_form(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0x1D)) >> 3); 
}

u16 pkx_get_item(u8* pkmn) { 
	return *(u16*)(pkmn + 0x0A); 
}

u8 pkx_get_hp_type(u8* pkmn) { 
	return 15 * ((getIV(pkmn, 0)& 1) 
		  + 2 * (getIV(pkmn, 1) & 1) 
		  + 4 * (getIV(pkmn, 2) & 1) 
		  + 8 * (getIV(pkmn, 3) & 1) 
		  + 16 * (getIV(pkmn, 4) & 1) 
		  + 32 * (getIV(pkmn, 5) & 1)) / 63; 
}

u8 pkx_get_ot_gender(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0xDD)) >> 7); 
}

bool pkx_is_egg(u8* pkmn) {
    u32 eggbuffer = *(u32*)(pkmn + 0x74);
    eggbuffer = eggbuffer >> 30;
    eggbuffer = eggbuffer & 0x1;
	
	return eggbuffer == 1 ? true : false;
}

void pkx_reroll_encryption_key(u8* pkmn) {
	srand(time(NULL));
	u32 encryptbuffer = rand();
	memcpy(&pkmn[0x0], &encryptbuffer, 4);
}
