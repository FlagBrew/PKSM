#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <string.h>

void injectWC6(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void injectLanguage(u8* mainbuf, int i);
void injectMoney(u8* mainbuf, u64 i);
void injectItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[]);
void injectBP(u8* mainbuf, int i);
void injectBadges(u8* mainbuf, int i);
void injectTM(u8* mainbuf);
void refreshValues(PrintConsole topScreen, int game, u64 money[], int BP[], int langCont, int moneyCont, int ballCont, int itemCont, int HitemCont, int BPCont, int badgeCont, int nInjected[]);
int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int nInjected[]);