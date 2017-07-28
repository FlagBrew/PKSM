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

#include "editor.h"

/* ************************ local variables ************************ */
int lookupHT[] = {0, 1, 2, 5, 3, 4};
u8 DPActiveFlag[] = {0x20, 0x83, 0xB8, 0xED};

bool isShiny(u8* pkmn) {
    u16 trainersv = (pkx_get_tid(pkmn) ^ pkx_get_sid(pkmn)) >> 4;
    u16 pkmnv = ((pkx_get_pid(pkmn) >> 16) ^ (pkx_get_pid(pkmn) & 0xFFFF)) >> 4;

    if (trainersv == pkmnv) return true;
    else return false;
}

bool isBattleBoxed(u8* mainbuf, int game, int box, int slot) {
	if (ISGEN6) //don't care about obsolete titles
		return false;
		
	if (ISSUMO) {
		u8 team_lookup[72];
		memcpy(&team_lookup[0], &mainbuf[0x4CC4], 72);
		for (int i = 0; i < 72; i += 2)
			if ((team_lookup[i] == (u8)slot) && (team_lookup[i + 1] == (u8)box))
				return true;
	}
	
	return false;
}

u16 getFormSpeciesNumber(u8 *pkmn) {	
	u16 tempspecies = pkx_get_species(pkmn);
	u8 form = pkx_get_form(pkmn);
	u8 formcnt = personal.pkmData[tempspecies][0x0E];

	if (form && form < formcnt) {
		u16 backspecies = tempspecies;
		memcpy(&tempspecies, &personal.pkmData[tempspecies][0x0C], 2);
		if (!tempspecies)
			tempspecies = backspecies;
		else if (form < formcnt)
			tempspecies += form - 1;
	}	
	return tempspecies;
}

FormData *getLegalFormData(u16 species, int game) {
	FormData *forms = malloc(sizeof(FormData));
	forms->spriteNum = 0;
	forms->stringNum = 0;
	forms->min = 0;
	forms->max = 0;
	forms->editable = true;

	bool sumo = false, oras = false, xy = false, b2w2 = false, bw = false, hgss = false, plat = false;

	switch (game)
	{
		case GAME_SUN:
		case GAME_MOON:
			sumo = true;
		case GAME_OR:
		case GAME_AS:
			oras = true;
		case GAME_X:
		case GAME_Y:
			xy = true;
		case GAME_B2:
		case GAME_W2:
			b2w2 = true;
		case GAME_B1:
		case GAME_W1:
			bw = true;
		case GAME_HG:
		case GAME_SS:
			hgss = true;
		case GAME_PLATINUM:
			plat = true;
	}

	switch (species) {
		case 19 :
			if (sumo) {
				forms->spriteNum = 1;
				forms->max = 1;
			} break;
		case 20 :
			if (sumo) {
				forms->spriteNum = 2;
				forms->max = 1;
			} break;
		case 25 :
			if (sumo) {
				forms->spriteNum = 9;
				forms->stringNum = 9;
				forms->max = 6;
			} else if (oras) {
				forms->spriteNum = 3;
				forms->stringNum = 2;
				forms->max = 6;
			} break;
		case 26 :
			if (sumo) {
				forms->spriteNum = 15;
				forms->max = 1;
			} break;
		case 27 :
			if (sumo) {
				forms->spriteNum = 16;
				forms->max = 1;
			} break;
		case 28 :
			if (sumo) {
				forms->spriteNum = 17;
				forms->max = 1;
			} break;
		case 37 :
			if (sumo) {
				forms->spriteNum = 18;
				forms->max = 1;
			} break;
		case 38 :
			if (sumo) {
				forms->spriteNum = 19;
				forms->max = 1;
			} break;
		case 50 :
			if (sumo) {
				forms->spriteNum = 20;
				forms->max = 1;
			} break;
		case 51 :
			if (sumo) {
				forms->spriteNum = 21;
				forms->max = 1;
			} break;
		case 52 :
			if (sumo) {
				forms->spriteNum = 22;
				forms->max = 1;
			} break;
		case 53 :
			if (sumo) {
				forms->spriteNum = 23;
				forms->max = 1;
			} break;
		case 74 :
			if (sumo) {
				forms->spriteNum = 24;
				forms->max = 1;
			} break;
		case 75 :
			if (sumo) {
				forms->spriteNum = 25;
				forms->max = 1;
			} break;
		case 76 :
			if (sumo) {
				forms->spriteNum = 26;
				forms->max = 1;
			} break;
		case 88 :
			if (sumo) {
				forms->spriteNum = 27;
				forms->max = 1;
			} break;
		case 89 :
			if (sumo) {
				forms->spriteNum = 28;
				forms->max = 1;
			} break;
		case 103 :
			if (sumo) {
				forms->spriteNum = 29;
				forms->max = 1;
			} break;
		case 105 :
			if (sumo) {
				forms->spriteNum = 30;
				forms->max = 1;
			} break;
		case 172 :
			if (hgss && !bw) {
				forms->spriteNum = 0;
				forms->stringNum = 141;
				forms->max = 1;
			} break;
		case 201 :
			forms->spriteNum = 31;
			forms->stringNum = 16;
			forms->max = 27;
			break;
		case 386 :
			forms->spriteNum = 58;
			forms->stringNum = 44;
			forms->max = 3;
			break;
		case 412 :
			forms->spriteNum = 61;
			forms->stringNum = 48;
			forms->max = 2;
			break;
		case 413 :
			forms->spriteNum = 63;
			forms->stringNum = 48;
			forms->max = 2;
			break;
		case 422 :
			forms->spriteNum = 65;
			forms->stringNum = 51;
			forms->max = 1;
			break;
		case 423 :
			forms->spriteNum = 66;
			forms->stringNum = 51;
			forms->max = 1;
			break;
		case 479 :
			if (plat) {
				forms->spriteNum = 67;
				forms->stringNum = 53;
				forms->max = 5;
			} break;
		case 487 :
			if (plat) {
				forms->spriteNum = 72;
				forms->stringNum = 59;
				forms->max = 1;
			} break;
		case 492 :
			if (plat) {
				forms->spriteNum = 73;
				forms->stringNum = 61;
				forms->max = 1;
			} break;
		case 493 :
			forms->spriteNum = 0;
			forms->max = (xy || (hgss && !bw)) ? 17 : 16;
			forms->editable = false;
			break;
		case 550 :
			forms->spriteNum = 74;
			forms->stringNum = 63;
			forms->max = 1;
			break;
		case 585 :
			forms->spriteNum = 75;
			forms->stringNum = 65;
			forms->max = 3;
			break;
		case 586 :
			forms->spriteNum = 78;
			forms->stringNum = 65;
			forms->max = 3;
			break;
		case 641 :
			if (b2w2) {
				forms->spriteNum = 81;
				forms->stringNum = 69;
				forms->max = 1;
			} break;
		case 642 :
			if (b2w2) {
				forms->spriteNum = 82;
				forms->stringNum = 69;
				forms->max = 1;
			} break;
		case 645 :
			if (b2w2) {
				forms->spriteNum = 83;
				forms->stringNum = 69;
				forms->max = 1;
			} break;
		case 646 :
			if (b2w2) {
				forms->spriteNum = 84;
				forms->stringNum = 71;
				forms->max = 2;
			} break;
		case 647 :
			if (b2w2) {
				forms->spriteNum = 86;
				forms->stringNum = 74;
				forms->max = 1;
			} break;
		case 648 :
			forms->spriteNum = 87;
			forms->stringNum = 76;
			forms->max = 1;
			break;
		case 649 :
			forms->spriteNum = 0;
			forms->max = 4;
			forms->editable = false;
			break;
		case 658 :
			if (sumo) {
				forms->spriteNum = 0;
				forms->stringNum = 78;
				forms->max = 1;
			} break;
		case 664 :
		case 665 :
			forms->spriteNum = 0;
			forms->stringNum = 80;
			forms->max = 19;
			break;
		case 666 :
			forms->spriteNum = 88;
			forms->stringNum = 80;
			forms->max = 19;
			break;
		case 669 :
			forms->spriteNum = 107;
			forms->stringNum = 100;
			forms->max = 4;
			break;
		case 670 :
			forms->spriteNum = 111;
			forms->stringNum = 100;
			forms->max = 5;
			break;
		case 671 :
			forms->spriteNum = 116;
			forms->stringNum = 100;
			forms->max = 4;
			break;
		case 676 :
			forms->spriteNum = 120;
			forms->stringNum = 106;
			forms->max = 9;
			break;
		case 678 :
			forms->spriteNum = 145;
			forms->stringNum = 143;
			forms->max = 1;
			break;
		case 710 :
		case 711 :
			forms->spriteNum = 0;
			forms->stringNum = 116;
			forms->max = 3;
			break;
		case 718 :
			forms->spriteNum = 129;
			forms->stringNum = 120;
			forms->max = 3;
			break;
		case 720 :
			forms->spriteNum = 132;
			forms->stringNum = 124;
			forms->max = 1;
			break;
		case 741 :
			forms->spriteNum = 133;
			forms->stringNum = 126;
			forms->max = 3;
			break;
		case 745 :
			forms->spriteNum = 136;
			forms->stringNum = 130;
			forms->max = 1;
			break;
		case 773 :
			forms->spriteNum = 0;
			forms->max = 17;
			forms->editable = false;
			break;
		case 774 :
			forms->spriteNum = 137;
			forms->stringNum = 132;
			forms->min = 7;
			forms->max = 13;
			break;
		case 801 :
			forms->spriteNum = 144;
			forms->stringNum = 139;
			forms->max = 1;
			break;
	}

	forms->editable = forms->editable && forms->max > 0;
	return forms;
}

