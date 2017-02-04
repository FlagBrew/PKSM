#include "editor.h"

bool hax;
bool pattern[4];
bool sector[PKMNLENGTH][2];

void fillSectorsHaxMode(bool sector[][2]);
void fillDescriptions(char *descriptions[]);
void fillSectors(bool sector[][2]);