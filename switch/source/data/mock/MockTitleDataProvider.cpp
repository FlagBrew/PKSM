#include "data/mock/MockTitleDataProvider.hpp"

MockTitleDataProvider::MockTitleDataProvider() {
    // Initialize mock data
    mockCartridgeTitle = std::make_shared<titles::Title>(
        "Pokémon Legends: Arceus", 
        "romfs:/gfx/mock/arceus_menu_icon.jpg", 
        0x00175E00
    );

    mockInstalledTitles = {
        std::make_shared<titles::Title>("Pokémon Violet", "romfs:/gfx/mock/violet_menu_icon.jpg", 0x00164800),
        std::make_shared<titles::Title>("Pokémon Brilliant Diamond", "romfs:/gfx/mock/brilliant_diamond_menu_icon.jpg", 0x00164900),
        std::make_shared<titles::Title>("Pokémon Scarlet", "romfs:/gfx/mock/scarlet_menu_icon.jpg", 0x00175F00),
        std::make_shared<titles::Title>("Pokémon: Let's Go Pikachu", "romfs:/gfx/mock/lg_pikachu_menu_icon.jpg", 0x00175200)
    };
}

titles::TitleRef MockTitleDataProvider::GetGameCardTitle() const {
    return mockCartridgeTitle;
}

std::vector<titles::TitleRef> MockTitleDataProvider::GetInstalledTitles() const {
    return mockInstalledTitles;
} 