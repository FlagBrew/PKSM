#include <stdlib.h>
#include <3ds.h>

void injectWC6(u8* mainbuf, u8* wc6buf, int game, int i, int nInjected[]);
void injectLanguage(u8* mainbuf, int i);
void injectMoney(u8* mainbuf, u64 i);
void injectItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[], int game);
void injectBP(u8* mainbuf, int i, int game);
void injectBadges(u8* mainbuf, int i);
void injectTM(u8* mainbuf);
void refreshValues(PrintConsole topScreen, int injectCont[], int nInjected[]);
int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8* mainbuf, int game, int nInjected[], int injectCont[]);