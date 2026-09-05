#include "utils/ItemSpriteManager.hpp"

#include <algorithm>

namespace pksm::utils {

namespace {

// Sheet keys are (space << 12) | id; Gen 1/2/3 keep their own id spaces, 3 is national
constexpr u32 SpaceKey(u32 space, u32 id) {
    return (space << 12) | id;
}

constexpr u32 DONUT_SPACE = 4;
constexpr u16 MAX_ITEM_ID = 0xFFF;  // the key packing's id field
constexpr u32 NO_SPRITE_KEY = 0xFFFFFFFE;  // an empty slot draws nothing
constexpr u32 UNKNOWN_ITEM_KEY = 0xFFFFFFFF;

}  // namespace

SpriteSheet& ItemSpriteManager::Sheet() {
    // Deliberately leaked for the same reason as PokemonSpriteManager's sheet
    static SpriteSheet* sheet = new SpriteSheet();
    return *sheet;
}

bool ItemSpriteManager::Initialize(const std::string& sheetPath) {
    // 32px icons draw at exactly 2x in the bag
    Sheet().SetScaleMode(SDL_ScaleModeNearest);
    return Sheet().Load(sheetPath);
}

u32 ItemSpriteManager::ItemKey(u16 itemId, ::pksm::Generation storageFormat) {
    if (itemId == 0) {
        return NO_SPRITE_KEY;
    }
    if (itemId > MAX_ITEM_ID) {
        return UNKNOWN_ITEM_KEY;
    }
    u32 space = 3;
    switch (storageFormat) {
        case ::pksm::Generation::ONE:
            space = 0;
            break;
        case ::pksm::Generation::TWO:
            space = 1;
            break;
        case ::pksm::Generation::THREE:
            space = 2;
            break;
        default:
            break;
    }
    return SpaceKey(space, itemId);
}

u32 ItemSpriteManager::DonutKey(u16 donutId, u8 stars) {
    // Ids 0-197 cycle through the six tastes (sweet, spicy, sour, bitter, fresh, mix) and
    // draw with 0-5 stars: sheet ids 0-35. The five legendary donuts, 198-202, follow as 36-40.
    constexpr u16 FIRST_SPECIAL = 198;
    constexpr u16 LAST_SPECIAL = 202;
    constexpr u32 SPECIAL_BASE = 36;
    if (donutId > LAST_SPECIAL) {
        return UNKNOWN_ITEM_KEY;  // keeps a garbage id inside the packing's id field
    }
    if (donutId >= FIRST_SPECIAL) {
        return SpaceKey(DONUT_SPACE, SPECIAL_BASE + (donutId - FIRST_SPECIAL));
    }
    return SpaceKey(DONUT_SPACE, (donutId % 6) * 6 + std::min<u8>(stars, 5));
}

SpriteRef ItemSpriteManager::GetSprite(u32 key) {
    if (key == NO_SPRITE_KEY) {
        return {};
    }
    if (auto ref = Sheet().Get(key)) {
        return ref;
    }
    return Sheet().Get(UNKNOWN_ITEM_KEY);
}

void ItemSpriteManager::ClearCache() {
    Sheet().ReleaseSprites();
}

}  // namespace pksm::utils
