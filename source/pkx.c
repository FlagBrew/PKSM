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

const int lookupHT[] = {0, 1, 2, 5, 3, 4};

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

FormData *pkx_get_legal_form_data(const u16 species, const int game) {
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
		case GAME_US:
		case GAME_UM:
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
		case 735 : // gumshoos
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 738 : // vikavolt
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 741 :
			forms->spriteNum = 133;
			forms->stringNum = 126;
			forms->max = 3;
			break;
		case 743 : // ribombee
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 744 :
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 745 :
			forms->spriteNum = 136;
			forms->stringNum = 130;
			forms->max = 1;
			break;
		case 752 : // araquanid
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 754 : // lurantis
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 758 : // salazzle
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
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
		case 777 : // togedemaru
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 778 : // mimikyu
			forms->spriteNum = 0;
			forms->max = 2;
			forms->editable = false;
			break;
		case 784 : // kommo-o
			forms->spriteNum = 0;
			forms->max = 1;
			forms->editable = false;
			break;
		case 800 :
			forms->spriteNum = 0;
			forms->max = 4;
			forms->editable = false;
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

u32 pkx_seedstep(const u32 seed) { 
	return seed * 0x41C64E6D + 0x00006073; 
}

u32 pkx_lcrng(u32 seed) { 
	return seed * 0x41C64E6D + 0x00006073; 
}

u32 pkx_get_save_address(const int boxnumber, const int indexnumber) {
    int boxpos = 0;
	
    if (game_getisXY())
		boxpos = boxnumber < 33 ? 0x22600 : 0x14200;
	else if (game_getisORAS())
		boxpos = boxnumber < 33 ? 0x33000 : 0x14200;
	else if (game_getisSUMO())
		boxpos = boxnumber < 33 ? 0x04E00 : 0x01400;
	else if (game_getisUSUM())
		boxpos = boxnumber < 33 ? 0x05200 : 0x01600;

	if (boxnumber < 33)
		return boxpos + (ofs.pkmnLength * 30 * boxnumber) + (indexnumber * ofs.pkmnLength);

	return boxpos + indexnumber * 260;
}

void pkx_calculate_checksum(u8* data) {
    u16 chk = 0;

    for (int i = 8; i < ofs.pkmnLength; i += 2)
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

    char pkmncpy[ofs.pkmnLength];
    char tmp[BLOCKLENGHT];

    memcpy(&pkmncpy, pkmn, ofs.pkmnLength);

    for (int i = 0; i < 4; i++) {
        memcpy(tmp, pkmncpy + 8 + BLOCKLENGHT * ord[i], BLOCKLENGHT);
        memcpy(pkmn + 8 + BLOCKLENGHT * i, tmp, BLOCKLENGHT);
    }
}

void pkx_decrypt(u8* pkmn) {
    u32 encryptionkey = *(u32*)(pkmn);
    u32 seed = encryptionkey;

    u16 temp;
    for (int i = 0x08; i < ofs.pkmnLength; i += 2) {
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
    for(int i = 0x08; i < ofs.pkmnLength; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (pkx_seedstep(seed) >> 16);
        seed = pkx_seedstep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
}

void pkx_get(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn) {
    memcpy(pkmn, &mainbuf[pkx_get_save_address(boxnumber, indexnumber)], ofs.pkmnLength);
    pkx_decrypt(pkmn);
}

void pkx_set(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn) {
	if (PKSM_Configuration.editInTransfers != 0) {
		u8 latestHandlers[10];
		char ot_name[ofs.nicknameLength];
		char save_name[ofs.nicknameLength];
		char ht_name[ofs.nicknameLength];
		memset(ht_name, 0, ofs.nicknameLength);

		memcpy(latestHandlers, &pkmn[0x94], 10);
		memcpy(ot_name, &pkmn[0xB0], ofs.nicknameLength);
		memcpy(save_name, &mainbuf[ofs.saveOT], ofs.nicknameLength);
		
		if (!((getSaveTID(mainbuf) == pkx_get_tid(pkmn)) && (getSaveSID(mainbuf) == pkx_get_sid(pkmn)) && 
			 (getSaveGender(mainbuf) == pkx_get_ot_gender(pkmn)) && !memcmp(ot_name, save_name, ofs.nicknameLength)) &&
			 !(pkx_is_egg(pkmn))) { //you're the first owner
			pkx_set_ht(pkmn, save_name);
			pkx_set_ht_gender(pkmn, getSaveGender(mainbuf));
		}
	}

    pkx_calculate_checksum(pkmn);
    pkx_encrypt(pkmn);

    memcpy(&mainbuf[pkx_get_save_address(boxnumber, indexnumber)], pkmn, ofs.pkmnLength);
}

void pkx_set_as_it_is(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn) {
    pkx_calculate_checksum(pkmn);
    pkx_encrypt(pkmn);
	
    memcpy(&mainbuf[pkx_get_save_address(boxnumber, indexnumber)], pkmn, ofs.pkmnLength);
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
	return 15 * ((pkx_get_iv(pkmn, 0)& 1) 
		  + 2 * (pkx_get_iv(pkmn, 1) & 1) 
		  + 4 * (pkx_get_iv(pkmn, 2) & 1) 
		  + 8 * (pkx_get_iv(pkmn, 3) & 1) 
		  + 16 * (pkx_get_iv(pkmn, 4) & 1) 
		  + 32 * (pkx_get_iv(pkmn, 5) & 1)) / 63; 
}

u8 pkx_get_ot_gender(u8* pkmn) { 
	return ((*(u8*)(pkmn + 0xDD)) >> 7); 
}

bool pkx_is_egg(u8* pkmn) {
    u32 buf = *(u32*)(pkmn + 0x74);
    buf = buf >> 30;
    buf = buf & 0x1;
	return buf == 1 ? true : false;
}

void pkx_reroll_encryption_key(u8* pkmn) {
	srand(time(NULL));
	u32 encryptbuffer = rand();
	memcpy(&pkmn[0x0], &encryptbuffer, 4);
}

void pkx_reroll_pid(u8* pkmn) {
    srand(pkx_get_pid(pkmn));
    u32 pidbuffer = rand();
    memcpy(&pkmn[0x18], &pidbuffer, 4);
}

u32 pkx_get_pid(u8* pkmn) {
    return *(u32*)(pkmn + 0x18);
}

bool pkx_get_nickname_flag(u8* pkmn) {
	u8 buf = *(u8*)(pkmn + 0x77);
	buf = buf >> 7;
	buf = buf & 0x1;
	return buf == 1 ? true : false;
}

u16 pkx_get_egg_move(u8 *pkmn, const int nmove) { 
	return *(u16*)(pkmn + 0x6A + nmove*2);
}

bool pkx_get_pokerus (u8* pkmn) {
	u8 pkrs = *(u8*)(pkmn + 0x2B);
	pkrs = pkrs >> 4;
	return pkrs > 0;
}

u16 pkx_get_move(u8* pkmn, const int nmove) {
	return *(u16*)(pkmn + 0x5A + nmove*2);
}

u32 *pkx_get_ot(u8* pkmn, u32* dst) {
	u16 src[ofs.nicknameLength];
	memcpy(src, &pkmn[0xB0], ofs.nicknameLength);
	utf16_to_utf32(dst, src, ofs.nicknameLength);
	return dst;
}

u32 *pkx_get_nickname(u8* pkmn, u32* dst) {
	u16 src[ofs.nicknameLength];
	memcpy(src, &pkmn[0x40], ofs.nicknameLength);
	utf16_to_utf32(dst, src, ofs.nicknameLength);
	return dst;
}

u8 *pkx_get_nickname_u8(u8* pkmn, u8* dst) {
	u16 src[ofs.nicknameLength];
	for (int i = 0; i < ofs.nicknameLength; i++)
		src[i] = *(u16*)(pkmn + 0x40 + i*2);
	utf16_to_utf8(dst, src, ofs.nicknameLength*2);
	return dst;
}

u32 *pkx_get_ht(u8* pkmn, u32* dst) {
	u16 src[ofs.nicknameLength];
	memcpy(src, &pkmn[0x78], ofs.nicknameLength);
	utf16_to_utf32(dst, src, ofs.nicknameLength);
	return dst;
}

u16 pkx_get_tid(u8* pkmn) {
    return *(u16*)(pkmn + 0x0C);
}

u16 pkx_get_sid(u8* pkmn) {
    return *(u16*)(pkmn + 0x0E);
}

u16 pkx_get_tsv(u8* pkmn) {
	return (pkx_get_tid(pkmn) ^ pkx_get_sid(pkmn)) >> 4;
}

u16 pkx_get_psv(u8* pkmn) {
	u32 pid = pkx_get_pid(pkmn);
	return ((pid >> 16) ^ (pid & 0xFFFF)) >> 4;
}

u16 pkx_get_species(u8* pkmn) {
    return *(u16*)(pkmn + 0x08);
}

u8 pkx_get_level(u8* pkmn) {
	u32 exp = *(u32*)(pkmn + 0x10);
	u8 xpType = personal.pkmData[pkx_get_species(pkmn)][0x08];
	
	u8 iterLevel = 1;
	while (iterLevel < 100 && exp >= expTable[iterLevel][xpType]) 
		iterLevel++;
	return iterLevel;
}

void pkx_set_level(u8* pkmn, const int lv) {
    u32 towrite = expTable[lv - 1][personal.pkmData[pkx_get_species(pkmn)][0x08]];
    memcpy(&pkmn[0x10], &towrite, 4);
}

bool pkx_is_shiny(u8* pkmn) {
    u16 trainersv = (pkx_get_tid(pkmn) ^ pkx_get_sid(pkmn)) >> 4;
    u16 pkmnv = ((pkx_get_pid(pkmn) >> 16) ^ (pkx_get_pid(pkmn) & 0xFFFF)) >> 4;

    return trainersv == pkmnv;
}

u16 pkx_get_form_species_number(u8 *pkmn) {	
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

u16 pkx_get_stat(u8* pkmn, const int stat) {
    u16 tempspecies = pkx_get_form_species_number(pkmn);

    u8 mult = 10, basestat = 0;
    u16 final;
    if (stat == 0) basestat = personal.pkmData[tempspecies][0x0];
    if (stat == 1) basestat = personal.pkmData[tempspecies][0x1];
    if (stat == 2) basestat = personal.pkmData[tempspecies][0x2];
    if (stat == 3) basestat = personal.pkmData[tempspecies][0x3];
    if (stat == 4) basestat = personal.pkmData[tempspecies][0x4];
    if (stat == 5) basestat = personal.pkmData[tempspecies][0x5];
    
    if (stat == 0)
        final = 10 + ((2 * basestat) + ((((pkx_get_HT(pkmn) >> lookupHT[stat]) & 1) == 1) ? 31 : pkx_get_iv(pkmn, stat)) + pkx_get_ev(pkmn, stat) / 4 + 100) * pkx_get_level(pkmn) / 100;
    else
        final = 5 + (2 * basestat + ((((pkx_get_HT(pkmn) >> lookupHT[stat]) & 1) == 1) ? 31 : pkx_get_iv(pkmn, stat)) + pkx_get_ev(pkmn, stat) / 4) * pkx_get_level(pkmn) / 100; 
    
    if (pkx_get_nature(pkmn) / 5 + 1 == stat)
        mult++;
    if (pkx_get_nature(pkmn) % 5 + 1 == stat)
        mult--;
  
    final = final * mult / 10;
    return final;
}

u8 pkx_get_friendship(u8* pkmn) {
	return (pkmn[0x93] == 0) ? pkx_get_ot_friendship(pkmn) : pkx_get_ht_friendship(pkmn);
}

u8 pkx_get_ht_friendship(u8* pkmn) {
	return *(u8*)(pkmn + 0xA2);
}

u8 pkx_get_ot_friendship(u8* pkmn) {
	return *(u8*)(pkmn + 0xCA);
}

u8 pkx_get_nature(u8* pkmn) {
	return *(u8*)(pkmn + 0x1C);
}

u8 pkx_get_ev(u8* pkmn, const int stat) {
	return *(u8*)(pkmn + 0x1E + stat);
}

u8 pkx_get_iv(u8* pkmn, const int stat) {
    u32 buffer = *(u32*)(pkmn + 0x74);
    buffer = buffer >> 5 * stat;
    buffer = buffer & 0x1F;
	
	u8 toreturn;
	memcpy(&toreturn, &buffer, 1);
	return toreturn;
}

u8 pkx_get_ball(u8* pkmn) {
	return *(u8*)(pkmn + 0xDC);
}

void pkx_set_item(u8* pkmn, const u16 item) {
    memcpy(&pkmn[0x0A], &item, 2);
}

void pkx_set_gender(u8* pkmn, const u8 val) { 
	pkmn[0x1D] = (u8)((pkmn[0x1D] & ~0x06) | (val << 1)); 
}

void pkx_set_form(u8* pkmn, const u8 val) { 
	pkmn[0x1D] = (u8)((pkmn[0x1D] & 0x07) | (val << 3)); 
}

void pkx_set_ball(u8* pkmn, const u8 val) { 
	pkmn[0xDC] = val; 
}

void pkx_set_ot_gender(u8* pkmn, const u8 val) { 
	pkx_set_flag(pkmn, 0xDD, 7, (val == 1) ? true : false); 
}

void pkx_set_tid(u8* pkmn, const u16 tid) {
    memcpy(&pkmn[0x0C], &tid, 2);
}

void pkx_set_sid(u8* pkmn, const u16 sid) {
    memcpy(&pkmn[0x0E], &sid, 2);
}

void pkx_set_ability(u8* pkmn, const u8 ability) {
    u16 tempspecies = pkx_get_form_species_number(pkmn);
	u8 abilitynum = 0;
	
	if (ability == 0)      abilitynum = 1;
	else if (ability == 1) abilitynum = 2;
	else                   abilitynum = 4;
	
	memcpy(&pkmn[0x15], &abilitynum, 1);
	memcpy(&pkmn[0x14], &personal.pkmData[tempspecies][0x09 + ability], 1);
}

void pkx_set_move(u8* pkmn, const u16 move, const int nmove) {
    memcpy(&pkmn[0x5A + (2 * nmove)], &move, 2);
}

void pkx_set_egg_move(u8* pkmn, const u16 move, const int nmove) {
    memcpy(&pkmn[0x6A + (2 * nmove)], &move, 2);
}

void pkx_set_ht(u8* pkmn, char* nick) {
    memcpy(&pkmn[0x78], nick, NICKNAMELENGTH);
}

void pkx_set_ht_gender(u8* pkmn, const u8 gender) {
	memcpy(&pkmn[0x92], &gender, 1);
}

void pkx_set_nature(u8* pkmn, const u8 nature) {
    memcpy(&pkmn[0x1C], &nature, 1);
}

void pkx_set_ht_friendship(u8* pkmn, const int val) {
	memcpy(&pkmn[0xA2], &val, 1);
}

void pkx_set_ot_friendship(u8* pkmn, const int val) {
	memcpy(&pkmn[0xCA], &val, 1);
}

void pkx_set_friendship(u8* pkmn, const int val) {
	if (pkmn[0x93] == 0)
		pkx_set_ot_friendship(pkmn, val);
	else
		pkx_set_ht_friendship(pkmn, val);
}

void pkx_set_ev(u8* pkmn, u8 val, const int stat) {
    memcpy(&pkmn[0x1E + stat], &val, 1);
}

void pkx_set_iv(u8* pkmn, u8 val, const int stat) {
	u32 nval = val;
	u32 mask = 0xFFFFFFFF;
	mask ^= 0x1F << (5 * stat);

	u32 buffer;
	memcpy(&buffer, &pkmn[0x74], 4);

	buffer &= mask;
	buffer ^= ((nval & 0x1F) << (5 * stat));
	memcpy(&pkmn[0x74], &buffer, 4);
}

void pkx_set_hp_type(u8* pkmn, const int val) {
    u8 ivstat[6];
    for (int i = 0; i < 6; i++)
        ivstat[i] = pkx_get_iv(pkmn, i);

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
        pkx_set_iv(pkmn, ivstat[i], i);
}

void pkx_set_shiny(u8* pkmn, const bool shiny) {
	if (!shiny)
		pkx_reroll_pid(pkmn);
	else {
		u16 tsv = (pkx_get_tid(pkmn) ^ pkx_get_sid(pkmn)) >> 4;
		u16 buffer = (pkx_get_pid(pkmn) >> 16) ^ (tsv << 4);
		memcpy(&pkmn[0x18], &buffer, 2);
	}
}

void pkx_set_nickname_flag(u8* pkmn) {
	u8 isnicknamed;
	memcpy(&isnicknamed, &pkmn[0x77], 1);
	isnicknamed |= 0x80;
	memcpy(&pkmn[0x77], &isnicknamed, 1);
}

void pkx_set_nickname(u8* pkmn, char* nick, const int dst) {
	// dst 0x40(Nickname) 0xB0(OT) 0x78(HT)
	u8 toinsert[ofs.nicknameLength];
	memset(toinsert, 0, ofs.nicknameLength);

	if (!memcmp(nick, toinsert, ofs.nicknameLength))
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
		
		w += 2;
		if (w > ofs.nicknameLength)
			break;
	}

	if (dst == 0x40 && !isGenerating()) {
		pkx_set_nickname_flag(pkmn);
	}

	memcpy(&pkmn[dst], toinsert, ofs.nicknameLength);
}

void pkx_set_ribbons(u8* pkmn, const int ribcat, const int ribnumber, const bool value) {
	u8 tmp = *(u8*)(pkmn + 0x30 + ribcat);
	tmp = (u8)((tmp & ~(1 << ribnumber)) | (value ? 1 << ribnumber : 0));
	memcpy(&pkmn[0x30 + ribcat], &tmp, 1);
}

bool pkx_get_ribbons(u8* pkmn, const int ribcat, const int ribnumber) {
	return (pkmn[0x30 + ribcat] & (1 << ribnumber)) == 1 << ribnumber;
}

void pkx_set_hti(u8* pkmn, const int htnumber, const bool value) {
	// HyperTrains htnumber HP,ATK,SPATK,SPDEF,SPEED
	u8 tmp = *(u8*)(pkmn + 0xDE);
	tmp = (u8)((tmp & ~(1 << htnumber)) | (value ? 1 << htnumber : 0));
	memcpy(&pkmn[0xDE], &tmp, 1);
}

bool pkx_get_hti(u8* pkmn, const int htnumber) {
	return (pkmn[0xDE] & (1 << htnumber)) == 1 << htnumber;
}

void pkx_set_flag(u8* pkmn, const int flgaddr, const int flgshift, const bool value) {
	if (flgaddr < 0 || PKMNLENGTH <= flgaddr || flgshift < 0 || 8 <= flgshift) 
		return;
	
	u8 tmp = *(u8*)(pkmn + flgaddr);
	tmp = (u8)((tmp & ~(1 << flgshift)) | (value ? 1 << flgshift : 0));
	memcpy(&pkmn[flgaddr], &tmp, 1);
}

void pkx_set_pokerus(u8* pkmn) {
	*(pkmn + 0x2B) = 0x11;
}

u8 pkx_get_version(u8* pkmn) {
	return *(u8*)(pkmn + 0xDF);
}