#include <assert.h>
#include <stddef.h>
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;

struct PKHeXPersonalEntry
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
    u8 ability1;
    u8 ability2;
    u8 abilityH;
    u8 escapeRate;
    u16 formStatIndex;
    u16 formSprite;
    u8 formCount;
    u8 spriteData;
    u16 expYield;
    u16 height;
    u16 weight;
    u8 TMHM[16];
    u8 typeTutors[4];
    u8 specialTutors[10];
    u8 empty1[6];
    u16 specialZItem;
    u16 specialZBaseMove;
    u16 specialZMove;
    u8 localVariant;
    u8 empty2;
};

#pragma pack(push, 1)
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
    u8 ability1;
    u8 ability2;
    u8 abilityH;
    u16 formStatIndex;
    u8 formCount;
};
#pragma pack(pop)

int main()
{
    assert(sizeof(PKSMPersonalEntry) == 17);
    FILE* personalFile = fopen("personal_uu", "rb");
    fseek(personalFile, 0, SEEK_END);
    auto size = ftell(personalFile);
    fseek(personalFile, 0, SEEK_SET);
    assert(size % sizeof(PKHeXPersonalEntry) == 0);
    size_t entries                = size / sizeof(PKHeXPersonalEntry);
    PKHeXPersonalEntry* inEntries = new PKHeXPersonalEntry[entries];
    fread(inEntries, sizeof(PKHeXPersonalEntry), entries, personalFile);
    fclose(personalFile);

    sizeof(PKSMPersonalEntry);

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
        outEntries[i].ability1       = inEntries[i].ability1;
        outEntries[i].ability2       = inEntries[i].ability2;
        outEntries[i].abilityH       = inEntries[i].abilityH;
        outEntries[i].formStatIndex  = inEntries[i].formStatIndex;
        outEntries[i].formCount      = inEntries[i].formCount;
    }

    printf("constexpr char personal_smusum[] = { ");
    for (size_t i = 0; i < entries * sizeof(PKSMPersonalEntry) - 1; i++)
    {
        printf("0x%x, ", *(((u8*)outEntries) + i));
    }
    printf("0x%x };\n", *(((u8*)outEntries) + entries * sizeof(PKSMPersonalEntry) - 1));

    printf("constexpr int personal_smusum_size = %li;\n", entries * sizeof(PKSMPersonalEntry));

    return 0;
}