u16 getStat(u8* pkmn, const int stat) {
    u16 tempspecies = getFormSpeciesNumber(pkmn);

    u8 mult = 10;
    u16 final;
    u8 basestat = 0;
    if (stat == 0) basestat = personal.pkmData[tempspecies][0x0];
    if (stat == 1) basestat = personal.pkmData[tempspecies][0x1];
    if (stat == 2) basestat = personal.pkmData[tempspecies][0x2];
    if (stat == 3) basestat = personal.pkmData[tempspecies][0x3];
    if (stat == 4) basestat = personal.pkmData[tempspecies][0x4];
    if (stat == 5) basestat = personal.pkmData[tempspecies][0x5];
    
    if (stat == 0)
        final = 10 + ((2 * basestat) + ((((pkx_get_HT(pkmn) >> lookupHT[stat]) & 1) == 1) ? 31 : getIV(pkmn, stat)) + getEV(pkmn, stat) / 4 + 100) * pkx_get_level(pkmn) / 100;
    else
        final = 5 + (2 * basestat + ((((pkx_get_HT(pkmn) >> lookupHT[stat]) & 1) == 1) ? 31 : getIV(pkmn, stat)) + getEV(pkmn, stat) / 4) * pkx_get_level(pkmn) / 100; 
    
    if (getNature(pkmn) / 5 + 1 == stat)
        mult++;
    if (getNature(pkmn) % 5 + 1 == stat)
        mult--;
  
    final = final * mult / 10;
    return final;
}

u32 *getSaveOT(u8* mainbuf, int game, u32* dst) {
	u16 src[NICKNAMELENGTH];
	memcpy(src, &mainbuf[ISGEN6 ? 0x14048 : 0X1238], NICKNAMELENGTH);
	
	utf16_to_utf32(dst, src, NICKNAMELENGTH);
	return dst;
}

u8 getFriendship(u8* pkmn) {
	return (pkmn[0x93] == 0) ? getOTFriendship(pkmn) : getHTFriendship(pkmn);
}

u8 getHTFriendship(u8* pkmn) {
    u8 friendship;
    memcpy(&friendship, &pkmn[0xA2], 1);
    return friendship;
}

u8 getOTFriendship(u8* pkmn) {
    u8 friendship;
    memcpy(&friendship, &pkmn[0xCA], 1);
    return friendship;
}

u8 getNature(u8* pkmn) {
    u8 nature;
    memcpy(&nature, &pkmn[0x1C], NATURELENGTH);
    return nature;
}

u8 getEV(u8* pkmn, const int stat) {
    u8 evbuffer[6];
    memcpy(evbuffer, &pkmn[0x1E], EVLENGTH * 6);
    return evbuffer[stat];
}

u8 getIV(u8* pkmn, const int stat) {
    u32 buffer;
    u8 toreturn;

    memcpy(&buffer, &pkmn[0x74], IVLENGTH);
    buffer = buffer >> 5 * stat;
    buffer = buffer & 0x1F;
    memcpy(&toreturn, &buffer, 1);
    return toreturn;
}

u8 getBall(u8* pkmn) {
    u8 ballbuffer;
    memcpy(&ballbuffer, &pkmn[0xDC], 1);
    
    return ballbuffer;
}

u8 getSaveGender(u8* mainbuf, int game) {
	u8 buffer;
    memcpy(&buffer, &mainbuf[(ISGEN6 ? 0x14000 : 0x01200) + 5], 1);
    return buffer;
}

u16 getSaveTID(u8* mainbuf, int game) {
    u16 buffer;
    memcpy(&buffer, &mainbuf[ISGEN6 ? 0x14000 : 0x01200], 2);
    return buffer;
}

u16 getSaveSID(u8* mainbuf, int game) {
    u16 buffer;
    memcpy(&buffer, &mainbuf[(ISGEN6 ? 0x14000 : 0x01200) + 2], 2);
    return buffer;
}

u16 getSaveTSV(u8* mainbuf, int game) {
	u16 TID = getSaveTID(mainbuf, game);
	u16 SID = getSaveSID(mainbuf, game);
	return (TID ^ SID) >> 4;
}

u32 getSaveSeed(u8* mainbuf, int game, int index) {
    u32 buffer;
    memcpy(&buffer, &mainbuf[(ISGEN6 ? 0 : 0x6B5DC) + index * 0x4], 4);
    return buffer;
} 

/* ************************ set ************************ */

void setItemEditor(u8* pkmn, u16 item) {
    memcpy(&pkmn[0x0A], &item, ITEMLENGTH);
}

void setGender(u8* pkmn, u8 val) { pkmn[0x1D] = (u8)((pkmn[0x1D] & ~0x06) | (val << 1)); }
void setForm(u8* pkmn, u8 val) { pkmn[0x1D] = (u8)((pkmn[0x1D] & 0x07) | (val << 3)); }
void setBall(u8* pkmn, u8 val) { pkmn[0xDC] = val; }
void setOTGender(u8* pkmn, u8 val) { setFlag(pkmn, 0xDD, 7, (val == 1) ? true : false); }

void setTID(u8* pkmn, u16 tid) {
    memcpy(&pkmn[0x0C], &tid, 2);
}

void setSID(u8* pkmn, u16 sid) {
    memcpy(&pkmn[0x0E], &sid, 2);
}

void setAbility(u8* pkmn, const u8 ability) {
    u16 tempspecies = getFormSpeciesNumber(pkmn);
	
	u8 abilitynum = 0;
	if (ability == 0)      abilitynum = 1;
	else if (ability == 1) abilitynum = 2;
	else                   abilitynum = 4;
	
	memcpy(&pkmn[0x15], &abilitynum, ABILITYNUMLENGTH);
	memcpy(&pkmn[0x14], &personal.pkmData[tempspecies][0x09 + ability], ABILITYLENGTH);
}

void setMove(u8* pkmn, const u16 move, const int nmove) {
    memcpy(&pkmn[0x5A + (MOVELENGTH * nmove)], &move, MOVELENGTH);
}

void setEggMove(u8* pkmn, const u16 move, const int nmove) {
    memcpy(&pkmn[0x6A + (EGGMOVELENGTH * nmove)], &move, EGGMOVELENGTH);
}

