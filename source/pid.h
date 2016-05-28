#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

void hexa2bin (int binary[], char hexadecimal[], int nbit);
long unsigned int hexa2dec (char hexadecimal[], int binary[], int nbit);
void PIDchecker(char pid_hexa_vett[], int var[]);
void posCursore (char cursore[], int pos[], int temp);
void incrementa(int var[], int posizione[], int temp);
void incrementaPID(char pid[], int counterPID[], char varPID[], int posizione[], int temp);
void checkValDec(int var[]);
void printCursore(char cur[]);
void PID(PrintConsole topScreen, PrintConsole bottomScreen);