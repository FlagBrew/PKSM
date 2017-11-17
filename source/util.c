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

#include "util.h"

void checkMaxValueBetweenBounds(u8* buf, int byte, int start, int len, int max) {
	u16 buf2 = 0;
	int buf4 = 0;
	
	if (len == 2) {
		buf[byte]++;
		buf2 = *(u16*)(buf + start);
		if (buf2 > (u16)max)
			buf[byte]--;
	} else if (len == 4) {
		buf[byte]++;
		buf4 = *(u32*)(buf + start);
		if (buf4 > max)
			buf[byte]--;	
	}
}

void checkMaxValue(u8* pkmn, int byteEntry, int value, int max) {
	u8 temp = pkmn[byteEntry];
	pkmn[byteEntry]++;
	if (value > max)
		pkmn[byteEntry] = temp;
}

void file_write(const char* path, void *buf, int size) {
	FILE *file = fopen(path, "wb");
	fwrite(buf, 1, size, file);
	fclose(file);
}

bool checkFile(const char* path) {
	FILE *temp = fopen(path, "rt");
	if (temp == NULL) {
		fclose(temp);
		return false;
	}
	fclose(temp);
	return true;
}

void loadPersonal() {
	FILE *fptr = fopen("romfs:/personal/personal.bin", "rt");
	if (fptr == NULL) {
		fclose(fptr);
		return;
	}
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	u8 *buf = (u8*)malloc(size);
	if (buf == NULL) {
		fclose(fptr);
		free(buf);
		return;
	}
	rewind(fptr);
	fread(buf, size, 1, fptr);
	fclose(fptr);
	memcpy(personal.pkmData, buf, size);
	free(buf);
}

bool isHBL() {
#if ROSALINA_3DSX
	return false;
#else
	u64 id;
	APT_GetProgramID(&id);
	return id != 0x000400000EC10000;
#endif
}

void fsStart() {
    if (isHBL()) {
        Handle fsHandle;
        srvGetServiceHandleDirect(&fsHandle, "fs:USER");
        FSUSER_Initialize(fsHandle);
        fsUseSession(fsHandle);
    }
}

void fsEnd() {
    if (isHBL())
        fsEndUseSession();
}

bool openSaveArch(FS_Archive *out, u64 id) {
	if (id == 0x00040000000C9B00 || !isHBL()) { //If we're using Pokebank or CIA
		u32 cardPath[3] = {MEDIATYPE_GAME_CARD, id, id >> 32}; //Card
		if (R_FAILED(FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, cardPath}))) { //If that fails, try digital
			u32 sdPath[3] = {MEDIATYPE_SD, id, id >> 32};
			if (R_FAILED(FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, sdPath})))
				return false;
			else {
				return true;
			}
		}
		else
			return true;
	}
	else {
		if (R_SUCCEEDED(FSUSER_OpenArchive(out, ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, ""))))
			return true;
		else
			return false;
	}

	return false;
}

/// Comparison function used for sorting items, abilities and all things
int ArrayUTF32_sort_cmp_PKMN_Things_List(const wchar_t *a,const wchar_t *b) {
	int result = wcscmp(a, b);
	wchar_t* unknownStrings[] = { L"???", L"？？？", L"(?)" };
	int totalUnknownStrings = 3;

	for (int i = 0; i < totalUnknownStrings; i++) {
		if ((wcscmp(a, unknownStrings[i]) == 0 && wcscmp(b, unknownStrings[i]) != 0)
			|| (wcscmp(b, unknownStrings[i]) == 0 && wcscmp(a, unknownStrings[i]) != 0)) {
			result = -1*result;
		}

	}
	// We inversed the result when there is 1 "???", so "???" will be always at the end of the list
	return result;
}