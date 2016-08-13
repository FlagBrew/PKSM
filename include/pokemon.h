#include <3ds.h>
#include <string.h>

u32 seedStep(const u32 seed);
void shuffleArray(u8* pkmn, const u32 encryptionkey);
void decryptPkmn(u8* pkmn);
int getPkmnAddress(const int boxnumber, const int indexnumber, int game);
void calculatePKMNChecksum(u8* data);
void encryptPkmn(u8* pkmn);
void encryptBattleSection(u8* pkmn);
void getPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game);
void setPkmn(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn, int game);
void refreshPokemon(PrintConsole topScreen, int game, int pokemonCont[]);

void setFriendship(u8* pkmn, u32 value);

int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int pokemonCont[]);