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

#include "events.h"

static const char *tags[] = {"jpn", "eng", "fre", "ita", "ger", "spa", "kor", "chs", "cht"};

int getI(char* str, bool is3ds)
{
	int i = 0, mult = 1;
	for (int k = is3ds ? 3 : 2; k >= 0; k--)
	{
		i += (int)(str[k] - '0') * mult;
		mult *= 10;
	}
	return i;
}

void getSinglePathPGF(char* path, const int lang, const int i)
{
	sprintf(path, "romfs:/pgf/%s/%d.pgf", tags[lang], i);
}

void getSinglePathPGT(char* path, const int lang, const int i)
{
	sprintf(path, "romfs:/pgt/%s/%d.pgt", tags[lang], i);
}

void getSinglePathWCX(char* path, const int lang, const int i)
{
	sprintf(path, "romfs:/wcx/%s/%d.wcx", tags[lang], i);
}

void getMultiplePathWCX(char* path, const int lang, const int i, const int j)
{
	sprintf(path, "romfs:/wcx/%s/%d-%d.wcx", tags[lang], i, j);
}

void getSinglePathWCXFull(char* path, const int lang, const int i)
{
	sprintf(path, "romfs:/wcxfull/%s/%d.wcxfull", tags[lang], i);
}

void getMultiplePathWCXFull(char* path, const int lang, const int i, const int j)
{
	sprintf(path, "romfs:/wcxfull/%s/%d-%d.wcxfull", tags[lang], i, j);
}

void patchWondercardDate(u8* buf)
{
	if (game_is3DS())
	{
		u32 rawdate = PKSM_Configuration.defaultYear * 10000 + PKSM_Configuration.defaultMonth * 100 + PKSM_Configuration.defaultDay;
		wcx_set_rawdate(buf, rawdate);
	}
}

void reloadPreviewBuf(u8* previewBuf, const int i, const int n)
{
	char testpath[40];
	if (game_isgen7())
	{
		getSinglePathWCXFull(testpath, n, i);
	}
	else if (game_isgen6())
	{
		getSinglePathWCX(testpath, n, i);
	}
	else if (game_isgen5())
	{
		getSinglePathPGF(testpath, n, i);
	}
	else if (game_isgen4())
	{
		getSinglePathPGT(testpath, n, i);
	}
	
	const u16 wcSize = ofs.wondercardSize;
	FILE* f = fopen(testpath, "r");
	if (f)
	{ 
		fseek(f, 0, SEEK_END);
		u32 sz = ftell(f);
		memset(previewBuf, 0, wcSize);
		rewind(f);
		if (sz == WCXFULL_SIZE)
		{
			fpos_t pos = 520;
			fsetpos(f, &pos);
		}
		fread(previewBuf, wcSize, 1, f);
	}
	fclose(f);
	
	patchWondercardDate(previewBuf);
}

void reloadMultiplePreviewBuf(u8* previewBuf, const int i, const int n, const int j)
{
	char testpath[40];
	if (game_isgen7())
	{
		getMultiplePathWCXFull(testpath, n, i, j);
	}	
	else if (game_isgen6())
	{
		getMultiplePathWCX(testpath, n, i, j);
	}
	else
	{
		return;
	}
	
	const u16 wcSize = ofs.wondercardSize;
	FILE* f = fopen(testpath, "r");
	if (f)
	{ 
		fseek(f, 0, SEEK_END);
		u32 sz = ftell(f);
		memset(previewBuf, 0, wcSize);
		rewind(f);
		if (sz == WCXFULL_SIZE)
		{
			fpos_t pos = 520;
			fsetpos(f, &pos);
		}
		fread(previewBuf, wcSize, 1, f); 
	}
	fclose(f);
	
	patchWondercardDate(previewBuf);
}

int getFreeLocationWC(u8 *mainbuf)
{
	if (!game_is3DS())
	{
		return 0;
	}

	u8 t;
	int temp;
	for (t = 0; t < ofs.maxWondercards; t++)
	{
		temp = 0;
		for (u32 j = 0; j < ofs.wondercardSize; j++)
			if (*(mainbuf + ofs.wondercardLocation + t * ofs.wondercardSize + j) == 0x00)
				temp++;

		if (temp == ofs.wondercardSize)
		{
			break;
		}
	}
	
	return t == 0 ? ofs.maxWondercards - 1 : t;
}

