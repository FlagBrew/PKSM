#include "data/providers/mock/MockSaveDataProvider.hpp"

MockSaveDataProvider::MockSaveDataProvider(const AccountUid& initialUserId) : initialUserId(initialUserId) {
    // Initialize mock save data
    mockConsoleSaves = {// Legends Arceus (cartridge)
                        {0x00175E00,
                         {pksm::saves::Save::New("Cartridge Story Mode", "Cartridge Story Mode"),
                          pksm::saves::Save::New("Cartridge Hardcore", "Cartridge Hardcore"),
                          pksm::saves::Save::New("Cartridge Monotype", "Cartridge Monotype"),
                          pksm::saves::Save::New("Digital Speedrun", "Digital Speedrun"),
                          pksm::saves::Save::New("Digital Tournament", "Digital Tournament"),
                          pksm::saves::Save::New("Digital Wonderlocke", "Digital Wonderlocke"),
                          pksm::saves::Save::New("Digital Randomizer", "Digital Randomizer")}},
                        // Violet
                        {0x00164800,
                         {pksm::saves::Save::New("Digital Adventure", "Digital Adventure"),
                          pksm::saves::Save::New("Digital Any%", "Digital Any%"),
                          pksm::saves::Save::New("Digital Collection", "Digital Collection"),
                          pksm::saves::Save::New("Digital Safari", "Digital Safari"),
                          pksm::saves::Save::New("Digital Ranked", "Digital Ranked"),
                          pksm::saves::Save::New("Digital Egglocke", "Digital Egglocke"),
                          pksm::saves::Save::New("Digital Challenge", "Digital Challenge")}},
                        // Brilliant Diamond
                        {0x00164900,
                         {
                             pksm::saves::Save::New("Digital Story", "Digital Story"),
                             pksm::saves::Save::New("Digital Wedlocke", "Digital Wedlocke"),
                             pksm::saves::Save::New("Digital Elite", "Digital Elite"),
                         }},
                        // Scarlet
                        {0x00175F00,
                         {pksm::saves::Save::New("Digital Journey", "Digital Journey"),
                          pksm::saves::Save::New("Digital Master", "Digital Master"),
                          pksm::saves::Save::New("Digital Glitchless", "Digital Glitchless"),
                          pksm::saves::Save::New("Digital Collector", "Digital Collector"),
                          pksm::saves::Save::New("Digital Hunter", "Digital Hunter"),
                          pksm::saves::Save::New("Digital Pro", "Digital Pro"),
                          pksm::saves::Save::New("Digital Soullocke", "Digital Soullocke"),
                          pksm::saves::Save::New("Digital Champion", "Digital Champion")}},
                        // Let's Go Pikachu
                        {0x00175200,
                         {pksm::saves::Save::New("Digital Quest", "Digital Quest"),
                          pksm::saves::Save::New("Digital Lucky", "Digital Lucky"),
                          pksm::saves::Save::New("Digital Archive", "Digital Archive"),
                          pksm::saves::Save::New("Digital Expert", "Digital Expert")}},
                        // Let's Go Eevee
                        {0x00175201,
                         {pksm::saves::Save::New("Digital Tale", "Digital Tale"),
                          pksm::saves::Save::New("Digital Safari", "Digital Safari"),
                          pksm::saves::Save::New("Digital Museum", "Digital Museum"),
                          pksm::saves::Save::New("Digital Ultra", "Digital Ultra"),
                          pksm::saves::Save::New("Digital League", "Digital League"),
                          pksm::saves::Save::New("Digital Gallery", "Digital Gallery"),
                          pksm::saves::Save::New("Digital Explorer", "Digital Explorer")}},
                        // Sword
                        {0x00175202,
                         {pksm::saves::Save::New("Digital Legend", "Digital Legend"),
                          pksm::saves::Save::New("Digital Extreme", "Digital Extreme"),
                          pksm::saves::Save::New("Digital Stadium", "Digital Stadium"),
                          pksm::saves::Save::New("Digital Archive", "Digital Archive"),
                          pksm::saves::Save::New("Digital Quest", "Digital Quest")}},
                        // Shield
                        {0x00175203,
                         {pksm::saves::Save::New("Digital Story", "Digital Story"),
                          pksm::saves::Save::New("Digital Master", "Digital Master"),
                          pksm::saves::Save::New("Digital Arena", "Digital Arena"),
                          pksm::saves::Save::New("Digital Museum", "Digital Museum"),
                          pksm::saves::Save::New("Digital Safari", "Digital Safari")}},
                        // Shining Pearl
                        {0x00175204,
                         {pksm::saves::Save::New("Digital Adventure", "Digital Adventure"),
                          pksm::saves::Save::New("Digital Typelocke", "Digital Typelocke"),
                          pksm::saves::Save::New("Digital Elite", "Digital Elite"),
                          pksm::saves::Save::New("Digital Archive", "Digital Archive")}}
    };

    mockEmulatorSaves = {
        // Alpha Sapphire
        {0x00180000,
         {pksm::saves::Save::New("Main Story", "Main Story"),
          pksm::saves::Save::New("Nuzlocke", "Nuzlocke"),
          pksm::saves::Save::New("Shiny Hunt", "Shiny Hunt")}},
        // Black
        {0x00180001,
         {pksm::saves::Save::New("Adventure", "Adventure"), pksm::saves::Save::New("Challenge Mode", "Challenge Mode")}
        },
        // Black 2
        {0x00180002,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Hard Mode", "Hard Mode"),
          pksm::saves::Save::New("Speedrun", "Speedrun")}},
        // Blue
        {0x00180003,
         {pksm::saves::Save::New("Classic Run", "Classic Run"), pksm::saves::Save::New("No Items", "No Items")}},
        // Crystal
        {0x00180004,
         {pksm::saves::Save::New("Main Game", "Main Game"), pksm::saves::Save::New("Catch Em All", "Catch Em All")}},
        // Diamond
        {0x00180005, {pksm::saves::Save::New("Story", "Story"), pksm::saves::Save::New("Competitive", "Competitive")}},
        // Emerald
        {0x00180006,
         {pksm::saves::Save::New("Battle Frontier", "Battle Frontier"),
          pksm::saves::Save::New("Monotype", "Monotype"),
          pksm::saves::Save::New("Speedrun", "Speedrun")}},
        // FireRed
        {0x00180007,
         {pksm::saves::Save::New("Main Story", "Main Story"), pksm::saves::Save::New("Nuzlocke", "Nuzlocke")}},
        // Gold
        {0x00180008, {pksm::saves::Save::New("Classic", "Classic"), pksm::saves::Save::New("Challenge", "Challenge")}},
        // HeartGold
        {0x00180009,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Pokeathlon", "Pokeathlon"),
          pksm::saves::Save::New("Safari Zone", "Safari Zone")}},
        // LeafGreen
        {0x0018000A, {pksm::saves::Save::New("Adventure", "Adventure"), pksm::saves::Save::New("Speedrun", "Speedrun")}
        },
        // Moon
        {0x0018000B,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Battle Tree", "Battle Tree"),
          pksm::saves::Save::New("Shiny Hunt", "Shiny Hunt")}},
        // Omega Ruby
        {0x0018000C,
         {pksm::saves::Save::New("Main Story", "Main Story"),
          pksm::saves::Save::New("Contest Star", "Contest Star"),
          pksm::saves::Save::New("Secret Bases", "Secret Bases")}},
        // Pearl
        {0x0018000D,
         {pksm::saves::Save::New("Story Mode", "Story Mode"), pksm::saves::Save::New("Underground", "Underground")}},
        // Platinum
        {0x0018000E,
         {pksm::saves::Save::New("Main Game", "Main Game"),
          pksm::saves::Save::New("Battle Frontier", "Battle Frontier"),
          pksm::saves::Save::New("Distortion World", "Distortion World")}},
        // Red
        {0x0018000F, {pksm::saves::Save::New("Classic", "Classic"), pksm::saves::Save::New("Glitchless", "Glitchless")}
        },
        // Ruby
        {0x00180010,
         {pksm::saves::Save::New("Story Mode", "Story Mode"), pksm::saves::Save::New("Contest Run", "Contest Run")}},
        // Sapphire
        {0x00180011,
         {pksm::saves::Save::New("Adventure", "Adventure"), pksm::saves::Save::New("Secret Base", "Secret Base")}},
        // Silver
        {0x00180012,
         {pksm::saves::Save::New("Main Story", "Main Story"), pksm::saves::Save::New("Challenge Mode", "Challenge Mode")
         }},
        // SoulSilver
        {0x00180013,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Pokewalker", "Pokewalker"),
          pksm::saves::Save::New("Bug Contest", "Bug Contest")}},
        // Sun
        {0x00180014,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Festival Plaza", "Festival Plaza"),
          pksm::saves::Save::New("Battle Royal", "Battle Royal")}},
        // Ultra Moon
        {0x00180015,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Ultra Wormhole", "Ultra Wormhole"),
          pksm::saves::Save::New("Battle Agency", "Battle Agency")}},
        // Ultra Sun
        {0x00180016,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Rainbow Rocket", "Rainbow Rocket"),
          pksm::saves::Save::New("Mantine Surf", "Mantine Surf")}},
        // White
        {0x00180017,
         {pksm::saves::Save::New("Story Mode", "Story Mode"), pksm::saves::Save::New("Dream World", "Dream World")}},
        // White 2
        {0x00180018,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Challenge Mode", "Challenge Mode"),
          pksm::saves::Save::New("PWT", "PWT")}},
        // X
        {0x00180019,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Friend Safari", "Friend Safari"),
          pksm::saves::Save::New("Battle Chateau", "Battle Chateau")}},
        // Y
        {0x0018001A,
         {pksm::saves::Save::New("Story Mode", "Story Mode"),
          pksm::saves::Save::New("Wonder Trade", "Wonder Trade"),
          pksm::saves::Save::New("Super Training", "Super Training")}},
        // Yellow
        {0x0018001B,
         {pksm::saves::Save::New("Classic", "Classic"), pksm::saves::Save::New("Pikachu Fan", "Pikachu Fan")}}
    };

    mockCustomSaves = {// Yellow
                       {0x00164A00,
                        {pksm::saves::Save::New("Main Story", "Main Story"),
                         pksm::saves::Save::New("Nuzlocke", "Nuzlocke"),
                         pksm::saves::Save::New("Shiny Hunt", "Shiny Hunt")}},
                       // Crystal
                       {0x00164B00,
                        {pksm::saves::Save::New("Main Story", "Main Story"),
                         pksm::saves::Save::New("Nuzlocke", "Nuzlocke"),
                         pksm::saves::Save::New("Shiny Hunt", "Shiny Hunt")}},
                       // Emerald
                       {0x00164C00,
                        {pksm::saves::Save::New("Main Story", "Main Story"),
                         pksm::saves::Save::New("Nuzlocke", "Nuzlocke"),
                         pksm::saves::Save::New("Shiny Hunt", "Shiny Hunt")}}
    };
}

