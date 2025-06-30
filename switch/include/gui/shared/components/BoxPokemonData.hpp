#pragma once

#include <pu/Plutonium>
#include <string>
#include <vector>

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

// Structure to encapsulate box data
struct BoxData {
    std::vector<BoxPokemonData> pokemon;  // The Pokémon in the box
    std::string name;  // Name of the box

    // Constructor
    BoxData(const std::string& name = "Box") : name(name) {}

    // Resize the box to hold a specific number of Pokémon
    void resize(size_t size, const BoxPokemonData& defaultData = BoxPokemonData()) {
        pokemon.resize(size, defaultData);
    }

    // Get the size of the box
    size_t size() const { return pokemon.size(); }

    // Access operators
    BoxPokemonData& operator[](size_t index) { return pokemon[index]; }

    const BoxPokemonData& operator[](size_t index) const { return pokemon[index]; }
};

}  // namespace pksm::ui