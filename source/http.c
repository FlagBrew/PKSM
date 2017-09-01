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
	freezeMsg(i18n(S_HTTP_CALLING_GITHUB_API));
	
	char* apiurl = "https://api.github.com/repos/BernardoGiordano/PKSM/releases/latest";
	char* apipath = "sdmc:/3ds/data/PKSM/apiresponse.txt";
	
	if (!downloadFile(apiurl, apipath)) {
		freezeMsg(i18n(S_HTTP_ELABORATING_API_RESPONSE));
		
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
		
		// check if latest is the current version
		char tag[100] = {0};
		sprintf(tag, "https://github.com/BernardoGiordano/PKSM/releases/tag/%d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_MICRO);
		if (strstr((char*)apiresponse, tag) != NULL) {
			infoDisp(i18n(S_HTTP_UPDATE_FAILED));
			remove(apipath);
			updating = false;
			return;
		}
		
		char* pointertourl = strstr((char*)apiresponse, "browser_download_url");
		char tokenbuffer[100];
		memcpy(tokenbuffer, pointertourl + 22, 100);
		char* releaseurl = strtok(tokenbuffer, "\"");
		
		free(apiresponse);
		
		freezeMsg(i18n(S_HTTP_DOWNLOADING_UPDATE));
		updating = true;
		downloadFile(releaseurl, "sdmc:/3ds/data/PKSM/release.cia");
	} else {
		infoDisp(i18n(S_HTTP_UPDATE_FAILED));
	}
	
	remove(apipath);
	
	updating = false;
}

Result downloadFile(char* url, char* path) {
	Result ret = 0;
    httpcInit(0);
    httpcContext context;
    u32 statuscode = 0;
    u32 contentsize = 0;

	ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    if (ret != 0) {
        infoDisp(i18n(S_HTTP_HTTP_CONTEXT_OPEN_FAILED));
		httpcCloseContext(&context);
        return -1;
    }

	ret = httpcAddRequestHeaderField(&context, "User-Agent", "PKSM");
    if (ret != 0) {
        infoDisp(i18n(S_HTTP_ADD_REQUEST_HEADER_FIELD_FAILED));
		httpcCloseContext(&context);
        return -1;
    }
	
	if (updating) {
		ret = httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
		if (ret != 0) {
			infoDisp(i18n(S_HTTP_ERROR_CERT));
			httpcCloseContext(&context);
			return -1;			
		}
		
		ret = httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);
		if (ret != 0) {
			infoDisp(i18n(S_HTTP_ERROR_CERT));
			httpcCloseContext(&context);
			return -1;			
		}
	} else {
		ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
		if (ret != 0) {
			infoDisp(i18n(S_HTTP_SET_SSLOPT_FAILED));
			httpcCloseContext(&context);
			return -1;
		}
	}
	
	ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");
	if (ret != 0) {
		infoDisp(i18n(S_HTTP_ERROR_REQUEST_HEADER));
		httpcCloseContext(&context);
		return -1;
	}

	ret = httpcBeginRequest(&context);
    if (ret != 0) {
        infoDisp(i18n(S_HTTP_BEGIN_HTTP_REQUEST_FAILED));
		httpcCloseContext(&context);
        return -1;
    }

	ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0) {
        infoDisp(i18n(S_HTTP_RECEIVE_STATUS_CODE_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[0x1000];
			
			ret = httpcGetResponseHeader(&context, (char*)"Location", newUrl, 0x1000);
            if (ret != 0) {
                infoDisp(i18n(S_HTTP_REDIRECTION_FAILED));
				httpcCloseContext(&context);
                return -1;
            }
			
            httpcCloseContext(&context);
            downloadFile(newUrl, path);
            return -1;
        } else {
            infoDisp(i18n(S_HTTP_REDIRECTION_FAILED));
            httpcCloseContext(&context);
            return -1;
        }
    }

	ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0) {
        infoDisp(i18n(S_HTTP_RECEIVE_DOWNLOAD_SIZE_FAILED));
        httpcCloseContext(&context);
        return -1;
    }

    u8 *buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
		free(buf);
        infoDisp(i18n(S_HTTP_ALLOC_MEMORY_FAILED));
		httpcCloseContext(&context);
        return -1;
    }
    memset(buf, 0, contentsize);

	ret = httpcDownloadData(&context, buf, contentsize, NULL);
    if (ret != 0) {
        free(buf);
		infoDisp(i18n(S_HTTP_ERROR_DOWNLOAD_DATA));
        httpcCloseContext(&context);
        return -1;
    }

	if (updating) {
		freezeMsg(i18n(S_HTTP_INSTALL_UPDATE));
	
		amInit();
		Handle handle;
		AM_TitleEntry metadata;
		u64 titleList[1] = {0x000400000EC10000};
		
		ret = AM_GetTitleInfo(MEDIATYPE_SD, 1, titleList, &metadata);
		if (ret == 0 && metadata.size != (u64)contentsize) {
			ret = AM_QueryAvailableExternalTitleDatabase(NULL);
			if (ret != 0) {
				wchar_t string[30];
				swprintf(string, 30, L"amquery: %08X", ret);
				infoDisp(string);
				return -1;
			}
			ret = AM_DeleteAppTitle(MEDIATYPE_SD, titleList[0]);
			if (ret != 0) {
				wchar_t string[30];
				swprintf(string, 30, L"deletetitle: %08X", ret);
				infoDisp(string);
				return -1;
			}
			ret = AM_StartCiaInstall(MEDIATYPE_SD, &handle);
			if (ret != 0) {
				wchar_t string[30];
				swprintf(string, 30, L"startinstall: %08X", ret);
				infoDisp(string);
				return -1;
			}
			ret = FSFILE_Write(handle, NULL, 0, buf, contentsize, 0);
			if (ret != 0) {
				wchar_t string[30];
				swprintf(string, 30, L"fsfile: %08X", ret);
				infoDisp(string);
				return -1;
			}
			ret = AM_FinishCiaInstall(handle);
			if (ret != 0) {
				wchar_t string[30];
				swprintf(string, 30, L"finish: %08X", ret);
				infoDisp(string);
				return -1;
			}
			
			infoDisp(i18n(S_HTTP_UPDATE_INSTALLED));
		} else {
			wchar_t string[30];
			swprintf(string, 30, L"titleinfo: %08X %llu %llu", ret, metadata.size, contentsize);
			infoDisp(string);
			infoDisp(L"Latest version already installed!");
		}
		
		amExit();
	} else {
		remove(path);
		file_write(path, buf, contentsize);	
	}
	
    free(buf);

    httpcCloseContext(&context);
    httpcExit();
    return 0;
}