void findFreeLocationWC(u8 *mainbuf, int nInjected[])
{
	nInjected[0] = getFreeLocationWC(mainbuf);
}

int getN(const int i) 
{
	// we decided to not handle multiple wcs
	// for gens prior to 6
	if (!game_is3DS())
	{
		return 0;
	}
	
	switch (i)
	{
		case 1 : return 10;
		case 48 : return 4;
		case 71 : return 5;
		case 81 : return 11;
		case 82 : return 6;
		case 108 : return 2;
		case 132 : return 5;
		case 133 : return 5;
		case 134 : return 5;
		case 135 : return 5;
		case 136 : return 5;
		case 142 : return 5;
		case 184 : return 11;
		case 201 : return 3;
		case 204 : return 7; 		
		case 205 : return 2;
		case 211 : return 7;
		case 221 : return 6;
		case 223 : return 10;
		case 230 : return 8;
		case 238 : return 2;
		case 243 : return 2;
		case 244 : return 3;
		case 245 : return 3;
		case 250 : return 3;
		case 264 : return 7;
		case 266 : return 4;
		case 273 : return 3;
		case 504 : return 2;
		case 515 : return 2;
		case 551 : return 2;
		case 552 : return 2;
		case 614 : return 2;
		case 627 : return 6;
		case 1111 : return 6;
		case 1114 : return 8;
		case 2016 : return 3;		
	}	
	return 0;
}

