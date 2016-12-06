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
#include <string.h>
#include <3ds.h>
#include "http.h"
#include "graphic.h"
#include "certs/cybertrust.h"
#include "certs/digicert.h"

Result http_download(u8* buf, char *url) {
	httpcInit(0);
    httpcContext context;
	Result ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    u32 statuscode = 0;
    u32 contentsize = 0;

    ret = httpcAddRequestHeaderField(&context, (char*)"User-Agent",  (char*)"EventAssistant");
    if (ret != 0) {
        infoDisp("Failed to add a request header field!");
        return ret;
    }
	
	ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0) {
        infoDisp("Failed to set SSLOpt!");
        return ret;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    ret = httpcBeginRequest(&context);
    if (ret != 0) {
        infoDisp("Failed to begin a http request!");
        return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0) {
        infoDisp("Failed to receive a status code!");
        return ret;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024);
            httpcCloseContext(&context);
            ret = http_download(buf, url);
            infoDisp("Redirection failed!");
			return ret;
        }
		infoDisp("Redirection failed!");
		return ret;
    }

    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0) {
        infoDisp("Failed to receive download size!");
        return ret;
    }

    memset(buf, 0, 1499);
    ret = httpcDownloadData(&context, buf, contentsize, NULL);
    if (ret != 0) {
        free(buf);
        infoDisp("Failed to download file!");
        return ret;
    }
	httpcCloseContext(&context);
	httpcExit();

    return 0;
}

Result downloadFile(char* url, char* path) {
    httpcInit(0);
    httpcContext context;
    Result ret = 0;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    if (ret != 0) {
        infoDisp("Failed to open http context!");
        return ret;
    }

    ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
    if (ret != 0) {
        infoDisp("Failed to add a request header field!");
        return ret;
    }
	
    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0) {
        infoDisp("Failed to set SSLOpt!");
        return ret;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    ret = httpcBeginRequest(&context);
    if (ret != 0) {
        infoDisp("Failed to begin a http request!");
        return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0) {
        infoDisp("Failed to receive a status code!");
        httpcCloseContext(&context);
        return ret;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            ret = httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024);
            if (ret != 0) {
                infoDisp("Redirection failed!");
                return ret;
            }
            httpcCloseContext(&context);
            ret = downloadFile(newUrl, path);
            return ret;
        } else {
            infoDisp("Redirection failed!");
            httpcCloseContext(&context);
            return -1;
        }
    }

    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0) {
        infoDisp("Failed to receive download size!");
        httpcCloseContext(&context);
        return ret;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
		free(buf);
        infoDisp("Failed to alloc memory!");
        return -2;
    }
    memset(buf, 0, contentsize);

    ret = httpcDownloadData(&context, buf, contentsize, NULL);
    if (ret != 0) {
        free(buf);
        infoDisp("Failed to download file!");
        httpcCloseContext(&context);
        return ret;
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