#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <time.h>
#include "util.h"
#include "inject.h"
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
    printf("\x1b[29;12HPress B to exit.");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    getText(topScreen, bottomScreen, url);

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        if (hidKeysDown() & KEY_B)
            break;
    }
}

void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int page) {
    consoleSelect (&bottomScreen);
    printf("\x1b[7;0HPage %d/7 ", page);
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    getText(topScreen, bottomScreen, url);
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

    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    if (ret != 0) {
        printf("Error in: \x1b[31mhttpcOpenContext\x1b[0m. Return: %lx\n", ret);
        return ret;
    }

    ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
    if (ret != 0) {
        printf("Error in: \x1b[31mhttpcAddRequestHeaderField\x1b[0m. Return: %lx\n", ret);
        return ret;
    }

    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0) {
        printf("Error in: \x1b[31mhttpcSetSSLOpt\x1b[0m. Return: %lx\n", ret);
        return ret;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    ret = httpcBeginRequest(&context);
    if(ret != 0) {
        printf("Error in: \x1b[31mhttpcBeginRequest\x1b[0m. Return: %lx\n", ret);
        return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0) {
        printf("Error in: \x1b[31mhttpcGetResponseStatusCode\x1b[0m. Return: %lx\n", ret);
        httpcCloseContext(&context);
        return ret;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            ret = httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024);
            if (ret != 0) {
                printf("\x1b[31mCould not get relocation header in 3XX http response\x1b[0m.\n");
                return ret;
            }
            httpcCloseContext(&context);
            printf("Retrying to call download function...\n\n");
            ret = downloadFile(topScreen, bottomScreen, newUrl, path);
            return ret;
        } else {
            printf("Error: \x1b[31mstatus code not 200 or redirection (3XX)\x1b[0m.\nStatus code: %lu\n", statuscode);
            httpcCloseContext(&context);
            return -1;
        }
    }

    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0) {
        printf("Error in: \x1b[31mhttpcGetDownloadSizeState\x1b[0m. Return: %lx\n", ret);
        httpcCloseContext(&context);
        return ret;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
        printf("\x1b[31mFailure to malloc buffer\x1b[0m.\n");
        return -2;
    }
    memset(buf, 0, contentsize);

    ret = httpcDownloadData(&context, buf, contentsize, NULL);
    if (ret != 0) {
        free(buf);
        printf("Error in: \x1b[31mhttpcDownloadData\x1b[0m. Return: %lx\n", ret);
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

Result printDB(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int i, int nInjected[], int game[], int overwrite[]) {
    char *language[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
    int langCont = 0;
    int adapt = 0;

    consoleSelect(&bottomScreen);
    printf("\x1b[2J");
    printf("----------------------------------------");
    printf("\x1b[32mSELECT\x1b[0m: change language | \x1b[32mA\x1b[0m: switch game");
    printf("\x1b[31mSTART\x1b[0m: inject in selected save");

    if (overwrite[0] == 0)
        printf("\x1b[3;0H\x1b[32mX\x1b[0m: Overwrite \x1b[32mDISABLED\x1b[0m");
    if (overwrite[0] == 1)
        printf("\x1b[3;0H\x1b[32mX\x1b[0m: Overwrite \x1b[32mENABLED \x1b[0m");

    if (adapt == 0)
        printf("\x1b[4;0H\x1b[32mY\x1b[0m: Adapt save to language: \x1b[32mNO \x1b[0m");
    else if (adapt == 1)
        printf("\x1b[4;0H\x1b[32mY\x1b[0m: Adapt save to language: \x1b[32mYES\x1b[0m");

    if (game[0] == 0)
        printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mX\x1b[0m  | Location: %d ", language[langCont], nInjected[0] + 1);
    else if (game[0] == 1)
        printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mY\x1b[0m  | Location: %d ", language[langCont], nInjected[0] + 1);
    else if (game[0] == 2)
        printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mOR\x1b[0m | Location: %d ", language[langCont], nInjected[0] + 1);
    else if (game[0] == 3)
        printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mAS\x1b[0m | Location: %d ", language[langCont], nInjected[0] + 1);
	
    printf("\n----------------------------------------");

    printf("\x1b[18;0H----------------------------------------");
    printf("\x1b[19;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this. This is a new way to inject WC6\nand I need time to perfect it.");
    printf("\x1b[24;0H----------------------------------------");
    printf("\x1b[29;11HPress B to return.");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    printf("\x1b[0;0HScanning server for available languages...");
    printf("\x1b[1;0HLanguages available: \x1b[32m");

    gfxFlushBuffers();
    gfxSwapBuffers();

    char *testurl = (char*)malloc(100 * sizeof(char));
    for (int j = 0; j < 7; j++) {
        switch (j) {
            case 0 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/jpn/%d.wc6", i);
                    break;
                }
            case 1 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/eng/%d.wc6", i);
                    break;
                }
            case 2 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/fre/%d.wc6", i);
                    break;
                }
            case 3 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/ita/%d.wc6", i);
                    break;
                }
            case 4 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/ger/%d.wc6", i);
                    break;
                }
            case 5 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/spa/%d.wc6", i);
                    break;
                }
            case 6 : {
                    snprintf(testurl, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/kor/%d.wc6", i);
                    break;
                }
        }

        httpcInit(0);
        httpcContext context;
        Result ret = 0;
        u32 statuscode = 0;

        ret = httpcOpenContext(&context, HTTPC_METHOD_GET, testurl, 0);
        if (ret != 0) break;
        ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
        if (ret != 0) break;
        ret = httpcSetSSLOpt(&context, 1 << 9);
        if (ret != 0) break;
        httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
        httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);
        ret = httpcBeginRequest(&context);
        if (ret != 0) break;
        ret = httpcGetResponseStatusCode(&context, &statuscode);
        if (ret != 0) break;

        if (statuscode == 200)
            printf("%s ", language[j]);

        gfxFlushBuffers();
        gfxSwapBuffers();

        httpcCloseContext(&context);
        httpcExit();
    }

    printf("\x1b[0m\n\n");
    getText(topScreen, bottomScreen, url);
    consoleSelect(&topScreen);
    printf("\x1b[0;45H\x1b[32mDONE!\x1b[0m");
    consoleSelect(&bottomScreen);

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        if (hidKeysDown() & KEY_B)
            break;

        if (hidKeysDown() & KEY_Y) {
            if (adapt == 0) adapt = 1;
            else if (adapt == 1) adapt = 0;

            if (adapt == 0)
                printf("\x1b[4;0H\x1b[32mY\x1b[0m: Adapt save to language: \x1b[32mNO \x1b[0m");
            else if (adapt == 1)
                printf("\x1b[4;0H\x1b[32mY\x1b[0m: Adapt save to language: \x1b[32mYES\x1b[0m");
        }

        if (hidKeysDown() & KEY_X) {
            if (overwrite[0] == 0) overwrite[0] = 1;
            else if (overwrite[0] == 1) {
                overwrite[0] = 0;
                nInjected[0] = 0;
            }

            if (overwrite[0] == 0)
                printf("\x1b[3;0H\x1b[32mX\x1b[0m: Overwrite \x1b[32mDISABLED\x1b[0m");
            if (overwrite[0] == 1)
                printf("\x1b[3;0H\x1b[32mX\x1b[0m: Overwrite \x1b[32mENABLED \x1b[0m");
        }

        if (hidKeysDown() & KEY_A) {
            if (game[0] < 3) {
                game[0] += 1;
                nInjected[0] = 0;
            } else if (game[0] == 3) {
                game[0] = 0;
                nInjected[0] = 0;
            }

			if (game[0] == 0)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mX\x1b[0m  | Location: %d ", language[langCont], nInjected[0] + 1);
			else if (game[0] == 1)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mY\x1b[0m  | Location: %d ", language[langCont], nInjected[0] + 1);
			else if (game[0] == 2)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mOR\x1b[0m | Location: %d ", language[langCont], nInjected[0] + 1);
			else if (game[0] == 3)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mAS\x1b[0m | Location: %d ", language[langCont], nInjected[0] + 1);
        }

        if (hidKeysDown() & KEY_SELECT) {
            if (langCont < 6) langCont++;
            else if (langCont == 6) langCont = 0;

			if (game[0] == 0)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mX\x1b[0m  | Location: %d ", language[langCont], nInjected[0] + 1);
			else if (game[0] == 1)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mY\x1b[0m  | Location: %d ", language[langCont], nInjected[0] + 1);
			else if (game[0] == 2)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mOR\x1b[0m | Location: %d ", language[langCont], nInjected[0] + 1);
			else if (game[0] == 3)
				printf("\x1b[5;0HLanguage: \x1b[32m%s\x1b[0m | Game: \x1b[32mAS\x1b[0m | Location: %d ", language[langCont], nInjected[0] + 1);
        }

        if (hidKeysDown() & KEY_START) {
            fsStart();
            //X, Y, OR, AS
            const u64 ids[4] = { 0x0004000000055E00, 0x0004000000055D00, 0x000400000011C400, 0x000400000011C500 };
            FS_Archive saveArch;
            if(openSaveArch(&saveArch, ids[game[0]])) {
                if (nInjected[0] == 23)
                    return -11;

                if (game[0] < 2 && i == 2048)
                    return -12;

                httpcInit(0);

                httpcContext context;
                Result ret = 0;
                u32 statuscode = 0;
                u32 contentsize = 0;

                char *wc6url = (char*)malloc(100 * sizeof(char));

                switch (langCont) {
                    case 0 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/jpn/%d.wc6", i);
                            break;
                        }
                    case 1 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/eng/%d.wc6", i);
                            break;
                        }
                    case 2 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/fre/%d.wc6", i);
                            break;
                        }
                    case 3 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/ita/%d.wc6", i);
                            break;
                        }
                    case 4 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/ger/%d.wc6", i);
                            break;
                        }
                    case 5 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/spa/%d.wc6", i);
                            break;
                        }
                    case 6 : {
                            snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/kor/%d.wc6", i);
                            break;
                        }
                }

                ret = httpcOpenContext(&context, HTTPC_METHOD_GET, wc6url, 0);
                if (ret != 0) return -2;

                ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
                if (ret != 0) return -3;

                ret = httpcSetSSLOpt(&context, 1 << 9);
                if (ret != 0) return -4;

                httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
                httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

                ret = httpcBeginRequest(&context);
                if (ret != 0) return -5;

                ret = httpcGetResponseStatusCode(&context, &statuscode);
                if (ret != 0) {
                    httpcCloseContext(&context);
                    return -6;
                }

                if (statuscode != 200)
                    return -6;

                ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
                if (ret != 0) {
                    httpcCloseContext(&context);
                    return -7;
                }

                u8 *wc6buf;
                wc6buf = (u8*)malloc(contentsize);
                if (wc6buf == NULL) return -8;
                memset(wc6buf, 0, contentsize);

                ret = httpcDownloadData(&context, wc6buf, contentsize, NULL);
                if (ret != 0) {
                    free(wc6buf);
                    httpcCloseContext(&context);
                    return -9;
                }

                //reading main
                Handle mainHandle;
                FSUSER_OpenFile(&mainHandle, saveArch, fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);

                //size
                u64 mainSize;
                FSFILE_GetSize(mainHandle, &mainSize);

                //buffer
                u8 *mainbuf = malloc(mainSize);

                //read
                FSFILE_Read(mainHandle, NULL, 0, mainbuf, mainSize);

                //finding first free location
                if (overwrite[0] == 0) {
                    if (game[0] == 0 || game[0] == 1) {
                        for (int t = 0; t < 23; t++)
                            if (*(mainbuf + 0x1BD00 + t * 264) == 0x00) {
                                nInjected[0] = t;
                                break;
                            }
                    } else if (game[0] == 2 || game[0] == 3) {
                        for (int t = 0; t < 23; t++)
                            if (*(mainbuf + 0x1CD00 + t * 264) == 0x00) {
                                nInjected[0] = t;
                                break;
                            }
                    }
                }

                if (adapt == 1)
                    injectLanguage(mainbuf, langCont);

                injectWC6(mainbuf, wc6buf, game[0], i, nInjected);

                int rwCHK = rewriteCHK(mainbuf, game[0]);
                if (rwCHK != 0)
                    return rwCHK;

                FSFILE_Write(mainHandle, NULL, 0, mainbuf, mainSize, FS_WRITE_FLUSH);
                FSUSER_ControlArchive(saveArch, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);

                FSUSER_CloseArchive(saveArch);

                free(mainbuf);
                free(wc6buf);

                httpcCloseContext(&context);
                httpcExit();
                fsEnd();
                return 1;
            } else 
				return -1;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    return 0;
}
