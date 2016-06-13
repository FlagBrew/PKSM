#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

#define DELAY 20

void hexa2bin (int binary[], char hexadecimal[], int nbit) {
	int k = 0;
	int i = 0;
	
	for (i = 0; i < nbit; i++) {
		k = i * 4;
		switch (hexadecimal[i]) {
			case '0' : {
				binary[k] = 0;
				binary[k+1] = 0;
				binary[k+2] = 0;
				binary[k+3] = 0;
				break;
			}
			case '1' : {
				binary[k] = 0;
				binary[k+1] = 0;
				binary[k+2] = 0;
				binary[k+3] = 1;
				break;
			}
			case '2' : {
				binary[k] = 0;
				binary[k+1] = 0;
				binary[k+2] = 1;
				binary[k+3] = 0;
				break;
			}
			case '3' : {
				binary[k] = 0;
				binary[k+1] = 0;
				binary[k+2] = 1;
				binary[k+3] = 1;
				break;
			}
			case '4' : {
				binary[k]=0;
				binary[k+1]=1;
				binary[k+2]=0;
				binary[k+3]=0;
				break;
			}
			case '5' : {
				binary[k]=0;
				binary[k+1]=1;
				binary[k+2]=0;
				binary[k+3]=1;
				break;
			}
			case '6' : {
				binary[k]=0;
				binary[k+1]=1;
				binary[k+2]=1;
				binary[k+3]=0;
				break;
			}
			case '7' : {
				binary[k]=0;
				binary[k+1]=1;
				binary[k+2]=1;
				binary[k+3]=1;
				break;
			}
			case '8' : {
				binary[k]=1;
				binary[k+1]=0;
				binary[k+2]=0;
				binary[k+3]=0;
				break;
			}
			case '9' : {
				binary[k]=1;
				binary[k+1]=0;
				binary[k+2]=0;
				binary[k+3]=1;
				break;
			}	
			case 'A' : {
				binary[k]=1;
				binary[k+1]=0;
				binary[k+2]=1;
				binary[k+3]=0;
				break;
			}	
			case 'B' : {
				binary[k]=1;
				binary[k+1]=0;
				binary[k+2]=1;
				binary[k+3]=1;
				break;
			}
			case 'C' : {
				binary[k]=1;
				binary[k+1]=1;
				binary[k+2]=0;
				binary[k+3]=0;
				break;
			}
			case 'D' : {
				binary[k]=1;
				binary[k+1]=1;
				binary[k+2]=0;
				binary[k+3]=1;
				break;
			}	
			case 'E' : {
				binary[k]=1;
				binary[k+1]=1;
				binary[k+2]=1;
				binary[k+3]=0;
				break;
			}
			case 'F' : {
				binary[k]=1;
				binary[k+1]=1;
				binary[k+2]=1;
				binary[k+3]=1;
				break;
			}						
		}
	}
}

long unsigned int hexa2dec (char hexadecimal[], int binary[], int nbit) {
	long unsigned int decimal = 0;
	
	hexa2bin (binary, hexadecimal, nbit);
	
	for (int i = 0; i < nbit * 4; i++) {
		long unsigned int pot = 1;
		int j = 0;
		while (j < i) {
			pot *= 2;
			j++;
		}
		decimal += binary[nbit * 4 - 1 - i] * pot;
	}
	return decimal;
}

