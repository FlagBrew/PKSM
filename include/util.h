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
void faq(PrintConsole topScreen, PrintConsole bottomScreen);