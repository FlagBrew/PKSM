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

#include "hex.h"

void fillDescriptions(wchar_t *descriptions[]) {
	for (int j = 0; j < PKMNLENGTH; j++)
		descriptions[j] = L" ";
	
	descriptions[0x00] = i18n(S_HEXEDITOR_DESC_0x00);
	descriptions[0x01] = i18n(S_HEXEDITOR_DESC_0x01);
	descriptions[0x02] = i18n(S_HEXEDITOR_DESC_0x02);
	descriptions[0x03] = i18n(S_HEXEDITOR_DESC_0x03);
	descriptions[0x04] = i18n(S_HEXEDITOR_DESC_0x04);
	descriptions[0x05] = i18n(S_HEXEDITOR_DESC_0x05);
	descriptions[0x06] = i18n(S_HEXEDITOR_DESC_0x06);
	descriptions[0x07] = i18n(S_HEXEDITOR_DESC_0x07);
	descriptions[0x08] = i18n(S_HEXEDITOR_DESC_0x08);
	descriptions[0x09] = i18n(S_HEXEDITOR_DESC_0x09);
	descriptions[0x0A] = i18n(S_HEXEDITOR_DESC_0x0A);
	descriptions[0x0B] = i18n(S_HEXEDITOR_DESC_0x0B);
	descriptions[0x0C] = i18n(S_HEXEDITOR_DESC_0x0C);
	descriptions[0x0D] = i18n(S_HEXEDITOR_DESC_0x0D);
	descriptions[0x0E] = i18n(S_HEXEDITOR_DESC_0x0E);
	descriptions[0x0F] = i18n(S_HEXEDITOR_DESC_0x0F);
	
	descriptions[0x10] = i18n(S_HEXEDITOR_DESC_0x10);
	descriptions[0x11] = i18n(S_HEXEDITOR_DESC_0x11);
	descriptions[0x12] = i18n(S_HEXEDITOR_DESC_0x12);
	descriptions[0x13] = i18n(S_HEXEDITOR_DESC_0x13);
	descriptions[0x14] = i18n(S_HEXEDITOR_DESC_0x14);
	descriptions[0x15] = i18n(S_HEXEDITOR_DESC_0x15);
	descriptions[0x16] = i18n(S_HEXEDITOR_DESC_0x16);
	descriptions[0x17] = i18n(S_HEXEDITOR_DESC_0x17);
	descriptions[0x18] = i18n(S_HEXEDITOR_DESC_0x18);
	descriptions[0x19] = i18n(S_HEXEDITOR_DESC_0x19);
	descriptions[0x1A] = i18n(S_HEXEDITOR_DESC_0x1A);
	descriptions[0x1B] = i18n(S_HEXEDITOR_DESC_0x1B);
	descriptions[0x1C] = i18n(S_HEXEDITOR_DESC_0x1C);
	descriptions[0x1D] = i18n(S_HEXEDITOR_DESC_0x1D);
	descriptions[0x1E] = i18n(S_HEXEDITOR_DESC_0x1E);
	descriptions[0x1F] = i18n(S_HEXEDITOR_DESC_0x1F);
	
	descriptions[0x20] = i18n(S_HEXEDITOR_DESC_0x20);
	descriptions[0x21] = i18n(S_HEXEDITOR_DESC_0x21);
	descriptions[0x22] = i18n(S_HEXEDITOR_DESC_0x22);
	descriptions[0x23] = i18n(S_HEXEDITOR_DESC_0x23);
	descriptions[0x24] = i18n(S_HEXEDITOR_DESC_0x24);
	descriptions[0x25] = i18n(S_HEXEDITOR_DESC_0x25);
	descriptions[0x26] = i18n(S_HEXEDITOR_DESC_0x26);
	descriptions[0x27] = i18n(S_HEXEDITOR_DESC_0x27);
	descriptions[0x28] = i18n(S_HEXEDITOR_DESC_0x28);
	descriptions[0x29] = i18n(S_HEXEDITOR_DESC_0x29);
	descriptions[0x2A] = i18n(S_HEXEDITOR_DESC_0x2A);
	descriptions[0x2B] = i18n(S_HEXEDITOR_DESC_0x2B);
	descriptions[0x2C] = i18n(S_HEXEDITOR_DESC_0x2C);
	descriptions[0x2D] = i18n(S_HEXEDITOR_DESC_0x2D);
	descriptions[0x2E] = i18n(S_HEXEDITOR_DESC_0x2E);
	descriptions[0x2F] = i18n(S_HEXEDITOR_DESC_0x2F);
	
	descriptions[0x30] = i18n(S_HEXEDITOR_DESC_0x30);
	descriptions[0x31] = i18n(S_HEXEDITOR_DESC_0x31);
	descriptions[0x32] = i18n(S_HEXEDITOR_DESC_0x32);
	descriptions[0x33] = i18n(S_HEXEDITOR_DESC_0x33);
	descriptions[0x34] = i18n(S_HEXEDITOR_DESC_0x34);
	descriptions[0x35] = i18n(S_HEXEDITOR_DESC_0x35);
	descriptions[0x36] = i18n(S_HEXEDITOR_DESC_0x36);
	descriptions[0x37] = i18n(S_HEXEDITOR_DESC_0x37);
	descriptions[0x38] = i18n(S_HEXEDITOR_DESC_0x38);
	descriptions[0x39] = i18n(S_HEXEDITOR_DESC_0x39);
	descriptions[0x3A] = i18n(S_HEXEDITOR_DESC_0x3A);
	
	descriptions[0x40] = i18n(S_HEXEDITOR_DESC_0x40);
	descriptions[0x41] = i18n(S_HEXEDITOR_DESC_0x41);
	descriptions[0x42] = i18n(S_HEXEDITOR_DESC_0x42);
	descriptions[0x43] = i18n(S_HEXEDITOR_DESC_0x43);
	descriptions[0x44] = i18n(S_HEXEDITOR_DESC_0x44);
	descriptions[0x45] = i18n(S_HEXEDITOR_DESC_0x45);
	descriptions[0x46] = i18n(S_HEXEDITOR_DESC_0x46);
	descriptions[0x47] = i18n(S_HEXEDITOR_DESC_0x47);
	descriptions[0x48] = i18n(S_HEXEDITOR_DESC_0x48);
	descriptions[0x49] = i18n(S_HEXEDITOR_DESC_0x49);
	descriptions[0x4A] = i18n(S_HEXEDITOR_DESC_0x4A);
	descriptions[0x4B] = i18n(S_HEXEDITOR_DESC_0x4B);
	descriptions[0x4C] = i18n(S_HEXEDITOR_DESC_0x4C);
	descriptions[0x4D] = i18n(S_HEXEDITOR_DESC_0x4D);
	descriptions[0x4E] = i18n(S_HEXEDITOR_DESC_0x4E);
	descriptions[0x4F] = i18n(S_HEXEDITOR_DESC_0x4F);
	
	descriptions[0x50] = i18n(S_HEXEDITOR_DESC_0x50);
	descriptions[0x51] = i18n(S_HEXEDITOR_DESC_0x51);
	descriptions[0x52] = i18n(S_HEXEDITOR_DESC_0x52);
	descriptions[0x53] = i18n(S_HEXEDITOR_DESC_0x53);
	descriptions[0x54] = i18n(S_HEXEDITOR_DESC_0x54);
	descriptions[0x55] = i18n(S_HEXEDITOR_DESC_0x55);
	descriptions[0x56] = i18n(S_HEXEDITOR_DESC_0x56);
	descriptions[0x57] = i18n(S_HEXEDITOR_DESC_0x57);
	
	descriptions[0x5A] = i18n(S_HEXEDITOR_DESC_0x5A);
	descriptions[0x5B] = i18n(S_HEXEDITOR_DESC_0x5B);
	descriptions[0x5C] = i18n(S_HEXEDITOR_DESC_0x5C);
	descriptions[0x5D] = i18n(S_HEXEDITOR_DESC_0x5D);
	descriptions[0x5E] = i18n(S_HEXEDITOR_DESC_0x5E);
	descriptions[0x5F] = i18n(S_HEXEDITOR_DESC_0x5F);
	
	descriptions[0x60] = i18n(S_HEXEDITOR_DESC_0x60);
	descriptions[0x61] = i18n(S_HEXEDITOR_DESC_0x61);
	descriptions[0x62] = i18n(S_HEXEDITOR_DESC_0x62);
	descriptions[0x63] = i18n(S_HEXEDITOR_DESC_0x63);
	descriptions[0x64] = i18n(S_HEXEDITOR_DESC_0x64);
	descriptions[0x65] = i18n(S_HEXEDITOR_DESC_0x65);
	descriptions[0x66] = i18n(S_HEXEDITOR_DESC_0x66);
	descriptions[0x67] = i18n(S_HEXEDITOR_DESC_0x67);
	descriptions[0x68] = i18n(S_HEXEDITOR_DESC_0x68);
	descriptions[0x69] = i18n(S_HEXEDITOR_DESC_0x69);
	descriptions[0x6A] = i18n(S_HEXEDITOR_DESC_0x6A);
	descriptions[0x6B] = i18n(S_HEXEDITOR_DESC_0x6B);
	descriptions[0x6C] = i18n(S_HEXEDITOR_DESC_0x6C);
	descriptions[0x6D] = i18n(S_HEXEDITOR_DESC_0x6D);
	descriptions[0x6E] = i18n(S_HEXEDITOR_DESC_0x6E);
	descriptions[0x6F] = i18n(S_HEXEDITOR_DESC_0x6F);
	
	descriptions[0x70] = i18n(S_HEXEDITOR_DESC_0x70);
	descriptions[0x71] = i18n(S_HEXEDITOR_DESC_0x71);
	descriptions[0x72] = i18n(S_HEXEDITOR_DESC_0x72);
	
	descriptions[0x74] = i18n(S_HEXEDITOR_DESC_0x74);
	descriptions[0x75] = i18n(S_HEXEDITOR_DESC_0x75);
	descriptions[0x76] = i18n(S_HEXEDITOR_DESC_0x76);
	descriptions[0x77] = i18n(S_HEXEDITOR_DESC_0x77);
	descriptions[0x78] = i18n(S_HEXEDITOR_DESC_0x78);
	descriptions[0x79] = i18n(S_HEXEDITOR_DESC_0x79);
	descriptions[0x7A] = i18n(S_HEXEDITOR_DESC_0x7A);
	descriptions[0x7B] = i18n(S_HEXEDITOR_DESC_0x7B);
	descriptions[0x7C] = i18n(S_HEXEDITOR_DESC_0x7C);
	descriptions[0x7D] = i18n(S_HEXEDITOR_DESC_0x7D);
	descriptions[0x7E] = i18n(S_HEXEDITOR_DESC_0x7E);
	descriptions[0x7F] = i18n(S_HEXEDITOR_DESC_0x7F);
	
	descriptions[0x80] = i18n(S_HEXEDITOR_DESC_0x80);
	descriptions[0x81] = i18n(S_HEXEDITOR_DESC_0x81);
	descriptions[0x82] = i18n(S_HEXEDITOR_DESC_0x82);
	descriptions[0x83] = i18n(S_HEXEDITOR_DESC_0x83);
	descriptions[0x84] = i18n(S_HEXEDITOR_DESC_0x84);
	descriptions[0x85] = i18n(S_HEXEDITOR_DESC_0x85);
	descriptions[0x86] = i18n(S_HEXEDITOR_DESC_0x86);
	descriptions[0x87] = i18n(S_HEXEDITOR_DESC_0x87);
	descriptions[0x88] = i18n(S_HEXEDITOR_DESC_0x88);
	descriptions[0x89] = i18n(S_HEXEDITOR_DESC_0x89);
	descriptions[0x8A] = i18n(S_HEXEDITOR_DESC_0x8A);
	descriptions[0x8B] = i18n(S_HEXEDITOR_DESC_0x8B);
	descriptions[0x8C] = i18n(S_HEXEDITOR_DESC_0x8C);
	descriptions[0x8D] = i18n(S_HEXEDITOR_DESC_0x8D);
	descriptions[0x8E] = i18n(S_HEXEDITOR_DESC_0x8E);
	descriptions[0x8F] = i18n(S_HEXEDITOR_DESC_0x8F);
	
	descriptions[0x92] = i18n(S_HEXEDITOR_DESC_0x92);
	descriptions[0x93] = i18n(S_HEXEDITOR_DESC_0x93);
	descriptions[0x94] = i18n(S_HEXEDITOR_DESC_0x94);
	descriptions[0x95] = i18n(S_HEXEDITOR_DESC_0x95);
	descriptions[0x96] = i18n(S_HEXEDITOR_DESC_0x96);
	descriptions[0x97] = i18n(S_HEXEDITOR_DESC_0x97);
	descriptions[0x98] = i18n(S_HEXEDITOR_DESC_0x98);
	descriptions[0x99] = i18n(S_HEXEDITOR_DESC_0x99);
	descriptions[0x9A] = i18n(S_HEXEDITOR_DESC_0x9A);
	descriptions[0x9B] = i18n(S_HEXEDITOR_DESC_0x9B);
	descriptions[0x9C] = i18n(S_HEXEDITOR_DESC_0x9C);
	descriptions[0x9D] = i18n(S_HEXEDITOR_DESC_0x9D);
	
	descriptions[0xA2] = i18n(S_HEXEDITOR_DESC_0xA2);
	descriptions[0xA3] = i18n(S_HEXEDITOR_DESC_0xA3);
	descriptions[0xA4] = i18n(S_HEXEDITOR_DESC_0xA4);
	descriptions[0xA5] = i18n(S_HEXEDITOR_DESC_0xA5);
	descriptions[0xA6] = i18n(S_HEXEDITOR_DESC_0xA6);
	
	descriptions[0xA8] = i18n(S_HEXEDITOR_DESC_0xA8);
	descriptions[0xA9] = i18n(S_HEXEDITOR_DESC_0xA9);
	descriptions[0xAE] = i18n(S_HEXEDITOR_DESC_0xAE);
	descriptions[0xAF] = i18n(S_HEXEDITOR_DESC_0xAF);
	
	descriptions[0xB0] = i18n(S_HEXEDITOR_DESC_0xB0);
	descriptions[0xB1] = i18n(S_HEXEDITOR_DESC_0xB1);
	descriptions[0xB2] = i18n(S_HEXEDITOR_DESC_0xB2);
	descriptions[0xB3] = i18n(S_HEXEDITOR_DESC_0xB3);
	descriptions[0xB4] = i18n(S_HEXEDITOR_DESC_0xB4);
	descriptions[0xB5] = i18n(S_HEXEDITOR_DESC_0xB5);
	descriptions[0xB6] = i18n(S_HEXEDITOR_DESC_0xB6);
	descriptions[0xB7] = i18n(S_HEXEDITOR_DESC_0xB7);
	descriptions[0xB8] = i18n(S_HEXEDITOR_DESC_0xB8);
	descriptions[0xB9] = i18n(S_HEXEDITOR_DESC_0xB9);
	descriptions[0xBA] = i18n(S_HEXEDITOR_DESC_0xBA);
	descriptions[0xBB] = i18n(S_HEXEDITOR_DESC_0xBB);
	descriptions[0xBC] = i18n(S_HEXEDITOR_DESC_0xBC);
	descriptions[0xBD] = i18n(S_HEXEDITOR_DESC_0xBD);
	descriptions[0xBE] = i18n(S_HEXEDITOR_DESC_0xBE);
	descriptions[0xBF] = i18n(S_HEXEDITOR_DESC_0xBF);
	
	descriptions[0xC0] = i18n(S_HEXEDITOR_DESC_0xC0);
	descriptions[0xC1] = i18n(S_HEXEDITOR_DESC_0xC1);
	descriptions[0xC2] = i18n(S_HEXEDITOR_DESC_0xC2);
	descriptions[0xC3] = i18n(S_HEXEDITOR_DESC_0xC3);
	descriptions[0xC4] = i18n(S_HEXEDITOR_DESC_0xC4);
	descriptions[0xC5] = i18n(S_HEXEDITOR_DESC_0xC5);
	descriptions[0xC6] = i18n(S_HEXEDITOR_DESC_0xC6);
	descriptions[0xC7] = i18n(S_HEXEDITOR_DESC_0xC7);
	
	descriptions[0xCA] = i18n(S_HEXEDITOR_DESC_0xCA);
	descriptions[0xCB] = i18n(S_HEXEDITOR_DESC_0xCB);
	descriptions[0xCC] = i18n(S_HEXEDITOR_DESC_0xCC);
	descriptions[0xCD] = i18n(S_HEXEDITOR_DESC_0xCD);
	descriptions[0xCE] = i18n(S_HEXEDITOR_DESC_0xCE);
	
	descriptions[0xD0] = i18n(S_HEXEDITOR_DESC_0xD0);
	descriptions[0xD1] = i18n(S_HEXEDITOR_DESC_0xD1);
	descriptions[0xD2] = i18n(S_HEXEDITOR_DESC_0xD2);
	descriptions[0xD3] = i18n(S_HEXEDITOR_DESC_0xD3);
	descriptions[0xD4] = i18n(S_HEXEDITOR_DESC_0xD4);
	descriptions[0xD5] = i18n(S_HEXEDITOR_DESC_0xD5);
	descriptions[0xD6] = i18n(S_HEXEDITOR_DESC_0xD6);

	descriptions[0xD8] = i18n(S_HEXEDITOR_DESC_0xD8);
	descriptions[0xDA] = i18n(S_HEXEDITOR_DESC_0xDA);
	
	descriptions[0xDC] = i18n(S_HEXEDITOR_DESC_0xDC);
	descriptions[0xDD] = i18n(S_HEXEDITOR_DESC_0xDD);
	descriptions[0xDE] = i18n(S_HEXEDITOR_DESC_0xDE);
	descriptions[0xDF] = i18n(S_HEXEDITOR_DESC_0xDF);
	
	descriptions[0xE0] = i18n(S_HEXEDITOR_DESC_0xE0);
	descriptions[0xE1] = i18n(S_HEXEDITOR_DESC_0xE1);
	descriptions[0xE2] = i18n(S_HEXEDITOR_DESC_0xE2);
	descriptions[0xE3] = i18n(S_HEXEDITOR_DESC_0xE3);
}

