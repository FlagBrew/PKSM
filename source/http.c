/* This file is part of PKSM

Copyright (C) 2016 Bernardo Giordano

>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "http.h"
#include "graphic.h"
#include "certs/cybertrust.h"
#include "certs/digicert.h"

Result downloadFile(char* url, char* path, bool install) {
    httpcInit(0);
    httpcContext context;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    if (httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0)) {
        infoDisp("Failed to open http context!");
        return -1;
    }

    if (httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant")) {
        infoDisp("Failed to add a request header field!");
        return -1;
    }
	
    if (httpcSetSSLOpt(&context, 1 << 9)) {
        infoDisp("Failed to set SSLOpt!");
        return -1;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    if (httpcBeginRequest(&context)) {
        infoDisp("Failed to begin a http request!");
        return -1;
    }

    if (httpcGetResponseStatusCode(&context, &statuscode)) {
        infoDisp("Failed to receive a status code!");
        httpcCloseContext(&context);
        return -1;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            if (httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024)) {
                infoDisp("Redirection failed!");
                return -1;
            }
            httpcCloseContext(&context);
            downloadFile(newUrl, path, install);
            return -1;
        } else {
            infoDisp("Redirection failed!");
            httpcCloseContext(&context);
            return -1;
        }
    }

    if (httpcGetDownloadSizeState(&context, NULL, &contentsize)) {
        infoDisp("Failed to receive download size!");
        httpcCloseContext(&context);
        return -1;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
		free(buf);
        infoDisp("Failed to alloc memory!");
        return -1;
    }
    memset(buf, 0, contentsize);

    if (httpcDownloadData(&context, buf, contentsize, NULL)) {
        free(buf);
        infoDisp("Failed to download file!");
        httpcCloseContext(&context);
        return -1;
    }
	
	if (install) {
		amInit();
		Handle handle;
		AM_QueryAvailableExternalTitleDatabase(NULL);
		AM_StartCiaInstall(MEDIATYPE_SD, &handle);
		FSFILE_Write(handle, NULL, 0, buf, contentsize, 0);
		AM_FinishCiaInstall(handle);
		amExit();
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