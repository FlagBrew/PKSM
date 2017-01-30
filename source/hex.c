#include <3ds.h>
#include "editor.h"
#include "hex.h"

void fillDescriptions(char *descriptions[]) {
	for (int j = 0; j < PKMNLENGTH; j++)
		descriptions[j] = " ";
	
	descriptions[0x00] = "0x00 - Encryption Constant; byte1";
	descriptions[0x01] = "0x01 - Encryption Constant; byte2";
	descriptions[0x02] = "0x02 - Encryption Constant; byte3";
	descriptions[0x03] = "0x03 - Encryption Constant; byte4";
}

void fillSectors(bool sector[][2]) {
	for (int j = 0; j < PKMNLENGTH; j++) {
		sector[j][0] = false;
		sector[j][1] = false;
	}

	// editable, requires Boolean
	sector[0x00][0] = true; // encryption constant byte1
	sector[0x01][0] = true; // encryption constant byte2
	sector[0x02][0] = true; // encryption constant byte3
	sector[0x03][0] = true; // encryption constant byte4
	
	sector[0x0C][0] = true; // TID byte1
	sector[0x0D][0] = true; // TID byte2
	sector[0x0E][0] = true; // SID byte1
	sector[0x0F][0] = true; // SID byte2
	
	sector[0x18][0] = true; // PID byte1
	sector[0x19][0] = true; // PID byte2
	sector[0x1A][0] = true; // PID byte3
	sector[0x1B][0] = true; // PID byte4
	
	sector[0x1D][0] = true; sector[0x1D][1] = true; // fateful encounter
	
	for (int j = 0x1E; j <= 0x2A; j++) // EV_HP to Pelago Status
		sector[j][0] = true;
		
	for (int j = 0x62; j <= 0x69; j++) // Move1_PP to Move4_PPUps
		sector[j][0] = true;
		
	for (int j = 0xA2; j <= 0xA6; j++) // HT_Friendship to HT_Feeling
		sector[j][0] = true;
		
	for (int j = 0xAE; j <= 0xAF; j++) // Fullness to Enjoyment
		sector[j][0] = true;
		
	for (int j = 0xCA; j <= 0xCD; j++) // OT_Friendship to OT_Memory
		sector[j][0] = true;
		
	for (int j = 0xD0; j <= 0xD6; j++) // OT_Feeling to Met Day
		sector[j][0] = true;
		
	for (int j = 0xDF; j <= 0xE3; j++) // Version to Language
		sector[j][0] = true;
}