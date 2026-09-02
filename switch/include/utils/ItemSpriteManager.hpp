#pragma once

#include <string>

#include "enums/Generation.hpp"
#include "utils/SpriteSheet.hpp"

namespace pksm::utils {

class ItemSpriteManager {
public:
    // Load the PKSS item sheet; item sprites cannot render without it
    static bool Initialize(const std::string& sheetPath);

    // itemId is format-native like strings::ItemName; items without art get the placeholder
    static SpriteRef GetItemSprite(u16 itemId, ::pksm::Generation storageFormat);

    // Drop the warm sprite textures (session teardown)
    static void ClearCache();

private:
    static SpriteSheet& Sheet();
};

}  // namespace pksm::utils
