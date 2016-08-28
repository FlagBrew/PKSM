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

#define ENTRIES 14

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

const int friendship[4] = {0, 70, 75, 255};

char *hpList[16] = {"Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel", "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark"};
static char *pokemon[722] = {"None", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew", "Sandslash", "Nidoran F", "Nidorina", "Nidoqueen", "Nidoran M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebell", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr. Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup ", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "Mime Jr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickylicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus", "Victini", "Snivy", "Servine", "Serperior", "Tepig", "Pignite", "Emboar", "Oshawott", "Dewott", "Samurott", "Patrat", "Watchog", "Lillipup", "Herdier", "Stoutland", "Purrloin", "Liepard", "Pansage", "Simisage", "Pansear", "Simisear", "Panpour", "Simipour", "Munna", "Musharna", "Pidove", "Tranquill", "Unfezant", "Blitzle", "Zebstrika", "Roggenrola", "Boldore", "Gigalith", "Woobat", "Swoobat", "Drilbur", "Excadrill", "Audino", "Timburr", "Gurdurr", "Conkeldurr", "Tympole", "Palpitoad", "Seismitoad", "Throh", "Sawk", "Sewaddle", "Swadloon", "Leavanny", "Venipede", "Whirlipede", "Scolipede", "Cottonee", "Whimsicott", "Petilil", "Lilligant", "Basculin", "Sandile", "Krokorok", "Krookodile", "Darumaka", "Darmanitan", "Maractus", "Dwebble", "Crustle", "Scraggy", "Scrafty", "Sigilyph", "Yamask", "Cofagrigus", "Tirtouga", "Carracosta", "Archen", "Archeops", "Trubbish", "Garbodor", "Zorua", "Zoroark", "Minccino", "Cinccino", "Gothita", "Gothorita", "Gothitelle", "Solosis", "Duosion", "Reuniclus", "Ducklett", "Swanna", "Vanillite", "Vanillish", "Vanilluxe", "Deerling", "Sawsbuck", "Emolga", "Karrablast", "Escavalier", "Foongus", "Amoonguss", "Frillish", "Jellicent", "Alomomola", "Joltik", "Galvantula", "Ferroseed", "Ferrothorn", "Klink", "Klang", "Klinklang", "Tynamo", "Eelektrik", "Eelektross", "Elgyem", "Beheeyem", "Litwick", "Lampent", "Chandelure", "Axew", "Fraxure", "Haxorus", "Cubchoo", "Beartic", "Cryogonal", "Shelmet", "Accelgor", "Stunfisk", "Mienfoo", "Mienshao", "Druddigon", "Golett", "Golurk", "Pawniard", "Bisharp", "Bouffalant", "Rufflet", "Braviary", "Vullaby", "Mandibuzz", "Heatmor", "Durant", "Deino", "Zweilous", "Hydreigon", "Larvesta", "Volcarona", "Cobalion", "Terrakion", "Virizon", "Tornadus", "Thundurus", "Reshiram", "Zekrom", "Landorus", "Kyurem", "Keldeo", "Meloetta", "Genesect", "Chespin", "Quilladin", "Chesnaught", "Fennekin", "Braixen", "Delphox", "Froakie", "Frogadier", "Greninja", "Bunnelby", "Diggersby", "Fletchling", "Fletchinder", "Talonflame", "Scatterbug", "Spewpa", "Vivillon", "Litleo", "Pyroar", "Flabébé", "Floette", "Florges", "Skiddo", "Gogoat", "Pancham", "Pangoro", "Furfrou", "Espurr", "Meowstic", "Honedge", "Doublade", "Aegislash", "Spritzee", "Aromatisse", "Swirlix", "Slurpuff", "Inkay", "Malamar", "Binacle", "Barbaracle", "Skrelp", "Dragalge", "Clauncher", "Clawitzer", "Helioptile", "Heliolisk", "Tyrunt", "Tyrantrum", "Amaura", "Aurorus", "Sylveon", "Hawlucha", "Dedenne", "Carbink", "Goomy", "Sliggoo", "Goodra", "Klefki", "Phantump", "Trevenant", "Pumpkaboo", "Gourgeist", "Bergmite", "Avalugg", "Noibat", "Noivern", "Xerneas", "Yveltal", "Zygarde", "Diancie", "Hoopa", "Volcanion"};
static char *natures[25] = {"Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed", "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest", "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky"};

int getPkmnAddress(const int boxnumber, const int indexnumber, int game) {
    int boxpos = 0;
    if (game == 0 || game == 1) 
		boxpos = 0x27A00 - OFFSET;
   
    if (game == 2 || game == 3) 
		boxpos = 0x38400 - OFFSET;

    const int PKMNNUM = 30;
    
	return boxpos + (PKMNLENGTH * PKMNNUM * boxnumber) + (indexnumber * PKMNLENGTH);
}

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

void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    memcpy(pkmn, &mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], PKMNLENGTH);
    decryptPkmn(pkmn);
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
 
void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game) {
    calculatePKMNChecksum(pkmn);
    encryptPkmn(pkmn);
        
    memcpy(&mainbuf[getPkmnAddress(boxnumber, indexnumber, game)], pkmn, PKMNLENGTH);
}

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

void setShiny(u8* pkmn, const bool shiny) {
	while(isShiny(pkmn) != shiny)
		rerollPID(pkmn);
}

