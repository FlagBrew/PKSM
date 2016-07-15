#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"

void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N) {	

	consoleSelect(&topScreen);
	printf("\x1b[2;0H\x1b[30;0m");
	for (int i = 0; i < N; i++) {
		if (i == currentEntry)
			printf("\x1b[30;32m%s\x1b[0m\n", lista[i]);
		else 
			printf("\x1b[0m%s\n", lista[i]);
	}
}

void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page) {

	consoleSelect(&topScreen);
	printf("\x1b[2;0H\x1b[30;0m");
	for (int i = 0; i < N; i++) {
		if (i == currentEntry)
			printf("\x1b[30;32m%s\x1b[0m\n", lista[i + page * N]);
		else 
			printf("\x1b[0m%s\n", lista[i + page * N]);
	}
}

void update(PrintConsole topScreen, PrintConsole bottomScreen) {
	char *ciaUrl = "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/latest/EventAssistant.cia";
	char *hblUrl = "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/latest/EventAssistant.3dsx";
	char *smdhUrl = "https://raw.githubusercontent.com/BernardoGiordano/EventAssistant/master/latest/EventAssistant.sdmh";
	char *ciaPath = "/EventAssistant.cia";
	char *hblPath = "/3ds/EventAssistant/EventAssistant.3dsx";
	char *smdhPath = "/3ds/EventAssistant/EventAssistant.sdmh";
	
	Result ret = 0;
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;34m                     Updater                      \x1b[0m\n");
	
	ret = downloadFile(topScreen, bottomScreen, ciaUrl, ciaPath);
	consoleSelect(&topScreen);
	if (ret == 0) 
		printf("\nDownload of EventAssistant.cia \x1b[32msucceded!\x1b[0m Install  it using a CIA manager.\n\n");
	else printf("\nDownload of EventAssistant.cia \x1b[31mfailed.\x1b[0m\nPlease report the issue to the dev.\n\n");
	
	ret = downloadFile(topScreen, bottomScreen, hblUrl, hblPath);
	consoleSelect(&topScreen);
	if (ret == 0) 
		printf("\nDownload of EventAssistant.3dsx \x1b[32msucceded!\x1b[0m Open it using your favourite entrypoint.\n\n");
	else printf("\nDownload of EventAssistant.3dsx \x1b[31mfailed.\x1b[0m\nPlease report the issue to the dev.\n\n");
	
	ret = downloadFile(topScreen, bottomScreen, smdhUrl, smdhPath);
	consoleSelect(&topScreen);
	if (ret == 0) 
		printf("\nDownload of EventAssistant.sdmh \x1b[32msucceded!\x1b[0m\n\n");
	else printf("\nDownload of EventAssistant.sdmh \x1b[31mfailed.\x1b[0m\nPlease report the issue to the dev.\n\n");
	
	printf("\x1b[29;15HPress Start to exit.");
	httpcExit();
	fsExit();

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) 
			break; 
		
		gfxFlushBuffers();
		gfxSwapBuffers();
	}	
}