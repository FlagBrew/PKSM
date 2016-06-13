#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "http.h"

void filldatabase(char *database[], char *links[]) {
	for (int j = 0; j < 27 * 76; j++) {
		database[j] = "";
		links[j] = "";
	}
}