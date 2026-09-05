#pragma once

#include <string>

#include "enums/Generation.hpp"
#include "utils/SpriteSheet.hpp"

namespace pksm::utils {

class ItemSpriteManager {
public:
    // Load the PKSS item sheet; item sprites cannot render without it
    static bool Initialize(const std::string& sheetPath);

    // Sheet key for an item; itemId is format-native like strings::ItemName
    static u32 ItemKey(u16 itemId, ::pksm::Generation storageFormat);

    // Sheet key for a Legends Z-A donut: its taste comes from the id, its look from the stars
    static u32 DonutKey(u16 donutId, u8 stars);

    // Sprite for a key from above; art the sheet lacks gets the placeholder
    static SpriteRef GetSprite(u32 key);

    // Drop the warm sprite textures (session teardown)
    static void ClearCache();

private:
    static SpriteSheet& Sheet();
};

}  // namespace pksm::utils
