#include "data/providers/CustomTitleProvider.hpp"

namespace {
std::vector<pksm::titles::Title::Ref> createInitialTitles() {
    // The old hardcoded GB/GBA entries moved to the emulator catalog
    // (romfs:/gfx/data/data.json); custom titles are a future concept
    return {};
}
}  // namespace

CustomTitleProvider::CustomTitleProvider() : customTitles(createInitialTitles()) {}

std::vector<pksm::titles::Title::Ref> CustomTitleProvider::GetCustomTitles() const {
    return customTitles;
}
