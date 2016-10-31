/*
* This file is part of EventAssistant
* Copyright (C) 2016 Bernardo Giordano
*
* Credits to SlashCash & PCHex++ for lots of functions redistributed in this software.
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
#include <string.h>
#include "graphic.h"
#include "editor.h"
#include "util.h"

#define BALLS 25
#define ITEM 18
#define HEAL 26
#define BERRIES 64

/* ************************ local variables ************************ */

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
const int FRIENDSHIPPOS = 0xA2;
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
 
u32 langValues[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08};
u32 balls[BALLS * 2] = {0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0X0E, 0X00, 0x0F, 0x00, 0x10, 0x00, 0xEC, 0x01, 0xED, 0x01, 0xEE, 0x01, 0xEF, 0x01, 0xF0, 0x01, 0xF2, 0x01, 0xF3, 0x01, 0xF4, 0x01};
u32 items[ITEM * 2] = {0xDB, 0x00, 0xDC, 0x00, 0xDD, 0x00, 0xDF, 0x00, 0xEA, 0x00, 0x0E, 0x01, 0x0F, 0x01, 0x10, 0x01, 0x11, 0x01, 0x13, 0x01, 0x18, 0x01, 0x1F, 0x01, 0x29, 0x01, 0x1A, 0x02, 0x1C, 0x02, 0x1D, 0x02, 0x7F, 0x02, 0x80, 0x02};
u32 heal[HEAL * 2] = {0x17, 0x00, 0x18, 0x00, 0x1C, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2C, 0x00, 0x2D, 0x00, 0x2E, 0x00, 0x2F, 0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x35, 0x02, 0x36, 0x02, 0x37, 0x02, 0x38, 0x02, 0x39, 0x02, 0x3A, 0x02, 0x3B, 0x02, 0x85, 0x02};

/* ************************ utilities ************************ */

