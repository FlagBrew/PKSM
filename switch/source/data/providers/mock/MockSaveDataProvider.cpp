#include "data/providers/mock/MockSaveDataProvider.hpp"

MockSaveDataProvider::MockSaveDataProvider() {
    // Initialize mock save data
    mockSaves = {// Legends Arceus (cartridge)
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
}

std::vector<pksm::saves::Save::Ref> MockSaveDataProvider::GetSavesForTitle(const pksm::titles::Title::Ref& title
) const {
    if (!title) {
        return {};
    }

    auto it = mockSaves.find(title->getTitleId());
    if (it != mockSaves.end()) {
        return it->second;
    }

    return {pksm::saves::Save::New("Main Save", "Main Save")};  // Default for unknown titles
}

bool MockSaveDataProvider::LoadSave(const pksm::titles::Title::Ref& title, const std::string& saveName) {
    // Mock implementation - just return true to indicate success
    return true;
}