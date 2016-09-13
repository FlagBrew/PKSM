/*
* This file is part of EventAssistant
* Copyright (C) 2016 Bernardo Giordano
*
* Credits to SlashCash & PCHex++ for lots of functions redistributed in this software.
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
#include <string.h>
#include "pokemon.h"
#include "util.h"

#define PK6ENTRIES 14
#define ITEMENTRIES 13

#define BALLS 25
#define ITEM 18
#define HEAL 26
#define BERRIES 64

/* ************************ local variables ************************ */

const int ORASWC6FLAGPOS = 0x1CC00;
const int ORASWC6POS = 0x1CD00;
const int XYWC6FLAGPOS = 0x1BC00;
const int XYWC6POS = 0x1BD00;
const int DPPGTFLAGPOS = 0xA6D0;
const int DPPGTPOS = 0xA7FC;
const int PTPGTFLAGPOS = 0xB4C0;
const int PTPGTPOS = 0xB5C0;
const int HGSSPGTFLAGPOS = 0x9D3C;
const int HGSSPGTPOS = 0x9E3C;
const int EONFLAGPOS = 0x319B8;
const int LANGUAGEPOS = 0x1402D;
const int MONEYPOS = 0x4208;
const int BADGEPOS = 0x420C;
const int TMSTARTPOS = 0xBC0;
const int OFFSET = 0x5400;
const int EVPOS = 0x1E;
const int PIDPOS = 0x18;
const int IVPOS = 0x74;
const int OTIDPOS = 0x0C;
const int SOTIDPOS = 0x0E;
const int NICKNAMEPOS = 0x40;
const int POKEDEXNUMBERPOS = 0x08;
const int NATUREPOS = 0x1C;
const int FRIENDSHIPPOS = 0xA2;
const int PGFSTARTPOS = 0x1C800;
const int BWSEEDPOS = 0x1D290;