int getActiveGBO(u8* mainbuf, int game) {
	int ofs = 0;
	int generalBlock = -1;
	
	int k = 0;
	u8 temp[10];
	
	memcpy(&temp, mainbuf, 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 1;
	
	k = 0;
	memcpy(&temp, &mainbuf[0x40000], 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 1;
	
	if (game == GAME_HG || game == GAME_SS)
		ofs = 0xF618;
	else if (game == GAME_PLATINUM) 
		ofs = 0xCF1C;
	else if (game == GAME_DIAMOND || game == GAME_PEARL)
		ofs = 0xC0F0;
	
	u16 c1;
	u16 c2;
	memcpy(&c1, &mainbuf[ofs], 2);
	memcpy(&c2, &mainbuf[ofs + 0x40000], 2);
	
	generalBlock = (c1 >= c2) ? 0 : 1;
	
	return generalBlock;
}

int getActiveSBO(u8* mainbuf, int game) {
	int ofs = 0;
	int storageBlock = -1;
	
	if (game == GAME_HG || game == GAME_SS)
		ofs = 0x21A00;
	else if (game == GAME_PLATINUM) 
		ofs = 0x1F100;
	else if (game == GAME_DIAMOND || game == GAME_PEARL)
		ofs = 0x1E2D0;
	
	int k = 0;
	u8 temp[10];
	
	memcpy(&temp, &mainbuf[ofs], 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 1;
	
	k = 0;
	memcpy(&temp, &mainbuf[ofs + 0x40000], 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 0;
	
	u16 c1;
	u16 c2;
	memcpy(&c1, &mainbuf[ofs], 2);
	memcpy(&c2, &mainbuf[ofs + 0x40000], 2);
	
	storageBlock = (c1 >= c2) ? 0 : 1;
	
	return storageBlock;
}

u32 CHKOffset(u32 i, int game) {
	if (game == GAME_X || game == GAME_Y) {
		const u32 _xy[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1AC00, 0x1B400, 0x1B600, 0x1B800, 0x1BE00, 0x1C000, 0x1C400, 0x1CC00, 0x1CE00, 0x1D000, 0x1D200, 0x1D400, 0x1D600, 0x1DE00, 0x1E400, 0x1E800, 0x20400, 0x20600, 0x20800, 0x20C00, 0x21000, 0x22C00, 0x23000, 0x23800, 0x23C00, 0x24600, 0x24A00, 0x25200, 0x26000, 0x26200, 0x26400, 0x27200, 0x27A00, 0x5C600, };
		return _xy[i] - 0x5400;
	}
	else if (game == GAME_OR || game == GAME_AS) {
		const u32 _oras[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1B600, 0x1BE00, 0x1C000, 0x1C200, 0x1C800, 0x1CA00, 0x1CE00, 0x1D600, 0x1D800, 0x1DA00, 0x1DC00, 0x1DE00, 0x1E000, 0x1E800, 0x1EE00, 0x1F200, 0x20E00, 0x21000, 0x21400, 0x21800, 0x22000, 0x23C00, 0x24000, 0x24800, 0x24C00, 0x25600, 0x25A00, 0x26200, 0x27000, 0x27200, 0x27400, 0x28200, 0x28A00, 0x28E00, 0x30A00, 0x38400, 0x6D000, };
		return _oras[i] - 0x5400;
	}
	else if (game == GAME_B1 || game == GAME_W1) {
		const u32 _bw[] = { 0x1C800, 0x23F00 };
		return _bw[i];
	}
	else if (game == GAME_B2 || game == GAME_W2) {
		const u32 _b2w2[] = { 0x1C800, 0x25F00 }; 
		return _b2w2[i];
	}
	else return 0;
}

u32 CHKLength(u32 i, int game) {
	if (game == GAME_X || game == GAME_Y) {
		const u32 _xy[] = { 0x002C8, 0x00B88, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 0x001C0, 0x000BE, 0x00024, 0x02100, 0x00140, 0x00440, 0x00574, 0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x006A0, 0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 0x002F8, 0x01B40, 0x001F4, 0x001F0, 0x00216, 0x00390, 0x01A90, 0x00308, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 0x00078, 0x00200, 0x00C84, 0x00628, 0x34AD0, 0x0E058, };
		return _xy[i];
	}
	else if (game == GAME_OR || game == GAME_AS) {
		const u32 _oras[] = { 0x002C8, 0x00B90, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 0x001C0, 0x000BE, 0x00024, 0x02100, 0x00130, 0x00440, 0x00574, 0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x011CC, 0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 0x002F8, 0x01B40, 0x001F4, 0x003E0, 0x00216, 0x00640, 0x01A90, 0x00400, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 0x00078, 0x00200, 0x00C84, 0x00628, 0x00400, 0x07AD0, 0x078B0, 0x34AD0, 0x0E058, };
		return _oras[i];
	}
	else if (game == GAME_W1 || game == GAME_B1) {
		const u32 _bw[] = { 0x0A94, 0x008C };
		return _bw[i];
	}
	else if (game == GAME_W2 || game == GAME_B2) {
		const u32 _b2w2[] = { 0x0A94, 0x0094 };
		return _b2w2[i];
	}
	else return 0;
}

u32 BWCHKOff(u32 i, int game) {
	if (game == GAME_B1 || game == GAME_W1) {
		const u32 _bw[] = { 0x1D296, 0x23F9A };
		return _bw[i];
	}
	else if (game == GAME_B2 || game == GAME_W2) {
		const u32 _b2w2[]= { 0x1D296,  0x25FA2 };
		return _b2w2[i];
	}
	else return 0;
}

u32 BWCHKMirr(u32 i, int game) {
	if (game == GAME_B1 || game == GAME_W1) {
		const u32 _bw[] = { 0x23F44, 0x23F9A };
		return _bw[i];
	}
	
	else if (game == GAME_W2 || game == GAME_B2) {
		const u32 _b2w2[] = { 0x25F44, 0x25FA2 };
		return _b2w2[i];
	}

	else return 0;	
}

u16 ccitt16(u8* data, u32 len) {
	u16 crc = 0xFFFF;

	for (u32 i = 0; i < len; i++) {
		crc ^= (u16) (data[i] << 8);

		for (u32 j = 0; j < 0x8; j++) {
			if ((crc & 0x8000) > 0)
				crc = (u16) ((crc << 1) ^ 0x1021);
			else
				crc <<= 1;
		}
	}

	return crc;
}

void rewriteCHK(u8 *mainbuf, int game) {
	u8 blockCount = 0;
	u8* tmp = (u8*)malloc(0x35000 * sizeof(u8));
	u16 cs;
	u32 csoff = 0;
	
	if (game == GAME_X || game == GAME_Y) {
		blockCount = 55;
		csoff = 0x6A81A - 0x5400;
	}

	else if (game == GAME_OR || game == GAME_AS) {
		blockCount = 58;
		csoff = 0x7B21A - 0x5400;
	}

	if (game == GAME_X || game == GAME_Y || game == GAME_OR || game == GAME_AS)
		for (u32 i = 0; i < blockCount; i++) {
			memcpy(tmp, mainbuf + CHKOffset(i, game), CHKLength(i, game));
			cs = ccitt16(tmp, CHKLength(i, game));
			memcpy(mainbuf + csoff + i * 8, &cs, 2);
		}

	else if (game == GAME_B1 || game == GAME_W1 || game == GAME_B2 || game == GAME_W2) {
		blockCount = 2;
		for (u32 i = 0; i < blockCount; i++) {
			memcpy(tmp, mainbuf + CHKOffset(i, game), CHKLength(i, game));
			cs = ccitt16(tmp, CHKLength(i, game));
			memcpy(mainbuf + BWCHKOff(i, game), &cs, 2);
			memcpy(mainbuf + BWCHKMirr(i, game), &cs, 2);
		}
	}

	free(tmp);
}

void rewriteCHK4(u8 *mainbuf, int game, int GBO, int SBO) {
	u8* tmp = (u8*)malloc(0x35000 * sizeof(u8));
	u16 cs;

	if (game == GAME_HG || game == GAME_SS) {
		memcpy(tmp, mainbuf + GBO, 0xF618);
		cs = ccitt16(tmp, 0xF618);
		memcpy(mainbuf + GBO + 0xF626, &cs, 2);

		memcpy(tmp, mainbuf + SBO + 0xF700, 0x12300);
		cs = ccitt16(tmp, 0x12300);
		memcpy(mainbuf + SBO + 0x21A0E, &cs, 2);
	}
	else if (game == GAME_PLATINUM) {
		memcpy(tmp, mainbuf + GBO, 0xCF18);
		cs = ccitt16(tmp, 0xCF18);
		memcpy(mainbuf + GBO + 0xCF2A, &cs, 2);

		memcpy(tmp, mainbuf + SBO + 0xCF2C, 0x121D0);
		cs = ccitt16(tmp, 0x121D0);
		memcpy(mainbuf + SBO + 0x1F10E, &cs, 2);		
	}
	else if (game == GAME_DIAMOND || game == GAME_PEARL) {
		memcpy(tmp, mainbuf + GBO, 0xC0EC);
		cs = ccitt16(tmp, 0xC0EC);
		memcpy(mainbuf + GBO + 0xC0FE, &cs, 2);

		memcpy(tmp, mainbuf + SBO + 0xC100, 0x121CC);
		cs = ccitt16(tmp, 0x121CC);
		memcpy(mainbuf + SBO + 0x1E2DE, &cs, 2);			
	}
	free(tmp);
}

u32 seedStep(const u32 seed) {
    return (seed * 0x41C64E6D + 0x00006073) & 0xFFFFFFFF;
}

u32 LCRNG(u32 seed) {
	return seed * 0x41C64E6D + 0x00006073;
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
    if (game == GAME_X || game == GAME_Y) 
		boxpos = 0x27A00 - OFFSET;
   
    if (game == GAME_OR || game == GAME_AS) 
		boxpos = 0x38400 - OFFSET;

    const int PKMNNUM = 30;
    
	return boxpos + (PKMNLENGTH * PKMNNUM * boxnumber) + (indexnumber * PKMNLENGTH);
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

void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    memcpy(pkmn, &mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], PKMNLENGTH);
    decryptPkmn(pkmn);
}

void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    calculatePKMNChecksum(pkmn);
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

void findFreeLocationWC(u8 *mainbuf, int game, int nInjected[]) {
	nInjected[0] = 0;
	int temp;
	
	if (game == GAME_X || game == GAME_Y) {
		for (int t = 0; t < 24; t++) {
			temp = 0;
			for (int j = 0; j < WC6LENGTH; j++)
				if (*(mainbuf + XYWC6POS + t * WC6LENGTH + j) == 0x00)
					temp++;
				
			if (temp == WC6LENGTH) {
				nInjected[0] = t;
				break;
			}
		}
	} else if (game == GAME_OR || game == GAME_AS) {
		for (int t = 0; t < 24; t++) {
			temp = 0;
			for (int j = 0; j < WC6LENGTH; j++)
				if (*(mainbuf + ORASWC6POS + t * WC6LENGTH + j) == 0x00)
					temp++;
				
			if (temp == WC6LENGTH) {
				nInjected[0] = t;
				break;
			}
		}
	}
}

/* ************************ get ************************ */

u8 getForm(u8* pkmn) {
    u8 bufferform;
    memcpy(&bufferform, &pkmn[GENDERPOS], GENDERLENGTH);
    bufferform = (bufferform >> 3);
    return bufferform;
}

u16 getStat(u8* pkmn, const int stat) {
    u16 tempspecies = getPokedexNumber(pkmn);
    if (getForm(pkmn))
        tempspecies = personal.pkmData[getPokedexNumber(pkmn) + getForm(pkmn) - 1][0x20];
    
    u8 mult = 10;
    u16 final;
    u8 basestat = 1;
    if (stat == 0) basestat = personal.pkmData[tempspecies][0x0];
    if (stat == 1) basestat = personal.pkmData[tempspecies][0x1];
    if (stat == 2) basestat = personal.pkmData[tempspecies][0x2];
    if (stat == 3) basestat = personal.pkmData[tempspecies][0x3];
    if (stat == 4) basestat = personal.pkmData[tempspecies][0x4];
    if (stat == 5) basestat = personal.pkmData[tempspecies][0x5];
    
    if (stat == 0)
        final = 10 + ((2 * basestat) + getIV(pkmn, stat) + getEV(pkmn, stat) / 4 + 100) * getLevel(pkmn) / 100;
    else
        final = 5 + (2 * basestat + getIV(pkmn, stat) + getEV(pkmn, stat) / 4) * getLevel(pkmn) / 100; 
    
    if (getNature(pkmn) / 5 + 1 == stat)
        mult++;
    if (getNature(pkmn) % 5 + 1 == stat)
        mult--;
  
    final = final * mult / 10;
    return final;
}

u8 getAbility(u8* pkmn) {
    u8 abilitybuffer;
    memcpy(&abilitybuffer, &pkmn[ABILITYNUMPOS], ABILITYNUMLENGTH);
    
    u8 resultset[3] = {0, 0, 0};
	resultset[0] = personal.pkmData[getPokedexNumber(pkmn)][0x18];
	resultset[1] = personal.pkmData[getPokedexNumber(pkmn)][0x19];
	resultset[0] = personal.pkmData[getPokedexNumber(pkmn)][0x1a];
	
    if (abilitybuffer == 1)
        abilitybuffer = 0;
    
    if (abilitybuffer == 2)
        abilitybuffer = 1;
    
    if (abilitybuffer == 4)
        abilitybuffer = 2;
    
    return resultset[abilitybuffer];
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

	while (iterLevel < 100 && exp >= expTable[iterLevel][xpType])
		iterLevel++;

	return iterLevel;
}

u16 getMove(u8* pkmn, int nmove) {
    u16 movebuffer[4];
    memcpy(&movebuffer, &pkmn[MOVEPOS], MOVELENGTH*4);
	
    return movebuffer[nmove];
}

u16 getItem(u8* pkmn) {
    u16 itembuffer;
    memcpy(&itembuffer, &pkmn[ITEMPOS], ITEMLENGTH);
    
    return itembuffer;
}

u8 getHPType(u8* pkmn) {
    return 15 * ((getIV(pkmn, 0)& 1) + 2 * (getIV(pkmn, 1) & 1) + 4 * (getIV(pkmn, 2) & 1) + 8 * (getIV(pkmn, 3) & 1) + 16 * (getIV(pkmn, 4) & 1) + 32 * (getIV(pkmn, 5) & 1)) / 63;
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
    memcpy(&friendship, &pkmn[FRIENDSHIPPOS], 1);
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
    buffer = buffer >> 5*stat;
    buffer = buffer & 0x1F;
    memcpy(&toreturn, &buffer, 1);
    
    return toreturn;
}

bool getPokerus(u8* pkmn) {
	u8 pkrs;
	memcpy(&pkrs, &pkmn[0x2B], 1);
	
	return pkrs;
}

/* ************************ set ************************ */

void setNickname(u8* pkmn, char* nick) {
    u8 toinsert[NICKNAMELENGTH];
    for (int i = 0; i < NICKNAMELENGTH; i++)
        toinsert[i] = 0;
    
    for (u16 i = 0, nicklen = strlen(nick); i < nicklen; i++)
        toinsert[i * 2] = *(nick + i);
    
    memcpy(&pkmn[NICKNAMEPOS], toinsert, NICKNAMELENGTH);
}

void setNature(u8* pkmn, const u8 nature) {
    memcpy(&pkmn[NATUREPOS], &nature, NATURELENGTH);
}

void setFriendship(u8* pkmn, const int val) {
	memcpy(&pkmn[FRIENDSHIPPOS], &val, 1);
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
	if (nInjected[0] >= 24)
		nInjected[0] = 0;
}

void setWC4(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[], int GBO) {	
		// I'm not using these (anymore) for now. They represent the correct way to handle wondercard injection,
		// but they work only the first time. Needing to find a way to fix this.
		
	if (game == GAME_HG || game == GAME_SS) {
		// *(mainbuf + HGSSPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		// memcpy((void*)(mainbuf + HGSSPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		// Weird but this works. Writing wondercard to both blocks will cause the game to find a corrupted save,
		// but it restores normally it and shows the correct wondercards, without any save loss.
		
		*(mainbuf + HGSSPGTFLAGPOS + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + HGSSPGTPOS + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + HGSSPGTFLAGPOS + 0x40000 + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + HGSSPGTPOS + 0x40000 + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}
	if (game == GAME_PLATINUM) {
		// *(mainbuf + PTPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		// memcpy((void*)(mainbuf + PTPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + PTPGTFLAGPOS + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + PTPGTPOS + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + PTPGTFLAGPOS + 0x40000 + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + PTPGTPOS + 0x40000 + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}
	if (game == GAME_DIAMOND || game == GAME_PEARL) {
		// *(mainbuf + DPPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		// memcpy((void*)(mainbuf + DPPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + DPPGTFLAGPOS + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + DPPGTPOS + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + DPPGTFLAGPOS + 0x40000 + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + DPPGTPOS + 0x40000 + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}

	nInjected[0] += 1;
}

void setLanguage(u8* mainbuf, int i) {
	*(mainbuf + LANGUAGEPOS) = langValues[i];
}

void setMoney(u8* mainbuf, u64 i) {
	switch (i) {
		case 0 : {
			*(mainbuf + MONEYPOS)     = 0x00;
			*(mainbuf + MONEYPOS + 1) = 0x00;
			*(mainbuf + MONEYPOS + 2) = 0x00;
			break;			
		}	
		case 200000 : {
			*(mainbuf + MONEYPOS)     = 0x40;
			*(mainbuf + MONEYPOS + 1) = 0x0D;
			*(mainbuf + MONEYPOS + 2) = 0x03;
			break;			
		}	
		case 1000000 : {
			*(mainbuf + MONEYPOS)     = 0x40;
			*(mainbuf + MONEYPOS + 1) = 0x42;
			*(mainbuf + MONEYPOS + 2) = 0x0F;
			break;			
		}
		case 9999999 : {
			*(mainbuf + MONEYPOS)     = 0x7F;
			*(mainbuf + MONEYPOS + 1) = 0x96;
			*(mainbuf + MONEYPOS + 2) = 0x98;
			break;
		}
	}
}

void setItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[], int game) {
	u32 offset[3] = {0x400, 0xD68, 0xE68};
	if (game == GAME_OR || game == GAME_AS) {
		offset[1] = 0xD70;
		offset[2] = 0xE70;
	}

	if (i % 2 == 0) {
		*(mainbuf + offset[type] + nInjected[type] * 4) = values[i];
		*(mainbuf + offset[type + 1] + nInjected[type] * 4) = values[i + 1];
		*(mainbuf + offset[type] + 2 + nInjected[type] * 4) = 0x01;
		*(mainbuf + offset[type] + 3 + nInjected[type] * 4) = 0x00;		
	}
	else {
		*(mainbuf + offset[type] + nInjected[type] * 4) = values[i - 1];
		*(mainbuf + offset[type] + 1 + nInjected[type] * 4) = values[i];
		*(mainbuf + offset[type] + 2 + nInjected[type] * 4) = 0xE3;
		*(mainbuf + offset[type] + 3 + nInjected[type] * 4) = 0x03;	
	}
	nInjected[type]++;
}

void setBP(u8* mainbuf, int i, int game) {
	const u32 offset[] = {0x423C, 0x423D, 0x4230, 0x4231};
	int type = 0;
	
	if (game == GAME_OR || game == GAME_AS) 
		type = 2;
	
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

void setBadges(u8* mainbuf, int i) {
	const u32 value[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
	
	*(mainbuf + BADGEPOS) = value[i];
}

void setTM(u8* mainbuf, int game) {
	const u32 values[] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0x6A, 0x6B, 0x6C, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9};

	if (game == GAME_X || game == GAME_Y) 
		for (int i = 0; i < 105; i++) {
			*(mainbuf + TMSTARTPOS + i * 4) = values[i];
			*(mainbuf + TMSTARTPOS + i * 4 + 1) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 2) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 3) = 0x00;
		}
	
	if (game == GAME_OR || game == GAME_AS) {
		for (int i = 0; i < 106; i++) {
			*(mainbuf + TMSTARTPOS + i * 4) = values[i];
			*(mainbuf + TMSTARTPOS + i * 4 + 1) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 2) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 3) = 0x00;
		}
		*(mainbuf + TMSTARTPOS + 4 * 106)     = 0xE1;
		*(mainbuf + TMSTARTPOS + 4 * 106 + 1) = 0x02;
		*(mainbuf + TMSTARTPOS + 4 * 106 + 2) = 0x01;
		*(mainbuf + TMSTARTPOS + 4 * 106 + 3) = 0x00;
	}
	
	for (int i = 0; i < 8; i++) {
		*(mainbuf + 0xD31 + i * 4) = 0x02;
	}
}

void saveFileEditor(u8* mainbuf, int game) {
	u32 berry[BERRIES * 2];
	for (int i = 0; i < BERRIES * 2 ; i += 2) {
		berry[i] = 0x95 + i / 2;
		berry[i + 1] = 0x00;
	}
	
	int nInjected[3] = {0, 0, 0};
	int currentEntry = 0;
	int langCont = 0;
	int badgeCont = 0;

	while(aptMainLoop()) {
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)
			break;
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 9;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry < 9) currentEntry++;
			else if (currentEntry == 9) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
			if (currentEntry >= 5) currentEntry -= 5;
		}
		
		if (hidKeysDown() & KEY_DRIGHT) {
			if (currentEntry <= 4) currentEntry += 5;
		}
		
		if (hidKeysDown() & KEY_A) {
			switch (currentEntry) {
				case 0 : {
					if (langCont < 6) langCont++;
					else if (langCont == 6) langCont = 0;
					break;
				}
				case 6 : {
					if (badgeCont < 8) badgeCont++;
					else if (badgeCont == 8) badgeCont = 0;
					break;
				}
			}
		}
		
		if (hidKeysDown() & KEY_START) {
			switch (currentEntry) {
				case 0 : {
					setLanguage(mainbuf, langCont);
					infoDisp("Language set successfully!");
					break;
				}
				case 1 : {
					setBP(mainbuf, 9999, game);
					infoDisp("Battle Points set successfully!");
					break;
				}
				case 2 : {
					nInjected[0] = 0;
					for (int i = 1; i < BALLS * 2; i += 2)
						setItem(mainbuf, i, balls, 0, nInjected, game);
					infoDisp("Balls injected successfully!");
					break;
				}
				case 3 : {
					nInjected[1] = 0;
					for (int i = 1; i < HEAL * 2; i += 2)
						setItem(mainbuf, i, heal, 1, nInjected, game);
					infoDisp("Heals injected successfully!");
					break;
				}
				case 4 : {
					nInjected[0] = 0;
					for (int i = 1; i < ITEM * 2; i += 2)
						setItem(mainbuf, i, items, 0, nInjected, game);
					infoDisp("Items injected successfully!");
					break;
				}
				case 5 : {
					setMoney(mainbuf, 9999999);
					infoDisp("Money set successfully!");
					break;
				}
				case 6 : {
					setBadges(mainbuf, badgeCont);
					infoDisp("Badges set successfully!");
					break;
				}
				case 7 : {
					setTM(mainbuf, game);
					infoDisp("TMs set successfully!");
					break;
				}
				case 8 : {
					int start = 0;
					if (game == GAME_X || game == GAME_Y)
						start = XYWC6FLAGPOS;
					else if (game == GAME_OR || game == GAME_AS)
						start = ORASWC6FLAGPOS;
					
					for (int i = 0; i < (0x100 + 24 * WC6LENGTH); i++)
						*(mainbuf + start + i) = 0x00;
					infoDisp("Mistery Gift box cleaned!");
					break;
				}
				case 9 : {
					nInjected[2] = 0;
					for (int i = 1; i < BERRIES * 2; i += 2)
						setItem(mainbuf, i, berry, 2, nInjected, game);
					infoDisp("Berries set successfully!");
					break;
				}
			}
		}
		
		printEditor(currentEntry, langCont, badgeCont);
	}
}

