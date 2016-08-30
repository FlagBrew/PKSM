/*
* This file is part of EventAssistant
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
#include "certs/cybertrust.h"
#include "certs/digicert.h"

Result http_download(PrintConsole topScreen, PrintConsole bottomScreen, httpcContext *context) {
    gfxFlushBuffers();
    gfxSwapBuffers();

    Result ret = 0;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    consoleSelect(&bottomScreen);

    ret = httpcAddRequestHeaderField(context, (char*)"User-Agent",  (char*)"EventAssistant");
    if (ret != 0)
        return ret;
	
	ret = httpcSetSSLOpt(context, 1 << 9);
    if (ret != 0)
        return ret;

    httpcAddTrustedRootCA(context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(context, digicert_cer, digicert_cer_len);

    ret = httpcBeginRequest(context);
    if (ret != 0)
        return ret;

    ret = httpcGetResponseStatusCode(context, &statuscode);
    if (ret != 0) {
        printf("\x1b[26;0HStatus: \x1b[31mNO INTERNET AVAILABLE\x1b[0m");
        return ret;
    }

    gfxFlushBuffers();
    gfxSwapBuffers();

    if (statuscode == 200)
        printf("\x1b[26;0HStatus: \x1b[32mOKAY                     \x1b[0m");
    else
        printf("\x1b[26;0HStatus: \x1b[31mPREVIEW NOT AVAILABLE YET\x1b[0m");

    gfxFlushBuffers();
    gfxSwapBuffers();

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            httpcGetResponseHeader(context, (char*)"Location", newUrl, 1024);
            httpcCloseContext(context);
            ret = http_download(topScreen, bottomScreen, context);
            return ret;
        }
        return -2;
    }

    ret = httpcGetDownloadSizeState(context, NULL, &contentsize);
    if (ret != 0)
        return ret;

    printf("\x1b[27;0HDownload size: %d bytes    ", (int)contentsize);

    gfxFlushBuffers();
    gfxSwapBuffers();

    buf = (u8*)malloc(contentsize);
    if (buf == NULL)
        return -1;
    memset(buf, 0, contentsize);

    ret = httpcDownloadData(context, buf, contentsize, NULL);
    if(ret != 0) {
        free(buf);
        return ret;
    }

    consoleSelect(&topScreen);
    printf("%s", buf);
    free(buf);
    return 0;
}

void getText(PrintConsole topScreen, PrintConsole bottomScreen, char *url) {
    Result ret = 0;
    httpcContext context;
    httpcInit(0);
    consoleSelect(&bottomScreen);
    gfxFlushBuffers();

    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);

    if (ret != 0)
        printf("\x1b[25;0HStatus: \x1b[31mURL NOT AVAILABLE\x1b[0m");
    gfxFlushBuffers();
    gfxSwapBuffers();

    if (ret == 0) {
        printf("\x1b[25;0HDownloading textual preview...");
        ret = http_download(topScreen, bottomScreen, &context);
        gfxFlushBuffers();
        httpcCloseContext(&context);
    }
    httpcExit();
}

void printDistro(PrintConsole topScreen, PrintConsole bottomScreen, char *url) {
    consoleSelect(&bottomScreen);
    printf("\x1b[2J");
    printf("----------------------------------------");
    printf("NA  - North America");
    printf("\nPAL - Europe, Australia");
    printf("\nJPN - Japan");
    printf("\nKOR - South Korea");
    printf("\nALL - All regions available\n");
    printf("----------------------------------------");
	printf("\x1b[29;8HTouch or press B to exit");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    getText(topScreen, bottomScreen, url);

	waitKey(KEY_B);
}

Result downloadFile(PrintConsole topScreen, PrintConsole bottomScreen, char* url, char* path) {
    fsInit();
    httpcInit(0);

    httpcContext context;
    Result ret = 0;
    u32 statuscode = 0;
    u32 contentsize = 0;
    u8 *buf;

    printf("Downloading file to sdmc:\x1b[32m%s\x1b[0m\n", path);

	consoleSelect(&bottomScreen);
    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    if (ret != 0) {
        errDisp(bottomScreen, 2, BOTTOM);
        return ret;
    }

    ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
    if (ret != 0) {
        errDisp(bottomScreen, 3, BOTTOM);
        return ret;
    }
	
    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0) {
        errDisp(bottomScreen, 4, BOTTOM);
        return ret;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    ret = httpcBeginRequest(&context);
    if (ret != 0) {
        errDisp(bottomScreen, 5, BOTTOM);
        return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0) {
        errDisp(bottomScreen, 6, BOTTOM);
        httpcCloseContext(&context);
        return ret;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            ret = httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024);
            if (ret != 0) {
                errDisp(bottomScreen, 7, BOTTOM);
                return ret;
            }
            httpcCloseContext(&context);
            printf("Retrying to call download function...\n\n");
            ret = downloadFile(topScreen, bottomScreen, newUrl, path);
            return ret;
        } else {
            errDisp(bottomScreen, 8, BOTTOM);
            httpcCloseContext(&context);
            return -1;
        }
    }

    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0) {
        errDisp(bottomScreen, 9, BOTTOM);
        httpcCloseContext(&context);
        return ret;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
        errDisp(bottomScreen, 10, BOTTOM);
        return -2;
    }
    memset(buf, 0, contentsize);

    ret = httpcDownloadData(&context, buf, contentsize, NULL);
    if (ret != 0) {
        free(buf);
        errDisp(bottomScreen, 11, BOTTOM);
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
    fsExit();

    return 0;
}