void setNicknameZ(u8* pkmn, char* nick, int dst) {
	// dst 0x40(Nickname) 0xB0(OT) 0x78(HT)
	u8 toinsert[NICKNAMELENGTH];
	memset(toinsert, 0, NICKNAMELENGTH);

	if (!memcmp(nick, toinsert, NICKNAMELENGTH))
		return;

	char buf;
	int nicklen = strlen(nick);
	int r = 0, w = 0, i = 0;
	while (r < nicklen) {
		buf = *(nick + r);
		r++;
		if ((buf & 0x80) == 0) {
			toinsert[w] = buf & 0x7f;
			i = 0;
		//	w += 2;
		}
		else if ((buf & 0xe0) == 0xc0) {
			toinsert[w] = buf & 0x1f;
			i = 1;
		}
		else if ((buf & 0xf0) == 0xe0) {
			toinsert[w] = buf & 0x0f;
			i = 2;
		}
		else break;
		for (int j = 0; j < i; j++) {
			buf = *(nick + r);
			r++;
			if (toinsert[w] > 0x04) {
				toinsert[w + 1] = (toinsert[w + 1] << 6) | (((toinsert[w] & 0xfc) >> 2) & 0x3f);
				toinsert[w] &= 0x03;
			}
			toinsert[w] = (toinsert[w] << 6) | (buf & 0x3f);
		}
		// r++;
		w += 2;
		if (w > NICKNAMELENGTH)
			break;
	}

	if (dst == 0x40) {
		u8 isnicknamed;
		memcpy(&isnicknamed, &pkmn[0x77], 1);
		isnicknamed |= 0x80;
		memcpy(&pkmn[0x77], &isnicknamed, 1);
	}

	memcpy(&pkmn[dst], toinsert, NICKNAMELENGTH);
}

void setHT(u8* pkmn, char* nick) {
    memcpy(&pkmn[0x78], nick, NICKNAMELENGTH);
}

void setHTGender(u8* pkmn, const u8 gender) {
	memcpy(&pkmn[0x92], &gender, 1);
}

void setNature(u8* pkmn, const u8 nature) {
    memcpy(&pkmn[0x1C], &nature, NATURELENGTH);
}

void setFriendship(u8* pkmn, const int val) {
	if (pkmn[0x93] == 0)
		setOTFriendship(pkmn, val);
	else
		setHTFriendship(pkmn, val);
}

void setHTFriendship(u8* pkmn, const int val) {
	memcpy(&pkmn[0xA2], &val, 1);
}

void setOTFriendship(u8* pkmn, const int val) {
	memcpy(&pkmn[0xCA], &val, 1);
}

void setEV(u8* pkmn, u8 val, const int stat) {
    memcpy(&pkmn[0x1E + (EVLENGTH*stat)], &val, EVLENGTH);
}

void setIV(u8* pkmn, u8 val, const int stat) {
	u32 nval = val;
	u32 mask = 0xFFFFFFFF;
	mask ^= 0x1F << (5 * stat);

	u32 buffer;
	memcpy(&buffer, &pkmn[0x74], IVLENGTH);

	buffer &= mask;
	buffer ^= ((nval & 0x1F) << (5 * stat));
	memcpy(&pkmn[0x74], &buffer, IVLENGTH);
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
	if (!shiny)
		pkx_reroll_pid(pkmn);
	else {
		u16 tsv = (pkx_get_tid(pkmn) ^ pkx_get_sid(pkmn)) >> 4;
		u16 buffer = (pkx_get_pid(pkmn) >> 16) ^ (tsv << 4);
		memcpy(&pkmn[0x18], &buffer, 2);
	}
}

void setWC(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[]) {
	if (ISXY) {
		*(mainbuf + 0x1BC00 + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + 0x1BD00 + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);
	}

	if (ISORAS) {
		*(mainbuf + 0x1CC00 + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + 0x1CD00 + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);

		if (i == 2048) {
			*(mainbuf + 0x319B8)     = 0xC2;
			*(mainbuf + 0x319B8 + 1) = 0x73;
			*(mainbuf + 0x319B8 + 2) = 0x5D;
			*(mainbuf + 0x319B8 + 3) = 0x22;
		}
	}

	if (ISSUMO) {
		*(mainbuf + 0x65C00 + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + 0x65C00 + 0x100 + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);
	}

	if (ISGEN5) {
		u32 seed;
		memcpy(&seed, &mainbuf[0x1D290], sizeof(u32));

		//decrypt
		u16 temp;
		for (int i = 0; i < 0xA90; i += 2) {
			memcpy(&temp, &mainbuf[0x1C800 + i], 2);
			temp ^= (pkx_lcrng(seed) >> 16);
			seed = pkx_lcrng(seed);
			memcpy(&mainbuf[0x1C800 + i], &temp, 2);
		}

		*(mainbuf + 0x1C800 + i / 8) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + 0x1C900 + nInjected[0] * PGFLENGTH), (const void*)wcbuf, PGFLENGTH);

		//encrypt
		memcpy(&seed, &mainbuf[0x1D290], sizeof(u32));
		for (int i = 0; i < 0xA90; i += 2) {
			memcpy(&temp, &mainbuf[0x1C800 + i], 2);
			temp ^= (pkx_lcrng(seed) >> 16);
			seed = pkx_lcrng(seed);
			memcpy(&mainbuf[0x1C800 + i], &temp, 2);
		}
	}

	nInjected[0] += 1;
	if (ISGEN6) {
		if (nInjected[0] >= 24)
			nInjected[0] = 0;
	} else if (nInjected[0] >= 48)
		nInjected[0] = 0;
}

void setWC4(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[], int GBO) {
	mainbuf[GBO + 72] = (u8)((mainbuf[GBO + 72] & 0xFE) | 1);
	if (game == GAME_HG || game == GAME_SS) {
		*(mainbuf + 0x9D3C + GBO + (2047 >> 3)) = 0x80;
		memcpy(&mainbuf[0x9E3C + GBO + nInjected[0] * PGTLENGTH], wcbuf, PGTLENGTH);
	}
	else if (game == GAME_PLATINUM) {
		*(mainbuf + 0xB4C0 + GBO + (2047 >> 3)) = 0x80;
		memcpy(&mainbuf[0xB5C0 + GBO + nInjected[0] * PGTLENGTH], wcbuf, PGTLENGTH);
	}
	else if (game == GAME_DIAMOND || game == GAME_PEARL) {
		memcpy(&mainbuf[0xA7D0 + GBO + nInjected[0] * 4], &DPActiveFlag[0], 4);
		memcpy(&mainbuf[0xA7FC + GBO + nInjected[0] * PGTLENGTH], wcbuf, PGTLENGTH);
	}

	nInjected[0] += 1;
	if (nInjected[0] >= 8)
		nInjected[0] = 0;
}

int getSaveLanguageAddress(u8* mainbuf, int game) {
	int address = 0;
	if (ISXY)
		address = 0x14000 + 0x2D;
	else if (ISORAS)
		address = 0x14000 + 0x2D;
	else if (ISSUMO)
		address = 0x01200 + 0x35;

	return address;
}

void setSaveLanguage(u8* mainbuf, int game, int i) {
	u8 langValues[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x09, 0x0A};
	memcpy(&mainbuf[getSaveLanguageAddress(mainbuf, game)], &langValues[i], sizeof(u8));
}


u8 getSaveLanguage(u8* mainbuf, int game) {
	return mainbuf[getSaveLanguageAddress(mainbuf, game)];
}


void setPokerus(u8* pkmn) {
	*(pkmn + 0x2B) = 0x11;
}

