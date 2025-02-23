#include "data/providers/CustomTitleProvider.hpp"

namespace {
std::vector<pksm::titles::Title::Ref> createInitialTitles() {
    return {
        pksm::titles::Title::New("Pokémon Yellow", "romfs:/gfx/mock/emulator/yellow_menu_icon.jpg", 0x00164A00),
        pksm::titles::Title::New("Pokémon Crystal", "romfs:/gfx/mock/emulator/crystal_menu_icon.jpg", 0x00164B00),
        pksm::titles::Title::New("Pokémon Emerald", "romfs:/gfx/mock/emulator/emerald_menu_icon.jpg", 0x00164C00)
    };
}
}  // namespace

CustomTitleProvider::CustomTitleProvider() : customTitles(createInitialTitles()) {}

std::vector<pksm::titles::Title::Ref> CustomTitleProvider::GetCustomTitles() const {
    return customTitles;
}
