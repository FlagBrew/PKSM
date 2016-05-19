#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <math.h>
#include "catch.h"

// working on this. it will take LOTS of time
int ratio[20] = {45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 120, 45, 255, 120, 45, 255, 120, 45, 255, 127};

void catchrate() {
	int HP_perc = 100;
	int bonusball = 1;
	int bonusstatus = 1;	
	
	printf("\x1b[2J");
	printf("\x1b[47;30m              Catch Rate  Calculator              \x1b[0m");
	printf("--------------------------------------------------");
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_START)
			break;
	}
}

// a = rate*bonusball*bonusstatus-(2/3)*rate*bonusball*nonustats*(HP_perc);