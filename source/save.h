#include <3ds.h>

int getActiveGBO(u8* mainbuf, int game);
int getActiveSBO(u8* mainbuf, int game);
u32 CHKOffset(u32 i, int game);
u32 CHKLength(u32 i, int game);
u32 getBlockID(u8* mainbuf, int csoff, u32 i);
u16 ccitt16(u8* data, u32 len);
u16 check16(u8 data[], u32 blockID, u32 len);
u32 BWCHKOff(u32 i, int game);
u32 BWCHKMirr(u32 i, int game);
void rewriteCHK(u8 *mainbuf, int game);
void rewriteCHK4(u8 *mainbuf, int game, int GBO, int SBO);