char *stats[] = {"All", "Health", "Attack", "Defense", "Speed", "Sp. Attack", "Sp. Defense"};
char *language[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
char *ballsList[BALLS] = {"Master Ball", "Ultra Ball", "Great Ball", "Poke Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Fast Ball", "Level Ball", "Lure Ball", "Heavy Ball", "Love Ball", "Friend Ball", "Moon Ball", "Sport Ball", "Park Ball"};
char *itemList[ITEM] = {"Mental Herb", "Choice Band", "King's Rock", "Amulet Coin", "Leftovers", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Focus Sash", "Destiny Knot", "Choice Scarf", "Choice Specs", "Eviolite", "Rocky Helmet", "Air Balloon", "Weakness Policy", "Assault Vest"};
char *healList[HEAL] = {"Full Restore", "Max Potion", "Revive", "Max Revive", "Ether", "Max Ether", "Elixir", "Max Elixir", "Sacred Ash", "HP Up   ", "Protein", "Iron   ", "Carbos", "Calcium", "Rare Candy", "PP UP  ", "Zinc   ", "PP Max ", "Health Wing", "Muscle Wing", "Resist Wing", "Genius Wing", "Clever Wing", "Swift Wing", "Pretty Wing", "Ability Capsule"};

u32 langValues[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08};
u32 balls[BALLS * 2] = {0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0X0E, 0X00, 0x0F, 0x00, 0x10, 0x00, 0xEC, 0x01, 0xED, 0x01, 0xEE, 0x01, 0xEF, 0x01, 0xF0, 0x01, 0xF2, 0x01, 0xF3, 0x01, 0xF4, 0x01};
u32 items[ITEM * 2] = {0xDB, 0x00, 0xDC, 0x00, 0xDD, 0x00, 0xDF, 0x00, 0xEA, 0x00, 0x0E, 0x01, 0x0F, 0x01, 0x10, 0x01, 0x11, 0x01, 0x13, 0x01, 0x18, 0x01, 0x1F, 0x01, 0x29, 0x01, 0x1A, 0x02, 0x1C, 0x02, 0x1D, 0x02, 0x7F, 0x02, 0x80, 0x02};
u32 heal[HEAL * 2] = {0x17, 0x00, 0x18, 0x00, 0x1C, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2C, 0x00, 0x2D, 0x00, 0x2E, 0x00, 0x2F, 0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x35, 0x02, 0x36, 0x02, 0x37, 0x02, 0x38, 0x02, 0x39, 0x02, 0x3A, 0x02, 0x3B, 0x02, 0x85, 0x02};

u64 money[4] = {0, 200000, 1000000, 9999999};
int BP[2] = {0, 9999};
const int friendship[4] = {0, 70, 75, 255};

char *hpList[16] = {"Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel", "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark"};
static char *pokemon[722] = {"None", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew", "Sandslash", "Nidoran F", "Nidorina", "Nidoqueen", "Nidoran M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebell", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr. Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup ", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "Mime Jr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickylicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus", "Victini", "Snivy", "Servine", "Serperior", "Tepig", "Pignite", "Emboar", "Oshawott", "Dewott", "Samurott", "Patrat", "Watchog", "Lillipup", "Herdier", "Stoutland", "Purrloin", "Liepard", "Pansage", "Simisage", "Pansear", "Simisear", "Panpour", "Simipour", "Munna", "Musharna", "Pidove", "Tranquill", "Unfezant", "Blitzle", "Zebstrika", "Roggenrola", "Boldore", "Gigalith", "Woobat", "Swoobat", "Drilbur", "Excadrill", "Audino", "Timburr", "Gurdurr", "Conkeldurr", "Tympole", "Palpitoad", "Seismitoad", "Throh", "Sawk", "Sewaddle", "Swadloon", "Leavanny", "Venipede", "Whirlipede", "Scolipede", "Cottonee", "Whimsicott", "Petilil", "Lilligant", "Basculin", "Sandile", "Krokorok", "Krookodile", "Darumaka", "Darmanitan", "Maractus", "Dwebble", "Crustle", "Scraggy", "Scrafty", "Sigilyph", "Yamask", "Cofagrigus", "Tirtouga", "Carracosta", "Archen", "Archeops", "Trubbish", "Garbodor", "Zorua", "Zoroark", "Minccino", "Cinccino", "Gothita", "Gothorita", "Gothitelle", "Solosis", "Duosion", "Reuniclus", "Ducklett", "Swanna", "Vanillite", "Vanillish", "Vanilluxe", "Deerling", "Sawsbuck", "Emolga", "Karrablast", "Escavalier", "Foongus", "Amoonguss", "Frillish", "Jellicent", "Alomomola", "Joltik", "Galvantula", "Ferroseed", "Ferrothorn", "Klink", "Klang", "Klinklang", "Tynamo", "Eelektrik", "Eelektross", "Elgyem", "Beheeyem", "Litwick", "Lampent", "Chandelure", "Axew", "Fraxure", "Haxorus", "Cubchoo", "Beartic", "Cryogonal", "Shelmet", "Accelgor", "Stunfisk", "Mienfoo", "Mienshao", "Druddigon", "Golett", "Golurk", "Pawniard", "Bisharp", "Bouffalant", "Rufflet", "Braviary", "Vullaby", "Mandibuzz", "Heatmor", "Durant", "Deino", "Zweilous", "Hydreigon", "Larvesta", "Volcarona", "Cobalion", "Terrakion", "Virizon", "Tornadus", "Thundurus", "Reshiram", "Zekrom", "Landorus", "Kyurem", "Keldeo", "Meloetta", "Genesect", "Chespin", "Quilladin", "Chesnaught", "Fennekin", "Braixen", "Delphox", "Froakie", "Frogadier", "Greninja", "Bunnelby", "Diggersby", "Fletchling", "Fletchinder", "Talonflame", "Scatterbug", "Spewpa", "Vivillon", "Litleo", "Pyroar", "Flabébé", "Floette", "Florges", "Skiddo", "Gogoat", "Pancham", "Pangoro", "Furfrou", "Espurr", "Meowstic", "Honedge", "Doublade", "Aegislash", "Spritzee", "Aromatisse", "Swirlix", "Slurpuff", "Inkay", "Malamar", "Binacle", "Barbaracle", "Skrelp", "Dragalge", "Clauncher", "Clawitzer", "Helioptile", "Heliolisk", "Tyrunt", "Tyrantrum", "Amaura", "Aurorus", "Sylveon", "Hawlucha", "Dedenne", "Carbink", "Goomy", "Sliggoo", "Goodra", "Klefki", "Phantump", "Trevenant", "Pumpkaboo", "Gourgeist", "Bergmite", "Avalugg", "Noibat", "Noivern", "Xerneas", "Yveltal", "Zygarde", "Diancie", "Hoopa", "Volcanion"};
static char *natures[25] = {"Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed", "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest", "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky"};

/* ************************ utilities ************************ */

int getActiveGBO(u8* mainbuf, int game) {
	int ofs = 0;
	int generalBlock = -1;
	
	int k = 0;
	u8 temp[10];
	
	memcpy(&temp, mainbuf, 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 1;
	
	k = 0;
	memcpy(&temp, &mainbuf[0x40000], 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 1;
	
	if (game == 8 || game == 9)
		ofs = 0xF626;
	else if (game == 10) 
		ofs = 0xCF1C;
	else if (game == 11 || game == 12)
		ofs = 0xC0F0;
	
	u16 c1;
	u16 c2;
	memcpy(&c1, &mainbuf[ofs], 2);
	memcpy(&c2, &mainbuf[ofs + 0x40000], 2);
	
	generalBlock = (c1 >= c2) ? 0 : 1;
	
	return generalBlock;
}

int getActiveSBO(u8* mainbuf, int game) {
	int ofs = 0;
	int storageBlock = -1;
	
	if (game == 8 || game == 9)
		ofs = 0x21A00;
	else if (game == 10) 
		ofs = 0x1F100;
	else if (game == 11 || game == 12)
		ofs = 0x1E2D0;
	
	int k = 0;
	u8 temp[10];
	
	memcpy(&temp, &mainbuf[ofs], 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 1;
	
	k = 0;
	memcpy(&temp, &mainbuf[ofs + 0x40000], 10);
	for (int t = 0; t < 10; t++)
		if (temp[t] == 0xFF)
			k++;
	if (k == 10)
		return 0;
	
	u16 c1;
	u16 c2;
	memcpy(&c1, &mainbuf[ofs], 2);
	memcpy(&c2, &mainbuf[ofs + 0x40000], 2);
	
	storageBlock = (c1 >= c2) ? 0 : 1;
	
	return storageBlock;
}

u32 CHKOffset(u32 i, int game) {
	if (game == 0 || game == 1) {
		const u32 _xy[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1AC00, 0x1B400, 0x1B600, 0x1B800, 0x1BE00, 0x1C000, 0x1C400, 0x1CC00, 0x1CE00, 0x1D000, 0x1D200, 0x1D400, 0x1D600, 0x1DE00, 0x1E400, 0x1E800, 0x20400, 0x20600, 0x20800, 0x20C00, 0x21000, 0x22C00, 0x23000, 0x23800, 0x23C00, 0x24600, 0x24A00, 0x25200, 0x26000, 0x26200, 0x26400, 0x27200, 0x27A00, 0x5C600, };
		return _xy[i] - 0x5400;
	}
	else if (game == 2 || game == 3) {
		const u32 _oras[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1B600, 0x1BE00, 0x1C000, 0x1C200, 0x1C800, 0x1CA00, 0x1CE00, 0x1D600, 0x1D800, 0x1DA00, 0x1DC00, 0x1DE00, 0x1E000, 0x1E800, 0x1EE00, 0x1F200, 0x20E00, 0x21000, 0x21400, 0x21800, 0x22000, 0x23C00, 0x24000, 0x24800, 0x24C00, 0x25600, 0x25A00, 0x26200, 0x27000, 0x27200, 0x27400, 0x28200, 0x28A00, 0x28E00, 0x30A00, 0x38400, 0x6D000, };
		return _oras[i] - 0x5400;
	}
	else if (game == 4 || game == 5) {
		const u32 _bw[] = { 0x1C800, 0x23F00 };
		return _bw[i];
	}
	else if (game == 6 || game == 7) {
		const u32 _b2w2[] = { 0x1C800, 0x25F00 }; 
		return _b2w2[i];
	}
	else return 0;
}

u32 CHKLength(u32 i, int game) {
	if (game == 0 || game == 1) {
		const u32 _xy[] = { 0x002C8, 0x00B88, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 0x001C0, 0x000BE, 0x00024, 0x02100, 0x00140, 0x00440, 0x00574, 0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x006A0, 0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 0x002F8, 0x01B40, 0x001F4, 0x001F0, 0x00216, 0x00390, 0x01A90, 0x00308, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 0x00078, 0x00200, 0x00C84, 0x00628, 0x34AD0, 0x0E058, };
		return _xy[i];
	}
	else if (game == 2 || game == 3) {
		const u32 _oras[] = { 0x002C8, 0x00B90, 0x0002C, 0x00038, 0x00150, 0x00004, 0x00008, 0x001C0, 0x000BE, 0x00024, 0x02100, 0x00130, 0x00440, 0x00574, 0x04E28, 0x04E28, 0x04E28, 0x00170, 0x0061C, 0x00504, 0x011CC, 0x00644, 0x00104, 0x00004, 0x00420, 0x00064, 0x003F0, 0x0070C, 0x00180, 0x00004, 0x0000C, 0x00048, 0x00054, 0x00644, 0x005C8, 0x002F8, 0x01B40, 0x001F4, 0x003E0, 0x00216, 0x00640, 0x01A90, 0x00400, 0x00618, 0x0025C, 0x00834, 0x00318, 0x007D0, 0x00C48, 0x00078, 0x00200, 0x00C84, 0x00628, 0x00400, 0x07AD0, 0x078B0, 0x34AD0, 0x0E058, };
		return _oras[i];
	}
	else if (game == 4 || game == 5) {
		const u32 _bw[] = { 0x0A94, 0x008C };
		return _bw[i];
	}
	else if (game == 6 || game == 7) {
		const u32 _b2w2[] = { 0x0A94, 0x0094 };
		return _b2w2[i];
	}
	else return 0;
}

u32 BWCHKOff(u32 i, int game) {
	if (game == 4 || game == 5) {
		const u32 _bw[] = { 0x1D296, 0x23F9A };
		return _bw[i];
	}
	else if (game == 6 || game == 7) {
		const u32 _b2w2[]= { 0x1D296,  0x25FA2 };
		return _b2w2[i];
	}
	else return 0;
}

u32 BWCHKMirr(u32 i, int game) {
	if (game == 4 || game == 5) {
		const u32 _bw[] = { 0x23F44, 0x23F9A };
		return _bw[i];
	}
	
	else if (game == 6 || game == 7) {
		const u32 _b2w2[] = { 0x25F44, 0x25FA2 };
		return _b2w2[i];
	}

	else return 0;	
}

u16 ccitt16(u8* data, u32 len) {
	u16 crc = 0xFFFF;

	for (u32 i = 0; i < len; i++) {
		crc ^= (u16) (data[i] << 8);

		for (u32 j = 0; j < 0x8; j++) {
			if ((crc & 0x8000) > 0)
				crc = (u16) ((crc << 1) ^ 0x1021);
			else
				crc <<= 1;
		}
	}

	return crc;
}

void rewriteCHK(u8 *mainbuf, int game) {
	u8 blockCount = 0;
	u32 csoff = 0;
	
	if (game == 0 || game == 1) {
		blockCount = 55;
		csoff = 0x6A81A - 0x5400;
	}

	if (game == 2 || game == 3) {
		blockCount = 58;
		csoff = 0x7B21A - 0x5400;
	}
	
	if (game == 4 || game == 5 || game == 6 || game == 7)
		blockCount = 2;

	u8* tmp = (u8*)malloc(0x35000 * sizeof(u8));
	u16 cs;

	if (game < 4) // XYORAS
		for (u32 i = 0; i < blockCount; i++) {
			memcpy(tmp, mainbuf + CHKOffset(i, game), CHKLength(i, game));
			cs = ccitt16(tmp, CHKLength(i, game));
			memcpy(mainbuf + csoff + i * 8, &cs, 2);
		}
	else if (game == 4 || game == 5 || game == 6 || game == 7) //BWB2W2
		for (u32 i = 0; i < blockCount; i++) {
			memcpy(tmp, mainbuf + CHKOffset(i, game), CHKLength(i, game));
			cs = ccitt16(tmp, CHKLength(i, game));
			memcpy(mainbuf + BWCHKOff(i, game), &cs, 2);
			memcpy(mainbuf + BWCHKMirr(i, game), &cs, 2);
		}

	free(tmp);
}

void rewriteCHK4(u8 *mainbuf, int game, int GBO, int SBO) {
	u8* tmp = (u8*)malloc(0x35000 * sizeof(u8));
	u16 cs;

	if (game == 8 || game == 9) { //HGSS
		memcpy(tmp, mainbuf + GBO, 0xF618);
		cs = ccitt16(tmp, 0xF618);
		memcpy(mainbuf + GBO + 0xF626, &cs, 2);

		memcpy(tmp, mainbuf + SBO + 0xF700, 0x12300);
		cs = ccitt16(tmp, 0x12300);
		memcpy(mainbuf + SBO + 0x21A0E, &cs, 2);
	}
	else if (game == 10) { //PT
		memcpy(tmp, mainbuf + GBO, 0xCF18);
		cs = ccitt16(tmp, 0xCF18);
		memcpy(mainbuf + GBO + 0xCF2A, &cs, 2);

		memcpy(tmp, mainbuf + SBO + 0xCF2C, 0x121D0);
		cs = ccitt16(tmp, 0x121D0);
		memcpy(mainbuf + SBO + 0x1F10E, &cs, 2);		
	}
	else if (game == 11 || game == 12) { //DP
		memcpy(tmp, mainbuf + GBO, 0xC0EC);
		cs = ccitt16(tmp, 0xC0EC);
		memcpy(mainbuf + GBO + 0xC0FE, &cs, 2);

		memcpy(tmp, mainbuf + SBO + 0xC100, 0x121CC);
		cs = ccitt16(tmp, 0x121CC);
		memcpy(mainbuf + SBO + 0x1E2DE, &cs, 2);			
	}
	free(tmp);
}

u32 seedStep(const u32 seed) {
    return (seed * 0x41C64E6D + 0x00006073) & 0xFFFFFFFF;
}

u32 LCRNG(u32 seed) {
	return seed * 0x41C64E6D + 0x00006073;
}

void shuffleArray(u8* pkmn, const u32 encryptionkey) {
    const int BLOCKLENGHT = 56;
    
    u8 seed = (((encryptionkey & 0x3E000) >> 0xD) % 24);
    
    int aloc[24] = { 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 };
    int bloc[24] = { 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 };
    int cloc[24] = { 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1 };
    int dloc[24] = { 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0 };
    int ord[4] = {aloc[seed], bloc[seed], cloc[seed], dloc[seed]};
    
    char pkmncpy[PKMNLENGTH];
    char tmp[BLOCKLENGHT];

    memcpy(&pkmncpy, pkmn, PKMNLENGTH);
    
    for (int i = 0; i < 4; i++) {
        memcpy(tmp, pkmncpy + 8 + BLOCKLENGHT * ord[i], BLOCKLENGHT);
        memcpy(pkmn + 8 + BLOCKLENGHT * i, tmp, BLOCKLENGHT);
    }
}

void decryptPkmn(u8* pkmn) {
    const int ENCRYPTIONKEYPOS = 0x0;
    const int ENCRYPTIONKEYLENGHT = 4;
    const int CRYPTEDAREAPOS = 0x08;
    
    u32 encryptionkey;
    memcpy(&encryptionkey, &pkmn[ENCRYPTIONKEYPOS], ENCRYPTIONKEYLENGHT);
    u32 seed = encryptionkey;
    
    u16 temp;
    for (int i = CRYPTEDAREAPOS; i < PKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (seedStep(seed) >> 16);
        seed = seedStep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
    
    shuffleArray(pkmn, encryptionkey);
}

int getPkmnAddress(const int boxnumber, const int indexnumber, int game) {
    int boxpos = 0;
    if (game == 0 || game == 1) 
		boxpos = 0x27A00 - OFFSET;
   
    if (game == 2 || game == 3) 
		boxpos = 0x38400 - OFFSET;

    const int PKMNNUM = 30;
    
	return boxpos + (PKMNLENGTH * PKMNNUM * boxnumber) + (indexnumber * PKMNLENGTH);
}

void calculatePKMNChecksum(u8* data) {
    u16 chk = 0;

    for (int i = 8; i < PKMNLENGTH; i += 2)
        chk += *(u16*)(data + i);
    
    memcpy(data + 6, &chk, 2);
}

void encryptPkmn(u8* pkmn) {
    const int ENCRYPTIONKEYPOS = 0x0;
    const int ENCRYPTIONKEYLENGHT = 4;
    const int CRYPTEDAREAPOS = 0x08;
    
    u32 encryptionkey;
    memcpy(&encryptionkey, &pkmn[ENCRYPTIONKEYPOS], ENCRYPTIONKEYLENGHT);
    u32 seed = encryptionkey;
    
    for(int i = 0; i < 11; i++)
        shuffleArray(pkmn, encryptionkey);
        
    u16 temp;
    for(int i = CRYPTEDAREAPOS; i < PKMNLENGTH; i += 2) {
        memcpy(&temp, &pkmn[i], 2);
        temp ^= (seedStep(seed) >> 16);
        seed = seedStep(seed);
        memcpy(&pkmn[i], &temp, 2);
    }
}

void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    memcpy(pkmn, &mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], PKMNLENGTH);
    decryptPkmn(pkmn);
}

void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    calculatePKMNChecksum(pkmn);
    encryptPkmn(pkmn);
        
    memcpy(&mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], pkmn, PKMNLENGTH);
}

bool isShiny(u8* pkmn) {
    u16 trainersv = (getOTID(pkmn) ^ getSOTID(pkmn)) >> 4;
    u16 pkmnv = ((getPID(pkmn) >> 16) ^ (getPID(pkmn) & 0xFFFF)) >> 4;
    
    if( trainersv == pkmnv) return true;
    else return false;
}

void rerollPID(u8* pkmn) {
    srand(getPID(pkmn));
    u32 pidbuffer = rand();
    
    memcpy(&pkmn[PIDPOS], &pidbuffer, PIDLENGTH);
}

void findFreeLocationWC(u8 *mainbuf, int game, int nInjected[]) {
	nInjected[0] = 0;
	int temp;
	
	if (game == 0 || game == 1) {
		for (int t = 0; t < 24; t++) {
			temp = 0;
			for (int j = 0; j < WC6LENGTH; j++)
				if (*(mainbuf + XYWC6POS + t * WC6LENGTH + j) == 0x00)
					temp++;
				
			if (temp == WC6LENGTH) {
				nInjected[0] = t;
				break;
			}
		}
	} else if (game == 2 || game == 3) {
		for (int t = 0; t < 24; t++) {
			temp = 0;
			for (int j = 0; j < WC6LENGTH; j++)
				if (*(mainbuf + ORASWC6POS + t * WC6LENGTH + j) == 0x00)
					temp++;
				
			if (temp == WC6LENGTH) {
				nInjected[0] = t;
				break;
			}
		}
	}
}

/* ************************ get ************************ */

u16 getOTID(u8* pkmn) {
    u16 otidbuffer;
    memcpy(&otidbuffer, &pkmn[OTIDPOS], OTIDLENGTH);
    
    return otidbuffer;
}

u16 getSOTID(u8* pkmn) {
    u16 sotidbuffer;
    memcpy(&sotidbuffer, &pkmn[SOTIDPOS], SOTIDLENGTH);
    
    return sotidbuffer;
}

u32 getPID(u8* pkmn) {
    u32 pidbuffer;
    memcpy(&pidbuffer, &pkmn[PIDPOS], PIDLENGTH);
    
    return pidbuffer;
}

u16 getTSV(u8* pkmn) {
	u16 TID = getOTID(pkmn);
	u16 SID = getSOTID(pkmn);
	return (TID ^ SID) >> 4;
}

u16 getPokedexNumber(u8* pkmn) {
    u16 pokedexnumber;
    memcpy(&pokedexnumber, &pkmn[POKEDEXNUMBERPOS], POKEDEXNUMBERLENGTH);
    return pokedexnumber;
}

u8 getFriendship(u8* pkmn) {
    u8 friendship;
    memcpy(&friendship, &pkmn[FRIENDSHIPPOS], 1);
    return friendship;
}

u8 getNature(u8* pkmn) {
    u8 nature;
    memcpy(&nature, &pkmn[NATUREPOS], NATURELENGTH);
    return nature;
}

u8 getEV(u8* pkmn, const int stat) {
    u8 evbuffer[6]; 
    memcpy(evbuffer, &pkmn[EVPOS], EVLENGTH * 6);
    
    return evbuffer[stat];
}

u8 getIV(u8* pkmn, const int stat) {
    u32 buffer;
    u8 toreturn;
    
    memcpy(&buffer, &pkmn[IVPOS], IVLENGTH);
    buffer = buffer >> 5*stat;
    buffer = buffer & 0x1F;
    memcpy(&toreturn, &buffer, 1);
    
    return toreturn;
}

/* ************************ set ************************ */

void setNickname(u8* pkmn, char* nick) {
    u8 toinsert[NICKNAMELENGTH];
    for (int i = 0; i < NICKNAMELENGTH; i++)
        toinsert[i] = 0;
    
    for (u16 i = 0; i < strlen(nick); i++)
        toinsert[i * 2] = *(nick + i);
    
    memcpy(&pkmn[NICKNAMEPOS], toinsert, NICKNAMELENGTH);
}

void setNature(u8* pkmn, const u8 nature) {
    memcpy(&pkmn[NATUREPOS], &nature, NATURELENGTH);
}

void setFriendship(u8* pkmn, const int val) {
	memcpy(&pkmn[FRIENDSHIPPOS], &val, 1);
}

void setEV(u8* pkmn, u8 val, const int stat) {
    memcpy(&pkmn[EVPOS+(EVLENGTH*stat)], &val, EVLENGTH);
}

void setIV(u8* pkmn, u8 val, const int stat) {
	u32 nval = val;
	u32 mask = 0xFFFFFFFF;
	mask ^= 0x1F << (5 * stat);

	u32 buffer;
	memcpy(&buffer, &pkmn[IVPOS], IVLENGTH);

	buffer &= mask;
	buffer ^= ((nval & 0x1F) << (5 * stat));
	memcpy(&pkmn[IVPOS], &buffer, IVLENGTH);
}

void setHPType(u8* pkmn, const int val) {
    u8 ivstat[6];
    for(int i = 0; i < 6; i++)
        ivstat[i] = getIV(pkmn, i);
    
    u8 hpivs[16][6] = {
        { 1, 1, 0, 0, 0, 0 }, // Fighting
        { 0, 0, 0, 1, 0, 0 }, // Flying
        { 1, 1, 0, 1, 0, 0 }, // Poison
        { 1, 1, 1, 1, 0, 0 }, // Ground
        { 1, 1, 0, 0, 1, 0 }, // Rock
        { 1, 0, 0, 1, 1, 0 }, // Bug
        { 1, 0, 1, 1, 1, 0 }, // Ghost
        { 1, 1, 1, 1, 1, 0 }, // Steel
        { 1, 0, 1, 0, 0, 1 }, // Fire
        { 1, 0, 0, 1, 0, 1 }, // Water
        { 1, 0, 1, 1, 0, 1 }, // Grass
        { 1, 1, 1, 1, 0, 1 }, // Electric
        { 1, 0, 1, 0, 1, 1 }, // Psychic
        { 1, 0, 0, 1, 1, 1 }, // Ice
        { 1, 0, 1, 1, 1, 1 }, // Dragon
        { 1, 1, 1, 1, 1, 1 }, // Dark
    };
    
    for(int i = 0; i < 6; i++)
         ivstat[i] = (ivstat[i] & 0x1E) + hpivs[val][i];
    
    for(int i = 0; i < 6; i++)
        setIV(pkmn, ivstat[i], i);
}

void setShiny(u8* pkmn, const bool shiny) {
	while(isShiny(pkmn) != shiny)
		rerollPID(pkmn);
}

void setWC(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[]) {
	if (game == 0 || game == 1) {
		*(mainbuf + XYWC6FLAGPOS + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + XYWC6POS + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);
	}	
	
	if (game == 2 || game == 3) {		
		*(mainbuf + ORASWC6FLAGPOS + i / 8) |= 0x1 << (i % 8);
		memcpy((void*)(mainbuf + ORASWC6POS + nInjected[0] * WC6LENGTH), (const void*)wcbuf, WC6LENGTH);
		
		if (i == 2048) {
			*(mainbuf + EONFLAGPOS) = 0xC2;
			*(mainbuf + EONFLAGPOS + 1) = 0x73;
			*(mainbuf + EONFLAGPOS + 2) = 0x5D;
			*(mainbuf + EONFLAGPOS + 3) = 0x22;
		}
	}
	if (game == 4 || game == 5 || game == 6 || game == 7) {
		u32 seed;
		memcpy(&seed, &mainbuf[BWSEEDPOS], sizeof(u32));
		
		//decrypt
		u16 temp;
		for (int i = 0; i < 0xA90; i += 2) {
			memcpy(&temp, &mainbuf[PGFSTARTPOS + i], 2);
			temp ^= (LCRNG(seed) >> 16);
			seed = LCRNG(seed);
			memcpy(&mainbuf[PGFSTARTPOS + i], &temp, 2);
		}
		
		*(mainbuf + PGFSTARTPOS + i / 8) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + 0x1C900 + nInjected[0] * PGFLENGTH), (const void*)wcbuf, PGFLENGTH);
		
		//encrypt
		memcpy(&seed, &mainbuf[BWSEEDPOS], sizeof(u32));
		for (int i = 0; i < 0xA90; i += 2) {
			memcpy(&temp, &mainbuf[PGFSTARTPOS + i], 2);
			temp ^= (LCRNG(seed) >> 16);
			seed = LCRNG(seed);
			memcpy(&mainbuf[PGFSTARTPOS + i], &temp, 2);
		}
	}
	
	nInjected[0] += 1;
	if (nInjected[0] >= 24)
		nInjected[0] = 0;
}

