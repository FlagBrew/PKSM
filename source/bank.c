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

#include "bank.h"

bool isMultipleSelection = false;
int multipleSelectionBuffer[MULTIPLESELECTIONSIZE][2];

void bank_free_multiple_selection_buffer() {
	for (int i = 0; i < MULTIPLESELECTIONSIZE; i++) {
		for (int j = 0; j < 2; j++) {
			multipleSelectionBuffer[i][j] = -1;
		}
	}
	
	isMultipleSelection = false;
}

bool bank_get_selected_slot(const int box, const int slot) {
	for (int i = 0; i < MULTIPLESELECTIONSIZE; i++) {
		if (multipleSelectionBuffer[i][0] == box &&
			multipleSelectionBuffer[i][1] == slot)
			return true;
	}
	
	return false;
}

void bank_free_multiple_selection_location(const int box, const int slot) {
	bool erased = false;
	for (int i = 0; i < MULTIPLESELECTIONSIZE && !erased; i++) {
		if (multipleSelectionBuffer[i][0] == box &&
			multipleSelectionBuffer[i][1] == slot) {
			
			multipleSelectionBuffer[i][0] = -1;
			multipleSelectionBuffer[i][1] = -1;
			erased = true;
		}		
	}
	
	// check if the buffer is empty
	bool empty = true;
	for (int i = 0; i < MULTIPLESELECTIONSIZE && empty; i++) {
		if (multipleSelectionBuffer[i][0] != 1 &&
			multipleSelectionBuffer[i][1] != -1)
			empty = false;
	}
	if (empty)
		isMultipleSelection = false;
}

void bank_set_selected_slot(const int box, const int slot) {
	bool inserted = false;
	for (int i = 0; i < MULTIPLESELECTIONSIZE && !inserted; i++) {
		if (multipleSelectionBuffer[i][0] == -1 &&
			multipleSelectionBuffer[i][1] == -1) {
			
			multipleSelectionBuffer[i][0] = box;
			multipleSelectionBuffer[i][1] = slot;
			
			isMultipleSelection = true;
			inserted = true;
		}		
	}	
}

bool isInternetWorking = false;

bool bank_getIsInternetWorking() {
	return isInternetWorking;
}

inline void setToWirelessBuf(u8* buf, int box, int slot, u8* pkmn) {
	memcpy(&buf[box*30*ofs.pkxLength + slot*ofs.pkxLength], pkmn, ofs.pkxLength);
}

inline void getFromWirelessBuf(u8* buf, int box, int slot, u8* pkmn) {
	memcpy(pkmn, &buf[box*30*ofs.pkxLength + slot*ofs.pkxLength], ofs.pkxLength);
}

void clearMarkings(u8* pkmn) {
	if (PKSM_Configuration.editInTransfers != 0)
		return;
	
	u8 version = pkmn[0xDF];
	if (!(version == 30 || version == 31) && !(version >= 35 && version <= 41) && !game_isgen7()) { // not SM
		pkmn[0x2A] = 0;
		pkmn[0x72] &= 0xFC;
		pkmn[0xDE] = 0;
		
		for (int i = 0x94; i < 0x9E; i++)
			pkmn[i] = 0;
		for (int i = 0xAA; i < 0xB0; i++)
			pkmn[i] = 0;
		for (int i = 0xE4; i < 0xE8; i++)
			pkmn[i] = 0;
		
		// trade memory
		u16 textvar = 0; // from bank
		pkmn[0xA4] = 1; 
		pkmn[0xA5] = 4; 
		pkmn[0xA6] = rand() % 10; // 0-9 bank 
		memcpy(&pkmn[0xA8], &textvar, 2);
	}
}

void dumpStorage2pk7(u8* bankbuf, u32 size) {
	char dmppath[100];
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);		
	snprintf(dmppath, 100, "sdmc:/3ds/PKSM/dump/storagedump_%02i%02i%02i%02i%02i%02i", 
			timeStruct->tm_year + 1900, 
			timeStruct->tm_mon + 1, 
			timeStruct->tm_mday, 
			timeStruct->tm_hour, 
			timeStruct->tm_min, 
			timeStruct->tm_sec);
	mkdir(dmppath, 777);
	chdir(dmppath);
	
	wchar_t step[20];		
	for (int i = 0, tot = size/ofs.pkxLength; i < tot; i++) {
		swprintf(step, 20, i18n(S_GRAPHIC_PKBANK_MESSAGE_DUMP), i + 1, tot);
		freezeMsg(step);
		
		u8 tmp[ofs.pkxLength];
		memcpy(&tmp[0], &bankbuf[i*ofs.pkxLength], ofs.pkxLength);
		if (pkx_get_species(tmp) > 0 && pkx_get_species(tmp) < 802) {
			char path[100];
			u8 nick[26*2];
			
			memset(nick, 0, 26*2);
			memset(path, 0, 100);
			
			pkx_get_nickname_u8(tmp, nick);
			sprintf(path, "%d - %s - %X.pk7", (int)pkx_get_species(tmp), nick, (int)pkx_get_pid(tmp));

			file_write(path, tmp, ofs.pkxLength);
		}
	}
}

