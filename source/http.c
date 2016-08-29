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
#include "util.h"
#include "pokemon.h"
#include "certs/cybertrust.h"
#include "certs/digicert.h"

#define ENTRIES 2

char *overwritechar[2] = {"DISABLED", "ENABLED "};
char *adaptchar[2] = {"NO ", "YES"};

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
    printf("\x1b[29;12H\x1b[47;34mPress B to exit.\x1b[0m");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
    getText(topScreen, bottomScreen, url);

	waitKey(KEY_B);
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

	consoleSelect(&bottomScreen);
    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
    if (ret != 0) {
        errDisp(bottomScreen, 2);
        return ret;
    }

    ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
    if (ret != 0) {
        errDisp(bottomScreen, 3);
        return ret;
    }
	
    ret = httpcSetSSLOpt(&context, 1 << 9);
    if (ret != 0) {
        errDisp(bottomScreen, 4);
        return ret;
    }

    httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
    httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

    ret = httpcBeginRequest(&context);
    if (ret != 0) {
        errDisp(bottomScreen, 5);
        return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0) {
        errDisp(bottomScreen, 6);
        httpcCloseContext(&context);
        return ret;
    }

    if (statuscode != 200) {
        if (statuscode >= 300 && statuscode < 400) {
            char newUrl[1024];
            ret = httpcGetResponseHeader(&context, (char*)"Location", newUrl, 1024);
            if (ret != 0) {
                errDisp(bottomScreen, 7);
                return ret;
            }
            httpcCloseContext(&context);
            printf("Retrying to call download function...\n\n");
            ret = downloadFile(topScreen, bottomScreen, newUrl, path);
            return ret;
        } else {
            errDisp(bottomScreen, 8);
            httpcCloseContext(&context);
            return -1;
        }
    }

    ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if (ret != 0) {
        errDisp(bottomScreen, 9);
        httpcCloseContext(&context);
        return ret;
    }

    buf = (u8*)malloc(contentsize);
    if (buf == NULL) {
        errDisp(bottomScreen, 10);
        return -2;
    }
    memset(buf, 0, contentsize);

    ret = httpcDownloadData(&context, buf, contentsize, NULL);
    if (ret != 0) {
        free(buf);
        errDisp(bottomScreen, 11);
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

int injectBoxBin(PrintConsole screen, u8* mainbuf, int game, int NBOXES, char* urls[]) {
	for (int i = 0; i < NBOXES; i++) {
		consoleSelect(&screen);
		printf("\x1b[%d;0HDownloading box %d", 4 + i, i + 1);
		gfxFlushBuffers();
		gfxSwapBuffers();
	
		httpcInit(0);

		httpcContext context;
		Result ret = 0;
		u32 statuscode = 0;
		u32 contentsize = 0;
		
		ret = httpcOpenContext(&context, HTTPC_METHOD_GET, urls[i], 0);
		if (ret != 0) 
			return 2;

		ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
		if (ret != 0) 
			return 3;
		
		ret = httpcSetSSLOpt(&context, 1 << 9);
		if (ret != 0) 
			return 4;

		httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
		httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

		ret = httpcBeginRequest(&context);
		if (ret != 0) 
			return 5;

		ret = httpcGetResponseStatusCode(&context, &statuscode);
		if (ret != 0) {
			httpcCloseContext(&context);
			return 15;
		}

		if (statuscode != 200)
			return 15;

		ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
		if (ret != 0) {
			httpcCloseContext(&context);
			return 7;
		}

		u8 *buf;
		buf = (u8*)malloc(contentsize);
		if (buf == NULL) 
			return 8;
		memset(buf, 0, contentsize);

		ret = httpcDownloadData(&context, buf, contentsize, NULL);
		if (ret != 0) {
			free(buf);
			httpcCloseContext(&context);
			return 9;
		}
		
		int boxpos = 0;
		if (game == 0 || game == 1) 
			boxpos = 0x27A00 - 0x5400;
		
		
		if (game == 2 || game == 3) 
			boxpos = 0x38400 - 0x5400;
		
		memcpy((void*)(mainbuf + boxpos + PKMNLENGTH * 30 * i), (const void*)buf, PKMNLENGTH * 30);
		
		free(buf);
		httpcCloseContext(&context);
		httpcExit();
	}
	return 1;
}

Result printDB(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, char *url, int i, int nInjected[], int game, int overwrite[]) {	
	char *language[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
	
    int langCont = 0;
    int adapt = 0;

    consoleSelect(&bottomScreen);
    printf("\x1b[2J");
    printf("----------------------------------------");
	printf("\x1b[32mSELECT\x1b[0m change language");
	printf("\n\x1b[32mX\x1b[0m      overwrite wondercards");
	printf("\n\x1b[32mY\x1b[0m      adapt save to language");
	printf("\n\x1b[31mSTART\x1b[0m  inject wc6 in slot");
	
	printf("\x1b[1;32H\x1b[32m%s\x1b[0m", language[langCont]);
	printf("\x1b[2;32H\x1b[32m%s\x1b[0m", overwritechar[overwrite[0]]);
	printf("\x1b[3;32H\x1b[32m%s\x1b[0m", adaptchar[adapt]);
	printf("\x1b[4;32H\x1b[32m%d \x1b[0m", nInjected[0] + 1);	
    printf("\x1b[5;0H----------------------------------------");

    printf("\x1b[18;0H----------------------------------------");
    printf("\x1b[19;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this. This is a new way to inject WC6\nand I need time to perfect it.");
    printf("\x1b[24;0H----------------------------------------");
    printf("\x1b[29;11H\x1b[47;34mPress B to return.\x1b[0m");
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
        if (ret != 0) 
			break;
		
        ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
        if (ret != 0) 
			break;

        ret = httpcSetSSLOpt(&context, 1 << 9);
        if (ret != 0) 
			break;

        httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
        httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);
		
        ret = httpcBeginRequest(&context);
        if (ret != 0) 
			break;
		
        ret = httpcGetResponseStatusCode(&context, &statuscode);
        if (ret != 0) 
			break;

        if (statuscode == 200)
            printf("%s ", language[j]);

        gfxFlushBuffers();
        gfxSwapBuffers();

        httpcCloseContext(&context);
        httpcExit();
    }
	
	free(testurl);

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
            if (adapt == 0) 
				adapt = 1;
            else if (adapt == 1) 
				adapt = 0;

			printf("\x1b[3;32H\x1b[32m%s\x1b[0m", adaptchar[adapt]);
        }

        if (hidKeysDown() & KEY_X) {
            if (overwrite[0] == 0) 
				overwrite[0] = 1;
            else if (overwrite[0] == 1) {
                overwrite[0] = 0;
                nInjected[0] = 0;
            }

			printf("\x1b[2;32H\x1b[32m%s\x1b[0m", overwritechar[overwrite[0]]);
        }

        if (hidKeysDown() & KEY_SELECT) {
            if (langCont < 6) 
				langCont++;
            else if (langCont == 6) 
				langCont = 0;

			printf("\x1b[1;32H\x1b[32m%s\x1b[0m", language[langCont]);
        }

        if (hidKeysDown() & KEY_START) {
			if (nInjected[0] >= 24)
				nInjected[0] = 0;

			if (game < 2 && i == 2048)
				return 12;

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
			if (ret != 0) 
				return 2;

			ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
			if (ret != 0) 
				return 3;

			ret = httpcSetSSLOpt(&context, 1 << 9);
			if (ret != 0) 
				return 4;
 
			httpcAddTrustedRootCA(&context, cybertrust_cer, cybertrust_cer_len);
			httpcAddTrustedRootCA(&context, digicert_cer, digicert_cer_len);

			ret = httpcBeginRequest(&context);
			if (ret != 0) 
				return 5;

			ret = httpcGetResponseStatusCode(&context, &statuscode);
			if (ret != 0) {
				httpcCloseContext(&context);
				return 15;
			}

			if (statuscode != 200)
				return 15;

			ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
			if (ret != 0) {
				httpcCloseContext(&context);
				return 7;
			}

			u8 *wc6buf;
			wc6buf = (u8*)malloc(contentsize);
			if (wc6buf == NULL) 
				return 8;
			memset(wc6buf, 0, contentsize);

			ret = httpcDownloadData(&context, wc6buf, contentsize, NULL);
			if (ret != 0) {
				free(wc6buf);
				httpcCloseContext(&context);
				return 9;
			}

			//finding first free location
			if (overwrite[0] == 0) {
				if (game == 0 || game == 1) {
					for (int t = 0; t < 23; t++)
						if (*(mainbuf + 0x1BD00 + t * 264) == 0x00) {
							nInjected[0] = t;
							break;
						}
				} else if (game == 2 || game == 3) {
					for (int t = 0; t < 23; t++)
						if (*(mainbuf + 0x1CD00 + t * 264) == 0x00) {
							nInjected[0] = t;
							break;
						}
				}
			}

			if (adapt == 1)
				setLanguage(mainbuf, langCont);

			setWC(mainbuf, wc6buf, game, i, nInjected);

			free(wc6url);
			free(wc6buf);

			httpcCloseContext(&context);
			httpcExit();
			return 0;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    return -1;
}