void PIDchecker(char pid_hexa_vett[], int var[]) {
	int pid_bin_vett[32];
	int tid_vett[16];
	int sid_vett[16];
	int xor_vett[16];
	char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ?!";
	
	int i = 0;
	long unsigned int decimal = 0;
	long unsigned int xor_result = 0;
	int tid_dec = var[0] * 10000 + var[1] * 1000 + var[2] * 100 + var[3] * 10 + var[4] * 1; 
	int sid_dec = var[5] * 10000 + var[6] * 1000 + var[7] * 100 + var[8] * 10 + var[9] * 1;
	
	decimal = hexa2dec (pid_hexa_vett, pid_bin_vett, 8);
	
	printf("\x1b[5;0H- Binary PID: ");
	for (int t = 0; t < 32; t++) {
		printf("%d", pid_bin_vett[t]);
	}
	
	printf("\x1b[7;0H- Nature (III/IV gen only): \x1b[32m");
	switch (decimal % 25) {	
		case 0 : { printf("Hardy   "); break;}
		case 1 : { printf("Lonely  "); break;}	
		case 2 : { printf("Brave   "); break;}
		case 3 : { printf("Adamant "); break;}	
		case 4 : { printf("Naughty "); break;}	
		case 5 : { printf("Bold    "); break;}
		case 6 : { printf("Docile  "); break;}
		case 7 : { printf("Relaxed "); break;}
		case 8 : { printf("Impish  "); break;}	
		case 9 : { printf("Lax     "); break;}		
		case 10 : { printf("Timid   "); break;}
		case 11 : { printf("Hasty   "); break;}
		case 12 : { printf("Serious "); break;}				
		case 13 : { printf("Jolly   "); break;}
		case 14 : { printf("Naive   "); break;}	
		case 15 : { printf("Modest  "); break;}	
		case 16 : { printf("Mild    "); break;}
		case 17 : { printf("Quiet   "); break;}
		case 18 : { printf("Bashful "); break;}
		case 19 : { printf("Rash    "); break;}	
		case 20 : { printf("Calm    "); break;}		
		case 21 : { printf("Gentle  "); break;}	
		case 22 : { printf("Sassy   "); break;}
		case 23 : { printf("Careful "); break;}	
		case 24 : { printf("Quirky  "); break;}		
	}
	
	printf("\x1b[0m\x1b[9;0H- Characteristic: \x1b[32m");
	switch (decimal % 6) {
		case 0 : { printf("HP     "); break;}
		case 1 : { printf("Attack "); break;}	
		case 2 : { printf("Defense"); break;}	
		case 3 : { printf("Speed  "); break;}
		case 4 : { printf("Sp. Atk"); break;}			
		case 5 : { printf("Sp. Def"); break;}				
	}
	printf("\x1b[0m");
	
	for (i = 0; i < 16; i++) {
		if ((tid_dec % 2) == 0) 
			tid_vett[15 - i] = 0;
		else 
			tid_vett[15 - i] = 1;
		if ((sid_dec % 2) == 0) 
			sid_vett[15 - i] = 0;
		else 
			sid_vett[15 - i] = 1;
		tid_dec /= 2;
		sid_dec /= 2;			
	}
		
	for (i = 0; i < 16; i++) {
		if (pid_bin_vett[i] == pid_bin_vett[16 + i])
			xor_vett[i] = 0;
		else 
			xor_vett[i] = 1;
		
		if (xor_vett[i] == tid_vett[i])
			xor_vett[i] = 0;
		else
			xor_vett[i] = 1;
		
		if (xor_vett[i] == sid_vett[i])
			xor_vett[i] = 0;
		else
			xor_vett[i] = 1;
	}

	for (i = 0; i < 16; i++) {
		long unsigned int pot = 1;
		int j = 0;
		while (j < i) {
			pot *= 2;
			j++;
		}
		xor_result += xor_vett[15 - i] * pot;	
	}
	
	printf ("\x1b[11;0H- Shiny in III-V gen: ");
	if (xor_result < 8)
		printf("\x1b[32mTRUE \x1b[0m");
	else 
		printf("\x1b[31mFALSE\x1b[0m");
	
	printf ("\x1b[13;0H- Shiny in VI gen: ");
	if (xor_result < 16)
		printf("\x1b[32mTRUE \x1b[0m");
	else
		printf("\x1b[31mFALSE\x1b[0m");
	
	int y1 = pid_bin_vett[0] * 8 + pid_bin_vett[1] * 4 + pid_bin_vett[2] * 2 + pid_bin_vett[3];
	int x1 = pid_bin_vett[4] * 8 + pid_bin_vett[5] * 4 + pid_bin_vett[6] * 2 + pid_bin_vett[7];
	int y2 = pid_bin_vett[8] * 8 + pid_bin_vett[9] * 4 + pid_bin_vett[10] * 2 + pid_bin_vett[11];
	int x2 = pid_bin_vett[12] * 8 + pid_bin_vett[13] * 4 + pid_bin_vett[14] * 2 + pid_bin_vett[15];
	int y3 = pid_bin_vett[16] * 8 + pid_bin_vett[17] * 4 + pid_bin_vett[18] * 2 + pid_bin_vett[19];
	int x3 = pid_bin_vett[20] * 8 + pid_bin_vett[21] * 4 + pid_bin_vett[22] * 2 + pid_bin_vett[23];
	int y4 = pid_bin_vett[24] * 8 + pid_bin_vett[25] * 4 + pid_bin_vett[26] * 2 + pid_bin_vett[27];
	int x4 = pid_bin_vett[28] * 8 + pid_bin_vett[29] * 4 + pid_bin_vett[30] * 2 + pid_bin_vett[31];
	
	printf("\x1b[15;0H- Spinda spots: (\x1b[32m%d;%d\x1b[0m), (\x1b[32m%d;%d\x1b[0m), (\x1b[32m%d;%d\x1b[0m), (\x1b[32m%d;%d\x1b[0m)                ", y1, x1, y2, x2, y3, x3, y4, x4);

	int alfa = pid_bin_vett[31] + pid_bin_vett[30] * 2 + pid_bin_vett[23] * 4 + pid_bin_vett[22] * 8 + pid_bin_vett[15] * 16 + pid_bin_vett[14] * 32 + pid_bin_vett[7] * 64 + pid_bin_vett[6] * 128;
	alfa = alfa % 28;
	
	printf("\x1b[17;0H- Unown's letter (Gen III only): \x1b[32m%c\x1b[0m", letters[alfa]);
}

