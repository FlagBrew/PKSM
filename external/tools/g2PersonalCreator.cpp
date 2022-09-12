#include <assert.h>
#include <stddef.h>
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;

struct PKHeXPersonalEntry
{
    u8 species;
    u8 hp;
    u8 atk;
    u8 def;
    u8 spe;
    u8 spa;
    u8 spd;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u8 expYield;
    u8 item1;
    u8 item2;
    u8 gender;
    u8 unknown1;
    u8 hatchCycles;
    u8 unknown2;
    u8 spriteDimensions;
    u8 filler[4];
    u8 expType;
    u8 eggGroups;

    // PKSM doesn't care about move legality, so no bitfield for you
    // In Crystal, also includes move tutor eligibility
    u8 tmHmFlags[8];
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
    u8 expType;
};
int main()
{
    assert(sizeof(PKSMPersonalEntry) == 10);
    FILE* personalFile = fopen("personal_c", "rb");
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
        outEntries[i].spa            = inEntries[i].spa;
        outEntries[i].spd            = inEntries[i].spd;
        outEntries[i].type1          = inEntries[i].type1;
        outEntries[i].type2          = inEntries[i].type2;
        outEntries[i].gender         = inEntries[i].gender;
        outEntries[i].expType        = inEntries[i].expType;
    }

    printf("constexpr unsigned char personal_gsc[] = { ");
    for (size_t i = 0; i < entries * sizeof(PKSMPersonalEntry) - 1; i++)
    {
        printf("0x%x, ", *(((u8*)outEntries) + i));
    }
    printf("0x%x };\n", *(((u8*)outEntries) + entries * sizeof(PKSMPersonalEntry) - 1));

    printf("constexpr int personal_gsc_size = %li;\n", entries * sizeof(PKSMPersonalEntry));

    printf("constexpr int personal_gsc_entrysize = %li;\n", sizeof(PKSMPersonalEntry));

    return 0;
}