u32 seedStep(const u32 seed) {
    return (seed * 0x41C64E6D + 0x00006073) & 0xFFFFFFFF;
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

void refreshPokemon(PrintConsole topScreen, u8* mainbuf, int cont[], int game) {
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	getPkmn(mainbuf, cont[1], cont[2], pkmn, game);
	
	consoleSelect(&topScreen);	
	printf("\x1b[2;31H\x1b[1;33m%d\x1b[0m ", cont[1] + 1);
	printf("\x1b[3;31H\x1b[1;33m%d\x1b[0m ", cont[2] + 1);
	printf("\x1b[5;31H\x1b[1;33m%s\x1b[0m   ", natures[cont[3]]);
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
		printf("\x1b[28;0HTID: %u ", getOTID(pkmn));
		printf("\x1b[28;23HSID: %u ", getSOTID(pkmn));
	} else {
		printf("\x1b[25;23H                           ");
		printf("\x1b[26;0H                           ");
		printf("\x1b[26;23H                                 ");
		printf("\x1b[27;0H               ");
		printf("\x1b[27;23H          ");
		printf("\x1b[28;0H           ");
		printf("\x1b[28;23H           ");
	}
	
	free(pkmn);
}

int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, u8 *mainbuf, int game, int cont[]) {
	char *menuEntries[ENTRIES] = {"Select box (1-31)", "Select index (1-30)", "Set nickname", "Set nature", "Set shiny", "Set non shiny", "Set friendship", "Set IVs", "Set EVs", "Set hidden power", "Set pokerus", "Remove pokerus", "Clone selected box to", "Clone pokemon to"};
	
	consoleSelect(&bottomScreen);
	printf("\x1b[2J");
	printf("----------------------------------------");
	printf("\x1b[32m\x19\x18\x1b[0m - Move cursor\n");
	printf("\x1b[32mA\x1b[0m - Switch setting\n");
	printf("\x1b[1;31mSTART\x1b[0m - Start selected change\n");
	printf("----------------------------------------");
	printf("\nYou need to put the whole number\n(ex: 075) into IVs, EVs and friendship\neditor.");
	printf("\x1b[17;0HYou can perform one edit, then you need to reopen this function to make another one.");
	printf("\x1b[21;0H----------------------------------------");
	printf("\x1b[22;14H\x1b[31mDISCLAIMER\x1b[0m\nI'm \x1b[31mNOT responsible\x1b[0m for any data loss,  save corruption or bans if you're using this.");
	printf("\x1b[26;0H----------------------------------------");
	printf("\x1b[29;12H\x1b[47;34mPress B to exit.\x1b[0m");
	
	consoleSelect(&topScreen);
	printf("\x1b[2J");
	printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");
	
	refresh(cont[0], topScreen, menuEntries, ENTRIES);
	refreshPokemon(topScreen, mainbuf, cont, game);
	
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		if (hidKeysDown() & KEY_B) 
			break; 
		
		if (hidKeysDown() & KEY_DUP) {
			if (cont[0] == 0) {
				cont[0] = ENTRIES - 1;
				refresh(cont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, mainbuf, cont, game);
			}
			else if (cont[0] > 0) {
				cont[0]--;
				refresh(cont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, mainbuf, cont, game);
			}
		}
		
		if (hidKeysDown() & KEY_DDOWN) {
			if (cont[0] == ENTRIES - 1) {
				cont[0] = 0;
				refresh(cont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, mainbuf, cont, game);	
			}
			else if (cont[0] < ENTRIES - 1) {
				cont[0]++;
				refresh(cont[0], topScreen, menuEntries, ENTRIES);
				refreshPokemon(topScreen, mainbuf, cont, game);
			}
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
			refresh(cont[0], topScreen, menuEntries, ENTRIES);
			refreshPokemon(topScreen, mainbuf, cont, game);
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

					char *statslist[] = {"Health", "Attack", "Defense", "Speed", "Sp. Attack", "Sp. Defense"};
					static SwkbdState swkbd;
					static char buf[3] = {'0', '0', '\0'};
					
					SwkbdButton button = SWKBD_BUTTON_NONE;
					
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					
					for (int i = 0; i < 6; i++) {
						buf[0] = '0';
						buf[1] = '0';
						
						consoleSelect(&topScreen);
						printf("\x1b[2J");
						printf("\x1b[15;%uH\x1b[33mA\x1b[0m: Set \x1b[32m%s\x1b[0m IVs", (39 - strlen(statslist[i])) / 2, statslist[i]);

						while (aptMainLoop()) {
							gspWaitForVBlank();
							hidScanInput();

							if (hidKeysDown() & KEY_A)
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

					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					consoleSelect(&bottomScreen);
					printf("\x1b[29;12HPress B to exit.");
					consoleSelect(&topScreen);
					printf("\x1b[2J");
					printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");					
					refresh(cont[0], topScreen, menuEntries, ENTRIES);
					refreshPokemon(topScreen, mainbuf, cont, game);
					break;
				}
				case 8 : {
					getPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					if (pkmn[0x08] == 0x00 && pkmn[0x09] == 0x00) 
						return 16;

					char *statslist[] = {"Health", "Attack", "Defense", "Speed", "Sp. Attack", "Sp. Defense"};
					static SwkbdState swkbd;
					static char buf[4] = {'0', '0', '0', '\0'};
					
					SwkbdButton button = SWKBD_BUTTON_NONE;
					
					consoleSelect(&bottomScreen);
					printf("\x1b[2J");
					
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

							if (hidKeysDown() & KEY_A)
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

					setPkmn(mainbuf, cont[1], cont[2], pkmn, game);

					consoleSelect(&bottomScreen);
					printf("\x1b[29;12HPress B to exit.");
					consoleSelect(&topScreen);
					printf("\x1b[2J");
					printf("\x1b[47;1;34m                  Pokemon Editor                  \x1b[0m\n");					
					refresh(cont[0], topScreen, menuEntries, ENTRIES);
					refreshPokemon(topScreen, mainbuf, cont, game);
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