void fillSectors(bool sector[][2]) {
	// editable, requires Boolean
	for (int j = 0; j < PKMNLENGTH; j++) {
		sector[j][0] = false;
		sector[j][1] = false;
	}
	
	for (int j = 0x30; j <= 0x36; j++) { // ribbons 1-6
		sector[j][0] = true;
		sector[j][1] = true;
	}
		
	for (int j = 0x00; j <= 0x03; j++) // encryption constant
		sector[j][0] = true;	

	for (int j = 0x0C; j <= 0x0F; j++) // tid to sid
		sector[j][0] = true;
		
	for (int j = 0x14; j <= 0x15; j++) // ability to ability number
		sector[j][0] = true;
	
	for (int j = 0x18; j <= 0x1B; j++) // pid
		sector[j][0] = true;

	for (int j = 0x40; j <= 0x57; j++) // Nickname
		sector[j][0] = true;	
		
	for (int j = 0x1D; j <= 0x29; j++) // Fateful encounter to Contest Value Sheen
		sector[j][0] = true;
		
	for (int j = 0x62; j <= 0x69; j++) // Move 1 PP to Move 4 PP Up
		sector[j][0] = true;

	for (int j = 0x74; j <= 0x8F; j++) // IV to Held Trainer Name
		sector[j][0] = true;
		
	for (int j = 0xA2; j <= 0xA6; j++) // HT_Friendship to HT_Feeling
		sector[j][0] = true;
		
	for (int j = 0xAE; j <= 0xAF; j++) // Fullness to Enjoyment
		sector[j][0] = true;
		
	for (int j = 0xB0; j <= 0xC7; j++) // OT Name
		sector[j][0] = true;
		
	for (int j = 0xCA; j <= 0xCD; j++) // OT_Friendship to OT_Memory
		sector[j][0] = true;
		
	for (int j = 0xD0; j <= 0xD6; j++) // OT_Feeling to Met Day
		sector[j][0] = true;

	sector[0x77][0] = true; // Egg / Nickname flag
	sector[0x77][1] = true; // Egg / Nickname flag
	sector[0xDD][0] = true; // Met Level / OT Gender
	sector[0xDD][1] = true;
	sector[0xDE][0] = true; // Hyper Train Flags
	sector[0xDE][1] = true; // Hyper Train Flags
	sector[0x1D][1] = true; // Fateful encounter
}

void fillSectorsHaxMode(bool sector[][2]) {
	for (int j = 0; j < PKMNLENGTH; j++) {
		sector[j][0] = true;
		sector[j][1] = false;
	}
	
	for (int j = 0x08; j <= 0x0B; j++)
		sector[j][0] = false;

	sector[0x1C][0] = false;
	sector[0xDC][0] = false;
	
	for (int j = 0x5A; j <= 0x61; j++) 
		sector[j][0] = false;
	
	for (int j = 0x6A; j <= 0x71; j++)
		sector[j][0] = false;
		
	for (int j = 0x30; j <= 0x36; j++)
		sector[j][1] = true;

	sector[0x77][1] = true; // Egg / Nickname flag
	sector[0xDD][1] = true;
	sector[0xDE][1] = true; // Hyper Train Flags
	sector[0x1D][1] = true; // Fateful encounter
}

void fillSaveSectors(bool saveSectors[][2]) {
	for (int j = 0; j < 0x76000; j++) {
		saveSectors[j][0] = true;
		saveSectors[j][1] = false;
	}
}
