#include <assert.h>
#include <stddef.h>
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;

#pragma pack(push, 1)
struct PKHeXPersonalEntry
{
    u8 species;
    u8 hp;
    u8 atk;
    u8 def;
    u8 spe;
    u8 spad;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u8 expYield;
    u8 spriteDimensions;
    u16 frontSpritePointer;
    u16 backSpritePointer;
    u8 levelOneAttacks[4];
    u8 expType;

    // PKSM doesn't care about move legality, so no bitfield for you
    u8 tmHmFlags[7];

    u8 padding;
};
#pragma pack(pop)

struct PKSMPersonalEntry
{
    u8 hp;
    u8 atk;
    u8 def;
    u8 spe;
    u8 spad;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u8 expType;
};

int main()
{
    assert(sizeof(PKSMPersonalEntry) == 9);
    FILE* personalFile = fopen("personal_y", "rb");
    fseek(personalFile, 0, SEEK_END);
    auto size = ftell(personalFile);
    fseek(personalFile, 0, SEEK_SET);
    assert(size % sizeof(PKHeXPersonalEntry) == 0);
    size_t entries                = size / sizeof(PKHeXPersonalEntry);
    PKHeXPersonalEntry* inEntries = new PKHeXPersonalEntry[entries];
    fread(inEntries, sizeof(PKHeXPersonalEntry), entries, personalFile);
    fclose(personalFile);

    PKSMPersonalEntry* outEntries = new PKSMPersonalEntry[entries];
    for (size_t i = 0; i < entries; i++)
    {
        outEntries[i].hp             = inEntries[i].hp;
        outEntries[i].atk            = inEntries[i].atk;
        outEntries[i].def            = inEntries[i].def;
        outEntries[i].spe            = inEntries[i].spe;
        outEntries[i].spad           = inEntries[i].spad;
        outEntries[i].type1          = inEntries[i].type1;
        outEntries[i].type2          = inEntries[i].type2;
        outEntries[i].catchRate      = inEntries[i].catchRate;
        outEntries[i].expType        = inEntries[i].expType;
    }

    printf("constexpr unsigned char personal_y[] = { ");
    for (size_t i = 0; i < entries * sizeof(PKSMPersonalEntry) - 1; i++)
    {
        printf("0x%x, ", *(((u8*)outEntries) + i));
    }
    printf("0x%x };\n", *(((u8*)outEntries) + entries * sizeof(PKSMPersonalEntry) - 1));

    printf("constexpr int personal_y_size = %li;\n", entries * sizeof(PKSMPersonalEntry));

    printf("constexpr int personal_y_entrysize = %li;\n", sizeof(PKSMPersonalEntry));

    return 0;
}