void saveFileEditor(u8* mainbuf, int game, u64 size) {
	int currentEntry = 0;
	int page = 0;
	int maxpages = size/240;
	int speed = 0;
	
	fillSaveSectors(saveSectors, game);
	while (aptMainLoop() & !(hidKeysDown() & KEY_B)) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		calcCurrentEntryMorePages(&currentEntry, &page, maxpages, 239, 16);

		bool downPlus = ((hidKeysDown() & KEY_TOUCH) && touch.px > 267 && touch.px < 284 && touch.py > 30 && touch.py < 48) || (hidKeysDown() & KEY_A);
		bool downMinus = ((hidKeysDown() & KEY_TOUCH) && touch.px > 244 && touch.px < 261 && touch.py > 30 && touch.py < 48) || (hidKeysDown() & KEY_X);
		bool heldPlus = ((hidKeysHeld() & KEY_TOUCH) && touch.px > 267 && touch.px < 284 && touch.py > 30 && touch.py < 48) || (hidKeysHeld() & KEY_A);
		bool heldMinus = ((hidKeysHeld() & KEY_TOUCH) && touch.px > 244 && touch.px < 261 && touch.py > 30 && touch.py < 48) || (hidKeysHeld() & KEY_X);
		
		if (heldMinus && heldPlus)
			speed = 0;
		else if (saveSectors[currentEntry + 240*page][0] && downMinus) {
			if (mainbuf[currentEntry + 240*page] > 0)
				mainbuf[currentEntry + 240*page]--;
		}
		else if (saveSectors[currentEntry + 240*page][0] && heldMinus) {
			if (speed < -30 && mainbuf[currentEntry + 240*page] > 0)
				mainbuf[currentEntry + 240*page]--;
			else
				speed--;
		}
		else if (saveSectors[currentEntry + 240*page][0] && downPlus) {
			if (mainbuf[currentEntry + 240*page] < 0xFF)
				parseSaveHexEditor(mainbuf, game, currentEntry + 240*page);
		}
		else if (saveSectors[currentEntry + 240*page][0] && heldPlus) {
			if (speed > 30 && mainbuf[currentEntry + 240*page] < 0xFF)
				parseSaveHexEditor(mainbuf, game, currentEntry + 240*page);
			else
				speed++;
		}
		else
			speed = 0;	

		printEditor(mainbuf, game, size, currentEntry, page);
	}
}

void setRibbons(u8* pkmn, int ribcat, int ribnumber, bool value) {
	u8 tmp;
	memcpy(&tmp, &pkmn[0x30 + ribcat], 1);
	tmp = (u8)((tmp & ~(1 << ribnumber)) | (value ? 1 << ribnumber : 0));
	memcpy(&pkmn[0x30 + ribcat], &tmp, 1);
}

bool getRibbons(u8* pkmn, int ribcat, int ribnumber) {
	return (pkmn[0x30 + ribcat] & (1 << ribnumber)) == 1 << ribnumber;
}

void setHTi(u8* pkmn, int htnumber, bool value) {
	// HyperTrains htnumber HP,ATK,SPATK,SPDEF,SPEED
	u8 tmp;
	memcpy(&tmp, &pkmn[0xDE], 1);
	tmp = (u8)((tmp & ~(1 << htnumber)) | (value ? 1 << htnumber : 0));
	memcpy(&pkmn[0xDE], &tmp, 1);
}

bool getHTi(u8* pkmn, int htnumber) {
	return (pkmn[0xDE] & (1 << htnumber)) == 1 << htnumber;
}

void setFlag(u8* pkmn, int flgaddr, int flgshift, bool value) {
	if (flgaddr < 0 || PKMNLENGTH <= flgaddr || flgshift < 0 || 8 <= flgshift) 
		return;
	
	u8 tmp;
	memcpy(&tmp, &pkmn[flgaddr], 1);
	tmp = (u8)((tmp & ~(1 << flgshift)) | (value ? 1 << flgshift : 0));
	memcpy(&pkmn[flgaddr], &tmp, 1);
}

void parseHexEditor(u8* pkmn, int game, int byteEntry) {	
	if (!hax) {
		if (byteEntry == 0x08 || byteEntry == 0x09)
			checkMaxValueBetweenBounds(pkmn, byteEntry, 0x08, 2, 802);
		else if (byteEntry == 0x14)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 231);
		else if (byteEntry == 0x15)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 1);
		else if (byteEntry == 0x1D)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 1);
		else if (byteEntry == 0x36)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 2);
		else if (byteEntry == 0x1E || byteEntry == 0x1F || byteEntry == 0x20 || byteEntry == 0x21 || byteEntry == 0x22 || byteEntry == 0x23) {
			int tot = 0;
			for (int i = 0; i < 6; i++)
				tot += getEV(pkmn, i);
			if (tot < 510)
				pkmn[byteEntry]++;
		}
		else if (byteEntry == 0x62 || byteEntry == 0x63 || byteEntry == 0x64 || byteEntry == 0x65)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 39);
		else if (byteEntry == 0x66 || byteEntry == 0x67 || byteEntry == 0x68 || byteEntry == 0x69)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 2);
		else if (byteEntry == 0xD1 || byteEntry == 0xD4) // year
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 98);
		else if (byteEntry == 0xD2 || byteEntry == 0xD5) // month
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 11);
		else if (byteEntry == 0xD3 || byteEntry == 0xD6) // day; yes, this is shitty as fuck
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 30);
		else if (byteEntry == 0xDD) { // Met Level & OT Gender
			int metLV = pkmn[byteEntry] & 0x7f;
			if (metLV < 100) pkmn[byteEntry]++;
		}
		else if (byteEntry == 0xDE)
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 0x3E);
		else
			pkmn[byteEntry]++;
	} else if (byteEntry == 0x14) // ability needs a dedicated check for hax mode too
			checkMaxValue(pkmn, byteEntry, pkmn[byteEntry], 231);
		else 
			pkmn[byteEntry]++;
}

void parseSaveHexEditor(u8* mainbuf, int game, int byte) {	
	if (ISSUMO) {
		if (byte >= SAVE_SM_ITEM && byte < SAVE_SM_ITEM_SIZE) {
			// check items
			for (unsigned int offset = SAVE_SM_ITEM; offset < SAVE_SM_ITEM_SIZE; offset += 4) {
				for (unsigned int cursor = offset; cursor <= offset + 1; cursor++) {
					unsigned int pos = offset + cursor;
					if (byte == pos)
						checkMaxValueBetweenBounds(mainbuf, byte, (pos % 2 == 0) ? pos : pos - 1, 2, 919);
				}
			}
		}
		else if (byte >= SAVE_SM_MONEY && byte < SAVE_SM_MONEY + 4)
			checkMaxValueBetweenBounds(mainbuf, byte, SAVE_SM_MONEY, 4, 9999999);
		else
			mainbuf[byte]++;
	} 
	else if (ISORAS) {
		if (byte >= SAVE_ORAS_MONEY && byte < SAVE_ORAS_MONEY + 4)
			checkMaxValueBetweenBounds(mainbuf, byte, SAVE_ORAS_MONEY, 4, 9999999);
		else
			mainbuf[byte]++;		
	} 
	else if (ISXY) {
		if (byte >= SAVE_XY_MONEY && byte < SAVE_XY_MONEY + 4)
			checkMaxValueBetweenBounds(mainbuf, byte, SAVE_XY_MONEY, 4, 9999999);
		else
			mainbuf[byte]++;		
	} 
	else if (game == GAME_B2 || game == GAME_W2) {
		
	} 
	else if (game == GAME_B1 || game == GAME_W1) {
		
	} 
	else if (game == GAME_DIAMOND) {
		
	} 
	else if (game == GAME_HG || game == GAME_SS) {
		
	} 
	else if (game == GAME_PEARL || game == GAME_DIAMOND) {
		
	}
}