void pokemonEditor(u8* mainbuf, int game) {	
	bool speedy = false;
	bool cloning = false;
	int box = 0;
	int currentEntry = 0;
	
	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		
		if (hidKeysDown() & KEY_B) break;
		
		if (hidKeysDown() & KEY_R) {
			if (box < 30) box++;
			else if (box == 30) box = 0;
		}
		
		if (hidKeysDown() & KEY_L) {
			if (box > 0) box--;
			else if (box == 0) box = 30;
		}
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 2 && touch.px < 17 && touch.py > 11 && touch.py < 33) {
				if (box > 0) box--;
				else if (box == 0) box = 30;
			}
			if (touch.px > 185 && touch.px < 200 && touch.py > 11 && touch.py < 33) {
				if (box < 30) box++;
				else if (box == 30) box = 0;
			}
			if (touch.px > 288 && touch.px < 310 && touch.py > 217 && touch.py < 235) break;
		}
		
		if (hidKeysDown() & KEY_DRIGHT) {
			if (currentEntry < 29) currentEntry++;
			else if (currentEntry == 29) currentEntry = 0;
		}
		
		if (hidKeysDown() & KEY_DLEFT) {
			if (currentEntry > 0) currentEntry--;
			else if (currentEntry == 0) currentEntry = 29;
		}
		
		if (hidKeysDown() & KEY_DUP) {
			if (currentEntry >= 6) currentEntry -= 6;
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (currentEntry <= 23) currentEntry += 6;
		}
		
		if ((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 89 && touch.py < 120)) {
			u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
			getPkmn(mainbuf, box, currentEntry, pkmn, game);
			
			while (aptMainLoop() && getPokedexNumber(pkmn)) {
				hidScanInput();
				hidTouchRead(&touch);

				if (hidKeysDown() & KEY_B) break;
				
				if (hidKeysDown() & KEY_L)
					speedy = false;
				
				if (hidKeysDown() & KEY_R)
					speedy = true;
				
				if (hidKeysDown() & KEY_TOUCH) {
					if (touch.px > 288 && touch.px < 310 && touch.py > 213 && touch.py < 231) break;
					
					if (touch.px > 126 && touch.px < 141 && touch.py > 60 && touch.py < 72) {
						if (getNature(pkmn) < 24) 
							setNature(pkmn, getNature(pkmn) + 1);
						else if (getNature(pkmn) == 24)
							setNature(pkmn, 0);
					}
					
					if (touch.px > 126 && touch.px < 141 && touch.py > 111 && touch.py < 123) {
						if (isShiny(pkmn)) 
							setShiny(pkmn, false);
						else setShiny(pkmn, true);
					}
					
					if (touch.px > 126 && touch.px < 141 && touch.py > 128 && touch.py < 140) {
						if (!(getPokerus(pkmn)))
							setPokerus(pkmn);
						else *(pkmn + 0x2B) = 0x00;
					}
					
					if (touch.px > 162 && touch.px < 177 && touch.py > 162 && touch.py < 174) {
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
					
					if (touch.px > 162 && touch.px < 177 && touch.py > 178 && touch.py < 190) {
						if (getHPType(pkmn) < 15) 
							setHPType(pkmn, getHPType(pkmn) + 1);
						else if (getHPType(pkmn) == 15)
							setHPType(pkmn, 0);
					}
					
					if (touch.px > 200 && touch.px < 308 && touch.py > 160 && touch.py < 190) {
						setPkmn(mainbuf, box, currentEntry, pkmn, game);
						infoDisp("Edits applied!");
						break;
					}
				}
				
				if ((hidKeysDown() & KEY_TOUCH) && !(speedy)) {
					if (touch.px > 110 && touch.px < 123 && touch.py > 144 && touch.py < 157) {
						if (getFriendship(pkmn) > 0)
							setFriendship(pkmn, getFriendship(pkmn) - 1);
					}
					
					if (touch.px > 150 && touch.px < 163 && touch.py > 144 && touch.py < 157) {
						if (getFriendship(pkmn) < 255)
							setFriendship(pkmn, getFriendship(pkmn) + 1);
					}
					
					// - ivs
					if (touch.px > 207 && touch.px < 220 && touch.py > 27 && touch.py < 40) 
						if (getIV(pkmn, 0) > 0)
							setIV(pkmn, getIV(pkmn, 0) - 1, 0);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 44 && touch.py < 57) 
						if (getIV(pkmn, 1) > 0)
							setIV(pkmn, getIV(pkmn, 1) - 1, 1);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 61 && touch.py < 74) 
						if (getIV(pkmn, 2) > 0)
							setIV(pkmn, getIV(pkmn, 2) - 1, 2);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 78 && touch.py < 91) 
						if (getIV(pkmn, 4) > 0)
							setIV(pkmn, getIV(pkmn, 4) - 1, 4);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 95 && touch.py < 108) 
						if (getIV(pkmn, 5) > 0)
							setIV(pkmn, getIV(pkmn, 5) - 1, 5);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 112 && touch.py < 125) 
						if (getIV(pkmn, 3) > 0)
							setIV(pkmn, getIV(pkmn, 3) - 1, 3);
						
					// + ivs
					if (touch.px > 242 && touch.px < 255 && touch.py > 27 && touch.py < 40) 
						if (getIV(pkmn, 0) < 31)
							setIV(pkmn, getIV(pkmn, 0) + 1, 0);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 44 && touch.py < 57) 
						if (getIV(pkmn, 1) < 31)
							setIV(pkmn, getIV(pkmn, 1) + 1, 1);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 61 && touch.py < 74) 
						if (getIV(pkmn, 2) < 31)
							setIV(pkmn, getIV(pkmn, 2) + 1, 2);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 78 && touch.py < 91) 
						if (getIV(pkmn, 4) < 31)
							setIV(pkmn, getIV(pkmn, 4) + 1, 4);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 95 && touch.py < 108) 
						if (getIV(pkmn, 5) < 31)
							setIV(pkmn, getIV(pkmn, 5) + 1, 5);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 112 && touch.py < 125) 
						if (getIV(pkmn, 3) < 31)
							setIV(pkmn, getIV(pkmn, 3) + 1, 3);

					// - evs
					if (touch.px > 257 && touch.px < 270 && touch.py > 27 && touch.py < 40) 
						if (getEV(pkmn, 0) > 0)
							setEV(pkmn, getEV(pkmn, 0) - 1, 0);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 44 && touch.py < 57) 
						if (getEV(pkmn, 1) > 0)
							setEV(pkmn, getEV(pkmn, 1) - 1, 1);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 61 && touch.py < 74) 
						if (getEV(pkmn, 2) > 0)
							setEV(pkmn, getEV(pkmn, 2) - 1, 2);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 78 && touch.py < 91) 
						if (getEV(pkmn, 4) > 0)
							setEV(pkmn, getEV(pkmn, 4) - 1, 4);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 95 && touch.py < 108) 
						if (getEV(pkmn, 5) > 0)
							setEV(pkmn, getEV(pkmn, 5) - 1, 5);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 112 && touch.py < 125) 
						if (getEV(pkmn, 3) > 0)
							setEV(pkmn, getEV(pkmn, 3) - 1, 3);
						
					// + evs
					if ((getEV(pkmn, 0) + getEV(pkmn, 1) + getEV(pkmn, 2) + getEV(pkmn, 3) + getEV(pkmn, 4) + getEV(pkmn, 5)) < 510) {
						if (touch.px > 296 && touch.px < 309 && touch.py > 27 && touch.py < 40) 
							if (getEV(pkmn, 0) < 252)
								setEV(pkmn, getEV(pkmn, 0) + 1, 0);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 44 && touch.py < 57) 
							if (getEV(pkmn, 1) < 252)
								setEV(pkmn, getEV(pkmn, 1) + 1, 1);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 61 && touch.py < 74) 
							if (getEV(pkmn, 2) < 252)
								setEV(pkmn, getEV(pkmn, 2) + 1, 2);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 78 && touch.py < 91) 
							if (getEV(pkmn, 4) < 252)
								setEV(pkmn, getEV(pkmn, 4) + 1, 4);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 95 && touch.py < 108) 
							if (getEV(pkmn, 5) < 252)
								setEV(pkmn, getEV(pkmn, 5) + 1, 5);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 112 && touch.py < 125) 
							if (getEV(pkmn, 3) < 252)
								setEV(pkmn, getEV(pkmn, 3) + 1, 3);
					}
				}
				
				if ((hidKeysHeld() & KEY_TOUCH) && speedy) {
					if (touch.px > 110 && touch.px < 123 && touch.py > 144 && touch.py < 157) {
						if (getFriendship(pkmn) > 0)
							setFriendship(pkmn, getFriendship(pkmn) - 1);
					}
					
					if (touch.px > 150 && touch.px < 163 && touch.py > 144 && touch.py < 157) {
						if (getFriendship(pkmn) < 255)
							setFriendship(pkmn, getFriendship(pkmn) + 1);
					}
					
					// - ivs
					if (touch.px > 207 && touch.px < 220 && touch.py > 27 && touch.py < 40) 
						if (getIV(pkmn, 0) > 0)
							setIV(pkmn, getIV(pkmn, 0) - 1, 0);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 44 && touch.py < 57) 
						if (getIV(pkmn, 1) > 0)
							setIV(pkmn, getIV(pkmn, 1) - 1, 1);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 61 && touch.py < 74) 
						if (getIV(pkmn, 2) > 0)
							setIV(pkmn, getIV(pkmn, 2) - 1, 2);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 78 && touch.py < 91) 
						if (getIV(pkmn, 4) > 0)
							setIV(pkmn, getIV(pkmn, 4) - 1, 4);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 95 && touch.py < 108) 
						if (getIV(pkmn, 5) > 0)
							setIV(pkmn, getIV(pkmn, 5) - 1, 5);
						
					if (touch.px > 207 && touch.px < 220 && touch.py > 112 && touch.py < 125) 
						if (getIV(pkmn, 3) > 0)
							setIV(pkmn, getIV(pkmn, 3) - 1, 3);
						
					// + ivs
					if (touch.px > 242 && touch.px < 255 && touch.py > 27 && touch.py < 40) 
						if (getIV(pkmn, 0) < 31)
							setIV(pkmn, getIV(pkmn, 0) + 1, 0);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 44 && touch.py < 57) 
						if (getIV(pkmn, 1) < 31)
							setIV(pkmn, getIV(pkmn, 1) + 1, 1);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 61 && touch.py < 74) 
						if (getIV(pkmn, 2) < 31)
							setIV(pkmn, getIV(pkmn, 2) + 1, 2);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 78 && touch.py < 91) 
						if (getIV(pkmn, 4) < 31)
							setIV(pkmn, getIV(pkmn, 4) + 1, 4);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 95 && touch.py < 108) 
						if (getIV(pkmn, 5) < 31)
							setIV(pkmn, getIV(pkmn, 5) + 1, 5);
						
					if (touch.px > 242 && touch.px < 255 && touch.py > 112 && touch.py < 125) 
						if (getIV(pkmn, 3) < 31)
							setIV(pkmn, getIV(pkmn, 3) + 1, 3);

					// - evs
					if (touch.px > 257 && touch.px < 270 && touch.py > 27 && touch.py < 40) 
						if (getEV(pkmn, 0) > 0)
							setEV(pkmn, getEV(pkmn, 0) - 1, 0);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 44 && touch.py < 57) 
						if (getEV(pkmn, 1) > 0)
							setEV(pkmn, getEV(pkmn, 1) - 1, 1);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 61 && touch.py < 74) 
						if (getEV(pkmn, 2) > 0)
							setEV(pkmn, getEV(pkmn, 2) - 1, 2);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 78 && touch.py < 91) 
						if (getEV(pkmn, 4) > 0)
							setEV(pkmn, getEV(pkmn, 4) - 1, 4);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 95 && touch.py < 108) 
						if (getEV(pkmn, 5) > 0)
							setEV(pkmn, getEV(pkmn, 5) - 1, 5);
						
					if (touch.px > 257 && touch.px < 270 && touch.py > 112 && touch.py < 125) 
						if (getEV(pkmn, 3) > 0)
							setEV(pkmn, getEV(pkmn, 3) - 1, 3);
						
					// + evs
					if ((getEV(pkmn, 0) + getEV(pkmn, 1) + getEV(pkmn, 2) + getEV(pkmn, 3) + getEV(pkmn, 4) + getEV(pkmn, 5)) < 510) {
						if (touch.px > 296 && touch.px < 309 && touch.py > 27 && touch.py < 40) 
							if (getEV(pkmn, 0) < 252)
								setEV(pkmn, getEV(pkmn, 0) + 1, 0);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 44 && touch.py < 57) 
							if (getEV(pkmn, 1) < 252)
								setEV(pkmn, getEV(pkmn, 1) + 1, 1);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 61 && touch.py < 74) 
							if (getEV(pkmn, 2) < 252)
								setEV(pkmn, getEV(pkmn, 2) + 1, 2);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 78 && touch.py < 91) 
							if (getEV(pkmn, 4) < 252)
								setEV(pkmn, getEV(pkmn, 4) + 1, 4);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 95 && touch.py < 108) 
							if (getEV(pkmn, 5) < 252)
								setEV(pkmn, getEV(pkmn, 5) + 1, 5);
							
						if (touch.px > 296 && touch.px < 309 && touch.py > 112 && touch.py < 125) 
							if (getEV(pkmn, 3) < 252)
								setEV(pkmn, getEV(pkmn, 3) + 1, 3);
					}
				}

				printPKEditor(pkmn, game, speedy);
			}
			
			free(pkmn);
		}

		if (hidKeysHeld() & KEY_TOUCH) {
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
		}
		
		if ((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 121 && touch.py < 151)) {
			u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
			getPkmn(mainbuf, box, currentEntry, pkmn, game);
			
			cloning = true;
			while(aptMainLoop()) {
				hidScanInput();
				touchPosition touch;
				hidTouchRead(&touch);
				
				if (hidKeysDown() & KEY_B) break;
				
				if (hidKeysDown() & KEY_R) {
					if (box < 30) box++;
					else if (box == 30) box = 0;
				}
				
				if (hidKeysDown() & KEY_L) {
					if (box > 0) box--;
					else if (box == 0) box = 30;
				}
				
				if (hidKeysDown() & KEY_TOUCH) {
					if (touch.px > 2 && touch.px < 17 && touch.py > 11 && touch.py < 33) {
						if (box > 0) box--;
						else if (box == 0) box = 30;
					}
					if (touch.px > 185 && touch.px < 200 && touch.py > 11 && touch.py < 33) {
						if (box < 30) box++;
						else if (box == 30) box = 0;
					}
				}
				
				if (hidKeysDown() & KEY_DRIGHT) {
					if (currentEntry < 29) currentEntry++;
					else if (currentEntry == 29) currentEntry = 0;
				}
				
				if (hidKeysDown() & KEY_DLEFT) {
					if (currentEntry > 0) currentEntry--;
					else if (currentEntry == 0) currentEntry = 29;
				}
				
				if (hidKeysDown() & KEY_DUP) {
					if (currentEntry >= 6) currentEntry -= 6;
				}
				
				if (hidKeysDown() & KEY_DDOWN) {
					if (currentEntry <= 23) currentEntry += 6;
				}

				if (hidKeysHeld() & KEY_TOUCH) {
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
				}
				
				if ((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touch.px > 214 && touch.px < 320 && touch.py > 121 && touch.py < 151)) {
					setPkmn(mainbuf, box, currentEntry, pkmn, game);
					break;
				}

				printPKViewer(mainbuf, game, currentEntry, box, cloning);
			}
			
			free(pkmn);
			cloning = false;
		}
		
		printPKViewer(mainbuf, game, currentEntry, box, cloning);
	}
}