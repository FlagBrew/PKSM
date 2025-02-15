#include "data/providers/mock/MockTitleDataProvider.hpp"

#include <algorithm>
#include <numeric>
#include <random>

MockTitleDataProvider::MockTitleDataProvider(const AccountUid& initialUserId)
  : initialUserId(initialUserId), userSpecificTitles(), rng(std::random_device{}()) {
    customTitleProvider = CustomTitleProvider::New();

    // Initialize mock data
    mockCartridgeTitle = std::make_shared<pksm::titles::Title>(
        "Pokémon Legends: Arceus",
        "romfs:/gfx/mock/console/arceus_menu_icon.jpg",
        0x00175E00
    );

    mockInstalledTitles = {
        std::make_shared<pksm::titles::Title>(
            "Pokémon Violet",
            "romfs:/gfx/mock/console/violet_menu_icon.jpg",
            0x00164800
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Brilliant Diamond",
            "romfs:/gfx/mock/console/brilliant_diamond_menu_icon.jpg",
            0x00164900
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Scarlet",
            "romfs:/gfx/mock/console/scarlet_menu_icon.jpg",
            0x00175F00
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon: Let's Go Pikachu",
            "romfs:/gfx/mock/console/lg_pikachu_menu_icon.jpg",
            0x00175200
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon: Let's Go Eevee",
            "romfs:/gfx/mock/console/lg_eevee_menu_icon.jpg",
            0x00175201
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Sword",
            "romfs:/gfx/mock/console/sword_menu_icon.jpg",
            0x00175202
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Shield",
            "romfs:/gfx/mock/console/shield_menu_icon.jpg",
            0x00175203
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Shining Pearl",
            "romfs:/gfx/mock/console/shining_pearl_menu_icon.jpg",
            0x00175204
        )
    };

    mockEmulatorTitles = {
        std::make_shared<pksm::titles::Title>(
            "Pokémon Alpha Sapphire",
            "romfs:/gfx/mock/emulator/alpha_sapphire_menu_icon.jpg",
            0x00180000
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Black",
            "romfs:/gfx/mock/emulator/black_menu_icon.jpg",
            0x00180001
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Black 2",
            "romfs:/gfx/mock/emulator/black_2_menu_icon.jpg",
            0x00180002
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Blue", "romfs:/gfx/mock/emulator/blue_menu_icon.jpg", 0x00180003),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Crystal",
            "romfs:/gfx/mock/emulator/crystal_menu_icon.jpg",
            0x00180004
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Diamond",
            "romfs:/gfx/mock/emulator/diamond_menu_icon.jpg",
            0x00180005
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Emerald",
            "romfs:/gfx/mock/emulator/emerald_menu_icon.jpg",
            0x00180006
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon FireRed",
            "romfs:/gfx/mock/emulator/fire_red_menu_icon.jpg",
            0x00180007
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Gold", "romfs:/gfx/mock/emulator/gold_menu_icon.jpg", 0x00180008),
        std::make_shared<pksm::titles::Title>(
            "Pokémon HeartGold",
            "romfs:/gfx/mock/emulator/heart_gold_menu_icon.jpg",
            0x00180009
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon LeafGreen",
            "romfs:/gfx/mock/emulator/leaf_green_menu_icon.jpg",
            0x0018000A
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Moon", "romfs:/gfx/mock/emulator/moon_menu_icon.jpg", 0x0018000B),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Omega Ruby",
            "romfs:/gfx/mock/emulator/omega_ruby_menu_icon.jpg",
            0x0018000C
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Pearl",
            "romfs:/gfx/mock/emulator/pearl_menu_icon.jpg",
            0x0018000D
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Platinum",
            "romfs:/gfx/mock/emulator/platinum_menu_icon.jpg",
            0x0018000E
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Red", "romfs:/gfx/mock/emulator/red_menu_icon.jpg", 0x0018000F),
        std::make_shared<pksm::titles::Title>("Pokémon Ruby", "romfs:/gfx/mock/emulator/ruby_menu_icon.jpg", 0x00180010),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Sapphire",
            "romfs:/gfx/mock/emulator/sapphire_menu_icon.jpg",
            0x00180011
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Silver",
            "romfs:/gfx/mock/emulator/silver_menu_icon.jpg",
            0x00180012
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon SoulSilver",
            "romfs:/gfx/mock/emulator/soul_silver_menu_icon.jpg",
            0x00180013
        ),
        std::make_shared<pksm::titles::Title>("Pokémon Sun", "romfs:/gfx/mock/emulator/sun_menu_icon.jpg", 0x00180014),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Ultra Moon",
            "romfs:/gfx/mock/emulator/ultra_moon_menu_icon.jpg",
            0x00180015
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Ultra Sun",
            "romfs:/gfx/mock/emulator/ultra_sun_menu_icon.jpg",
            0x00180016
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon White",
            "romfs:/gfx/mock/emulator/white_menu_icon.jpg",
            0x00180017
        ),
        std::make_shared<pksm::titles::Title>(
            "Pokémon White 2",
            "romfs:/gfx/mock/emulator/white_2_menu_icon.jpg",
            0x00180018
        ),
        std::make_shared<pksm::titles::Title>("Pokémon X", "romfs:/gfx/mock/emulator/x_menu_icon.jpg", 0x00180019),
        std::make_shared<pksm::titles::Title>("Pokémon Y", "romfs:/gfx/mock/emulator/y_menu_icon.jpg", 0x0018001A),
        std::make_shared<pksm::titles::Title>(
            "Pokémon Yellow",
            "romfs:/gfx/mock/emulator/yellow_menu_icon.jpg",
            0x0018001B
        )
    };
}

std::vector<pksm::titles::Title::Ref> MockTitleDataProvider::GenerateRandomTitleSubset(
    const std::vector<pksm::titles::Title::Ref>& source
) const {
    std::vector<pksm::titles::Title::Ref> subset;
    std::vector<size_t> indices(source.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Randomly select 30-70% of the titles
    std::shuffle(indices.begin(), indices.end(), rng);
    size_t count = source.size() * (0.3 + (rng() % 40) / 100.0);

    subset.reserve(count);
    for (size_t i = 0; i < count && i < indices.size(); i++) {
        subset.push_back(source[indices[i]]);
    }

    return subset;
}

pksm::titles::Title::Ref MockTitleDataProvider::GetGameCardTitle() const {
    return mockCartridgeTitle;
}

std::vector<pksm::titles::Title::Ref> MockTitleDataProvider::GetInstalledTitles(const AccountUid& userId) const {
    // For the initial user, return all titles
    if (userId.uid[0] == initialUserId.uid[0] && userId.uid[1] == initialUserId.uid[1]) {
        return mockInstalledTitles;
    }

    // For other users, check if we have already generated their subset
    auto it = userSpecificTitles.find(userId);
    if (it != userSpecificTitles.end()) {
        return it->second;
    }

    // Generate and store a new random subset for this user
    auto subset = GenerateRandomTitleSubset(mockInstalledTitles);
    userSpecificTitles[userId] = subset;
    return subset;
}

std::vector<pksm::titles::Title::Ref> MockTitleDataProvider::GetEmulatorTitles() const {
    return mockEmulatorTitles;
}

std::vector<pksm::titles::Title::Ref> MockTitleDataProvider::GetCustomTitles() const {
    return customTitleProvider->GetCustomTitles();
}