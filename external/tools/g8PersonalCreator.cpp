#include <assert.h>
#include <stddef.h>
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;

struct SWSHPersonalEntry
{
    u8 hp;
    u8 atk;
    u8 def;
    u8 spe;
    u8 spa;
    u8 spd;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u8 evoStage;
    u16 evYield;
    u16 item1;
    u16 item2;
    u16 item3;
    u8 gender;
    u8 hatchCycles;
    u8 baseFriendship;
    u8 expType;
    u8 eggGroup1;
    u8 eggGroup2;
    u16 ability1;
    u16 ability2;
    u16 abilityH;
    u16 formStatIndex;
    u8 formCount;
    u8 color : 6;
    bool presentInGame : 1;
    bool spriteForm : 1;
    u16 baseExp;
    u16 height;
    u16 weight;       // 0x26-0x27
    u8 TMFlags[16];   // 0x28-0x37
    u8 typeTutors[4]; // 0x38-0x3B
    u8 TRFlags[16];   // 0x3C-0x4B
    u16 species;
    u8 unknown[8];
    u16 baseSpecies;
    u16 baseSpeciesForm;
    u16 flags;
    u16 pokedexIndex;
    u16 formIndex;
    u8 padding[76];
    u16 armordexIndex;
    u16 crowndexIndex;
};

struct PKSMPersonalEntry
{
    u8 hp;
    u8 atk;
    u8 def;
    u8 spe;
    u8 spa;
    u8 spd;
    u8 type1;
    u8 type2;
    u8 gender;
    u8 baseFriendship;
    u8 expType;
    u8 formCount;
    u16 ability1;
    u16 ability2;
    u16 abilityH;
    u16 formStatIndex;
    u16 pokedexIndex;
    u16 armordexIndex;
    u16 crowndexIndex;
    u8 TRFlags[14]; // Not 13 because padding
};

int main()
{
    static_assert(sizeof(SWSHPersonalEntry) == 0xB0);
    static_assert(sizeof(PKSMPersonalEntry) == 40);
    FILE* personalFile = fopen("personal_swsh", "rb");
    fseek(personalFile, 0, SEEK_END);
    auto size = ftell(personalFile);
    fseek(personalFile, 0, SEEK_SET);
    assert(size % sizeof(SWSHPersonalEntry) == 0);
    size_t entries               = size / sizeof(SWSHPersonalEntry);
    SWSHPersonalEntry* inEntries = new SWSHPersonalEntry[entries];
    fread(inEntries, sizeof(SWSHPersonalEntry), entries, personalFile);
    fclose(personalFile);

    PKSMPersonalEntry* outEntries = new PKSMPersonalEntry[entries];
    for (size_t i = 0; i < entries; i++)
    {
        outEntries[i].hp             = inEntries[i].hp;
        outEntries[i].atk            = inEntries[i].atk;
        outEntries[i].def            = inEntries[i].def;
        outEntries[i].spe            = inEntries[i].spe;
        outEntries[i].spa            = inEntries[i].spa;
        outEntries[i].spd            = inEntries[i].spd;
        outEntries[i].type1          = inEntries[i].type1;
        outEntries[i].type2          = inEntries[i].type2;
        outEntries[i].gender         = inEntries[i].gender;
        outEntries[i].baseFriendship = inEntries[i].baseFriendship;
        outEntries[i].expType        = inEntries[i].expType;
        outEntries[i].formCount      = inEntries[i].formCount;
        outEntries[i].ability1       = inEntries[i].ability1;
        outEntries[i].ability2       = inEntries[i].ability2;
        outEntries[i].abilityH       = inEntries[i].abilityH;
        outEntries[i].formStatIndex  = inEntries[i].formStatIndex;
        outEntries[i].pokedexIndex   = inEntries[i].pokedexIndex;
        outEntries[i].armordexIndex  = inEntries[i].armordexIndex;
        outEntries[i].crowndexIndex  = inEntries[i].crowndexIndex;

        for (size_t j = 0; j < 14; j++)
        {
            outEntries[i].TRFlags[j] = inEntries[i].TRFlags[j];
        }
    }

    printf("constexpr unsigned char personal_swsh[] = { ");
    for (size_t i = 0; i < entries * sizeof(PKSMPersonalEntry) - 1; i++)
    {
        printf("0x%x, ", *(((u8*)outEntries) + i));
    }
    printf("0x%x };\n", *(((u8*)outEntries) + entries * sizeof(PKSMPersonalEntry) - 1));

    printf("constexpr int personal_swsh_size = %li;\n", entries * sizeof(PKSMPersonalEntry));

    printf("constexpr int personal_swsh_entrysize = %li;\n", sizeof(PKSMPersonalEntry));

    return 0;
}
