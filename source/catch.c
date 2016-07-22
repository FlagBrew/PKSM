#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include <math.h>
#include "catch.h"
#include "pid.h"

#define DEX 721
#define DELAY 10

void check(int number[]) {
	if ((number[0] * 100 + number[1] * 10 + number[2]) > DEX) {
		number[0] = 7;
		number[1] = 2;
		number[2] = 1;
	}
	if ((number[0] * 100 + number[1] * 10 + number[2]) == 0) {
		number[0] = 0;
		number[1] = 0;
		number[2] = 1;
	}
}

void printCursoreC(char cur[]) {
	printf("\x1b[3;24H%c%c%c", cur[0], cur[1], cur[2]);
	printf("\x1b[3;47H%c", cur[3]);
	printf("\x1b[9;33H%c", cur[4]);
	printf("\x1b[11;15H%c", cur[5]);
	printf("\x1b[13;18H%c", cur[6]);
	printf("\x1b[15;17H%c", cur[7]);
	printf("\x1b[17;31H%c", cur[8]);
}

void showC(int number[], int ratio[], int HP_perc, float bonusballvett[], int bonusindex, float status[], int statusindex, int r, int gen, float captureOgenV[], float captureOgenVI[], int captureOindex, int speed) {
	static char *pokemon[721] = {"Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew", "Sandslash", "Nidoran F", "Nidorina", "Nidoqueen", "Nidoran M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebell", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr. Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup ", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "Mime Jr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickylicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus", "Victini", "Snivy", "Servine", "Serperior", "Tepig", "Pignite", "Emboar", "Oshawott", "Dewott", "Samurott", "Patrat", "Watchog", "Lillipup", "Herdier", "Stoutland", "Purrloin", "Liepard", "Pansage", "Simisage", "Pansear", "Simisear", "Panpour", "Simipour", "Munna", "Musharna", "Pidove", "Tranquill", "Unfezant", "Blitzle", "Zebstrika", "Roggenrola", "Boldore", "Gigalith", "Woobat", "Swoobat", "Drilbur", "Excadrill", "Audino", "Timburr", "Gurdurr", "Conkeldurr", "Tympole", "Palpitoad", "Seismitoad", "Throh", "Sawk", "Sewaddle", "Swadloon", "Leavanny", "Venipede", "Whirlipede", "Scolipede", "Cottonee", "Whimsicott", "Petilil", "Lilligant", "Basculin", "Sandile", "Krokorok", "Krookodile", "Darumaka", "Darmanitan", "Maractus", "Dwebble", "Crustle", "Scraggy", "Scrafty", "Sigilyph", "Yamask", "Cofagrigus", "Tirtouga", "Carracosta", "Archen", "Archeops", "Trubbish", "Garbodor", "Zorua", "Zoroark", "Minccino", "Cinccino", "Gothita", "Gothorita", "Gothitelle", "Solosis", "Duosion", "Reuniclus", "Ducklett", "Swanna", "Vanillite", "Vanillish", "Vanilluxe", "Deerling", "Sawsbuck", "Emolga", "Karrablast", "Escavalier", "Foongus", "Amoonguss", "Frillish", "Jellicent", "Alomomola", "Joltik", "Galvantula", "Ferroseed", "Ferrothorn", "Klink", "Klang", "Klinklang", "Tynamo", "Eelektrik", "Eelektross", "Elgyem", "Beheeyem", "Litwick", "Lampent", "Chandelure", "Axew", "Fraxure", "Haxorus", "Cubchoo", "Beartic", "Cryogonal", "Shelmet", "Accelgor", "Stunfisk", "Mienfoo", "Mienshao", "Druddigon", "Golett", "Golurk", "Pawniard", "Bisharp", "Bouffalant", "Rufflet", "Braviary", "Vullaby", "Mandibuzz", "Heatmor", "Durant", "Deino", "Zweilous", "Hydreigon", "Larvesta", "Volcarona", "Cobalion", "Terrakion", "Virizon", "Tornadus", "Thundurus", "Reshiram", "Zekrom", "Landorus", "Kyurem", "Keldeo", "Meloetta", "Genesect", "Chespin", "Quilladin", "Chesnaught", "Fennekin", "Braixen", "Delphox", "Froakie", "Frogadier", "Greninja", "Bunnelby", "Diggersby", "Fletchling", "Fletchinder", "Talonflame", "Scatterbug", "Spewpa", "Vivillon", "Litleo", "Pyroar", "Flabébé", "Floette", "Florges", "Skiddo", "Gogoat", "Pancham", "Pangoro", "Furfrou", "Espurr", "Meowstic", "Honedge", "Doublade", "Aegislash", "Spritzee", "Aromatisse", "Swirlix", "Slurpuff", "Inkay", "Malamar", "Binacle", "Barbaracle", "Skrelp", "Dragalge", "Clauncher", "Clawitzer", "Helioptile", "Heliolisk", "Tyrunt", "Tyrantrum", "Amaura", "Aurorus", "Sylveon", "Hawlucha", "Dedenne", "Carbink", "Goomy", "Sliggoo", "Goodra", "Klefki", "Phantump", "Trevenant", "Pumpkaboo", "Gourgeist", "Bergmite", "Avalugg", "Noibat", "Noivern", "Xerneas", "Yveltal", "Zygarde", "Diancie", "Hoopa", "Volcanion"};
	
	printf("\x1b[2;0HNumber in National Dex: \x1b[32m%d%d%d\x1b[0m", number[0], number[1], number[2]);
	printf("            Gen is: \x1b[32m%d\x1b[0m", gen);
	printf("\n\nPokemon is: \x1b[32m%s\x1b[0m           ", pokemon[(number[0] * 100 + number[1] * 10 + number[2]) - 1]);
	printf("\n\nHas a catch rate of \x1b[32m%d\x1b[0m  ", ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1]);
	printf("\n\nApprossimative HP percentage is: \x1b[32m%d\x1b[0m%%  ", HP_perc);
	printf("\n\nBall bonus is: \x1b[32m%4.2f\x1b[0mx   ", bonusballvett[bonusindex]);
	printf("\n\nStatus' bonus is: \x1b[32m%4.2f\x1b[0mx  ", status[statusindex]);
	printf("\n\nNumber of tries: \x1b[32m%d\x1b[0m  ", r);
	if (gen == 5)
		printf("\n\nCapture O-Power multiplier is: \x1b[32m%4.2f\x1b[0mx  ", captureOgenV[captureOindex]);
	else if (gen == 6)
		printf("\n\nCapture O-Power multiplier is: \x1b[32m%4.2f\x1b[0mx  ", captureOgenVI[captureOindex]);
	else printf("\n\nCapture O-Power multiplier is: \x1b[32m%d\x1b[0mx   ", 1);
	
	float HP_perc_float = (float)HP_perc / 100;
	
	float a = -1;
	float b = -1;
	
	if (gen == 3 || gen == 4) {
		a = (ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1] * bonusballvett[bonusindex] * status[statusindex]) - (0.666666) * (ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1] * bonusballvett[bonusindex] * status[statusindex]) * (HP_perc_float);
		b = 1048560 / sqrt(sqrt(16711680 / a));
	}
	
	if (gen == 5) {
		a = (ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1] * bonusballvett[bonusindex] * status[statusindex]) - (0.666666) * (ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1] * bonusballvett[bonusindex] * status[statusindex]) * (HP_perc_float);		
		a *= captureOgenV[captureOindex];
		b = 65536 / sqrt(sqrt(255 / a));
	}
	
	if (gen == 6) {
		a = (ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1] * bonusballvett[bonusindex] * status[statusindex]) - (0.666666) * (ratio[(number[0] * 100 + number[1] * 10 + number[2]) - 1] * bonusballvett[bonusindex] * status[statusindex]) * (HP_perc_float);		
		a *= captureOgenVI[captureOindex];
		
		b = 255 / a;
		b = 65536 / pow(b, 0.1875);
	}
	
	float p = ((b + 1) / 65536) * ((b + 1) / 65536) * ((b + 1) / 65536) * ((b + 1) / 65536);
	
	if (a >= 255)
		p = 1;
	else {
		float temp = (b + 1) / 65536;
		for (int i = 0; i < 2; i++)
			temp *= temp;
		p = temp;
	}
	
	float temp = (1 - p);
	for (int i = 0; i < r - 1; i++)
		temp = temp * (1 - p);
	
	float P = 1 - temp;
	
	if (P > 1) P = 1;
	
	// printf("\n\nModified catch rate: \x1b[32m%Lf\x1b[0m   ", a);
	printf("\n\nProbability of capture tends to: \x1b[32m%f\x1b[0m%%      ", (P * 100));
	if (speed == 0) printf("\x1b[28;0HUsage Mode: \x1b[32mFAST\x1b[0m");
	if (speed == 1) printf("\x1b[28;0HUsage Mode: \x1b[32mSLOW\x1b[0m");
}


