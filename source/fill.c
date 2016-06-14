#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "http.h"

void filldatabase(char *database[], char *links[]) {
	for (int j = 0; j < 27 * 76; j++) {
		database[j] = ".";
		links[j] = "";
	}
	
	database[114] = "0114 - Expo Charizard";
	database[117] = "0117 - Expo Zoroark";
	database[147] = "0147 - Scrap Eevee";
	database[148] = "0148 - Scrap Articuno";
	database[149] = "0149 - Scrap Zapdos";
	database[150] = "0150 - Scrap Moltres";
	database[167] = "0167 - Hyadain Landorus";
	database[510] = "0510 - Arash's Mamoswine";
	database[511] = "0511 - Paris Vivillon";
	database[525] = "0525 - November Diancie";
	database[1060] = "1060 - XYZ Xerneas";
	database[1061] = "1061 - XYZ Yveltal";
	database[1064] = "1064 - XYZ Zygarde";
	database[1503] = "1503 - Fancy Vivillon";

	links[114] = "http://eventcatchersitalia.altervista.org/database/0114_Expo_Charizard.txt";	
	links[117] = "http://eventcatchersitalia.altervista.org/database/0117_Expo_Zoroark.txt";
	links[147] = "http://eventcatchersitalia.altervista.org/database/0147_Scrap_Eevee.txt";
	links[148] = "http://eventcatchersitalia.altervista.org/database/0148_Scrap_Articuno.txt";
	links[149] = "http://eventcatchersitalia.altervista.org/database/0149_Scrap_Zapdos.txt";
	links[150] = "http://eventcatchersitalia.altervista.org/database/0150_Scrap_Moltres.txt";
	links[167] = "http://eventcatchersitalia.altervista.org/database/0167_Hyadain_Landorus.txt";
	links[510] = "http://eventcatchersitalia.altervista.org/database/0510_Arash_Mamoswine.txt";
	links[511] = "http://eventcatchersitalia.altervista.org/database/0511_Paris_Vivillon.txt";
	links[525] = "http://eventcatchersitalia.altervista.org/database/0525_NOVEMBRE14_Diancie.txt";
	links[1060] = "http://eventcatchersitalia.altervista.org/database/1060_XYZ_Xerneas.txt";
	links[1061] = "http://eventcatchersitalia.altervista.org/database/1061_XYZ_Yveltal.txt";
	links[1064] = "http://eventcatchersitalia.altervista.org/database/1064_XYZ_Zygarde.txt";
	links[1503] = "http://eventcatchersitalia.altervista.org/database/1503_Fancy_Vivillon.txt";
}