void generate(u8* mainbuf, int game, bool isTeam, int box, int currentEntry, int page, int genEntry) {
	FILE *fptr = fopen(ISGEN6 ? "romfs:/misc/living6.bin" : "romfs:/misc/living7.bin", "rt");
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	u8* livingbuf = (u8*)malloc(size);
	memset(livingbuf, 0, size);
	rewind(fptr);
	fread(livingbuf, size, 1, fptr);
	fclose(fptr);

	memcpy(&mainbuf[pkx_get_save_address((isTeam) ? 33 : box, currentEntry, game)], &livingbuf[(page * 40 + genEntry) * 232], 232);
	u8 tempkmn[PKMNLENGTH];
	pkx_get(mainbuf, (isTeam) ? 33 : box, currentEntry, tempkmn, game);
	memcpy(&tempkmn[0xE3], &mainbuf[ISGEN6 ? 0x1402D : 0x1235], 1); // nats

	// Correct Nickname of current language
	char nick[NICKNAMELENGTH] = "";

	utf32_to_utf8((unsigned char*)nick, (uint32_t*)listSpecies.items[pkx_get_species(tempkmn)], NICKNAMELENGTH);
	nick[NICKNAMELENGTH - 1] = '\0';

	setNicknameZ(tempkmn, nick, 0x40);

	// Randomizing the encryption constant
	pkx_reroll_encryption_key(tempkmn);
	setDex(mainbuf, tempkmn, game);
	pkx_set(mainbuf, (isTeam) ? 33 : box, currentEntry, tempkmn, game);

	free(livingbuf);	
}

void pokemonEditor(u8* mainbuf, int game) {
	int lookup[] = {0, 1, 2, 4, 5, 3};
	
	int modeFlag = ED_STANDARD;
	bool isTeam = false;
	int speed = 0;
	int box = 0;
	int currentEntry = 0;
	int menuEntry = 0;
	int byteEntry = 0;
	int boxmax = ISGEN6 ? 30 : 31;
	int touchExecuting = 0;
	int oldEntry = 0;
	
	wchar_t* descriptions[PKMNLENGTH];
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	int ability = (int)pkx_get_ability_number(pkmn);

	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		if (!isTeam) {
			oldEntry = currentEntry;
			calcCurrentEntryMorePages(&currentEntry, &box, boxmax + 1, 29, 6);
			if (oldEntry != currentEntry)
				touchExecuting = currentEntry;
		}
		
		if (hidKeysDown() & KEY_B) 
			break;
		
		if (hidKeysDown() & KEY_X) {
			isTeam = isTeam ? false : true;
			if (isTeam)
				currentEntry = currentEntry > 5 ? 5 : currentEntry;
		}

 		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 210 && touch.px < 320 && touch.py > 0 && touch.py < 210) {
				currentEntry = -1;
				if (!isTeam)
					touchExecuting = -1;
				isTeam = true;
			}
			if (touch.px > 0 && touch.px < 210 && touch.py > 0 && touch.py < 210) {
				currentEntry = -1;
				if (isTeam)
					touchExecuting = -1;
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
			
			if (touch.px > 0 && touch.px < 210 && touch.py > 210 && touch.py < 240)
				modeFlag = (modeFlag == ED_STANDARD) ? ED_SEED : ED_STANDARD;
			
			if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
				break;
		}
		
 		if (hidKeysHeld() & KEY_TOUCH) {
			if (!isTeam) {
				int x_start, y_start = 45;	
				for (int i = 0; i < 5; i++) {
					x_start = 4;
					for (int j = 0; j < 6; j++) {
						if ((touch.px > x_start) && (touch.px < (x_start + 34)) && (touch.py > y_start) && (touch.py < (y_start + 30))) {
							currentEntry = i * 6 + j;
							if (touchExecuting == currentEntry + 40)
								touchExecuting += 40;
							else
								touchExecuting = currentEntry;
						}
						x_start += 34;
					}
					y_start += 30;
				}
			} else {
				for (int i = 0, j = 0, k = 0; i < 6; i++) {
					if (touch.px > 214 + j && touch.px < 265 + j && touch.py > 40 + k && touch.py < 85 + k) {
						currentEntry = i;
						if (touchExecuting == currentEntry + 40)
							touchExecuting += 40;
						else
							touchExecuting = currentEntry;
					}
					if (i % 2 == 0) {
						j = 52;
						k += 20;
					} else {
						j = 0;
						k += 25;
					}
				}
			}
		}
		
#ifdef PKSV
#else
		if (((hidKeysDown() & KEY_Y) || ((hidKeysDown() & KEY_TOUCH) && touch.px > 240 && touch.px < 276 && touch.py > 210 && touch.py < 240)) && !isTeam) {
			if (!socket_init())
				break;
			
			// work in temporary variable
			int tempVett[2]; // box, currentEntry
			tempVett[0] = box;
			tempVett[1] = currentEntry;

			do {
				hidScanInput();
				calcCurrentEntryMorePages(&tempVett[1], &tempVett[0], boxmax + 1, 29, 6);

				if (hidKeysDown() & KEY_X) {
					u8 toBeChecked[PKMNLENGTH];
					pkx_get(mainbuf, tempVett[0], tempVett[1], toBeChecked, game);
					if (pkx_get_species(toBeChecked)) {
						if (!socket_is_legality_address_set())
							socket_set_legality_address(true);
						
						else if (socket_is_legality_address_set())
							processLegality(toBeChecked);
					}
				}
				
				process_pkx(mainbuf, game, tempVett);
				printPKViewer(mainbuf, pkmn, isTeam, game, tempVett[1], menuEntry, tempVett[0], ED_OTA, 0, 0);	
			} while (aptMainLoop() && !(hidKeysDown() & KEY_B));
			socket_shutdown();
			
			//swap
			box = tempVett[0];
			currentEntry = tempVett[1];
		}
