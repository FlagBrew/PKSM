#include <3ds.h>
#include <string.h>

u32 seedStep(const u32 seed);
void shuffleArray(char* pkmn, const u32 encryptionkey);
void decryptPkmn(char* pkmn);
int getPkmnAddress(const int boxnumber, const int indexnumber, int game);
void calculatePKMNChecksum(char* data);
void encryptPkmn(char* pkmn);
void encryptBattleSection(char* pkmn);
void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, char* pkmn, int game);
void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, char* pkmn, int game);
void refreshPokemon(PrintConsole topScreen, int game, int pokemonCont[]);
int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int pokemonCont[]);