#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

Result http_download(httpcContext *context);
void getText(char *url);
void printDistro(PrintConsole topScreen, PrintConsole bottomScreen, char *url);
void printPSdates(PrintConsole topScreen, PrintConsole bottomScreen, char *url);
