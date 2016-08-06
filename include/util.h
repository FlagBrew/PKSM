#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N);
void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page);
void update(PrintConsole topScreen, PrintConsole bottomScreen);
u32 CHKOffset(u32 i, int game);
u32 CHKLength(u32 i, int game);
u16 ccitt16(u8* data, u32 len);
int rewriteCHK(u8 *mainbuf, int game);
void injectWC6(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void injectLanguage(u8* mainbuf, int i);
void injectMoney(u8* mainbuf, u64 i);
void injectItem(u8* mainbuf, int i);
void injectHItem(u8* mainbuf, int i);
void injectBP(u8* mainbuf, int i);
void injectBadges(u8* mainbuf, int i);
void injectTM(u8* mainbuf);
void faq(PrintConsole topScreen, PrintConsole bottomScreen);
void refreshValues(PrintConsole topScreen, int game, int langCont, u64 money[], int moneyCont, char* item[], int itemCont, char* Hitem[], int HitemCont, int BP[], int BPCont, int badgeCont);
int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[]);