std::vector<pksm::saves::Save::Ref> MockSaveDataProvider::GetUserSpecificSaves(
    const std::vector<pksm::saves::Save::Ref>& baseSaves,
    bool isOtherUser
) const {
    if (!isOtherUser) {
        return baseSaves;
    }

    // For other users, append "Extra" to save names
    std::vector<pksm::saves::Save::Ref> userSaves;
    userSaves.reserve(baseSaves.size());
    for (const auto& save : baseSaves) {
        std::string newName = save->getName() + " Extra";
        userSaves.push_back(pksm::saves::Save::New(newName, newName));
    }
    return userSaves;
}

std::vector<pksm::saves::Save::Ref> MockSaveDataProvider::GetSavesForTitle(
    const pksm::titles::Title::Ref& title,
    const std::optional<AccountUid>& currentUser
) const {
    if (!title) {
        return {};
    }

    // Check if this is a console save
    auto consoleIt = mockConsoleSaves.find(title->getTitleId());
    if (consoleIt != mockConsoleSaves.end()) {
        // For console saves, we need a user ID
        if (!currentUser) {
            return {};  // No saves available without a user ID for console titles
        }

        // Check if this is the initial user
        bool isOtherUser = !(initialUserId.uid[0] == currentUser->uid[0] && initialUserId.uid[1] == currentUser->uid[1]);
        return GetUserSpecificSaves(consoleIt->second, isOtherUser);
    }

    // For emulator and custom saves, return as is (no user-specific modifications)
    auto emulatorIt = mockEmulatorSaves.find(title->getTitleId());
    if (emulatorIt != mockEmulatorSaves.end()) {
        return emulatorIt->second;
    }

    auto customIt = mockCustomSaves.find(title->getTitleId());
    if (customIt != mockCustomSaves.end()) {
        return customIt->second;
    }

    return {pksm::saves::Save::New("Main Save", "Main Save")};
}

bool MockSaveDataProvider::LoadSave(
    const pksm::titles::Title::Ref& title,
    const std::string& saveName,
    const AccountUid* userId
) {
    // For installed titles, userId is required
    if (mockConsoleSaves.find(title->getTitleId()) != mockConsoleSaves.end()) {
        if (!userId) {
            return false;
        }
    }

    // Mock implementation - just return true to indicate success
    return true;
}