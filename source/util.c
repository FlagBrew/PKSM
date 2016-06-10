#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N) {	

	consoleSelect(&topScreen);
	printf("\x1b[2;0H");
	for (int i = 0; i < N; i++) {
		if (i == currentEntry)
			printf("\x1b[32m%s\x1b[0m\n", lista[i]);
		else 
			printf("%s\n", lista[i]);
	}
}

void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page) {

	consoleSelect(&topScreen);
	printf("\x1b[2;0H");
	for (int i = 0; i < N; i++) {
		if (i == currentEntry)
			printf("\x1b[32m%s\x1b[0m\n", lista[i + page * N]);
		else 
			printf("%s\n", lista[i + page * N]);
	}
}