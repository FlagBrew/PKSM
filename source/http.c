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

#include "http.h"
#include "certs/digicert.h"
#include "certs/cybertrust.h"

bool updating = false;

void update() {
	freezeMsg(L"Calling github APIs...");
	
	char* apiurl = "https://api.github.com/repos/BernardoGiordano/PKSM/releases/latest";
	char* apipath = "sdmc:/3ds/data/PKSM/apiresponse.txt";
	char* downloadpath = "sdmc:/3ds/data/PKSM/latest.cia";
	
	if (!downloadFile(apiurl, apipath)) {
		freezeMsg(L"Elaborating API response...");
		
		u8* apiresponse;
		
		FILE *apiptr = fopen(apipath, "rt");
		if (apiptr == NULL)
			return;
		fseek(apiptr, 0, SEEK_END);
		u32 apiresponsesize = ftell(apiptr);
		apiresponse = (u8*)malloc(apiresponsesize);
		memset(apiresponse, 0, apiresponsesize);
		rewind(apiptr);
		fread(apiresponse, apiresponsesize, 1, apiptr);
		fclose(apiptr);
		
		char* pointertourl = strstr((char*)apiresponse, "browser_download_url");
		char tokenbuffer[100];
		memcpy(tokenbuffer, pointertourl + 22, 100);
		char* releaseurl = strtok(tokenbuffer, "\"");
		//debuglogf(releaseurl + 40);
		
		free(apiresponse);
		
		freezeMsg(L"Downloading update...");
		updating = true;
		if (!downloadFile(releaseurl, downloadpath)) {
			freezeMsg(L"Installing update...");
			
			u8* newcia;
			
			FILE *ciaptr = fopen(apipath, "rt");
			if (ciaptr == NULL)
				return;
			fseek(ciaptr, 0, SEEK_END);
			u32 ciasize = ftell(ciaptr);
			newcia = (u8*)malloc(ciasize);
			memset(newcia, 0, ciasize);
			rewind(ciaptr);
			fread(newcia, ciasize, 1, ciaptr);
			fclose(ciaptr);
					
			Handle handle;
			AM_QueryAvailableExternalTitleDatabase(NULL);
			AM_StartCiaInstall(MEDIATYPE_SD, &handle);
			FSFILE_Write(handle, NULL, 0, newcia, (u32)ciasize, 0);
			AM_FinishCiaInstall(handle);

			free(newcia);
			infoDisp(L"Update installed!");
		} else {
			infoDisp(L"Update failed!");
		}
	} else {
		infoDisp(L"Update failed!");
	}
	
	remove(apipath);
	remove(downloadpath);
	
	updating = false;
}

Result downloadFile(char* url, char* path) {
    httpcInit(0);
    httpcContext context;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    if (httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0)) {
        infoDisp(i18n(S_HTTP_HTTP_CONTEXT_OPEN_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }

    if (httpcAddRequestHeaderField(&context, "User-Agent", "PKSM")) {
        infoDisp(i18n(S_HTTP_ADD_REQUEST_HEADER_FIELD_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }
	
	if (updating) {
		if (httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len)) {
			infoDisp(L"Failed to add cert!");
			return -1;			
		}
		if (httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len)) {
			infoDisp(L"Failed to add cert!");
			return -1;			
		}
	} else {
		if (httpcSetSSLOpt(&context, SSLCOPT_DisableVerify)) {
			infoDisp(i18n(S_HTTP_SET_SSLOPT_FAILED));
			infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
			return -1;
		}
	}
	
	httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

    if (httpcBeginRequest(&context)) {
        infoDisp(i18n(S_HTTP_BEGIN_HTTP_REQUEST_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }

    if (httpcGetResponseStatusCode(&context, &statuscode)) {
        infoDisp(i18n(S_HTTP_RECEIVE_STATUS_CODE_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[0x1000];
            if (httpcGetResponseHeader(&context, (char*)"Location", newUrl, 0x1000)) {
                infoDisp(i18n(S_HTTP_REDIRECTION_FAILED));
				infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
                return -1;
            }
            httpcCloseContext(&context);
            downloadFile(newUrl, path);
            return -1;
        } else {
            infoDisp(i18n(S_HTTP_REDIRECTION_FAILED));
			infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
            httpcCloseContext(&context);
            return -1;
        }
    }

    if (httpcGetDownloadSizeState(&context, NULL, &contentsize)) {
        infoDisp(i18n(S_HTTP_RECEIVE_DOWNLOAD_SIZE_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
		free(buf);
        infoDisp(i18n(S_HTTP_ALLOC_MEMORY_FAILED));
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        return -1;
    }
    memset(buf, 0, contentsize);

    if (httpcDownloadData(&context, buf, contentsize, NULL)) {
        free(buf);
		infoDisp(i18n(S_HTTP_DOWNLOAD_ASSETS_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    remove(path);
    FILE *fptr = fopen(path, "wb");
    fwrite(buf, 1, contentsize, fptr);
    fclose(fptr);
    free(buf);

    httpcCloseContext(&context);
    httpcExit();
    return 0;
}