void setWC4(u8* mainbuf, u8* wcbuf, int game, int i, int nInjected[], int GBO) {	
		// I'm not using these (anymore) for now. They represent the correct way to handle wondercard injection,
		// but they work only the first time. Needing to find a way to fix this.
		
	if (game == 8 || game == 9) {
		// *(mainbuf + HGSSPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		// memcpy((void*)(mainbuf + HGSSPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		// Weird but this works. Writing wondercard to both blocks will cause the game to find a corrupted save,
		// but it restores normally it and shows the correct wondercards, without any save loss.
		
		*(mainbuf + HGSSPGTFLAGPOS + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + HGSSPGTPOS + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + HGSSPGTFLAGPOS + 0x40000 + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + HGSSPGTPOS + 0x40000 + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}
	if (game == 10) {
		// *(mainbuf + PTPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		// memcpy((void*)(mainbuf + PTPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + PTPGTFLAGPOS + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + PTPGTPOS + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + PTPGTFLAGPOS + 0x40000 + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + PTPGTPOS + 0x40000 + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}
	if (game == 11 || game == 12) {
		// *(mainbuf + DPPGTFLAGPOS + GBO + (i >> 3)) |= 0x1 << (i & 7);
		// memcpy((void*)(mainbuf + DPPGTPOS + GBO + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + DPPGTFLAGPOS + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + DPPGTPOS + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
		
		*(mainbuf + DPPGTFLAGPOS + 0x40000 + (i >> 3)) |= 0x1 << (i & 7);
		memcpy((void*)(mainbuf + DPPGTPOS + 0x40000 + nInjected[0] * PGTLENGTH), (const void*)wcbuf, PGTLENGTH);
	}

	nInjected[0] += 1;
}

void setLanguage(u8* mainbuf, int i) {
	*(mainbuf + LANGUAGEPOS) = langValues[i];
}

void setMoney(u8* mainbuf, u64 i) {
	switch (i) {
		case 0 : {
			*(mainbuf + MONEYPOS) = 0x00;
			*(mainbuf + MONEYPOS + 1) = 0x00;
			*(mainbuf + MONEYPOS + 2) = 0x00;
			break;			
		}	
		case 200000 : {
			*(mainbuf + MONEYPOS) = 0x40;
			*(mainbuf + MONEYPOS + 1) = 0x0D;
			*(mainbuf + MONEYPOS + 2) = 0x03;
			break;			
		}	
		case 1000000 : {
			*(mainbuf + MONEYPOS) = 0x40;
			*(mainbuf + MONEYPOS + 1) = 0x42;
			*(mainbuf + MONEYPOS + 2) = 0x0F;
			break;			
		}
		case 9999999 : {
			*(mainbuf + MONEYPOS) = 0x7F;
			*(mainbuf + MONEYPOS + 1) = 0x96;
			*(mainbuf + MONEYPOS + 2) = 0x98;
			break;
		}
	}
}

void setItem(u8* mainbuf, int i, u32 values[], int type, int nInjected[], int game) {
	u32 offset[3] = {0x400, 0xD68, 0xE68};
	if (game == 2 || game == 3) {
		offset[1] = 0xD70;
		offset[2] = 0xE70;
	}

	if (i % 2 == 0) {
		*(mainbuf + offset[type] + nInjected[type] * 4) = values[i];
		*(mainbuf + offset[type + 1] + nInjected[type] * 4) = values[i + 1];
		*(mainbuf + offset[type] + 2 + nInjected[type] * 4) = 0x01;
		*(mainbuf + offset[type] + 3 + nInjected[type] * 4) = 0x00;		
	}
	else {
		*(mainbuf + offset[type] + nInjected[type] * 4) = values[i - 1];
		*(mainbuf + offset[type] + 1 + nInjected[type] * 4) = values[i];
		*(mainbuf + offset[type] + 2 + nInjected[type] * 4) = 0xE3;
		*(mainbuf + offset[type] + 3 + nInjected[type] * 4) = 0x03;	
	}
	nInjected[type]++;
}

void setBP(u8* mainbuf, int i, int game) {
	const u32 offset[] = {0x423C, 0x423D, 0x4230, 0x4231};
	int type = 0;
	
	if (game == 2 || game == 3) type = 2;
		switch (i) {
			case 0 : {
				*(mainbuf + offset[type]) = 0x00;
				*(mainbuf + offset[type + 1]) = 0x00;
				break;			
			}
			case 9999 : {
				*(mainbuf + offset[type]) = 0x0F;
				*(mainbuf + offset[type + 1]) = 0x27;
				break;			
			}
		}
}

void setBadges(u8* mainbuf, int i) {
	const u32 value[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
	
	*(mainbuf + BADGEPOS) = value[i];
}

void setTM(u8* mainbuf, int game) {
	const u32 values[] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0x6A, 0x6B, 0x6C, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9};

	if (game == 0 || game == 1) 
		for (int i = 0; i < 105; i++) {
			*(mainbuf + TMSTARTPOS + i * 4) = values[i];
			*(mainbuf + TMSTARTPOS + i * 4 + 1) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 2) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 3) = 0x00;
		}
	
	if (game == 2 || game == 3) {
		for (int i = 0; i < 106; i++) {
			*(mainbuf + TMSTARTPOS + i * 4) = values[i];
			*(mainbuf + TMSTARTPOS + i * 4 + 1) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 2) = 0x01;
			*(mainbuf + TMSTARTPOS + i * 4 + 3) = 0x00;
		}
		*(mainbuf + TMSTARTPOS + 4 * 106) = 0xE1;
		*(mainbuf + TMSTARTPOS + 4 * 106 + 1) = 0x02;
		*(mainbuf + TMSTARTPOS + 4 * 106 + 2) = 0x01;
		*(mainbuf + TMSTARTPOS + 4 * 106 + 3) = 0x00;
	}
	
	for (int i = 0; i < 8; i++) {
		*(mainbuf + 0xD31 + i * 4) = 0x02;
	}
}

