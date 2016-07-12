#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "util.h"

Result http_download(PrintConsole topScreen, PrintConsole bottomScreen, httpcContext *context) {	
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	Result ret = 0;
	u32 statuscode = 0;
	u32 contentsize = 0;
	u8 *buf;
	
	consoleSelect(&bottomScreen);
	
	ret = httpcBeginRequest(context);
	if (ret != 0) {		
		return ret;
	}

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

	if (statuscode != 200) 
		return -2;

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

	ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
	
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
	printf("\x1b[5;0HPage %d ", page);
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