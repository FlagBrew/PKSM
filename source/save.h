/*
* This file is part of PKSM
* Copyright (C) 2016 Bernardo Giordano
*
* This software is provided 'as-is', 
* without any express or implied warranty. 
* In no event will the authors be held liable for any damages 
* arising from the use of this software.
*
* This code is subject to the following restrictions:
*
* 1) The origin of this software must not be misrepresented; 
* 2) You must not claim that you wrote the original software. 
*
*/

#include <3ds.h>

int getActiveGBO(u8* mainbuf, int game);
int getActiveSBO(u8* mainbuf, int game);
u32 CHKOffset(u32 i, int game);
u32 CHKLength(u32 i, int game);
u16 getBlockID(u8* mainbuf, int csoff, u32 i);
u16 ccitt16(u8* data, u32 len);
u16 check16(u8 data[], u32 blockID, u32 len);
u32 BWCHKOff(u32 i, int game);
u32 BWCHKMirr(u32 i, int game);
void rewriteCHK(u8 *mainbuf, int game);
void rewriteCHK4(u8 *mainbuf, int game, int GBO, int SBO);