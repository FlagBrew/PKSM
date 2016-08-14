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
void setEV(u8* pkmn, u8 val, const int stat);
void setIV(u8* pkmn, u8 val, const int stat);
u8 getIV(u8* pkmn, const int stat);
void setHPType(u8* pkmn, const int val);
u16 getOTID(u8* pkmn);
u16 getSOTID(u8* pkmn);
u32 getPID(u8* pkmn);
bool isShiny(u8* pkmn);
void rerollPID(u8* pkmn);
void setShiny(u8* pkmn, const bool shiny);

int pokemonEditor(PrintConsole topScreen, PrintConsole bottomScreen, int game[], int pokemonCont[]);