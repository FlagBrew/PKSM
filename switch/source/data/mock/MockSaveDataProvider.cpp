#include "data/mock/MockSaveDataProvider.hpp"

MockSaveDataProvider::MockSaveDataProvider() {
    // Initialize mock save data
    mockSaves = {
        // Legends Arceus (cartridge)
        {0x00175E00, {
            "Cartridge Main Save",
            "Cartridge Challenge Mode",
            "Cartridge Nuzlocke",
            "Digital Shiny Hunt",
            "Digital Competitive",
            "Digital Nuzlocke",
            "Digital Battle Tree"
        }},
        // Violet
        {0x00164800, {
            "Digital Main Save",
            "Digital Speedrun",
            "Digital Living Dex",
            "Digital Shiny Hunt",
            "Digital Competitive",
            "Digital Nuzlocke",
            "Digital Battle Tree"
        }},
        // Brilliant Diamond
        {0x00164900, {
            "Digital Main Save",
            "Digital Nuzlocke",
            "Digital Challenge Mode",
        }},
        // Scarlet
        {0x00175F00, {
            "Digital Main Save",
            "Digital Challenge Mode",
            "Digital Speedrun",
            "Digital Living Dex",
            "Digital Shiny Hunt",
            "Digital Competitive",
            "Digital Nuzlocke",
            "Digital Battle Tree"
        }},
        // Let's Go Pikachu
        {0x00175200, {
            "Digital Main Save",
            "Digital Shiny Hunt",
            "Digital Living Dex",
            "Digital Master Trainer"
        }},
        // Let's Go Eevee
        {0x00175201, {
            "Digital Main Save",
            "Digital Shiny Hunt",
            "Digital Living Dex",
            "Digital Master Trainer"
        }},
        // Sword
        {0x00175202, {
            "Digital Main Save",
            "Digital Challenge Mode",
            "Digital Competitive",
            "Digital Living Dex",
            "Digital Shiny Hunt"
        }},
        // Shield
        {0x00175203, {
            "Digital Main Save",
            "Digital Challenge Mode",
            "Digital Competitive",
            "Digital Living Dex",
            "Digital Shiny Hunt"
        }},
        // Shining Pearl
        {0x00175204, {
            "Digital Main Save",
            "Digital Nuzlocke",
            "Digital Challenge Mode",
            "Digital Living Dex"
        }}
    };
}

std::vector<std::string> MockSaveDataProvider::GetSavesForTitle(const titles::TitleRef& title) const {
    if (!title) {
        return {};
    }
    
    auto it = mockSaves.find(title->getTitleId());
    if (it != mockSaves.end()) {
        return it->second;
    }
    
    return {"Main Save"};  // Default for unknown titles
}

bool MockSaveDataProvider::LoadSave(const titles::TitleRef& title, const std::string& saveName) {
    // Mock implementation - just return true to indicate success
    return true;
} 