int setBoxBin(PrintConsole screen, u8* mainbuf, int game, int NBOXES, int N, char* path[]) {
	for (int i = 0; i < NBOXES; i++) {
		FILE *fptr = fopen(path[i], "rt");
		if (fptr == NULL) 
			return 15;
		fseek(fptr, 0, SEEK_END);
		u32 contentsize = ftell(fptr);
		u8 *buf = (u8*)malloc(contentsize);
		if (buf == NULL) 
			return 8;
		rewind(fptr);
		fread(buf, contentsize, 1, fptr);
		fclose(fptr);

		int boxpos = 0;
		if (game == 0 || game == 1) 
			boxpos = 0x27A00 - 0x5400;
	
		if (game == 2 || game == 3) 
			boxpos = 0x38400 - 0x5400;
		
		memcpy((void*)(mainbuf + boxpos + PKMNLENGTH * 30 * i), (const void*)buf, PKMNLENGTH * N);
		
		free(buf);
	}
	return 1;
}

/* ************************ graphics ************************ */

void refreshItem(PrintConsole topScreen, int cont[], int nInjected[]) {
	consoleSelect(&topScreen);
    printf("\x1b[2;28H\x1b[1;33m%s\x1b[0m", language[cont[1]]);
    printf("\x1b[3;28H\x1b[1;33m%llu\x1b[0m$       ", money[cont[2]]);
	
    printf("\x1b[4;19H%d:\x1b[4;28H\x1b[1;33m%s\x1b[0m     ", nInjected[0] + 1,  ballsList[cont[3] / 2]);
    if (cont[3] % 2 == 0) 
		printf("\x1b[4;45Hx1  ");
    else printf("\x1b[4;45Hx995");
	
    printf("\x1b[5;19H%d:\x1b[5;28H\x1b[1;33m%s\x1b[0m     ", nInjected[0] + 1, itemList[cont[4] / 2]);
    if (cont[4] % 2 == 0) 
		printf("\x1b[5;45Hx1  ");
    else printf("\x1b[5;45Hx995");
	
    printf("\x1b[6;19H%d:\x1b[6;28H\x1b[1;33m%s\x1b[0m     ", nInjected[1] + 1, healList[cont[5] / 2]);
    if (cont[5] % 2 == 0) 
		printf("\x1b[6;45Hx1  ");
    else printf("\x1b[6;45Hx995");
	
    printf("\x1b[7;28H\x1b[1;33m%d\x1b[0m    ", BP[cont[6]]);
    printf("\x1b[8;28H\x1b[1;33m%d\x1b[0m badges", cont[7]);
    printf("\x1b[29;5HInjection locations: \x1b[32m%d\x1b[0m/%d | \x1b[32m%d\x1b[0m/%d | \x1b[32m%d\x1b[0m/%d  ", nInjected[0], BALLS + ITEM, nInjected[1], HEAL, nInjected[2], BERRIES);
}

