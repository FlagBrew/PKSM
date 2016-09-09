/*
* This file is part of EventAssistant
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

#include <stdlib.h>
#include <3ds.h>

#define TOP 50
#define BOTTOM 40

/* Errors
	 1: Game not found
	 2: OpenContext failed
	 3: AddRequestHeaderField failed
	 4: SSLOpt failed
	 5: BeginRequest failed
	 6: Response code failed
	 7: New header failed
	 8: Redirection failed
	 9: Download size error
	 10: Buffer alloc error
	 11: DownloadData failed
	 12: Feature N/A in XY
	 13: Switch game also in the app
	 14: Maximum item reached
	 15: File not available
	 16: Selected slot is empty
	 17: No cart inserted
	 18: No NDS cart inserted
 */

void printfile(char* path);
void waitKey(u32 key);
int waitKeyRet();
void refresh(int currentEntry, PrintConsole topScreen, char *lista[], int N);
void refreshDB(int currentEntry, PrintConsole topScreen, char *lista[], int N, int page);
void update(PrintConsole topScreen, PrintConsole bottomScreen);
void credits(PrintConsole topScreen, PrintConsole bottomScreen);
bool isHBL();
void fsStart();
void fsEnd();
bool openSaveArch(FS_Archive *out, u64 id);
void errDisp(PrintConsole screen, int i, u16 columns);
void infoDisp(PrintConsole screen, int i, u16 columns);