void catchrate(PrintConsole topScreen, PrintConsole bottomScreen) {
	int ratio[DEX] = {45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 120, 45, 255, 120, 45, 255, 120, 45, 255, 127, 255, 90, 255, 90, 190, 75, 255, 90, 235, 120, 45, 235, 120, 45, 150, 25, 190, 75, 170, 50, 255, 90, 255, 120, 45, 190, 75, 190, 75, 255, 50, 255, 90, 190, 75, 190, 75, 190, 75, 255, 120, 45, 200, 100, 50, 180, 90, 45, 255, 120, 45, 190, 60, 255, 120, 45, 190, 60, 190, 75, 190, 60, 45, 190, 45, 190, 75, 190, 75, 190, 60, 190, 90, 45, 45, 190, 75, 225, 60, 190, 60, 90, 45, 190, 75, 45, 45, 45, 190, 60, 120, 60, 30, 45, 45, 225, 75, 225, 60, 225, 60, 45, 45, 45, 45, 45, 45, 45, 225, 45, 45, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 25, 3, 3, 3, 45, 45, 45, 3, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 90, 255, 90, 255, 90, 255, 90, 90, 190, 75, 190, 150, 170, 190, 75, 190, 75, 235, 120, 45, 45, 190, 75, 65, 45, 255, 120, 45, 45, 235, 120, 75, 255, 90, 45, 45, 30, 70, 45, 225, 45, 60, 190, 75, 190, 60, 25, 190, 75, 45, 25, 190, 45, 60, 120, 60, 190, 75, 225, 75, 60, 190, 75, 45, 25, 25, 120, 45, 45, 120, 60, 45, 45, 45, 75, 45, 45, 45, 45, 45, 30, 3, 3, 3, 45, 45, 45, 3, 3, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 127, 255, 90, 255, 120, 45, 120, 45, 255, 120, 45, 255, 120, 45, 200, 45, 190, 45, 235, 120, 45, 200, 75, 255, 90, 255, 120, 45, 255, 120, 45, 190, 120, 45, 180, 200, 150, 255, 255, 60, 45, 45, 180, 90, 45, 180, 90, 120, 45, 200, 200, 150, 150, 150, 225, 75, 225, 60, 125, 60, 255, 150, 90, 255, 60, 255, 255, 120, 45, 190, 60, 255, 45, 90, 90, 45, 45, 190, 75, 205, 155, 255, 90, 45, 45, 45, 45, 255, 60, 45, 200, 225, 45, 190, 90, 200, 45, 30, 125, 190, 75, 255, 120, 45, 255, 60, 60, 25, 225, 45, 45, 45, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 120, 45, 255, 127, 255, 45, 235, 120, 45, 255, 75, 45, 45, 45, 45, 120, 45, 45, 120, 45, 200, 190, 75, 190, 75, 190, 75, 45, 125, 60, 190, 60, 45, 30, 190, 75, 120, 225, 60, 255, 90, 255, 145, 130, 30, 100, 45, 45, 45, 50, 75, 45, 140, 60, 120, 45, 140, 75, 200, 190, 75, 25, 120, 60, 45, 30, 30, 30, 30, 30, 30, 30, 30, 45, 45, 30, 50, 30, 45, 60, 45, 75, 45, 3, 3, 3, 3, 3, 3, 3, 3, 3, 30, 3, 3, 45, 3, 3, 45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 255, 255, 120, 45, 255, 90, 190, 75, 190, 75, 190, 75, 190, 75, 255, 120, 45, 190, 75, 255, 120, 45, 190, 45, 120, 60, 255, 180, 90, 45, 255, 120, 45, 45, 45, 255, 120, 45, 255, 120, 45, 190, 75, 190, 75, 25, 180, 90, 45, 120, 60, 255, 190, 75, 180, 90, 45, 190, 90, 45, 45, 45, 45, 190, 60, 75, 45, 255, 60,200, 100, 50, 200, 100, 50, 190, 45, 255, 120, 45, 190, 75, 200, 200, 75, 190, 75, 190, 60, 75, 190, 75, 255, 90, 130, 60, 30, 190, 60, 30, 255, 90, 190, 90, 45, 75, 60, 45, 120, 60, 25, 200, 75, 75, 180, 45, 45, 190, 90, 120, 45, 45, 190, 60, 190, 60, 90, 90, 45, 45, 45, 45, 15, 3, 3, 3, 3, 3 ,3, 3, 3, 3, 3, 3, 3, 45, 45, 45, 45, 45, 45, 45, 45, 45, 255, 127, 255, 120, 45, 255, 120, 45, 220, 65, 225, 120, 45, 200, 45, 220, 65, 160, 190, 75, 180, 90, 45, 200, 140, 200, 140, 190, 80, 120, 45, 225, 55, 225, 55, 190, 75, 45, 45, 45, 45, 45, 100, 180, 60, 45, 45, 45, 75, 120, 60, 120, 60, 190, 55, 190, 45, 45, 45, 3, 3, 3, 3};
	float bonusballvett[9] = {1, 1.5, 2, 3, 3.5, 4, 5, 8, 255};
	float status[4] = {1, 1.5, 2, 2.5};
	float captureOgenV[4] = {1, 1.1, 1.2, 1.3};
	float captureOgenVI[4] = {1, 1.5, 2, 2.5};
	int captureOindex = 0;
	
	int posizione[1] = {0};
	char cursore[9] = {'^', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
	
	int number[3] = {0, 0, 1};
	int HP_perc = 100;
	int bonusindex = 0;
	int statusindex = 0;
	int r = 1;
	int gen = 6;
	
	int speed = 0;
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1A\x1B - Move cursor\n");
	printf("\x18\x19 - Change values\n");
	printf("L/R - Change usage mode\n");
	printf("SELECT - Reset values\n");
	printf("----------------------------------------");
	printf("\nSleep, freeze - \x1b[32m2\x1b[0mx (III/IV), \x1b[32m2.5\x1b[0mx (V/VI)");
	printf("Burn, paralysis, poison - \x1b[32m1.5\x1b[0mx");
	printf("\nNo status conditions - \x1b[32m1\x1b[0mx\n\n");
	printf("----------------------------------------");
	printf("\nPoke/Friend/Premier/Luxury/Heal ball- \x1b[32m1\x1b[mx");
	printf("Mega/Safari/Sport ball - \x1b[32m1.5\x1b[0mx");
	printf("\nUltra ball - \x1b[32m2\x1b[0mx");
	printf("\nLure/Net/Repeat ball - \x1b[32m3\x1b[mx (cond. only)");
	printf("\nDive/Dusk ball - \x1b[32m3.5\x1b[0mx (cond. only)");
	printf("\nMoon/Fast/Nest/Timer/Quick ball - \x1b[32m4\x1b[mx");
	printf("\nFirst turn Quick ball - \x1b[32m5\x1b[0mx");
	printf("\nLove/Level ball - \x1b[32m8\x1b[0mx (cond. only)");
	printf("\nMaster ball - \x1b[32m255\x1b[0mx");
	printf("\x1b[29;12HPress B to exit.");
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;34m          Capture Probability Calculator          \x1b[0m");
	printf("--------------------------------------------------");
	showC(number, ratio, HP_perc, bonusballvett, bonusindex, status, statusindex, r, gen, captureOgenV, captureOgenVI, captureOindex, speed);
	printCursoreC(cursore);
	
	int refresh = 0;
	int t_frame = 1;
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B)
			break;
		
		if (hidKeysDown() & KEY_R) {
			speed = 1;
			showC(number, ratio, HP_perc, bonusballvett, bonusindex, status, statusindex, r, gen, captureOgenV, captureOgenVI, captureOindex, speed);
		}
		
		if (hidKeysDown() & KEY_L) {
			speed = 0;
			showC(number, ratio, HP_perc, bonusballvett, bonusindex, status, statusindex, r, gen, captureOgenV, captureOgenVI, captureOindex, speed);
		}
		
		if ((hidKeysDown() & KEY_DRIGHT) && (posizione[0] < 8)) {
			posCursore(cursore, posizione, 1);
			showC(number, ratio, HP_perc, bonusballvett, bonusindex, status, statusindex, r, gen, captureOgenV, captureOgenVI, captureOindex, speed);
			printCursoreC(cursore);
		}
		
		if ((hidKeysDown() & KEY_DLEFT) && (posizione[0] > 0)) {
			posCursore(cursore, posizione, -1);
			showC(number, ratio, HP_perc, bonusballvett, bonusindex, status, statusindex, r, gen, captureOgenV, captureOgenVI, captureOindex, speed);
			printCursoreC(cursore);
		}
		if (speed == 1) {
			if (hidKeysDown() & KEY_DUP) {
				if (posizione[0] == 0) {
					if (number[posizione[0]] < 9) {
						number[posizione[0]]++;
						check(number);
					}
				}
				if (posizione[0] == 1 || posizione[0] == 2) {
					if (number[posizione[0]] < 9) {
						number[posizione[0]]++;
						check(number);
					}
				}
				if (posizione[0] == 3) {
					if (gen < 6) gen++;
				}
				if (posizione[0] == 4) {
					if (HP_perc < 100) HP_perc += 1;
					else if (HP_perc == 100) HP_perc = 1;
				}
				if (posizione[0] == 5) {
					if (bonusindex < 8) bonusindex++;
				}
				if (posizione[0] == 6) {
					if (statusindex < 3) statusindex++;
				}
				if (posizione[0] == 7) {
					if (r < 200) r++;
				}
				if (posizione[0] == 8) {
					if (captureOindex < 3) captureOindex++;
				}
				refresh = 1;
			}
			
			if (hidKeysDown() & KEY_DDOWN) {
				if (posizione[0] == 0 || posizione[0] == 1 || posizione[0] == 2) {
					if (number[posizione[0]] > 0) {
						number[posizione[0]]--;
						check(number);
					}
				}
				if (posizione[0] == 3) {
					if (gen > 3) gen--;
				}
				if (posizione[0] == 4) {
					if (HP_perc > 1) HP_perc -= 1;
					else if (HP_perc == 1) HP_perc = 100;
				}
				if (posizione[0] == 5) {
					if (bonusindex > 0) bonusindex--;
				}
				if (posizione[0] == 6) {
					if (statusindex > 0) statusindex--;
				}
				if (posizione[0] == 7) {
					if (r > 1) r--;
				}
				if (posizione[0] == 8) {
					if (captureOindex > 0) captureOindex--;
				}
				refresh = 1;
			}
		}
		
		if (speed == 0) {
			if ((hidKeysDown() & KEY_DUP) ^ (hidKeysHeld() & KEY_DUP && t_frame % DELAY == 1)) {
				if (posizione[0] == 0) {
					if (number[posizione[0]] < 9) {
						number[posizione[0]]++;
						check(number);
					}
				}
				if (posizione[0] == 1 || posizione[0] == 2) {
					if (number[posizione[0]] < 9) {
						number[posizione[0]]++;
						check(number);
					}
				}
				if (posizione[0] == 3) {
					if (gen < 6) gen++;
				}
				if (posizione[0] == 4) {
					if (HP_perc < 100) HP_perc += 1;
					else if (HP_perc == 100) HP_perc = 1;
				}
				if (posizione[0] == 5) {
					if (bonusindex < 8) bonusindex++;
				}
				if (posizione[0] == 6) {
					if (statusindex < 3) statusindex++;
				}
				if (posizione[0] == 7) {
					if (r < 200) r++;
				}
				if (posizione[0] == 8) {
					if (captureOindex < 3) captureOindex++;
				}
				refresh = 1;
			}
			
			if ((hidKeysDown() & KEY_DDOWN) ^ (hidKeysHeld() & KEY_DDOWN && t_frame % DELAY == 1)) {
				if (posizione[0] == 0 || posizione[0] == 1 || posizione[0] == 2) {
					if (number[posizione[0]] > 0) {
						number[posizione[0]]--;
						check(number);
					}
				}
				if (posizione[0] == 3) {
					if (gen > 3) gen--;
				}
				if (posizione[0] == 4) {
					if (HP_perc > 1) HP_perc -= 1;
					else if (HP_perc == 1) HP_perc = 100;
				}
				if (posizione[0] == 5) {
					if (bonusindex > 0) bonusindex--;
				}
				if (posizione[0] == 6) {
					if (statusindex > 0) statusindex--;
				}
				if (posizione[0] == 7) {
					if (r > 1) r--;
				}
				if (posizione[0] == 8) {
					if (captureOindex > 0) captureOindex--;
				}
				refresh = 1;
			}
		}
		
		if (hidKeysDown() & KEY_SELECT) {
			posizione[0] = 0;
			cursore[0] = '^';
			for (int i = 1; i < 7; i++)
				cursore[i] = ' ';
			number[0] = 0;
			number[1] = 0;
			number[2] = 1;
			HP_perc = 100;
			bonusindex = 0;
			statusindex = 0;
			r = 1;
			gen = 6;
			captureOindex = 0;
			printf("\x1b[2J");
			printf("\x1b[47;34m          Capture Probability Calculator          \x1b[0m");
			printf("--------------------------------------------------");
			refresh = 1;
			printCursoreC(cursore);			
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		
		t_frame++;
		if (t_frame > 5000) t_frame = 1;
		
		if (refresh == 1) {
			showC(number, ratio, HP_perc, bonusballvett, bonusindex, status, statusindex, r, gen, captureOgenV, captureOgenVI, captureOindex, speed);
			refresh = 0;
		}
	}
}