void posCursore (char cursore[], int pos[], int temp) {
	cursore[pos[0]] = ' ';
	cursore[pos[0] + temp] = '^';
	pos[0] += temp;
}

void incrementa(int var[], int posizione[], int temp) {
	var[posizione[0]] += temp;
}

void incrementaPID(char pid[], int counterPID[], char varPID[], int posizione[], int temp) {
	counterPID[posizione[0] - 10] += temp;
	pid[posizione[0] - 10] = varPID[counterPID[posizione[0] - 10]];
}

void checkValDec(int var[]) {
	if ((var[0] * 10000 + var[1] * 1000 + var[2] * 100 + var[3] * 10 + var[4]) >= 65536) {
		var[0] = 6;
		var[1] = 5;
		var[2] = 5;
		var[3] = 3;
		var[4] = 5;
	}
	if ((var[5] * 10000 + var[6] * 1000 + var[7] * 100 + var[8] * 10 + var[9]) >= 65536) {
		var[5] = 6;
		var[6] = 5;
		var[7] = 5;
		var[8] = 3;
		var[9] = 5;
	}
}

void printCursore(char cur[]) {
	printf("\x1b[3;0H----%c%c%c%c%c",cur[0], cur[1], cur[2], cur[3], cur[4]);
	printf("--------------%c%c%c%c%c",cur[5], cur[6], cur[7], cur[8], cur[9]);
	printf("--------------%c%c%c%c%c%c%c%c",cur[10], cur[11], cur[12], cur[13], cur[14], cur[15], cur[16], cur[17]);
}

void show(int var[], char pid[]) {
	printf("\x1b[2;0HTID:%d%d%d%d%d",var[0], var[1], var[2], var[3], var[4]);
	printf("          SID:%d%d%d%d%d",var[5], var[6], var[7], var[8], var[9]);
	printf("          PID:%c%c%c%c%c%c%c%c", pid[0], pid[1], pid[2], pid[3], pid[4], pid[5], pid[6], pid[7]);
}