#endif

		if (!(hidKeysDown() & KEY_TOUCH) && !(hidKeysHeld() & KEY_TOUCH) && touchExecuting >= 0 && touchExecuting / 40 == 0)// && !teamChanged)
			touchExecuting += 40;

		if (currentEntry < 0) {
			currentEntry = 0;
			touchExecuting = 0;
		}

		#ifdef PKSV
		#else
		if (((hidKeysDown() & KEY_A) || touchExecuting / 40 == 2) && !isBattleBoxed(mainbuf, game, box, currentEntry)) {
			touchExecuting = currentEntry;

			pkx_get(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
			bool operationDone = false;

			touchExecuting = menuEntry;
			while (aptMainLoop() && (pkx_get_species(pkmn) > 0 && pkx_get_species(pkmn) < 822) && !operationDone && !(hidKeysDown() & KEY_B)) {
				hidScanInput();
				hidTouchRead(&touch);
				oldEntry = menuEntry;
				menuEntry = calcCurrentEntryOneScreen(menuEntry, 4, 1);
				if (oldEntry != menuEntry)
					touchExecuting = menuEntry;
				
				printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_MENU, 0, 0);
				
				if (hidKeysHeld() & KEY_TOUCH) {
					if (touch.px > 242 && touch.px < 283 && touch.py > 5 && touch.py < 25 && !isTeam && getBall(pkmn) != CHERISH_BALL) {
						setTID(pkmn, getSaveTID(mainbuf, game));
						setSID(pkmn, getSaveSID(mainbuf, game));

						int language =  getSaveLanguage(mainbuf, game);
						memcpy(&pkmn[0xE3], &language, 1); // nats
						
						pkmn[0x93] = 0; // current handler to 0
						pkmn[0xA2] = 0; // held trainer friendship to 0

						u32 nick32[NICKNAMELENGTH];
						u8 nick[NICKNAMELENGTH];
						memset(nick32, 0, NICKNAMELENGTH*sizeof(u32));
						memset(nick, 0, NICKNAMELENGTH);
						getSaveOT(mainbuf, game, nick32);
						utf32_to_utf8(nick, nick32, NICKNAMELENGTH);

						setNicknameZ(pkmn, (char*)nick, 0xb0);
						setOTGender(pkmn, (getSaveGender(mainbuf, game)));

						pkx_set(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
						operationDone = true;
						break;
					}
					
					if (touch.px > 208 && touch.px < 317) {
						for (int i = 0; i < 5; i++) {
							if (touch.py > 42 + 27 * i + i*4 && touch.py < 69 + 27 * i + i*4) {
								menuEntry = i;
								if (touchExecuting == menuEntry + 40)
									touchExecuting += 40;
								else
									touchExecuting = menuEntry;
							}
						}
					}
				}
				if (!(hidKeysDown() & KEY_TOUCH) && !(hidKeysHeld() & KEY_TOUCH) && touchExecuting / 40 == 0)
					touchExecuting += 40;

				if (((hidKeysDown() & KEY_A) || touchExecuting / 40 == 2) && menuEntry < 4) {
					touchExecuting = menuEntry;
					switch (menuEntry) {
						case 0: {
							while (aptMainLoop() && !operationDone && !isTeam) {
								hidScanInput();
								touchPosition touch;
								hidTouchRead(&touch);

								if (hidKeysDown() & KEY_B) {
									operationDone = true;
									break;
								}
								
								// hex editor
								if ((hidKeysDown() & KEY_TOUCH) && touch.px > 290 && touch.px < 320 && touch.py > 0 && touch.py < 24)  {
									fillSectors(sector);
									fillDescriptions(descriptions);
									hax = false;
									
									while(aptMainLoop() && !(hidKeysDown() & KEY_B)) {
										hidScanInput();
										hidTouchRead(&touch);
										byteEntry = calcCurrentEntryOneScreen(byteEntry, 231, 16);

										if (hidKeysDown() & KEY_TOUCH) {
											if (touch.px > 0 && touch.px < 20 && touch.py > 0 && touch.py < 20) pattern[0] = true;
											if (touch.px > 300 && touch.px < 320 && touch.py > 0 && touch.py < 20) pattern[1] = true;
											if (touch.px > 0 && touch.px < 20 && touch.py > 220 && touch.py < 240) pattern[2] = true;
											if (touch.px > 300 && touch.px < 320 && touch.py > 220 && touch.py < 240) pattern[3] = true;
										}
										
										if (pattern[0] && pattern[1] && pattern[2] && pattern[3]) {
											if (!hax)
												fillSectorsHaxMode(sector);
											else
												fillSectors(sector);
											hax = (hax) ? false : true;
											for (int i = 0; i < 4; i++) pattern[i] = false;
										}

										if (sector[byteEntry][1]) {
											if (byteEntry == 0x30 || byteEntry == 0x31 || byteEntry == 0x32 || byteEntry == 0x33 || byteEntry == 0x34 || byteEntry == 0x35 || byteEntry == 0x36) {
												for (int i = 0; i < 8; i++) {
													if ((hidKeysDown() & KEY_TOUCH) && touch.px > 90 && touch.px < 103 && touch.py > 70 + i*17 && touch.py < 83 + i*17 && !(byteEntry == 0x36 && i > 1))
														setRibbons(pkmn, byteEntry - 0x30, i, !getRibbons(pkmn, byteEntry - 0x30, i));
												}
											}
											if (byteEntry == 0x77) {
												if ((hidKeysDown() & KEY_TOUCH) && touch.px > 90 && touch.px < 103 && touch.py > 70 && touch.py < 83)
													setFlag(pkmn, 0x77, 7, !pkx_get_nickname_flag(pkmn));
												if ((hidKeysDown() & KEY_TOUCH) && touch.px > 90 && touch.px < 103 && touch.py > 70 + 17 && touch.py < 83 + 17)
													setFlag(pkmn, 0x77, 6, !pkx_is_egg(pkmn));
											}
											if (byteEntry == 0xDD) {
												if ((hidKeysDown() & KEY_TOUCH) && touch.px > 100 - 3 && touch.px < 100 + 15 && touch.py > 89 - 6 && touch.py < 89 + 14)
													setFlag(pkmn, 0xdd, 7, !pkx_get_ot_gender(pkmn));
											}
											if (byteEntry == 0xDE) {
												for (int i = 0; i < 6; i++) {
													if ((hidKeysDown() & KEY_TOUCH) && touch.px > 90 && touch.px < 103 && touch.py > 70 + i * 17 && touch.py < 83 + i * 17)
														setHTi(pkmn, i, !getHTi(pkmn, i));
												}
											}
											if (byteEntry == 0x1D) {
												if ((hidKeysDown() & KEY_TOUCH) && touch.px > 90 && touch.px < 103 && touch.py > 70 && touch.py < 83)
													setFlag(pkmn, 0x1D, 0, !((pkmn[byteEntry] & 1) == 1));
											}
										}

										bool downPlus = ((hidKeysDown() & KEY_TOUCH) && touch.px > 247 && touch.px < 264 && touch.py > 31 && touch.py < 49) || (hidKeysDown() & KEY_A);
										bool downMinus = ((hidKeysDown() & KEY_TOUCH) && touch.px > 224 && touch.px < 241 && touch.py > 31 && touch.py < 49) || (hidKeysDown() & KEY_X);
										bool heldPlus = ((hidKeysHeld() & KEY_TOUCH) && touch.px > 247 && touch.px < 264 && touch.py > 31 && touch.py < 49) || (hidKeysHeld() & KEY_A);
										bool heldMinus = ((hidKeysHeld() & KEY_TOUCH) && touch.px > 224 && touch.px < 241 && touch.py > 31 && touch.py < 49) || (hidKeysHeld() & KEY_X);
										
										if (heldMinus && heldPlus)
											speed = 0;
										else if (sector[byteEntry][0] && downMinus) {
											if (pkmn[byteEntry] > 0)
												pkmn[byteEntry]--;
										}
										else if (sector[byteEntry][0] && heldMinus) {
											if (speed < -30 && pkmn[byteEntry] > 0)
												pkmn[byteEntry]--;
											else
												speed--;
										}
										else if (sector[byteEntry][0] && downPlus) {
											if (pkmn[byteEntry] < 0xFF)
												parseHexEditor(pkmn, game, byteEntry);
										}
										else if (sector[byteEntry][0] && heldPlus) {
											if (speed > 30 && pkmn[byteEntry] < 0xFF)
												parseHexEditor(pkmn, game, byteEntry);
											else
												speed++;
										}
										else
											speed = 0;
										
										printPKEditor(pkmn, game, byteEntry, 0, 0, ED_HEX, descriptions);
									}
								}
								
								if (hidKeysDown() & KEY_TOUCH) {
									if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
										break;
			
									if (touch.px > 206 && touch.px < 317 && touch.py > 110 && touch.py < 141) {
										while(aptMainLoop() && !(hidKeysDown() & KEY_B)) {
											hidScanInput();
											touchPosition touch;
											hidTouchRead(&touch);
											
											if (hidKeysDown() & KEY_TOUCH) {
												if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) 
													break;
												if (touch.px > 291 && touch.px < 316 && touch.py > 175 && touch.py < 187) {
													int hpEntry = 0;
													while(aptMainLoop() && !(hidKeysDown() & KEY_B)) {
														hidScanInput();
														hpEntry = calcCurrentEntryOneScreen(hpEntry, 15, 4);
	
														if (hidKeysDown() & KEY_A) {
															setHPType(pkmn, (u8)hpEntry);
															break;
														}
														
														printPKEditor(pkmn, game, hpEntry, 0, 0, ED_HIDDENPOWER, descriptions);
													}
												}
											}
											
											if (hidKeysDown() & KEY_TOUCH) {
												for (int i = 0; i < 6; i++) {
													int iv = getIV(pkmn, lookup[i]);
													int ev = getEV(pkmn, lookup[i]);
													int oldev = ev;
													int tot = 0;
													
													if (touch.px > 96 && touch.px < 109 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														setIV(pkmn, (iv > 0) ? iv - 1 : 31, lookup[i]);
													if (touch.px > 139 && touch.px < 152 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														setIV(pkmn, (iv < 31) ? iv + 1 : 0, lookup[i]);
													if (touch.px > 177 && touch.px < 190 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														setEV(pkmn, (ev > 0) ? ev - 1 : 252, lookup[i]);
													if (touch.px > 218 && touch.px < 231 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20)
														setEV(pkmn, (ev < 252) ? ev + 1 : 0, lookup[i]);
													
													for (int i = 0; i < 6; i++)
														tot += getEV(pkmn, i);
													if (tot > 510)
														setEV(pkmn, oldev, lookup[i]);
												}
											}
											
											else if (hidKeysHeld() & KEY_TOUCH) {
												bool touched = false;

												for (int i = 0; i < 6; i++) {
													if (touch.px > 96 && touch.px < 109 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20) {
														touched = true;
														if (speed < -30 && getIV(pkmn, lookup[i]) > 0)
															setIV(pkmn, getIV(pkmn, lookup[i]) - 1, lookup[i]);
														else
															speed--;
													}
													else if (touch.px > 139 && touch.px < 152 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20) {
														touched = true;
														if (speed > 30 && getIV(pkmn, lookup[i]) < 31)
															setIV(pkmn, getIV(pkmn, lookup[i]) + 1, lookup[i]);
														else
															speed++;
													}
													else if (touch.px > 177 && touch.px < 190 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20) {
														touched = true;
														if (speed < -30 && getEV(pkmn, lookup[i]) > 0)
															setEV(pkmn, getEV(pkmn, lookup[i]) - 1, lookup[i]);
														else
															speed--;
													}
													else if (touch.px > 218 && touch.px < 231 && touch.py > 49 + i * 20 && touch.py < 62 + i * 20) {
														touched = true;
														if (speed > 30 && getEV(pkmn, lookup[i]) < 252) {
															int tot = 0;
															for (int i = 0; i < 6; i++)
																tot += getEV(pkmn, i);
															if (tot < 510)
																setEV(pkmn, getEV(pkmn, lookup[i]) + 1, lookup[i]);
														}
														else
															speed++;
													}
												}

												if (!touched)
													speed = 0;
											}
											else
												speed = 0;
											
											printPKEditor(pkmn, game, 0, 0, 0, ED_STATS, descriptions);
										}
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 51 && touch.py < 63) {
										int natureEntry = 0;
										while(aptMainLoop() && !(hidKeysDown() & KEY_B)) {
											hidScanInput();
											natureEntry = calcCurrentEntryOneScreen(natureEntry, 24, 5);
											
											if (hidKeysDown() & KEY_A) {
												setNature(pkmn, natureEntry);
												break;
											}
											
											printPKEditor(pkmn, game, natureEntry, 0, 0, ED_NATURES, descriptions);
										}
									}
									
									if (touch.px > 0 && touch.px < 26 && touch.py > 0 && touch.py < 20) {
										int ballEntry = 0;
										while(aptMainLoop() && !(hidKeysDown() & KEY_B)) {
											hidScanInput();
											ballEntry = calcCurrentEntryOneScreen(ballEntry, 25, 6);

											if (hidKeysDown() & KEY_A) {
												setBall(pkmn, (u8)ballEntry + 1);
												break;
											}
											
											printPKEditor(pkmn, game, ballEntry, 0, 0, ED_BALLS, descriptions);
										}
									}
									
									if (touch.px > 227 && touch.px < 300 && touch.py > 24 && touch.py < 92) {
										u16 species = pkx_get_species(pkmn);
										FormData *forms = getLegalFormData(species, game);
										if (forms->editable) {
											int numforms = forms->max - forms->min + 1;
											int columns;
											if (numforms <= 16)
												columns = 4;
											else
												columns = 6;
											
											u8 form = pkx_get_form(pkmn);
											int formEntry = form >= forms->min && form <= forms->max ? form - forms->min : 0;
											while(aptMainLoop() && !(hidKeysDown() & KEY_B)) {
												hidScanInput();

												if (hidKeysDown() & KEY_RIGHT)
													if (formEntry + 1 < numforms) 
														formEntry++;
												
												if (hidKeysDown() & KEY_LEFT)
													if (formEntry > 0) 
														formEntry--;
												
												if (hidKeysDown() & KEY_UP)
													if (formEntry >= columns) 
														formEntry -= columns;
												
												if (hidKeysDown() & KEY_DOWN)
													if (formEntry + columns < numforms)
														formEntry += columns;
													
												if (hidKeysDown() & KEY_A) {
													setForm(pkmn, (u8)(formEntry + forms->min));
													setAbility(pkmn, ability);
													break;
												}
												
												printPKEditor(pkmn, game, formEntry, (int)species, 0, ED_FORMS, descriptions);
											}
										}
										free(forms);
									}

									if (touch.px > 180 && touch.px < 195 && touch.py > 71 && touch.py < 83) {
										ability = (ability + 1) % 3;
										setAbility(pkmn, ability);
									}
									
									if (touch.px > 156 && touch.px < 174 && touch.py > 0 && touch.py < 20) {
										if (pkx_get_gender(pkmn) != 2) 
											setGender(pkmn, ((pkx_get_gender(pkmn) == 0) ? 1 : 0));
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 111 && touch.py < 123)
										setShiny(pkmn, isShiny(pkmn) ? false : true);
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 131 && touch.py < 143) {
										if (!(pkx_get_pokerus(pkmn)))
											setPokerus(pkmn);
										else 
											*(pkmn + 0x2B) = 0x00;
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 171 && touch.py < 183) {
										static SwkbdState swkbd;
										static char nick[NICKNAMELENGTH];
										memset(nick, 0, NICKNAMELENGTH);

										SwkbdButton button = SWKBD_BUTTON_NONE;
										swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, 12);
										swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_DIGITS | SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 2);
										swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);

										// Fix for swkKeyboard being in UTF8 only (char*)
										i18n_initTextSwkbd(&swkbd, S_EDITOR_TEXT_CANCEL, S_EDITOR_TEXT_SET, S_EDITOR_TEXT_ENTER_NICKNAME_POKEMON);

										button = swkbdInputText(&swkbd, nick, NICKNAMELENGTH);
										nick[NICKNAMELENGTH - 1] = '\0';

										if (button != SWKBD_BUTTON_NONE)
											setNicknameZ(pkmn, nick, 0x40);
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 151 && touch.py < 163) {
										static SwkbdState swkbd;
										static char nick[NICKNAMELENGTH];
										memset(nick, 0, NICKNAMELENGTH);
										
										SwkbdButton button = SWKBD_BUTTON_NONE;
										swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, 12);
										swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_DIGITS | SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 2);
										swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);

										// Fix for swkKeyboard being in UTF8 only (char*)
										i18n_initTextSwkbd(&swkbd, S_EDITOR_TEXT_CANCEL, S_EDITOR_TEXT_SET, S_EDITOR_TEXT_ENTER_TRAINER_NAME);

										button = swkbdInputText(&swkbd, nick, NICKNAMELENGTH);
										nick[NICKNAMELENGTH - 1] = '\0';

										if (button != SWKBD_BUTTON_NONE)
											setNicknameZ(pkmn, nick, 0xb0);
									}
									
									if (touch.px > 206 && touch.px < 315 && touch.py > 172 && touch.py < 203) {
										pkx_set(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
										operationDone = true;
										break;
									}
									
									if (touch.px > 206 && touch.px < 317 && touch.py > 141 && touch.py < 172) {
										int *movesSorted = listMoves.sortedItemsID;
										int moveEntry = 0;
										int entryBottom = 0;
										int page = 0, maxpages = 18;

										while (aptMainLoop() && !(hidKeysDown() & KEY_B)) {
											hidScanInput();
											touchPosition touch;
											hidTouchRead(&touch);
											calcCurrentEntryMorePagesReversed(&moveEntry, &page, maxpages, 39, 20);

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

											printPKEditor(pkmn, game, moveEntry, page, entryBottom, ED_MOVES, descriptions);
										}
									}
									
									if (touch.px > 180 && touch.px < 195 && touch.py > 90 && touch.py < 102) {
										int *itemsSorted = listItems.sortedItemsID;
										int itemEntry = 0;
										int page = 0, maxpages = 23;
										
										while (aptMainLoop() && !(hidKeysDown() & KEY_B)) {
											hidScanInput();
											calcCurrentEntryMorePagesReversed(&itemEntry, &page, maxpages, 39, 20);
											
											if (hidKeysDown() & KEY_A) {
												setItemEditor(pkmn, itemsSorted[itemEntry + page * 40]);
												break;
											}
											
											printPKEditor(pkmn, game, itemEntry, page, 0, ED_ITEMS, descriptions);
										}
									}
								}
								
								if (hidKeysDown() & KEY_TOUCH) {
									if (touch.px > 180 && touch.px < 193 && touch.py > 29 && touch.py < 42)
										pkx_set_level(pkmn, (pkx_get_level(pkmn) < 100) ? pkx_get_level(pkmn) + 1 : 1);
									
									if (touch.px > 137 && touch.px < 150 && touch.py > 29 && touch.py < 42)
										pkx_set_level(pkmn, (pkx_get_level(pkmn) > 1) ? pkx_get_level(pkmn) - 1 : 100);
									
									if (touch.px > 137 && touch.px < 150 && touch.py > 189 && touch.py < 202) {
										if (pkx_is_egg(pkmn))
											setOTFriendship(pkmn, (getOTFriendship(pkmn) > 0) ? getOTFriendship(pkmn) - 1 : 255);
										else
											setFriendship(pkmn, (getFriendship(pkmn) > 0) ? getFriendship(pkmn) - 1 : 255);
									}

									if (touch.px > 180 && touch.px < 193 && touch.py > 189 && touch.py < 202) {
										if (pkx_is_egg(pkmn))
											setOTFriendship(pkmn, (getOTFriendship(pkmn) < 255) ? getOTFriendship(pkmn) + 1 : 0);
										else
											setFriendship(pkmn, (getFriendship(pkmn) < 255) ? getFriendship(pkmn) + 1 : 0);
									}
								}

								if (hidKeysHeld() & KEY_TOUCH) {
									if (touch.px > 180 && touch.px < 193 && touch.py > 29 && touch.py < 42) {
										if (speed > 30 && pkx_get_level(pkmn) < 100)
											pkx_set_level(pkmn, pkx_get_level(pkmn) + 1);
										else
											speed++;
									}
									else if (touch.px > 137 && touch.px < 150 && touch.py > 29 && touch.py < 42) {
										if (speed < -30 && pkx_get_level(pkmn) > 1)
											pkx_set_level(pkmn, pkx_get_level(pkmn) - 1);
										else
											speed--;
									}
									else if (touch.px > 137 && touch.px < 150 && touch.py > 189 && touch.py < 202) {
										if (pkx_is_egg(pkmn)) {
											if (speed < -30 && getOTFriendship(pkmn) > 0)
												setOTFriendship(pkmn, getOTFriendship(pkmn) - 1);
											else
												speed--;
										}
										else {
											if (speed < -30 && getFriendship(pkmn) > 0)
												setFriendship(pkmn, getFriendship(pkmn) - 1);
											else
												speed--;
										}
									}
									else if (touch.px > 180 && touch.px < 193 && touch.py > 189 && touch.py < 202) {
										if (pkx_is_egg(pkmn)) {
											if (speed > 30 && getOTFriendship(pkmn) < 255)
												setOTFriendship(pkmn, getOTFriendship(pkmn) + 1);
											else
												speed++;
										}
										else {
											if (speed > 30 && getFriendship(pkmn) < 255)
												setFriendship(pkmn, getFriendship(pkmn) + 1);
											else
												speed++;
										}
									}
									else
										speed = 0;
								}
								else
									speed = 0;
								
								printPKEditor(pkmn, game, 0, 0, 0, ED_BASE, descriptions);
							}
							break;
						}
						case 1 : {
							isTeam = false;
							int cloneEntry = currentEntry;
							while(aptMainLoop() && !operationDone && !(hidKeysDown() & KEY_B)) {
								hidScanInput();
								touchPosition touch;
								hidTouchRead(&touch);
								if (!isTeam)
									calcCurrentEntryMorePages(&cloneEntry, &box, boxmax + 1, 29, 6);

								if (hidKeysDown() & KEY_TOUCH) {
									if (touch.px > 210 && touch.px < 320 && touch.py > 0 && touch.py < 210) {
										cloneEntry = 0;
										isTeam = true;
									}
									if (touch.px > 0 && touch.px < 210 && touch.py > 0 && touch.py < 210) {
										cloneEntry = 0;
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
			
								if (hidKeysHeld() & KEY_TOUCH) {
									if (!isTeam) {
										int x_start, y_start = 45;
										for (int i = 0; i < 5; i++) {
											x_start = 4;
											for (int j = 0; j < 6; j++) {
												if ((touch.px > x_start) && (touch.px < (x_start + 34)) && (touch.py > y_start) && (touch.py < (y_start + 30)))
													cloneEntry = i * 6 + j;
												x_start += 34;
											}
											y_start += 30;
										}
									} else {
										if (touch.px > 214 && touch.px < 265 && touch.py > 40 && touch.py < 85) cloneEntry = 0;
										if (touch.px > 266 && touch.px < 317 && touch.py > 60 && touch.py < 105) cloneEntry = 1;
										if (touch.px > 214 && touch.px < 265 && touch.py > 85 && touch.py < 130) cloneEntry = 2;
										if (touch.px > 266 && touch.px < 317 && touch.py > 105 && touch.py < 150) cloneEntry = 3;
										if (touch.px > 214 && touch.px < 265 && touch.py > 130 && touch.py < 175) cloneEntry = 4;
										if (touch.px > 266 && touch.px < 317 && touch.py > 150 && touch.py < 195) cloneEntry = 5;
									}
								}
								if ((hidKeysDown() & KEY_A) && !isTeam) {
									pkx_set(mainbuf, box, cloneEntry, pkmn, game);
									operationDone = true;
									currentEntry = cloneEntry;
									break;
								}

								printPKViewer(mainbuf, pkmn, isTeam, game, cloneEntry, menuEntry, box, ED_CLONE, 0, 0);
							}
							break;
						}
						case 2 : {
							if (!isTeam && confirmDisp(i18n(S_EDITOR_Q_CONFIRM_RELEASE))) {
								memset(pkmn, 0, PKMNLENGTH);
								pkx_set(mainbuf, box, currentEntry, pkmn, game);
								infoDisp(i18n(S_EDITOR_RELEASED));
								operationDone = true;
							}
							break;
						}
						case 3 : {
							if (!isTeam) {
								int genEntry = ((int)pkx_get_species(pkmn) - 1) % 40;
								int page = ((int)pkx_get_species(pkmn) - 1) / 40, maxpages = ISGEN6 ? 18 : 21;
								
								while (aptMainLoop() && !(hidKeysDown() & KEY_B)) {
									hidScanInput();
									calcCurrentEntryMorePages(&genEntry, &page, maxpages, 39, 8);
									
									if (hidKeysDown() & KEY_A) {
										if (!(ISGEN6 && ((genEntry + 1) > 721)) && (page*40 + genEntry < 802)) {
											generate(mainbuf, game, isTeam, box, currentEntry, page, genEntry);
											operationDone = true;
										}
										break;
									}
									printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_GENERATE, genEntry, page);
								}
							}
							break;
						}
					}
				}
				
				if (((hidKeysDown() & KEY_A) || touchExecuting / 40 == 2) && menuEntry == 4) {
					touchExecuting = currentEntry;
					break;
				}
				if (operationDone || (hidKeysDown() & KEY_B)) {
					touchExecuting = currentEntry;
					break;
				}
			}
			if (!pkx_get_species(pkmn) && !isTeam && !operationDone) {
				int genEntry = 0;
				int page = 0, maxpages = ISGEN6 ? 18 : 21;
				
				while (aptMainLoop() && !(hidKeysDown() & KEY_B)) {
					hidScanInput();
					calcCurrentEntryMorePages(&genEntry, &page, maxpages, 39, 8);
					
					if (hidKeysDown() & KEY_A) {
						if (!(ISGEN6 && ((genEntry + 1) > 721)) && (page*40 + genEntry < 802)) {
							generate(mainbuf, game, isTeam, box, currentEntry, page, genEntry);
							operationDone = true;
						}
						break;
					}
					
					printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, ED_GENERATE, genEntry, page);
				}
			}
		}
		#endif
		printPKViewer(mainbuf, pkmn, isTeam, game, currentEntry, menuEntry, box, modeFlag, 0, 0);
	}
	free(pkmn);
}
