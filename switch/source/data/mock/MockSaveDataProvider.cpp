#include "data/mock/MockSaveDataProvider.hpp"

MockSaveDataProvider::MockSaveDataProvider() {
    // Initialize mock save data
    mockSaves = {
        // Ultra Sun (cartridge)
        {0x00175E00, {
            "Cartridge Main Save",
            "Cartridge Challenge Mode",
            "Cartridge Nuzlocke"
        }},
        // Sun
        {0x00164800, {
            "Digital Main Save",
            "Digital Speedrun",
            "Digital Living Dex",
            "Digital Shiny Hunt",
            "Digital Competitive",
            "Digital Nuzlocke",
            "Digital Battle Tree"
        }},
        // Moon
        {0x00164900, {
            "Digital Main Save",
            "Digital Nuzlocke"
        }},
        // Ultra Moon
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
        // Omega Ruby
        {0x00175200, {
            "Emulator Main Save",
            "Emulator Nuzlocke",
            "Emulator Speedrun",
            "Emulator Competitive",
            "Emulator Battle Tree",
            "Emulator Living Dex",
            "Emulator Shiny Hunt"
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