void PID(PrintConsole topScreen, PrintConsole bottomScreen){
	int posizione[1] = {0};
	int var[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	char pid[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};
	char cursore[18] = {'^', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', };
	char varPID[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	int counterPID[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1A\x1B - Move cursor\n");
	printf("\x18\x19 - Change values (0-9/A-F)\n");
	printf("SELECT - Reset values\n");
	printf("----------------------------------------");
	printf("\x1b[16;0H----------------------------------------");
	printf("About Characteristic's test:");
	printf("\x1b[19;0HFrom Generation IV onward, Pokemon have a Characteristic which indicates their\nhighest IV. In the case of a tie, the\npersonality value is used to determine  which stat wins the tie.\nThe first stat checked that is tied for highest wins the tie and will determine the Characteristic.");
	printf("\x1b[27;0H----------------------------------------");
	printf("\x1b[29;10HPress START to exit.");
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;30m                   PID Checker                    \x1b[0m");
	printf("--------------------------------------------------");	
	
	show(var, pid);
	printCursore(cursore);
	PIDchecker(pid, var);
	
	int t_frame = 0;
	int refresh = 0;
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		
		if (((kDown & KEY_DRIGHT) ^ (hidKeysHeld() & KEY_DRIGHT && t_frame % DELAY == 1)) && (posizione[0] < 17)) {
			posCursore(cursore, posizione, 1);
			printCursore(cursore);
		}
		
		if (((kDown & KEY_DLEFT) ^ (hidKeysHeld() & KEY_DLEFT && t_frame % DELAY == 1)) && (posizione[0] > 0)) {
			posCursore(cursore, posizione, -1);
			printCursore(cursore);
		}
		
		if (((kDown & KEY_DUP) ^ (hidKeysHeld() & KEY_DUP && t_frame % DELAY == 1)) && (var[posizione[0]] < 6) && (posizione[0] == 0)) { // posizione 0
			incrementa(var, posizione, 1);
			checkValDec(var);
			refresh = 1;
		}
		if (((kDown & KEY_DUP) ^ (hidKeysHeld() & KEY_DUP && t_frame % DELAY == 1)) && (var[posizione[0]] < 6) && (posizione[0] == 5)) { // posizione 5
			incrementa(var, posizione, 1);
			checkValDec(var);
			refresh = 1;
		}
		if (((kDown & KEY_DUP) ^ (hidKeysHeld() & KEY_DUP && t_frame % DELAY == 1)) && (var[posizione[0]] < 9) && (posizione[0] > 0) && (posizione[0] < 10) && (posizione[0] != 5)) { // posizione 1,2,3,4,6,7,8,9
			incrementa(var, posizione, 1);	
			checkValDec(var);
			refresh = 1;
		}
		if (((kDown & KEY_DUP) ^ (hidKeysHeld() & KEY_DUP && t_frame % DELAY == 1)) && (posizione[0] > 9) && (counterPID[posizione[0] - 10] < 15)) {
			incrementaPID(pid, counterPID, varPID, posizione, 1);
			refresh = 1;
		}
				
		if (((kDown & KEY_DDOWN) ^ (hidKeysHeld() & KEY_DDOWN && t_frame % DELAY == 1)) && (var[posizione[0]] > 0) && (posizione[0] >= 0) && (posizione[0] < 10)) { // tutte le posizioni
			incrementa(var, posizione, -1);
			refresh = 1;
		}
		if (((kDown & KEY_DDOWN) ^ (hidKeysHeld() & KEY_DDOWN && t_frame % DELAY == 1)) && (posizione[0] > 9) && (counterPID[posizione[0] - 10] > 0)) {
			incrementaPID(pid, counterPID, varPID, posizione, -1);
			refresh = 1;
		}
		
		if (kDown & KEY_SELECT) {
			int j = 0;
			posizione[0] = 0;
			for (j = 0; j < 10; j++)
				var[j] = 0;
			for (j = 0; j < 8; j++)
				pid[j] = '0'; 
			cursore[0] = '^';
			for (j = 1; j < 18; j++)
			cursore[j] = ' ';
			for (j = 1; j < 8; j++)
			counterPID[j] = 0;
			printf("\x1b[2J");
			printf("\x1b[47;30m                   PID Checker                    \x1b[0m");
			printf("---------------------------------------------------------");	
			show(var, pid);
			PIDchecker(pid, var);
			printCursore(cursore);	
			
		}
		
		if (kDown & KEY_START)
			break;
		
		if (refresh == 1) {
			show(var, pid);
			PIDchecker(pid, var);
			refresh = 0;
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		t_frame++;
		if (t_frame > 5000) t_frame = 1;
	}
}