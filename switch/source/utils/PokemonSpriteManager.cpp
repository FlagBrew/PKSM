#include "utils/PokemonSpriteManager.hpp"

#include "utils/Logger.hpp"

namespace pksm::utils {

SpriteSheet& PokemonSpriteManager::Sheet() {
    // Deliberately leaked: a static's destructor would run after SDL
    // teardown and free the page textures through a dead renderer
    static SpriteSheet* sheet = new SpriteSheet();
    return *sheet;
}

bool PokemonSpriteManager::Initialize(const std::string& sheetPath) {
    return Sheet().Load(sheetPath);
}

SpriteRef PokemonSpriteManager::GetPokemonSprite(u16 species, u8 form, bool shiny) {
    if (species == 0) {
        return {};
    }

    if (auto ref = Sheet().Get(SpriteSheet::Key(species, form, shiny))) {
        return ref;
    }
    if (form > 0) {
        if (auto ref = Sheet().Get(SpriteSheet::Key(species, 0, shiny))) {
            return ref;
        }
    }
    if (shiny) {
        if (auto ref = Sheet().Get(SpriteSheet::Key(species, form, false))) {
            return ref;
        }
    }
    if (shiny && form > 0) {
        if (auto ref = Sheet().Get(SpriteSheet::Key(species, 0, false))) {
            return ref;
        }
    }

    LOG_ERROR(
        "Failed to find sprite for species " + std::to_string(species) + ", form " + std::to_string(form) + ", shiny " +
        std::to_string(shiny)
    );
    return {};
}

void PokemonSpriteManager::ClearCache() {
    Sheet().ReleaseSprites();
}

}  // namespace pksm::utils
