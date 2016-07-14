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
	char *ciaUrl = "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/latest/ECITool.cia";
	char *hblUrl = "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/latest/ECITool.3dsx";
	char *smdhUrl = "https://raw.githubusercontent.com/BernardoGiordano/ECITool/master/latest/ECITool.smdh";
	char *ciaPath = "/ECITool.cia";
	char *hblPath = "/3ds/ECITool/ECITool.3dsx";
	char *smdhPath = "/3ds/ECITool/ECITool.smdh";
	
	Result ret = 0;
	
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;34m                     Updater                      \x1b[0m");
	printf("---------------------------------------------------------");	
	
	ret = downloadFile(topScreen, bottomScreen, ciaUrl, ciaPath);
	if (ret == 0) 
		printf("Download of ECITool.cia succeded! Install it using a CIA manager.\n\n");
	else printf("Download of ECITool.cia failed. Please report the issue to the dev.\n\n");
	
	ret = downloadFile(topScreen, bottomScreen, ciaUrl, ciaPath);
	if (ret == 0) 
		printf("Download of ECITool.cia succeded! Install it using a CIA manager.\n\n");
	else printf("Download of ECITool.cia failed. Please report the issue to the dev.\n\n");
	
	ret = downloadFile(topScreen, bottomScreen, hblUrl, hblPath);
	if (ret == 0) 
		printf("Download of ECITool.3dsx succeded! Open it using your favourite entrypoint.\n\n");
	else printf("Download of ECITool.3dsx failed. Please report the issue to the dev.\n\n");
	
	ret = downloadFile(topScreen, bottomScreen, smdhUrl, smdhPath);
	if (ret == 0) 
		printf("Download of ECITool.smdh succeded!\n\n");
	else printf("Download of ECITool.smdh failed. Please report the issue to the dev.\n\n");
	
	printf("Press START to exit.");

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