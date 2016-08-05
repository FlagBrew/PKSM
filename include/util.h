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
void injectMoney(u8* mainbuf, int i);
void faq(PrintConsole topScreen, PrintConsole bottomScreen);
void refreshValues(PrintConsole topScreen, int game, int langCont);
int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen);