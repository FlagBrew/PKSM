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

#include "config.h"

static const char* config_path = "sdmc:/3ds/PKSM/cfg.bin";
static const u8 PKSMOT[24] = {
	0x50, 0x00, 0x4B, 0x00, 0x53, 0x00, 0x4D, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static u8 config_buf[CONFIG_SIZE];
static wchar_t* descriptions[CONFIG_SIZE];

static void patchConfigStorageSize(void)
{
	static const char* storagePath = "sdmc:/3ds/PKSM/bank/bank.bin";
	if (checkFile(storagePath))
	{
		FILE *bank = fopen(storagePath, "rt");
		fseek(bank, 0, SEEK_END);
		u32 size = ftell(bank);
		fclose(bank);
		PKSM_Configuration.storageSize = size / (30 * 232);
	}
	else
	{
		PKSM_Configuration.storageSize = 150;
	}
}

void config_init(void)
{
	if (!checkFile(config_path))
	{
		CFGU_GetSystemLanguage(&PKSM_Configuration.pksmLanguage);
		PKSM_Configuration.automaticSaveBackup = 1;
		patchConfigStorageSize();
		PKSM_Configuration.defaultTID = 12345;
		PKSM_Configuration.defaultSID = 54321;
		memcpy(PKSM_Configuration.defaultOTName, PKSMOT, 24);
		PKSM_Configuration.defaultNationality = 1;
		PKSM_Configuration.editInTransfers = 1;
		PKSM_Configuration.defaultDay = 1;
		PKSM_Configuration.defaultMonth = 1;
		PKSM_Configuration.defaultYear = 0;
		PKSM_Configuration.fixBadSectors = 0;

		config_set();
	}
	else
	{
		config_load();
		patchConfigStorageSize();
		config_set();
	}
}

void config_fill_values(void)
{
	PKSM_Configuration.pksmLanguage = *(u8*)(config_buf + 0x0);
	PKSM_Configuration.automaticSaveBackup = *(u8*)(config_buf + 0x1);
	PKSM_Configuration.storageSize = *(u16*)(config_buf + 0x2);
	PKSM_Configuration.defaultTID = *(u16*)(config_buf + 0x4);
	PKSM_Configuration.defaultSID = *(u16*)(config_buf + 0x6);
	memcpy(PKSM_Configuration.defaultOTName, config_buf + 0x8, 24);
	PKSM_Configuration.defaultNationality = *(u8*)(config_buf + 0x20);
	PKSM_Configuration.editInTransfers = *(u8*)(config_buf + 0x21);
	PKSM_Configuration.defaultDay = *(u8*)(config_buf + 0x22);
	PKSM_Configuration.defaultMonth = *(u8*)(config_buf + 0x23);
	PKSM_Configuration.defaultYear = *(u8*)(config_buf + 0x24);
	PKSM_Configuration.fixBadSectors = *(u8*)(config_buf + 0x25);
}

void configbuf_set_values(void)
{
	if (PKSM_Configuration.storageSize < 2)
	{
		PKSM_Configuration.storageSize = 2;
	}
	if (PKSM_Configuration.defaultDay == 0)
	{
		PKSM_Configuration.defaultDay = 1;
	}
	if (PKSM_Configuration.defaultMonth == 0)
	{
		PKSM_Configuration.defaultMonth = 1;
	}
	
	*(config_buf + 0x0) = PKSM_Configuration.pksmLanguage;
	*(config_buf + 0x1) = PKSM_Configuration.automaticSaveBackup;
	memcpy(config_buf + 0x2, &PKSM_Configuration.storageSize, 2);
	memcpy(config_buf + 0x4, &PKSM_Configuration.defaultTID, 2);
	memcpy(config_buf + 0x6, &PKSM_Configuration.defaultSID, 2);
	memcpy(config_buf + 0x8, PKSM_Configuration.defaultOTName, 24);
	*(config_buf + 0x20) = PKSM_Configuration.defaultNationality;
	*(config_buf + 0x21) = PKSM_Configuration.editInTransfers;
	*(config_buf + 0x22) = PKSM_Configuration.defaultDay;
	*(config_buf + 0x23) = PKSM_Configuration.defaultMonth;
	*(config_buf + 0x24) = PKSM_Configuration.defaultYear;
	*(config_buf + 0x25) = PKSM_Configuration.fixBadSectors;
	
	for (int i = CONFIG_USED; i < CONFIG_SIZE; i++)
	{
		config_buf[i] = 0;
	}
}

void config_load(void)
{
	FILE *fptr = fopen(config_path, "rt");
	if (fptr == NULL)
		return;
	fseek(fptr, 0, SEEK_END);
	memset(config_buf, 0, CONFIG_SIZE);
	rewind(fptr);
	fread(config_buf, CONFIG_SIZE, 1, fptr);
	fclose(fptr);
	
	// load values
	config_fill_values();
}

void config_set(void)
{
	remove(config_path);
	configbuf_set_values();
	file_write(config_path, config_buf, CONFIG_SIZE);
}

void parseConfigHexEditor(int byte)
{
	if (byte == 0x00)
		checkMaxValue(config_buf, byte, config_buf[byte], 9);
	else if (byte == 0x01)
		checkMaxValue(config_buf, byte, config_buf[byte], 0);
	else if (byte == 0x02 || byte == 0x03)
		checkMaxValueBetweenBounds(config_buf, byte, 0x02, 2, 1000);
	else if (byte == 0x20)
		checkMaxValue(config_buf, byte, config_buf[byte], 7);
	else if (byte == 0x21)
		checkMaxValue(config_buf, byte, config_buf[byte], 0);
	else if (byte == 0x22) // day
		checkMaxValue(config_buf, byte, config_buf[byte], 30);
	else if (byte == 0x23) // month
		checkMaxValue(config_buf, byte, config_buf[byte], 11);
	else if (byte == 0x24) // year
		checkMaxValue(config_buf, byte, config_buf[byte], 98);
	else if (byte == 0x25)
		checkMaxValue(config_buf, byte, config_buf[byte], 0);
	else
		config_buf[byte]++;
}

void configMenu(void)
{
	for (int j = 0; j < CONFIG_SIZE; j++)
	{
		descriptions[j] = L" ";
	}
	
	descriptions[0x00] = L"PKSM interface language";
	descriptions[0x01] = L"Perform automatic backup on load (0: NO/1: YES)";
	descriptions[0x02] = L"PKSM Storage size, byte 1";
	descriptions[0x03] = L"PKSM Storage size, byte 2";
	descriptions[0x04] = L"Default TID, byte 1";
	descriptions[0x05] = L"Default TID, byte 2";
	descriptions[0x06] = L"Default SID, byte 1";
	descriptions[0x07] = L"Default SID, byte 2";
	descriptions[0x08] = L"Default OT Name, byte 1";
	descriptions[0x09] = L"Default OT Name, byte 2";
	descriptions[0x0A] = L"Default OT Name, byte 3";
	descriptions[0x0B] = L"Default OT Name, byte 4";
	descriptions[0x0C] = L"Default OT Name, byte 5";
	descriptions[0x0D] = L"Default OT Name, byte 6";
	descriptions[0x0E] = L"Default OT Name, byte 7";
	descriptions[0x0F] = L"Default OT Name, byte 8";
	descriptions[0x10] = L"Default OT Name, byte 9";
	descriptions[0x11] = L"Default OT Name, byte 10";
	descriptions[0x12] = L"Default OT Name, byte 11";
	descriptions[0x13] = L"Default OT Name, byte 12";
	descriptions[0x14] = L"Default OT Name, byte 13";
	descriptions[0x15] = L"Default OT Name, byte 14";
	descriptions[0x16] = L"Default OT Name, byte 15";
	descriptions[0x17] = L"Default OT Name, byte 16";
	descriptions[0x18] = L"Default OT Name, byte 17";
	descriptions[0x19] = L"Default OT Name, byte 18";
	descriptions[0x1A] = L"Default OT Name, byte 19";
	descriptions[0x1B] = L"Default OT Name, byte 20";
	descriptions[0x1C] = L"Default OT Name, byte 21";
	descriptions[0x1D] = L"Default OT Name, byte 22";
	descriptions[0x1E] = L"Default OT Name, byte 23";
	descriptions[0x1F] = L"Default OT Name, byte 24";
	descriptions[0x20] = L"Default Nationality";
	descriptions[0x21] = L"Edit during transfers (0: NO/1:YES)";
	descriptions[0x22] = L"Default day";
	descriptions[0x23] = L"Default month";
	descriptions[0x24] = L"Default year";
	descriptions[0x25] = L"Fix storage bad sectors on exit (0: NO/1:YES)";
	
	int byteEntry = 0, speed = 0;
	
	while(aptMainLoop() && !(hidKeysDown() & KEY_B))
	{
		touchPosition touch;
		hidScanInput();
		hidTouchRead(&touch);
		byteEntry = calcCurrentEntryOneScreen(byteEntry, CONFIG_USED - 1, 16);
		
		bool downPlus = ((hidKeysDown() & KEY_TOUCH) && touch.px > 247 && touch.px < 264 && touch.py > 31 && touch.py < 49) || (hidKeysDown() & KEY_A);
		bool downMinus = ((hidKeysDown() & KEY_TOUCH) && touch.px > 224 && touch.px < 241 && touch.py > 31 && touch.py < 49) || (hidKeysDown() & KEY_X);
		bool heldPlus = ((hidKeysHeld() & KEY_TOUCH) && touch.px > 247 && touch.px < 264 && touch.py > 31 && touch.py < 49) || (hidKeysHeld() & KEY_A);
		bool heldMinus = ((hidKeysHeld() & KEY_TOUCH) && touch.px > 224 && touch.px < 241 && touch.py > 31 && touch.py < 49) || (hidKeysHeld() & KEY_X);
		
		if (heldMinus && heldPlus)
			speed = 0;
		else if (downMinus)
		{
			if (config_buf[byteEntry] > 0)
				config_buf[byteEntry]--;
		}
		else if (heldMinus)
		{
			if (speed < -30 && config_buf[byteEntry] > 0)
				config_buf[byteEntry]--;
			else
				speed--;
		}
		else if (downPlus)
		{
			if (config_buf[byteEntry] < 0xFF)
				parseConfigHexEditor(byteEntry);
		}
		else if (heldPlus)
		{
			if (speed > 30 && config_buf[byteEntry] < 0xFF)
				parseConfigHexEditor(byteEntry);
			else
				speed++;
		}
		else
			speed = 0;
		
		config_fill_values();
		printSettings(config_buf, byteEntry, CONFIG_USED, descriptions);
	}
	
	config_set();
}
