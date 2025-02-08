#include "data/mock/MockTitleDataProvider.hpp"

MockTitleDataProvider::MockTitleDataProvider() {
    // Initialize mock data
    mockCartridgeTitle = std::make_shared<pksm::titles::Title>(
        "Pokémon Legends: Arceus",
        "romfs:/gfx/mock/arceus_menu_icon.jpg",
        0x00175E00
    );

    mockInstalledTitles = {
        std::make_shared<pksm::titles::Title>("Pokémon Violet", "romfs:/gfx/mock/violet_menu_icon.jpg", 0x00164800),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Brilliant Diamond",
            "romfs:/gfx/mock/brilliant_diamond_menu_icon.jpg",
            0x00164900
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Scarlet", "romfs:/gfx/mock/scarlet_menu_icon.jpg", 0x00175F00),
        std::make_shared<pksm::titles::Title>(
            "Pokémon: Let's Go Pikachu",
            "romfs:/gfx/mock/lg_pikachu_menu_icon.jpg",
            0x00175200
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon: Let's Go Eevee",
            "romfs:/gfx/mock/lg_eevee_menu_icon.jpg",
            0x00175201
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Sword", "romfs:/gfx/mock/sword_menu_icon.jpg", 0x00175202),
        std::make_shared<pksm::titles::Title>("Pokémon Shield", "romfs:/gfx/mock/shield_menu_icon.jpg", 0x00175203),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Shining Pearl",
            "romfs:/gfx/mock/shining_pearl_menu_icon.jpg",
            0x00175204
        )
    };
}

pksm::titles::Title::Ref MockTitleDataProvider::GetGameCardTitle() const {
    return mockCartridgeTitle;
}

std::vector<pksm::titles::Title::Ref> MockTitleDataProvider::GetInstalledTitles() const {
    return mockInstalledTitles;
}