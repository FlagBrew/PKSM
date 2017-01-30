#include <3ds.h>
#include "editor.h"
#include "hex.h"

void fillDescriptions(char *descriptions[]) {
	for (int j = 0; j < PKMNLENGTH; j++)
		descriptions[j] = " ";
	
	descriptions[0x00] = "Encryption Constant, byte 1";
	descriptions[0x01] = "Encryption Constant, byte 2";
	descriptions[0x02] = "Encryption Constant, byte 3";
	descriptions[0x03] = "Encryption Constant, byte 4";
	descriptions[0x04] = "Sanity, byte 1";
	descriptions[0x05] = "Sanity, byte 2";
	descriptions[0x06] = "Checksum, byte 1";
	descriptions[0x07] = "Checksum, byte 2";
	descriptions[0x08] = "Species, byte 1";
	descriptions[0x09] = "Species, byte 2";
	descriptions[0x0A] = "Held Item, byte 1";
	descriptions[0x0B] = "Held Item, byte 2";
	descriptions[0x0C] = "TID, byte 1";
	descriptions[0x0D] = "TID, byte 2";
	descriptions[0x0E] = "SID, byte 1";
	descriptions[0x0F] = "SID, byte 2";
	
	descriptions[0x10] = "Experience, byte 1";
	descriptions[0x11] = "Experience, byte 2";
	descriptions[0x12] = "Experience, byte 3";
	descriptions[0x13] = "Experience, byte 4";
	descriptions[0x14] = "Ability";
	descriptions[0x15] = "Ability Number";
	descriptions[0x16] = "MarkValue, byte 1";
	descriptions[0x17] = "MarkValue, byte 2";
	descriptions[0x18] = "PID, byte 1";
	descriptions[0x19] = "PID, byte 2";
	descriptions[0x1A] = "PID, byte 3";
	descriptions[0x1B] = "PID, byte 4";
	descriptions[0x1C] = "Nature";
	descriptions[0x1D] = "Fateful Encounter flag / Gender / Alternative Form";
	descriptions[0x1E] = "HP Effort Value";
	descriptions[0x1F] = "ATK Effort Value";
	
	descriptions[0x20] = "DEF Effort Value";
	descriptions[0x21] = "SPE Effort Value";
	descriptions[0x22] = "SPA Effort Value";
	descriptions[0x23] = "SPD Effort Value";
	descriptions[0x24] = "Contest value Cool";
	descriptions[0x25] = "Contest value Beauty";
	descriptions[0x26] = "Contest value Cute";
	descriptions[0x27] = "Contest value Smart";
	descriptions[0x28] = "Contest value Tough";
	descriptions[0x29] = "Contest value Sheen";
	descriptions[0x2A] = "PokePelago Event Status flag";
	descriptions[0x2B] = "Pokerus";
	descriptions[0x2C] = "Super Train flag , byte 1";
	descriptions[0x2D] = "Super Train flag , byte 2";
	descriptions[0x2E] = "Super Train flag , byte 3";
	descriptions[0x2F] = "Super Train flag , byte 4";
	
	descriptions[0x30] = "Ribbon flag, byte 1";
	descriptions[0x31] = "Ribbon flag, byte 2";
	descriptions[0x32] = "Ribbon flag, byte 3";
	descriptions[0x33] = "Ribbon flag, byte 4";
	descriptions[0x34] = "Ribbon flag, byte 5";
	descriptions[0x35] = "Ribbon flag, byte 6";
	descriptions[0x36] = "Ribbon flag, byte 7";
	descriptions[0x37] = "Ribbon flag, byte 8";
	descriptions[0x38] = "Ribbon Memory Contest";
	descriptions[0x39] = "Ribbon Memory Battle";
	descriptions[0x3A] = "Dist byte";
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

	sector[0x0A][0] = true; // item byte1
	sector[0x0B][0] = true; // item byte2	
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