void bank(u8* mainbuf) {
	int game = game_get();
	
	FILE *fptr = fopen("/3ds/PKSM/bank/bank.bin", "rt");
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	
	if (size % (30 * ofs.pkxLength)) {
		fclose(fptr);
		infoDisp(i18n(S_BANK_SIZE_ERROR));
		return;
	}
	u8 *bankbuf = (u8*)malloc(size * sizeof(u8));
	if (bankbuf == NULL) {
		fclose(fptr);
		free(bankbuf);
		return;
	}
	rewind(fptr);
	fread(bankbuf, size, 1, fptr);
	fclose(fptr);

	int speed = 0;
	int zSpeed = 0;
	bool isSeen = false;
	bool bufferizedfrombank = false;
	bool isBufferized = false;
	int saveBox = 0, bankBox = 0;
	int currentEntry = 30;
	int coordinate[2] = {0, 0};
	
	u8* wirelessbuf = (u8*)malloc(30*232*ofs.maxBoxes);
	memset(wirelessbuf, 0, 30*232*ofs.maxBoxes);
	bool isWirelessBuffer = false;
	
	u8* pkmn = (u8*)malloc(ofs.pkxLength * sizeof(u8));
	memset(pkmn, 0, ofs.pkxLength);
	
	bank_free_multiple_selection_buffer();
	
	while (aptMainLoop()) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		currentEntry = calcCurrentEntryOneScreen(currentEntry, 59, 6);

		if (speed >= -30 && (hidKeysDown() & KEY_R)) {
			if (currentEntry < 30) {
				if (bankBox < size / (30 * ofs.pkxLength) - 1) 
					bankBox++;
				else if (bankBox == size / (30 * ofs.pkxLength) - 1) 
					bankBox = 0;
			} else {
				if (saveBox < ofs.maxBoxes - 1) 
					saveBox++;
				else if (saveBox == ofs.maxBoxes - 1)
					saveBox = 0;
			}
		}
		
		if (speed <= 30 && (hidKeysDown() & KEY_L)) {
			if (currentEntry < 30) {
				if (bankBox > 0) 
					bankBox--;
				else if (bankBox == 0) 
					bankBox = size / (30 * ofs.pkxLength) - 1;
			} else {
				if (saveBox > 0) 
					saveBox--;
				else if (saveBox == 0) 
					saveBox = ofs.maxBoxes - 1;	
			}
		}
		
		if (hidKeysHeld() & KEY_R && hidKeysHeld() & KEY_L)
			speed = 0;
		else if (hidKeysHeld() & KEY_R) {
			if (speed > 30) {
				if (currentEntry < 30) {
					if (bankBox < size / (30 * ofs.pkxLength) - 1) 
						bankBox++;
					else if (bankBox == size / (30 * ofs.pkxLength) - 1) 
						bankBox = 0;
				} else {
					if (saveBox < ofs.maxBoxes - 1) 
						saveBox++;
					else if (saveBox == ofs.maxBoxes - 1) 
						saveBox = 0;
				}
			}
			else
				speed++;
		}
		else if (hidKeysHeld() & KEY_L) {
			if (speed < -30) {
				if (currentEntry < 30) {
					if (bankBox > 0) 
						bankBox--;
					else if (bankBox == 0) 
						bankBox = size / (30 * ofs.pkxLength) - 1;
				} else {
					if (saveBox > 0) 
						saveBox--;
					else if (saveBox == 0) 
						saveBox = ofs.maxBoxes - 1;	
				}
			}
			else
				speed--;
		}
		else
			speed = 0;

		if (zSpeed >= -30 && (hidKeysDown() & KEY_ZR)) {
			if (bankBox < size / (30 * ofs.pkxLength) - 1) 
				bankBox++;
			else if (bankBox == size / (30 * ofs.pkxLength) - 1) 
				bankBox = 0;
		}
		
		if (zSpeed <= 30 && (hidKeysDown() & KEY_ZL)) {
			if (bankBox > 0) 
				bankBox--;
			else if (bankBox == 0) 
				bankBox = size / (30 * ofs.pkxLength) - 1;
		}
		
		if (hidKeysHeld() & KEY_ZR && hidKeysHeld() & KEY_ZL)
			zSpeed = 0;
		else if (hidKeysHeld() & KEY_ZR) {
			if (zSpeed > 30) {
				if (bankBox < size / (30 * ofs.pkxLength) - 1) 
					bankBox++;
				else if (bankBox == size / (30 * ofs.pkxLength) - 1) 
					bankBox = 0;
			}
			else
				zSpeed++;
		}
		else if (hidKeysHeld() & KEY_ZL) {
			if (zSpeed < -30) {
				if (bankBox > 0) 
					bankBox--;
				else if (bankBox == 0) 
					bankBox = size / (30 * ofs.pkxLength) - 1;
			}
			else
				zSpeed--;
		}
		else
			zSpeed = 0;

		if (((hidKeysDown() & KEY_Y) || ((hidKeysDown() & KEY_TOUCH) && touch.px > 240 && touch.px < 276 && touch.py > 210 && touch.py < 240)) && !isBufferized) {
			isWirelessBuffer = isWirelessBuffer ? false : true;
			
			if (!isInternetWorking) {
				if (!socket_init())
					break;
				else
					isInternetWorking = true;
			} else {
				socket_shutdown();
				isInternetWorking = false;
			}
		}
		
		if (hidKeysDown() & KEY_B) {
			if (isBufferized) {
				if (bufferizedfrombank)
					memcpy(&bankbuf[coordinate[0] * 30 * ofs.pkxLength + coordinate[1] * ofs.pkxLength], pkmn, ofs.pkxLength);
				else if (!isWirelessBuffer)
					pkx_set_as_it_is(mainbuf, coordinate[0], coordinate[1], pkmn);
				else if (isWirelessBuffer)
					setToWirelessBuf(wirelessbuf, coordinate[0], coordinate[1], pkmn);
				
				memset(pkmn, 0, ofs.pkxLength);
				isBufferized = false;
			} else
				break;
		}
		
		if (hidKeysDown() & KEY_TOUCH) {
			if (touch.px > 7 && touch.px < 23 && touch.py > 17 && touch.py < 37) {
				if (saveBox > 0) 
					saveBox--;
				else if (saveBox == 0) 
					saveBox = ofs.maxBoxes - 1;
			}
			
			if (touch.px > 185 && touch.px < 201 && touch.py > 17 && touch.py < 37) {
				if (saveBox < ofs.maxBoxes - 1) 
					saveBox++;
				else if (saveBox == ofs.maxBoxes - 1) 
					saveBox = 0;
			}
			
			if ((touch.px > 208 && touch.px < 317 && touch.py > 70 && touch.py < 97) && !(isBufferized)) {
				if (confirmDisp(i18n(S_BANK_Q_ERASE_SELECTED_BOX))) {
					u8 tmp[ofs.pkxLength];
					memset(tmp, 0, ofs.pkxLength);
					for (u32 i = 0; i < 30; i++) {
						if (currentEntry < 30)
							memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + i * ofs.pkxLength], tmp, ofs.pkxLength);
						else if (!isBattleBoxed(mainbuf, saveBox, i) && !isWirelessBuffer)
							pkx_set_as_it_is(mainbuf, saveBox, i, tmp);
						else if (isWirelessBuffer)
							setToWirelessBuf(wirelessbuf, saveBox, i, tmp);
					}
				}
			}
			
			if (touch.px > 242 && touch.px < 285 && touch.py > 5 && touch.py < 25) {
				u8 buffer[ofs.pkxLength];
				u8 temp[ofs.pkxLength];
				memset(buffer, 0, ofs.pkxLength);
				memset(temp, 0, ofs.pkxLength);
				
				for (u32 i = 0; i < 30; i++) {
					if (!isBattleBoxed(mainbuf, saveBox, i) && !isWirelessBuffer) {
						pkx_get(mainbuf, saveBox, i, buffer); // pkx_get -> buffer
						memcpy(temp, &bankbuf[bankBox * 30 * ofs.pkxLength + i * ofs.pkxLength], ofs.pkxLength); // memcpy bank -> temp
						
						u16 species = pkx_get_species(temp);
						u8 form = pkx_get_form(temp);
						FormData *forms = pkx_get_legal_form_data(species, game);
						bool illegalform = form < forms->min || form > forms->max;
						bool illegalspecies = species > ofs.maxSpecies;
						free(forms);

						if (!(illegalspecies || illegalform)) { // prevent that gen7 stuff goes into gen6 save
							pkx_set(mainbuf, saveBox, i, temp); // pkx_set -> temp
							memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + i * ofs.pkxLength], buffer, ofs.pkxLength); // memcpy bank -> buffer
						}
					} else if (isWirelessBuffer) {
						getFromWirelessBuf(wirelessbuf, saveBox, i, buffer); // pkx_get -> buffer
						memcpy(temp, &bankbuf[bankBox * 30 * ofs.pkxLength + i * ofs.pkxLength], ofs.pkxLength); // memcpy bank -> temp
						setToWirelessBuf(wirelessbuf, saveBox, i, temp); // pkx_set -> temp
						memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + i * ofs.pkxLength], buffer, ofs.pkxLength); // memcpy bank -> buffer						
					}
				}
			}
			
			if ((touch.px > 208 && touch.px < 317 && touch.py > 97 && touch.py < 124) && !(isBufferized)) {
				u8 tmp[ofs.pkxLength];
				memset(tmp, 0, ofs.pkxLength);
				if (currentEntry < 30) 
					memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], tmp, ofs.pkxLength);
				else if (!isBattleBoxed(mainbuf, saveBox, currentEntry - 30) && !isWirelessBuffer)
					pkx_set_as_it_is(mainbuf, saveBox, currentEntry - 30, tmp);
				else if (isWirelessBuffer)
					setToWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, tmp);
			}
			
			if (touch.px > 208 && touch.px < 317 && touch.py > 153 && touch.py < 180) {
				int dexEntry = 0;
				int page = 0, maxpages = ofs.maxSpecies / 40 + 1;
				int seen = 0;
				int caught = 0;
				
				for (int i = 1; i <= ofs.maxSpecies; i++) {
					seen = getSeen(mainbuf, i) ? seen + 1 : seen;
					caught = getCaught(mainbuf, i) ? caught + 1 : caught;
				}
				
				while (aptMainLoop() && !(hidKeysDown() & KEY_B)) {
					hidScanInput();
					calcCurrentEntryMorePages(&dexEntry, &page, maxpages, 39, 8);
					
					printDexViewer(mainbuf, dexEntry, page, seen, caught);
				}
			}
			
			if (touch.px > 208 && touch.px < 317 && touch.py > 180 && touch.py < 210) {
				if (confirmDisp(i18n(S_GRAPHIC_PKBANK_MESSAGE_CONFIRM_DUMP))) {
					dumpStorage2pk7(bankbuf, size);
				} else {
					
				}
				
				hidScanInput();
			}
				
			if (touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240) break;
		}
		
		if (hidKeysHeld() & KEY_TOUCH) {
			int x_start, y_start = 45;
			for (int i = 0; i < 5; i++) {
				x_start = 4;
				for (int j = 0; j < 6; j++) {
					if ((touch.px > x_start) && (touch.px < (x_start + 34)) && (touch.py > y_start) && (touch.py < (y_start + 30)))
						currentEntry = 30 + i * 6 + j;
					x_start += 34;
				}
				y_start += 30;
			}
		}
		
		if (((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touch.px > 208 && touch.px < 317 && touch.py > 43 && touch.py < 74)) && !isBufferized) {
			isSeen = true;
			u8 tmp[ofs.pkxLength];
			memset(tmp, 0, ofs.pkxLength);
			
			if (currentEntry < 30)
				memcpy(tmp, &bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], ofs.pkxLength);
			else if (!isWirelessBuffer)
				pkx_get(mainbuf, saveBox, currentEntry - 30, tmp);
			else if (isWirelessBuffer)
				getFromWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, tmp);
			
			while (aptMainLoop() && pkx_get_species(tmp)) {
				hidScanInput();
				touchPosition touch;
				hidTouchRead(&touch);
				
				if ((hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touch.px > 280 && touch.px < 318 && touch.py > 210 && touch.py < 240)) 
					break;
				
				printPKBank(bankbuf, mainbuf, wirelessbuf, tmp, currentEntry, saveBox, bankBox, isBufferized, isSeen, isWirelessBuffer);
			}
			
			isSeen = false;
		}
		
 		if (hidKeysDown() & KEY_A) {
			if (isBufferized) {
				u16 species = pkx_get_species(pkmn);
				u8 form = pkx_get_form(pkmn);
				FormData *forms = pkx_get_legal_form_data(species, game);
				bool illegalform = form < forms->min || form > forms->max;
				bool illegalspecies = species > ofs.maxSpecies;
				free(forms);

				if (isWirelessBuffer || !((illegalspecies || illegalform) && currentEntry > 29)) { // prevent that gen7 stuff goes into gen6 save
					u8 tmp[ofs.pkxLength];
					
					if ((bufferizedfrombank == (currentEntry < 30)) && (coordinate[0] == ((currentEntry < 30) ? bankBox : saveBox)) && (coordinate[1] == currentEntry))
						//remains at the same place
						memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], pkmn, ofs.pkxLength);
					else if ((!bufferizedfrombank == (currentEntry > 29)) && (coordinate[0] == ((currentEntry < 30) ? bankBox : saveBox)) && (coordinate[1] == currentEntry - 30)) {
						//remains at the same place
						if (!isWirelessBuffer)
							pkx_set_as_it_is(mainbuf, saveBox, currentEntry - 30, pkmn);
						else if (isWirelessBuffer)
							setToWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, pkmn);
					}
					else if (currentEntry < 30) {
						memcpy(tmp, &bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], ofs.pkxLength);
						memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], pkmn, ofs.pkxLength);
						if (bufferizedfrombank) 
							memcpy(&bankbuf[coordinate[0] * 30 * ofs.pkxLength + coordinate[1] * ofs.pkxLength], tmp, ofs.pkxLength);
						else if (!isWirelessBuffer)
							pkx_set(mainbuf, coordinate[0], coordinate[1], tmp);
						else if (isWirelessBuffer)
							setToWirelessBuf(wirelessbuf, coordinate[0], coordinate[1], tmp);
					}
					else {
						if (!isWirelessBuffer) {
							pkx_get(mainbuf, saveBox, currentEntry - 30, tmp);
							setDex(mainbuf, pkmn);
							pkx_set(mainbuf, saveBox, currentEntry - 30, pkmn);
						}
						else if (isWirelessBuffer) {
							getFromWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, tmp);
							setToWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, pkmn);
						}

						if (bufferizedfrombank) 
							memcpy(&bankbuf[coordinate[0] * 30 * ofs.pkxLength + coordinate[1] * ofs.pkxLength], tmp, ofs.pkxLength);
						else if (!isWirelessBuffer)
							pkx_set(mainbuf, coordinate[0], coordinate[1], tmp);
						else if (isWirelessBuffer)
							setToWirelessBuf(wirelessbuf, coordinate[0], coordinate[1], tmp);
					}
					memset(pkmn, 0, ofs.pkxLength);
					isBufferized = false;
				}
			}
			else {
				if (isWirelessBuffer || (!(isBattleBoxed(mainbuf, saveBox, currentEntry - 30) && (currentEntry > 29)))) {
					u8 tmp[ofs.pkxLength];
					memset(tmp, 0, ofs.pkxLength);
				
					coordinate[0] = (currentEntry < 30) ? bankBox : saveBox;
					coordinate[1] = (currentEntry > 29) ? (currentEntry - 30) : currentEntry;
					
					if (currentEntry < 30) {
						memcpy(pkmn, &bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], ofs.pkxLength);
						clearMarkings(pkmn);
						if (pkx_get_species(pkmn) == 0 || pkx_get_species(pkmn) > ofs.maxSpecies) {
							memset(pkmn, 0, ofs.pkxLength);
							isBufferized = false;
						} else {
							memcpy(&bankbuf[bankBox * 30 * ofs.pkxLength + currentEntry * ofs.pkxLength], tmp, ofs.pkxLength);
							isBufferized = true;
						}
					} else {
						if (!isWirelessBuffer) {
							pkx_get(mainbuf, saveBox, currentEntry - 30, pkmn);
							clearMarkings(pkmn);
						} else if (isWirelessBuffer) {
							getFromWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, pkmn);
						}
						
						if (pkx_get_species(pkmn) == 0 || pkx_get_species(pkmn) > ofs.maxSpecies) {
							memset(pkmn, 0, ofs.pkxLength);
							isBufferized = false;
						} else {
							if (!isWirelessBuffer)
								pkx_set(mainbuf, saveBox, currentEntry - 30, tmp);
							else if (isWirelessBuffer)
								setToWirelessBuf(wirelessbuf, saveBox, currentEntry - 30, tmp);
							isBufferized = true;
						}
					}
					
					if (isBufferized)					
						bufferizedfrombank = (currentEntry < 30) ? true : false;
				}
			}
		}
		
		if (isInternetWorking) {
			process_bank(wirelessbuf);
		}
		
		printPKBank(bankbuf, mainbuf, wirelessbuf, pkmn, currentEntry, saveBox, bankBox, isBufferized, isSeen, isWirelessBuffer);
	}
	
	if (confirmDisp(i18n(S_BANK_Q_SAVE_CHANGES)))
		file_write("/3ds/PKSM/bank/bank.bin", bankbuf, size);
	
	free(bankbuf);
	free(wirelessbuf);
}
