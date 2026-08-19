#pragma once

#include <pu/Plutonium>
#include <string>
#include <vector>

#include "enums/Gender.hpp"
#include "utils/PokemonSpriteManager.hpp"

namespace pksm::ui {

// Structure to represent a Pokémon for mock data purposes
// This will be replaced with proper PKX classes when PKSM-core is integrated
struct BoxPokemonData {
    u16 species;  // Pokémon species ID
    u8 form;  // Form number (0 for normal form)
    bool shiny;  // Whether the Pokémon is shiny
    pksm::Gender gender;  // Pokémon gender
    // No backing storage in the save (padding columns, partial last box)
    bool unusable = false;
    // 1-based party membership badge; 0 for non-members
    u8 partyNumber = 0;

    // Constructor for easy creation
    BoxPokemonData(
        u16 species = 0,
        u8 form = 0,
        bool shiny = false,
        pksm::Gender gender = pksm::Gender{pksm::Gender::Genderless}
    )
      : species(species), form(form), shiny(shiny), gender(gender) {}

    // Helper to determine if this is an empty slot
    bool isEmpty() const { return species == 0; }

    // Helper to get the sheet sprite for this Pokemon
    utils::SpriteRef getSprite() const {
        if (isEmpty()) {
            return {};
        }
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