int massInjecter(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game) {
	int cont = 0;
	char *menuEntries[ENTRIES] = {"XD collection", "Colosseum collection"};
	
    consoleSelect(&bottomScreen);
    printf("\x1b[2J");
    printf("----------------------------------------");
	printf("\x1b[31mSTART\x1b[0m  inject wc6 in slot");	
    printf("\x1b[2;0H----------------------------------------");

	printf("\x1b[18;0HThis will \x1b[31mOVERWRITE\x1b[0m the first N boxes ofyour pcdata.");
    printf("\x1b[21;0H----------------------------------------");
    printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
    printf("\x1b[26;0H----------------------------------------");
    printf("\x1b[29;11H\x1b[47;34mPress B to return.\x1b[0m");
    consoleSelect(&topScreen);
    printf("\x1b[2J");
	printf("\x1b[47;1;34m                  Mass Injecter                   \x1b[0m\n");	
	refresh(cont, topScreen, menuEntries, ENTRIES);

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        if (hidKeysDown() & KEY_B)
            break;

		if (hidKeysDown() & KEY_DUP) {
			if (cont == 0) {
				cont = ENTRIES - 1;
				refresh(cont, topScreen, menuEntries, ENTRIES);
			}
			else if (cont > 0) {
				cont--;
				refresh(cont, topScreen, menuEntries, ENTRIES);	
			}
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (cont == ENTRIES - 1) {
				cont = 0;
				refresh(cont, topScreen, menuEntries, ENTRIES);	
			}
			else if (cont < ENTRIES - 1) {
				cont++;
				refresh(cont, topScreen, menuEntries, ENTRIES);
			}
		}

        if (hidKeysDown() & KEY_START) {
			switch (cont) {
				case 0 : {
					char *urls[3] = {"https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/misc/xd/1.bin", "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/misc/xd/2.bin", "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/misc/xd/3.bin"};
					
					consoleSelect(&topScreen);
					printf("\x1b[15;%dH\x1b[31mSTART\x1b[0m: \x1b[33m%d\x1b[0m box will be replaced", 9, 3);
					while (aptMainLoop()) {
						gspWaitForVBlank();
						hidScanInput();
						
						if (hidKeysDown() & KEY_B)
							return 0;

						if (hidKeysDown() & KEY_START)
							break;

						gfxFlushBuffers();
						gfxSwapBuffers();
					}
					
					int res = injectBoxBin (bottomScreen, mainbuf, game, 3, urls);
					if (res != 1) 
						return res;
					break;
				}
				case 1 : {
					char *urls[2] = {"https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/misc/colosseum/1.bin", "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/misc/colosseum/2.bin"};

					consoleSelect(&topScreen);
					printf("\x1b[15;%dH\x1b[31mSTART\x1b[0m: \x1b[33m%d\x1b[0m box will be replaced", 9, 2);
					while (aptMainLoop()) {
						gspWaitForVBlank();
						hidScanInput();
						
						if (hidKeysDown() & KEY_B)
							return 0;

						if (hidKeysDown() & KEY_START)
							break;

						gfxFlushBuffers();
						gfxSwapBuffers();
					}					
					
					int res = injectBoxBin (bottomScreen, mainbuf, game, 2, urls);
					if (res != 1) 
						return res;
					break;
				}
			}
			return 1;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    return 0;
}