#include "utils/ItemSpriteManager.hpp"

namespace pksm::utils {

namespace {

// Sheet keys are (space << 12) | itemId; Gen 1/2/3 keep their own id spaces
u32 ItemKey(u16 itemId, ::pksm::Generation storageFormat) {
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
    return (space << 12) | itemId;
}

constexpr u16 MAX_ITEM_ID = 0xFFF;  // the key packing's id field
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

SpriteRef ItemSpriteManager::GetItemSprite(u16 itemId, ::pksm::Generation storageFormat) {
    if (itemId == 0) {
        return {};
    }
    if (itemId <= MAX_ITEM_ID) {
        if (auto ref = Sheet().Get(ItemKey(itemId, storageFormat))) {
            return ref;
        }
    }
    return Sheet().Get(UNKNOWN_ITEM_KEY);
}

void ItemSpriteManager::ClearCache() {
    Sheet().ReleaseSprites();
}

}  // namespace pksm::utils
