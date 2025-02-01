#include "data/mock/MockTitleDataProvider.hpp"

MockTitleDataProvider::MockTitleDataProvider() {
    // Initialize mock data
    mockCartridgeTitle = std::make_shared<titles::Title>(
        "Pokémon Ultra Sun", 
        "romfs:/gfx/ui/gameselector_cart.png", 
        0x00175E00
    );

    mockInstalledTitles = {
        std::make_shared<titles::Title>("Pokémon Sun", "romfs:/gfx/ui/gameselector_cart.png", 0x00164800),
        std::make_shared<titles::Title>("Pokémon Moon", "romfs:/gfx/ui/gameselector_cart.png", 0x00164900),
        std::make_shared<titles::Title>("Pokémon Ultra Moon", "romfs:/gfx/ui/gameselector_cart.png", 0x00175F00),
        std::make_shared<titles::Title>("Pokémon Omega Ruby", "romfs:/gfx/ui/gameselector_cart.png", 0x00175E00)
    };
}

titles::TitleRef MockTitleDataProvider::GetGameCardTitle() const {
    return mockCartridgeTitle;
}

std::vector<titles::TitleRef> MockTitleDataProvider::GetInstalledTitles() const {
    return mockInstalledTitles;
} 