void refreshPK(PrintConsole topScreen, u8* mainbuf, int cont[], int game) {
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	getPkmn(mainbuf, cont[1], cont[2], pkmn, game);
	
	consoleSelect(&topScreen);	
	printf("\x1b[2;31H\x1b[1;33m%d\x1b[0m ", cont[1] + 1);
	printf("\x1b[3;31H\x1b[1;33m%d\x1b[0m ", cont[2] + 1);
	printf("\x1b[5;31H\x1b[1;33m%s\x1b[0m   ", natures[cont[3]]);
	printf("\x1b[9;31H\x1b[1;33m%s\x1b[0m        ", stats[cont[7]]);
	printf("\x1b[10;31H\x1b[1;33m%s\x1b[0m        ", stats[cont[8]]);
	printf("\x1b[11;31H\x1b[1;33m%s\x1b[0m    ", hpList[cont[4]]);
	printf("\x1b[14;31H\x1b[1;33mB%d\x1b[0m ", cont[5] + 1);
	printf("\x1b[15;31H\x1b[1;33mB%d/S%d\x1b[0m  ", cont[5] + 1, cont[6] + 1);
	
	
	printf("\x1b[25;0HPokemon: \x1b[33m%s\x1b[0m          ", pokemon[getPokedexNumber(pkmn)]);
	
	if (getPokedexNumber(pkmn)) {
		printf("\x1b[25;23HNature: %s     ", natures[getNature(pkmn)]);
		printf("\x1b[26;0HIV: %d/%d/%d/%d/%d/%d      ", getIV(pkmn, 0), getIV(pkmn, 1), getIV(pkmn, 2), getIV(pkmn, 4), getIV(pkmn, 5), getIV(pkmn, 3));
		printf("\x1b[26;23HEV: %d/%d/%d/%d/%d/%d            ", getEV(pkmn, 0), getEV(pkmn, 1), getEV(pkmn, 2), getEV(pkmn, 4), getEV(pkmn, 5), getEV(pkmn, 3));
		printf("\x1b[27;0HFriendship: %d  ", getFriendship(pkmn));
		printf("\x1b[27;23H");
		if (isShiny(pkmn))
			 printf("Shiny    ");
		else printf("Non shiny");
		printf("\x1b[28;0HTID: %u     ", getOTID(pkmn));
		printf("\x1b[28;23HSID: %u     ", getSOTID(pkmn));
		printf("\x1b[29;0HShiny value: %u     ", getTSV(pkmn));
	} else {
		printf("\x1b[25;23H                           ");
		printf("\x1b[26;0H                           ");
		printf("\x1b[26;23H                                 ");
		printf("\x1b[27;0H               ");
		printf("\x1b[27;23H          ");
		printf("\x1b[28;0H           ");
		printf("\x1b[28;23H           ");
		printf("\x1b[29;0H                  ");
	}
	
	free(pkmn);
}

int saveFileEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game, int nInjected[], int cont[]) {
	char *menuEntries[ITEMENTRIES] = {"Set language to:", "Set money to:", "Set ball to slot", "Set item to slot", "Set heal to slot", "Set Battle Points to:", "Set number of badges to:", "Set all TMs", "Set Poke Balls to max", "Set all available items to max", "Set all available heals to max", "Set all available berries to max", "Clean Mistery Gift box"};
	
	//fill berries
	u32 berry[BERRIES * 2];
	for (int i = 0; i < BERRIES * 2 ; i += 2) {
		berry[i] = 0x95 + i / 2;
		berry[i + 1] = 0x00;
	}

	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m    Move cursor\n");
	printf("\x1b[32mA\x1b[0m     Switch setting\n");
	printf("\x1b[1;31mSTART\x1b[0m Apply selected change\n");
	printf("----------------------------------------");
	printf("\x1b[6;0HYou need to have the latest updates for the selected game installed.\n\nYou can perform one edit, then you need to reopen this function to make another one.");
	printf("\x1b[21;0H----------------------------------------");
	printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
	printf("\x1b[26;0H----------------------------------------");
	printf("\x1b[29;8HTouch or press B to exit");
	
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                 Save file Editor                 \x1b[0m\n");
	
	refresh(cont[0], topScreen, menuEntries, ITEMENTRIES);
	refreshItem(topScreen, cont, nInjected);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B || hidKeysDown() & KEY_TOUCH) 
			break; 
		
		if (hidKeysDown() & KEY_DUP) {
			if (cont[0] == 0)
				cont[0] = ITEMENTRIES - 1;
			else if (cont[0] > 0)
				cont[0]--;

			refresh(cont[0], topScreen, menuEntries, ITEMENTRIES);
			refreshItem(topScreen, cont, nInjected);
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (cont[0] == ITEMENTRIES - 1)
				cont[0] = 0;
			else if (cont[0] < ITEMENTRIES - 1)
				cont[0]++;

			refresh(cont[0], topScreen, menuEntries, ITEMENTRIES);
			refreshItem(topScreen, cont, nInjected);
		}
		
		if (hidKeysDown() & KEY_A) {
			switch (cont[0]) {
				case 0 : {
					if (cont[1] < 6) 
						cont[1]++;
					else if (cont[1] == 6) 
						cont[1] = 0;
					break;					
				}
				case 1 : {
					if (cont[2] < 3) 
						cont[2]++;
					else if (cont[2] == 3) 
						cont[2] = 0;
					break;					
				}
				case 2 : {
					if (cont[3] < BALLS * 2 - 1) 
						cont[3]++;
					else if (cont[3] == BALLS * 2 - 1) 
						cont[3] = 0;
					break;					
				}
				case 3 : {
					if (cont[4] < ITEM * 2 - 1) 
						cont[4]++;
					else if (cont[4] == ITEM * 2 - 1) 
						cont[4] = 0;
					break;						
				}
				case 4 : {
					if (cont[5] < HEAL * 2 - 1) 
						cont[5]++;
					else if (cont[5] == HEAL * 2 - 1) 
						cont[5] = 0;
					break;						
				}
				case 5 : {
					if (cont[6] < 1) 
						cont[6]++;
					else if (cont[6] == 1) 
						cont[6] = 0;
					break;					
				}
				case 6 : {
					if (cont[7] < 8) 
						cont[7]++;
					else if (cont[7] == 8) 
						cont[7] = 0;
					break;
				}
			}
			refreshItem(topScreen, cont, nInjected);	
		}

		if (hidKeysDown() & KEY_START) {			
			switch (cont[0]) {
				case 0 : {
					setLanguage(mainbuf, cont[1]);
					break;
				}
				case 1 : {
					setMoney(mainbuf, money[cont[2]]);
					break;
				}
				case 2 : {
					if (nInjected[0] < BALLS + ITEM) {
						setItem(mainbuf, cont[3], balls, 0, nInjected, game);
						break;
					}
					else return 14;
				}
				case 3 : {
					if (nInjected[0] < BALLS + ITEM) {
						setItem(mainbuf, cont[4], items, 0, nInjected, game);
						break;
					}
					else return 14;
				}
				case 4 : {
					if (nInjected[1] < HEAL) {
						setItem(mainbuf, cont[5], heal, 1, nInjected, game);
						break;
					}
					else return 14;
				}
				case 5 : {
					setBP(mainbuf, BP[cont[6]], game);
					break;
				}
				case 6 : {
					setBadges(mainbuf, cont[7]);
					break;
				}
				case 7 : {
					setTM(mainbuf, game);
					break;
				}
				case 8 : {
					nInjected[0] = 0;
					for (int i = 1; i < BALLS * 2; i += 2)
						setItem(mainbuf, i, balls, 0, nInjected, game);
					break;
				}
				case 9 : {
					nInjected[0] = 0;
					for (int i = 1; i < ITEM * 2; i += 2)
						setItem(mainbuf, i, items, 0, nInjected, game);
					break;
				}
				case 10 : {
					nInjected[1] = 0;
					for (int i = 1; i < HEAL * 2; i += 2)
						setItem(mainbuf, i, heal, 1, nInjected, game);
					break;
				}
				case 11 : {
					nInjected[2] = 0;
					for (int i = 1; i < BERRIES * 2; i += 2)
						setItem(mainbuf, i, berry, 2, nInjected, game);
					break;
				}
				case 12 : {
					int start = 0;
					if (game == 0 || game == 1)
						start = XYWC6FLAGPOS;
					else if (game == 2 || game == 3)
						start = ORASWC6FLAGPOS;
					
					for (int i = 0; i < (0x100 + 24 * WC6LENGTH); i++)
						*(mainbuf + start + i) = 0x00;
					
					break;
				}
			}
			return 1;
		}
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	return -1;
}

int PKEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game, int cont[]) {
	char *menuEntries[PK6ENTRIES] = {"Select box (1-31)", "Select index (1-30)", "Set nickname", "Set nature", "Set shiny", "Set non shiny", "Set friendship", "Set IVs", "Set EVs", "Set hidden power", "Set pokerus", "Remove pokerus", "Clone selected box to", "Clone pokemon to"};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
	printf("\x1b[32mA\x1b[0m - Switch setting\n");
	printf("\x1b[32mSELECT\x1b[0m - Reset menu\n");
	printf("\x1b[1;31mSTART\x1b[0m - Start selected change\n");
	printf("----------------------------------------");
	printf("\nYou need to put the whole number\n(ex: 075) into IVs, EVs and friendship\neditor.");
	printf("\x1b[17;0HYou can perform one edit, then you need to reopen this function to make another one.");
	printf("\x1b[21;0H----------------------------------------");
	printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
	printf("\x1b[26;0H----------------------------------------");
	printf("\x1b[29;8HTouch or press B to exit");
	
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");
	
	refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
	refreshPK(topScreen, mainbuf, cont, game);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B || hidKeysDown() & KEY_TOUCH) 
			break; 
		
		if (hidKeysDown() & KEY_DUP) {
			if (cont[0] == 0)
				cont[0] = PK6ENTRIES - 1;
			else if (cont[0] > 0)
				cont[0]--;

			refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
			refreshPK(topScreen, mainbuf, cont, game);
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (cont[0] == PK6ENTRIES - 1)
				cont[0] = 0;
			else if (cont[0] < PK6ENTRIES - 1)
				cont[0]++;
			
			refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
			refreshPK(topScreen, mainbuf, cont, game);
		}
		
		if (hidKeysDown() & KEY_SELECT) {
			for (int i = 0; i < 7; i++)
				cont[i] = 0;
				refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
				refreshPK(topScreen, mainbuf, cont, game);
		}
		
		if (hidKeysDown() & KEY_A) {
			switch (cont[0]) {
				case 0 : {
					if (cont[1] < BOXMAX - 1) 
						cont[1] += 1;
					else if (cont[1] == BOXMAX - 1) 
						cont[1] = 0;
					break;
				}
				case 1 : {
					if (cont[2] < 29) 
						cont[2] += 1;
					else if (cont[2] == 29) 
						cont[2] = 0;
					break;
				}
				case 3 : {
					if (cont[3] < 24)
						cont[3] += 1;
					else if (cont[3] == 24)
						cont[3] = 0;
					break;
				}
				case 7 : {
					if (cont[7] < 6)
						cont[7] += 1;
					else if (cont[7] == 6)
						cont[7] = 0;					
					break;
				}
				case 8 : {
					if (cont[8] < 6)
						cont[8] += 1;
					else if (cont[8] == 6)
						cont[8] = 0;					
					break;
				}
				case 9 : {
					if (cont[4] < 15) 
						cont[4] += 1;
					else if (cont[4] == 15) 
						cont[4] = 0;
					break;
				}
				case 12 : {
					if (cont[5] < BOXMAX - 1) 
						cont[5] += 1;
					else if (cont[5] == BOXMAX - 1) 
						cont[5] = 0;
					break;
				}
				case 13 : {
					if (cont[6] < 29) 
						cont[6] += 1;
					else if (cont[6] == 29) 
						cont[6] = 0;
					break;
				}
			}
			refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
			refreshPK(topScreen, mainbuf, cont, game);
		}
		
		if (hidKeysDown() & KEY_START && cont[0] != 0 && cont[0] != 1) {
			u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
			
			switch (cont[0]) {
				case 2 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					static SwkbdState swkbd;
					static char buf[60];
					
					SwkbdButton button = SWKBD_BUTTON_NONE;
					swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 1, (NICKNAMELENGTH / 2) - 1);
					swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_DIGITS | SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 2);
					swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
					swkbdSetHintText(&swkbd, "Enter your nickname");
					button = swkbdInputText(&swkbd, buf, sizeof(buf));
					
					static char nick[NICKNAMELENGTH];
					for (int i = 0; i < NICKNAMELENGTH; i++)
						nick[i] = 0;
					memcpy(nick, buf, NICKNAMELENGTH);
					nick[NICKNAMELENGTH - 1] = '\0';
					
					if (button != SWKBD_BUTTON_NONE) {
						setNickname(pkmn, nick);
						setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					} else 
						return 0;
					
					break;
				}
				case 3 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					
					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					setNature(pkmn, (u8)(cont[3]));
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 4 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					
					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					setShiny(pkmn, true);
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 5 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					setShiny(pkmn, false);
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 6 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					
					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					static SwkbdState swkbd;
					static char mybuf[4] = {'0', '0', '0', '\0'};

					SwkbdButton button = SWKBD_BUTTON_NONE;	
					
					swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 3);
					swkbdSetValidation(&swkbd, SWKBD_FIXEDLEN, 0, 0);
					swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
					button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
					
					int cent = mybuf[0] - '0';
					int dec = mybuf[1] - '0';
					int uni = mybuf[2] - '0';
					int friendship = cent * 100 + dec * 10 + uni * 1;

					if (friendship > 255)
						friendship = 255;
					
					if (button != SWKBD_BUTTON_NONE)
						setFriendship(pkmn, friendship);
					
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 7 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;

					bool other = false;
					char *statslist[] = {"Health", "Attack", "Defense", "Speed", "Sp. Attack", "Sp. Defense"};
					static SwkbdState swkbd;
					static char buf[3] = {'0', '0', '\0'};
					
					SwkbdButton button = SWKBD_BUTTON_NONE;
					
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					printf("\x1b[14;17HTouch!");
				
					switch(cont[7]) {
						case 0 : {
							for (int i = 0; i < 6; i++) {
								buf[0] = '0';
								buf[1] = '0';
								
								consoleSelect(&topScreen);
								printf("\x1b[2J");
								printf("\x1b[15;%uH\x1b[33mA\x1b[0m: Set \x1b[32m%s\x1b[0m IVs", (39 - strlen(statslist[i])) / 2, statslist[i]);

								while (aptMainLoop()) {
									gspWaitForVBlank();
									hidScanInput();

									if (hidKeysDown() & KEY_A || hidKeysDown() & KEY_TOUCH)
										break;

									gfxFlushBuffers();
									gfxSwapBuffers();
								}
								
								swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 2);
								swkbdSetValidation(&swkbd, SWKBD_FIXEDLEN, 0, 0);
								swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
								button = swkbdInputText(&swkbd, buf, sizeof(buf));
								
								int dec = (buf[0] - '0');
								int uni = (buf[1] - '0');
								int iv = dec * 10 + uni * 1;
								
								if (iv > 31)
									iv = 31;
								
								if (button != SWKBD_BUTTON_NONE)
									setIV(pkmn, iv, i);
							}
							break;
						}
						case 1 : { 
							other = true;
							break;
						}
						case 2 : { 
							other = true;
							break;
						}
						case 3 : { 
							other = true;
							break;
						}
						case 4 : { 
							other = true;
							break;
						}
						case 5 : { 
							other = true;
							break;
						}
						case 6 : { 
							other = true;
							break;
						}
					}

					if (other) {
						buf[0] = '0';
						buf[1] = '0';
						
						swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 2);
						swkbdSetValidation(&swkbd, SWKBD_FIXEDLEN, 0, 0);
						swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
						button = swkbdInputText(&swkbd, buf, sizeof(buf));
						
						int dec = (buf[0] - '0');
						int uni = (buf[1] - '0');
						int iv = dec * 10 + uni * 1;
						
						if (iv > 31)
							iv = 31;
						
						if (button != SWKBD_BUTTON_NONE)
							setIV(pkmn, iv, cont[7] - 1);
					}					

					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					consoleSelect(&bottomScreen);
					printf("\x1b[29;12HPress B to exit.");
					consoleSelect(&topScreen);
					printf("\x1b[2J");
					printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");					
					refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
					refreshPK(topScreen, mainbuf, cont, game);
					break;
				}
				case 8 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;

					bool other = false;
					char *statslist[] = {"Health", "Attack", "Defense", "Speed", "Sp. Attack", "Sp. Defense"};
					static SwkbdState swkbd;
					static char buf[4] = {'0', '0', '0', '\0'};
					
					SwkbdButton button = SWKBD_BUTTON_NONE;
					
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					printf("\x1b[14;17HTouch!");
				
					switch(cont[8]) {
						case 0 : {
							for (int i = 0; i < 6; i++) {
								buf[0] = '0';
								buf[1] = '0';
								buf[2] = '0';
								
								consoleSelect(&topScreen);
								printf("\x1b[2J");
								printf("\x1b[15;%uH\x1b[33mA\x1b[0m: Set \x1b[32m%s\x1b[0m EVs", (39 - strlen(statslist[i])) / 2, statslist[i]);

								while (aptMainLoop()) {
									gspWaitForVBlank();
									hidScanInput();

									if (hidKeysDown() & KEY_A || hidKeysDown() & KEY_TOUCH)
										break;

									gfxFlushBuffers();
									gfxSwapBuffers();
								}
								
								swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 3);
								swkbdSetValidation(&swkbd, SWKBD_FIXEDLEN, 0, 0);
								swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
								button = swkbdInputText(&swkbd, buf, sizeof(buf));
								
								int cent = (buf[0] - '0');
								int dec = (buf[1] - '0');
								int uni = (buf[2] - '0');
								int ev = cent * 100 + dec * 10 + uni * 1;

								if (ev > 252)
									ev = 252;
								
								if (button != SWKBD_BUTTON_NONE)
									setEV(pkmn, ev, i);
							}
							break;
						}
						case 1 : { 
							other = true;
							break;
						}
						case 2 : { 
							other = true;
							break;
						}
						case 3 : { 
							other = true;
							break;
						}
						case 4 : { 
							other = true;
							break;
						}
						case 5 : { 
							other = true;
							break;
						}
						case 6 : { 
							other = true;
							break;
						}
					}

					if (other) {
						buf[0] = '0';
						buf[1] = '0';
						buf[2] = '0';
						
						swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 3);
						swkbdSetValidation(&swkbd, SWKBD_FIXEDLEN, 0, 0);
						swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
						button = swkbdInputText(&swkbd, buf, sizeof(buf));
						
						int cent = (buf[0] - '0');
						int dec = (buf[1] - '0');
						int uni = (buf[2] - '0');
						int ev = cent * 100 + dec * 10 + uni * 1;

						if (ev > 252)
							ev = 252;
						
						if (button != SWKBD_BUTTON_NONE)
							setEV(pkmn, ev, cont[8] - 1);
					}

					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					consoleSelect(&bottomScreen);
					printf("\x1b[29;12HPress B to exit.");
					consoleSelect(&topScreen);
					printf("\x1b[2J");
					printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");					
					refresh(cont[0], topScreen, menuEntries, PK6ENTRIES);
					refreshPK(topScreen, mainbuf, cont, game);
					break;
				}
				case 9 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					for (int i = 0; i < 6; i++) 
						setIV(pkmn, 31, i);
					
					setHPType(pkmn, cont[4]);
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 10 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					*(pkmn + 0x2B) = 0x11;
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 11 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;
					
					*(pkmn + 0x2B) = 0x00;
					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					break;
				}
				case 12 : {
					char pkmncpy[PKMNLENGTH];
					for (int i = 0; i < 30; i++) {
						getPkmn(mainbuf, cont[1], i, pkmn, game);
						memcpy(&pkmncpy, pkmn, PKMNLENGTH);
						setPkmn(mainbuf, cont[5], i, pkmn, game);
					}
					break;
				}
				case 13 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);
					char pkmncpy[PKMNLENGTH];
					memcpy(&pkmncpy, pkmn, PKMNLENGTH);
					setPkmn(mainbuf, cont[5], cont[6], pkmn, game);
					break;
				}
			}
			
			free(pkmn);
			return 1;
		}
		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	return 0;
}