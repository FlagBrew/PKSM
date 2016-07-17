#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "util.h"
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
		
	ret = httpcSetSSLOpt(context, 1<<9);
	if (ret != 0)
		return ret; 
		
	httpcAddTrustedRootCA(context, cybertrust_cer, cybertrust_cer_len);
	httpcAddTrustedRootCA(context, digicert_cer, digicert_cer_len);
	
	ret = httpcBeginRequest(context);
	if (ret != 0)		
		return ret;

	ret = httpcGetResponseStatusCode(context, &statuscode, 0);
	if (ret != 0) {
		printf("\x1b[26;0HStatus: \x1b[31mNO INTERNET AVAILABLE\x1b[0m");
		return ret;
	}
	
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	if (statuscode == 200)
		printf("\x1b[26;0HStatus: \x1b[32mOKAY                  \x1b[0m");
	else 
		printf("\x1b[26;0HStatus: \x1b[31mFILE NOT AVAILABLE YET\x1b[0m");
	
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
		printf("\x1b[25;0HDownloading...");
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
	printf("\x1b[29;10HPress A to continue.");
	consoleSelect(&topScreen);		
	printf("\x1b[2J");
	getText(topScreen, bottomScreen, url);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_A) 
			break; 			 
	}
}

void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int page) {
	consoleSelect (&bottomScreen);
	printf("\x1b[5;0HPage %d/7 ", page);
	consoleSelect(&topScreen);		
	printf("\x1b[2J");
	getText(topScreen, bottomScreen, url);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_A) 
			break; 			 
	}
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
	
	ret = httpcSetSSLOpt(&context, 1<<9);
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
	
	ret = httpcGetResponseStatusCode(&context, &statuscode, 0);
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
		}
		else {
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
	if(ret != 0) {
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

Result printDB(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int i) {

	char *language[7] = {"ENG", "JPN", "ITA", "FRE", "SPA", "GER", "KOR"};
	int langCont = 0;
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("NA  - North America");
	printf("\nPAL - Europe, Australia");
	printf("\nJPN - Japan");
	printf("\nKOR - South Korea");
	printf("\nALL - All regions available\n");
	printf("----------------------------------------");
	printf("Press SELECT to change language\n");
	printf("Press START to inject in OR/AS\n");
	printf("----------------------------------------");
	printf("\x1b[11;0HLanguage selected: \x1b[32m%s\x1b[0m", language[langCont]);
	printf("\x1b[29;10HPress A to continue.");
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	getText(topScreen, bottomScreen, url);
	
	consoleSelect(&bottomScreen);
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_A) 
			break; 	
		
		if (kDown & KEY_SELECT) {
			if (langCont < 6) langCont++;
			else if (langCont == 6) langCont = 0;
			printf("\x1b[11;0HLanguage selected: \x1b[32m%s\x1b[0m", language[langCont]);
		}

		if (kDown & KEY_START) {
			fsInit();
			httpcInit(0);
			
			httpcContext context;
			Result ret = 0;
			u32 statuscode = 0;
			u32 contentsize = 0;
			
			char *wc6url = (char*)malloc(100*sizeof(char));
			switch (langCont) {
				case 0 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/eng/%d.wc6", i);
					break;
				}
				case 1 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/jpn/%d.wc6", i);
					break;
				}
				case 2 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/ita/%d.wc6", i);
					break;
				}
				case 3 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/fre/%d.wc6", i);
					break;
				}
				case 4 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/spa/%d.wc6", i);
					break;
				}
				case 5 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/ger/%d.wc6", i);
					break;
				}
				case 6 : {
					snprintf(wc6url, 100, "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/resources/wc6/kor/%d.wc6", i);
					break;
				}
			}
					
			ret = httpcOpenContext(&context, HTTPC_METHOD_GET, wc6url, 0);
			if (ret != 0) {
				printf("Error in: \x1b[31mhttpcOpenContext\x1b[0m. Return: %lx\n", ret);
				return ret;
			}
			
			ret = httpcAddRequestHeaderField(&context, "User-Agent", "EventAssistant");
			if (ret != 0) {
				printf("Error in: \x1b[31mhttpcAddRequestHeaderField\x1b[0m. Return: %lx\n", ret);
				return ret;
			}
			
			ret = httpcSetSSLOpt(&context, 1<<9);
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
			
			ret = httpcGetResponseStatusCode(&context, &statuscode, 0);
			if (ret != 0) {
				printf("Error in: \x1b[31mhttpcGetResponseStatusCode\x1b[0m. Return: %lx\n", ret);
				httpcCloseContext(&context);
				return ret;
			}
			
			ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
			if (ret != 0) {
				printf("Error in: \x1b[31mhttpcGetDownloadSizeState\x1b[0m. Return: %lx\n", ret);
				httpcCloseContext(&context);
				return ret;
			}
			
			u8 *wc6buf;
			wc6buf = (u8*)malloc(contentsize);
			if (wc6buf == NULL) {
				printf("\x1b[31mFailure to malloc buffer\x1b[0m.\n");
				return -2;
			}
			memset(wc6buf, 0, contentsize);
			
			ret = httpcDownloadData(&context, wc6buf, contentsize, NULL);
			if(ret != 0) {
				free(wc6buf);
				printf("Error in: \x1b[31mhttpcDownloadData\x1b[0m. Return: %lx\n", ret);
				httpcCloseContext(&context);
				return ret;
			}
			
			//reading main
			FILE *fptr = fopen("/main", "rt");
			if (fptr == NULL) {
				return -3;
			}
			fseek(fptr, 0, SEEK_END);
			u32 mainsize = ftell(fptr);
			u8* mainbuf = malloc(mainsize);
			rewind(fptr);
			fread(mainbuf, mainsize, 1, fptr);
			fclose(fptr);
			
			//doing backup
			FILE *fptr1 = fopen("/main.bak", "wb");
			fwrite(mainbuf, 1, mainsize, fptr1);
			fclose(fptr1);
			

			memcpy((void*)(mainbuf+118016), (const void*)wc6buf, 264);
			
			//updating checksums 
			u8 blockCount = 58;
			u32 csoff = 0x7B21A - 0x5400;

			u8* tmp = (u8*)malloc(0x35000*sizeof(u8));
			u16 cs;

			if (!tmp) return -1;

			for (u32 i = 0; i < blockCount; i++) {
				memcpy(tmp, mainbuf + CHKOffset(i), CHKLength(i));
				cs = ccitt16(tmp, CHKLength(i));
				memcpy(mainbuf + csoff + i * 8, &cs, 2);
			}
			
			FILE *fptr2 = fopen("/main", "wb");
			fwrite(mainbuf, 1, mainsize, fptr2);
			fclose(fptr2);
			free(mainbuf);
			free(wc6buf);
			
			httpcCloseContext(&context);
			httpcExit();
			fsExit();
		}
	}
	return 0;
}