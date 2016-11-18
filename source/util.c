/*
* This file is part of PKSM
* Copyright (C) 2016 Bernardo Giordano
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

#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "editor.h"
#include "http.h"
#include "util.h"
#include "graphic.h"

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

void loadFile(u8* buf, char* path) {
	FILE *fptr = fopen(path, "rt");
	if (fptr == NULL)
		return;
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	memset(buf, 0, 1499);
	rewind(fptr);
	fread(buf, size, 1, fptr);
	fclose(fptr);
}

void injectFromFile(u8* mainbuf, char* path, u32 offset) {
	FILE *fptr = fopen(path, "rt");
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

	memcpy(&mainbuf[offset], buf, size);
	free(buf);
}

int autoupdater() {
	int temp = 0;
	char* ver = (char*)malloc(6 * sizeof(u8));
	snprintf(ver, 6, "%d.%d.%d", V1, V2, V3);

	Result ret = downloadFile("https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/resources/ver.ver", "/3ds/data/PKSM/builds/ver.ver");
	if (ret != 0) {
		free(ver);
		return 1;
	}

	FILE *fptr = fopen("3ds/data/PKSM/builds/ver.ver", "rt");
	if (fptr == NULL) {
		fclose(fptr);
		free(ver);
		return 15;
	}
	fseek(fptr, 0, SEEK_END);
	u32 contentsize = ftell(fptr);
	char *verbuf = (char*)malloc(contentsize);
	if (verbuf == NULL) {
		fclose(fptr);
		free(verbuf);
		free(ver);
		return 8;
	}
	rewind(fptr);
	fread(verbuf, contentsize, 1, fptr);
	fclose(fptr);

	remove("/3ds/data/PKSM/builds/ver.ver");

	for (int i = 0; i < 5; i++)
		if (*(ver + i) == *(verbuf + i))
			temp++;

	free(ver);
	free(verbuf);

	if (temp < 5) {
		update();
		return 1;
	}
	return 0;
}

void update() {
	char *ciaUrl = "https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/PKSM/PKSM.cia";
	char *ciaPath = (char*)malloc(100 * sizeof(char));

	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);

	snprintf(ciaPath, 100, "/3ds/data/PKSM/builds/PKSM_%i-%i-%i-%02i%02i%02i.cia", timeStruct->tm_mday, timeStruct->tm_mon + 1,  timeStruct->tm_year + 1900, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);

	freezeMsg("Downloading latest build...");
	Result ret = downloadFile(ciaUrl, ciaPath);

	if (ret == 0) infoDisp("Download succeeded!");
	else infoDisp("Download failed!");

	free(ciaPath);
	infoDisp("Close the app!");
}

bool isHBL() {
    u64 id;
    APT_GetProgramID(&id);

    return id != 0x000400000EC10000;
}

void fsStart() {
    if(isHBL()) {
        Handle fsHandle;
        srvGetServiceHandleDirect(&fsHandle, "fs:USER");
        FSUSER_Initialize(fsHandle);
        fsUseSession(fsHandle);
    }
}

void fsEnd() {
    if(isHBL())
        fsEndUseSession();
}

bool openSaveArch(FS_Archive *out, u64 id) {
	bool ret = false;
	//Try cart first
	u32 path[3] = {MEDIATYPE_GAME_CARD, id, id >> 32};
	Result res = FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, path});
	if (res) {
		//Try SD
		u32 path[3] = {MEDIATYPE_SD, id, id >> 32};
		res = FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 0xC, path});
		if (res)
			ret = false;
		else
			ret = true;
	}
	else
		ret = true;

	return ret;
}
