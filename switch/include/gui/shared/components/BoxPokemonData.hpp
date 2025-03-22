#pragma once

#include <pu/Plutonium>

#include "utils/PokemonSpriteManager.hpp"

namespace pksm::ui {

// Structure to represent a Pokémon for mock data purposes
// This will be replaced with proper PKX classes when PKSM-core is integrated
struct BoxPokemonData {
    u16 species;  // Pokémon species ID
    u8 form;  // Form number (0 for normal form)
    bool shiny;  // Whether the Pokémon is shiny

    // Constructor for easy creation
    BoxPokemonData(u16 species = 0, u8 form = 0, bool shiny = false) : species(species), form(form), shiny(shiny) {}

    // Helper to determine if this is an empty slot
    bool isEmpty() const { return species == 0; }

    // Helper to get the texture handle for the sprite
    pu::sdl2::TextureHandle::Ref getSprite() const {
        if (isEmpty()) {
            return nullptr;
        }

        // Get the sprite from the sprite manager
        return utils::PokemonSpriteManager::GetPokemonSprite(species, form, shiny);
    }
};

}  // namespace pksm::ui