void eventDatabase(u8* mainbuf) {
	const u32 dbCount = game_is3DS() ? SMCOUNT : (game_isgen5() ? 170 : 190);
	const u16 wcSize = ofs.wondercardSize;
	
	char *database[dbCount];
	int *spriteArray = (int*)malloc(dbCount * sizeof(int));
	u8 *previewbuf = (u8*)malloc(wcSize);
	memset(previewbuf, 0, wcSize);
	
	if (game_isgen7())
	{
		filldatabase7(database, spriteArray);
	}
	else if (game_isgen6())
	{
		filldatabase6(database, spriteArray);
	}
	else if (game_isgen5())
	{
		filldatabase5(database, spriteArray);
	}
	else if (game_isgen4())
	{
		filldatabase4(database, spriteArray);
	}
	else
	{
		free(spriteArray);
		free(previewbuf);
		return;
	}
	
	int currentEntry = 0;
	int page = 0;
	
	bool adapt = false;
	bool langVett[9];
	bool overwrite = false;
	int nInjected[1] = {0};
	int langSelected = -1;
	int currentMultipleWCSelected = 0;
	
	findFreeLocationWC(mainbuf, nInjected);
	
	while(aptMainLoop())
	{
		hidScanInput();
		calcCurrentEntryMorePagesReversed(&currentEntry, &page, fill_get_index()/10, 9, 5);
		
		if (hidKeysDown() & KEY_B)
		{
			break;
		}

		// if an entry is pressed and is valid
		if (hidKeysDown() & KEY_A && spriteArray[page*10+currentEntry] != -1)
		{
			const int total = (game_isgen7()) ? 9 : 7;
			int i = getI(database[page * 10 + currentEntry], game_is3DS() ? true : false);
			
			// check for single events
			char testpath[40];
			for (int j = 0; j < total; j++)
			{
				if (game_isgen7())
				{
					getSinglePathWCXFull(testpath, j, i);
				}
				else if (game_isgen6())
				{
					getSinglePathWCX(testpath, j, i);
				}
				else if (game_isgen5())
				{
					getSinglePathPGF(testpath, j, i);
				}
				else if (game_isgen4())
				{
					getSinglePathPGT(testpath, j, i);
				}

				FILE* f = fopen(testpath, "r");
				langVett[j] = f ? true : false;
				fclose(f); 
			}
			
			//check for multiple wcx events
			int k, n = getN(i);
			if (n != 0)
			{
				for (int j = 0; j < total; j++)
				{
					k = 0;
					for (int t = 0; t < n; t++)
					{	
						if (game_isgen7())
						{
							getMultiplePathWCXFull(testpath, j, i, t + 1);
						}
						else if (game_isgen6())
						{
							getMultiplePathWCX(testpath, j, i, t + 1);
						}

						FILE* f = fopen(testpath, "r");
						if (f)
						{
							k++;
							if (currentMultipleWCSelected == 0)
							{
								currentMultipleWCSelected = t + 1;
							}
						}
						fclose(f); 
					}
					langVett[j] = (k == n) ? true : false;
				}
			}

			// set first lang selected
			langSelected = -1;
			for (int i = 0; i < total && langSelected == -1; i++)
			{
				if (langVett[i])
				{
					langSelected = i;
				}
			}
			
			if (langSelected != -1)
			{
				reloadPreviewBuf(previewbuf, i, langSelected);
				reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);					
			}
			
			while (aptMainLoop())
			{
				hidScanInput();
				touchPosition touch;
				hidTouchRead(&touch);
				
				if (hidKeysDown() & KEY_B)
				{
					currentMultipleWCSelected = 0;
					break;
				}
				
				// in case of multiple WCs for the same ID, switch'em all
				if ((hidKeysDown() & KEY_L) || (hidKeysDown() & KEY_R))
				{
					int n = getN(i);
					if (n != 0)
					{
						currentMultipleWCSelected--;
						if (currentMultipleWCSelected == 0)
						{
							currentMultipleWCSelected = n;
						}
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
				}
				
				if ((hidKeysHeld() & KEY_L) && (hidKeysHeld() & KEY_R) && game_is3DS())
				{
					camera_set_qrmode(true);
					camera_init();
					
					while(camera_get_qrmode())
					{
						camera_take_qr(previewbuf, MODE_WCX);
					}
				}

				// wireless injection toggle
				if (hidKeysDown() & KEY_Y)
				{
					// allowing wireless injection for games prior to gen6
					// is kinda useless since we already have almost all the original wondercards
					if (game_is3DS())
					{
						if (!socket_init())
							break;
						
						do {
							hidScanInput();
							process_wcx(previewbuf);
							printEventInjector(previewbuf, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0], EVENTS_OTA);
						} while (aptMainLoop() && !(hidKeysDown() & KEY_B));
						socket_shutdown();						
					}
				}
				
				// slot selection
				if (hidKeysDown() & KEY_TOUCH && touch.px > 251 && touch.px < 287 && touch.py > 168 && touch.py < 193)
				{
					if (game_is3DS())
					{
						int max = getFreeLocationWC(mainbuf);
						int entry = 0;
						int page = 0, maxpages = ofs.maxWondercards / 40 + 1;
						while(aptMainLoop() && !(hidKeysDown() & KEY_B))
						{
							hidScanInput();
							calcCurrentEntryMorePages(&entry, &page, maxpages, max, 8);
							printEventInjector(mainbuf, entry, page, langVett, adapt, overwrite, langSelected, max, EVENTS_SLOT);
							
							if (hidKeysDown() & KEY_X)
							{
								u8 empty[ofs.wondercardSize];
								memset(empty, 0, ofs.wondercardSize);
								u8 tmp[ofs.wondercardSize];
								memcpy(tmp, mainbuf + ofs.wondercardLocation + entry*ofs.wondercardSize, ofs.wondercardSize);
								if (memcmp(tmp, empty, ofs.wondercardSize))
								{
									u8 title[72] = {0};
									char path[150] = {0};
									ssize_t len = wcx_title(tmp, title);
									if (len > 0 && wcx_get_id(tmp) != 145)
									{
										sprintf(path, "%d - %.*s.%s", (int)wcx_get_id(tmp), len, title, game_isgen6() ? "wc6" : "wc7");
									}
									else
									{
										sprintf(path, "%d.%s", (int)wcx_get_id(tmp), game_isgen6() ? "wc6" : "wc7");
									}
									
									char dmppath[100];
									time_t unixTime = time(NULL);
									struct tm* timeStruct = gmtime((const time_t *)&unixTime);
									snprintf(dmppath, 100, "sdmc:/3ds/PKSM/dump/%04i%02i%02i", 
											timeStruct->tm_year + 1900, 
											timeStruct->tm_mon + 1, 
											timeStruct->tm_mday);
									mkdir(dmppath, 777);
									chdir(dmppath);

									file_write(path, tmp, ofs.wondercardSize);
									infoDisp(L"Wondercard extracted!");
								}
							}
							
							if (hidKeysDown() & KEY_A)
							{
								nInjected[0] = entry + 40*page;
								break;
							}
						}
					}
				}
				
				if (hidKeysHeld() & KEY_TOUCH)
				{
					bool languageTouched = false;
					if (touch.px > 114 && touch.px < 150 && touch.py > 50 && touch.py < 71 && langVett[0]) {
						langSelected = 0;
						languageTouched = true;
					}
					if (touch.px > 153 && touch.px < 189 && touch.py > 50 && touch.py < 71 && langVett[1]) {
						langSelected = 1;
						languageTouched = true;
					}
					if (touch.px > 192 && touch.px < 228 && touch.py > 50 && touch.py < 71 && langVett[2]) {
						langSelected = 2;
						languageTouched = true;
					}
					if (touch.px > 231 && touch.px < 267 && touch.py > 50 && touch.py < 71 && langVett[3]) {
						langSelected = 3;
						languageTouched = true;
					}
					if (touch.px > 270 && touch.px < 306 && touch.py > 50 && touch.py < 71 && langVett[4]) {
						langSelected = 4;
						languageTouched = true;
					}
					if (touch.px > 153 && touch.px < 189 && touch.py > 74 && touch.py < 95 && langVett[5]) {
						langSelected = 5;
						languageTouched = true;
					}
					if (touch.px > 192 && touch.px < 228 && touch.py > 74 && touch.py < 95 && langVett[6]) {
						langSelected = 6;
						languageTouched = true;
					}
					
					if (total == 9)
					{
						if (touch.px > 231 && touch.px < 267 && touch.py > 74 && touch.py < 95 && langVett[7]) {
							langSelected = 7;
							languageTouched = true;
						}
						if (touch.px > 270 && touch.px < 306 && touch.py > 74 && touch.py < 95 && langVett[8]) {
							langSelected = 8;
							languageTouched = true;
						}
					}
					
					if (languageTouched)
					{
						reloadPreviewBuf(previewbuf, i, langSelected);
						reloadMultiplePreviewBuf(previewbuf, i, langSelected, currentMultipleWCSelected);
					}
					
					if (touch.px > 231 && touch.px < 267 && touch.py > 110 && touch.py < 131) {
						overwrite = true;
						nInjected[0] = 0;
					}
					if (touch.px > 270 && touch.px < 306 && touch.py > 110 && touch.py < 131) {
						overwrite = false;
						findFreeLocationWC(mainbuf, nInjected);
					}
					
					if (touch.px > 231 && touch.px < 267 && touch.py > 138 && touch.py < 159) adapt = true;
					if (touch.px > 270 && touch.px < 306 && touch.py > 138 && touch.py < 159) adapt = false;
				}

				// inject the wondercards
				if (hidKeysDown() & KEY_START)
				{	
					// Eon Ticket is not available in XY
					if (game_getisXY() && i == 2048)
					{
						infoDisp(i18n(S_DATABASE_ITEM_NOT_AVAILABLE_XY));
						break;
					}

					if (game_is3DS())
					{
						if (!overwrite)
						{
							findFreeLocationWC(mainbuf, nInjected);
						}

						if (adapt)
						{
							setSaveLanguage(mainbuf, langSelected);
						}
					}
					
					// reached last slot, reset the slot to 0
					if (nInjected[0] >= ofs.maxWondercards)
					{
						nInjected[0] = 0;
					}

					setWC(mainbuf, previewbuf, i, nInjected);
					findFreeLocationWC(mainbuf, nInjected);
					infoDisp(i18n(S_DATABASE_SUCCESS_INJECTION));
					break;
				}
				
				printEventInjector(previewbuf, spriteArray[i], i, langVett, adapt, overwrite, langSelected, nInjected[0], EVENTS_DEFAULT);
			}
		}
		
		printEventList(database, currentEntry, page, spriteArray);
	}
	
	free(spriteArray);
	free(previewbuf);
}