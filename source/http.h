#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

Result http_download(PrintConsole topScreen, PrintConsole bottomScreen, httpcContext *context);
void getText(PrintConsole topScreen, PrintConsole bottomScreen, char *url);
void printDistro(PrintConsole topScreen, PrintConsole bottomScreen, char *url);
void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int n);
Result downloadFile(PrintConsole topScreen, PrintConsole bottomScreen, char* url, char* path);
Result printDB(PrintConsole topScreen, PrintConsole bottomScreen, char *url, int i);