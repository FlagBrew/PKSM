#pragma once

#include <string>

#include "utils/SpriteSheet.hpp"

namespace pksm::utils {

class PokemonSpriteManager {
public:
    // Load the PKSS spritesheet; sprites cannot render without it
    static bool Initialize(const std::string& sheetPath);

    // Sprite for a specific Pokemon, falling back form -> 0 and shiny ->
    // regular when the exact variant has no art
    static SpriteRef GetPokemonSprite(u16 species, u8 form = 0, bool shiny = false);

    // Drop the warm sprite textures (session teardown)
    static void ClearCache();

private:
    static SpriteSheet& Sheet();
};

}  // namespace pksm::utils
