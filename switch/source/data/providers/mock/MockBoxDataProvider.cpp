#include "data/providers/mock/MockBoxDataProvider.hpp"

#include <functional>  // For std::hash
#include <sstream>  // For std::stringstream

#include "utils/Logger.hpp"

MockBoxDataProvider::MockBoxDataProvider() {
    pksm::utils::Logger::Debug("Initializing MockBoxDataProvider...");
}

std::string MockBoxDataProvider::GenerateSaveKey(const pksm::saves::SaveData::Ref& saveData) const {
    // Create a unique key based on save data properties
    std::stringstream ss;

    // Combine trainer name, TID, SID, and game version to create a unique identifier
    ss << saveData->getOTName() << "_" << saveData->getTID() << "_" << saveData->getSID() << "_"
       << static_cast<int>(saveData->getVersion());

    return ss.str();
}

std::mt19937 MockBoxDataProvider::CreateSeededRNG(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const {
    // Generate a base key from the save
    std::string baseKey = GenerateSaveKey(saveData);

    // Append box index to make it unique per box
    std::string seedStr = baseKey + "_box_" + std::to_string(boxIndex);

    // Use std::hash to create a seed from the combined string
    std::hash<std::string> hasher;
    std::size_t hashValue = hasher(seedStr);
    std::seed_seq seed{static_cast<unsigned int>(hashValue)};

    // Create and return a seeded random number generator
    return std::mt19937(seed);
}

size_t MockBoxDataProvider::GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const {
    // Different games have different box counts
    switch (saveData->getVersion()) {
        case pksm::saves::GameVersion::RD:
        case pksm::saves::GameVersion::GN:
        case pksm::saves::GameVersion::BU:
        case pksm::saves::GameVersion::YW:
            return 12;  // Gen 1

        case pksm::saves::GameVersion::GD:
        case pksm::saves::GameVersion::SV:
        case pksm::saves::GameVersion::C:
            return 14;  // Gen 2

        case pksm::saves::GameVersion::R:
        case pksm::saves::GameVersion::S:
        case pksm::saves::GameVersion::E:
        case pksm::saves::GameVersion::FR:
        case pksm::saves::GameVersion::LG:
            return 14;  // Gen 3

        case pksm::saves::GameVersion::D:
        case pksm::saves::GameVersion::P:
        case pksm::saves::GameVersion::Pt:
        case pksm::saves::GameVersion::HG:
        case pksm::saves::GameVersion::SS:
            return 18;  // Gen 4

        case pksm::saves::GameVersion::B:
        case pksm::saves::GameVersion::W:
        case pksm::saves::GameVersion::B2:
        case pksm::saves::GameVersion::W2:
            return 24;  // Gen 5

        case pksm::saves::GameVersion::X:
        case pksm::saves::GameVersion::Y:
        case pksm::saves::GameVersion::OR:
        case pksm::saves::GameVersion::AS:
            return 31;  // Gen 6

        case pksm::saves::GameVersion::SN:
        case pksm::saves::GameVersion::MN:
        case pksm::saves::GameVersion::US:
        case pksm::saves::GameVersion::UM:
            return 32;  // Gen 7

        case pksm::saves::GameVersion::GP:
        case pksm::saves::GameVersion::GE:
            return 34;  // LGPE

        case pksm::saves::GameVersion::SW:
        case pksm::saves::GameVersion::SH:
        case pksm::saves::GameVersion::BD:
        case pksm::saves::GameVersion::SP:
        case pksm::saves::GameVersion::PLA:
            return 32;  // Gen 8

        case pksm::saves::GameVersion::SL:
        case pksm::saves::GameVersion::VL:
            return 32;  // Gen 9

        default:
            return 30;  // Default value
    }
}

pksm::ui::BoxData MockBoxDataProvider::GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const {
    // Check if we have this save's boxes cached already
    std::string saveKey = GenerateSaveKey(saveData);

    // If we already have the box data in cache, return it
    if (boxDataCache.find(saveKey) != boxDataCache.end()) {
        auto& boxes = boxDataCache[saveKey];

        // If the requested box exists in the cache, return it
        if (boxIndex < static_cast<int>(boxes.size())) {
            return boxes[boxIndex];
        }
    }

    // Generate box data if it wasn't in the cache
    pksm::ui::BoxData boxData = GenerateBoxData(saveData, boxIndex);

    // Create or update the cache
    if (boxDataCache.find(saveKey) == boxDataCache.end()) {
        // Create a new entry
        boxDataCache[saveKey] = std::vector<pksm::ui::BoxData>();
    }

    // Ensure we have enough space for this box
    if (boxIndex >= static_cast<int>(boxDataCache[saveKey].size())) {
        boxDataCache[saveKey].resize(boxIndex + 1);
    }

    // Store the data in the cache
    boxDataCache[saveKey][boxIndex] = boxData;

    return boxData;
}

pksm::ui::BoxData MockBoxDataProvider::GenerateBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const {
    // Box size is always 30 (6x5)
    const int SLOTS_PER_BOX = 30;

    // Create a seeded RNG for this box
    std::mt19937 seededRng = CreateSeededRNG(saveData, boxIndex);

    // Generate box name - with 8% chance of custom name
    std::string boxName;

    // Distribution for determining if we use a custom name (8% chance)
    std::uniform_int_distribution<int> customNameDist(1, 100);
    // Distribution for selecting from our custom names
    std::uniform_int_distribution<int> nameSelectionDist(0, 13);

    // List of fun/plausible custom box names
    const std::vector<std::string> customNames = {
        "Favorites",
        "Shinies",
        "Breeding",
        "Legendaries",
        "Teams",
        "Trade",
        "Starters",
        "Contest",
        "Battlers",
        "Rare Finds",
        "Dreams",
        "Mythicals",
        "Ultra-Beasts",
        "Marked shinies"
    };

    // 8% chance to use a custom name
    if (customNameDist(seededRng) <= 8) {
        boxName = customNames[nameSelectionDist(seededRng)];
    } else {
        // Default naming scheme
        boxName = "Box " + std::to_string(boxIndex + 1);
    }

    // Create the BoxData with the generated name
    pksm::ui::BoxData boxData(boxName);

    // Determine the generation for species range
    // Default max species based on generation
    int maxSpecies;
    switch (saveData->getGeneration()) {
        case pksm::saves::Generation::ONE:
            maxSpecies = 151;  // Gen 1 max species (Mew)
            break;
        case pksm::saves::Generation::TWO:
            maxSpecies = 251;  // Gen 2 max species (Celebi)
            break;
        case pksm::saves::Generation::THREE:
            maxSpecies = 386;  // Gen 3 max species (Deoxys)
            break;
        case pksm::saves::Generation::FOUR:
            maxSpecies = 493;  // Gen 4 max species (Arceus)
            break;
        case pksm::saves::Generation::FIVE:
            maxSpecies = 649;  // Gen 5 max species (Genesect)
            break;
        case pksm::saves::Generation::SIX:
            maxSpecies = 721;  // Gen 6 max species (Volcanion)
            break;
        case pksm::saves::Generation::SEVEN:
            maxSpecies = 809;  // Gen 7 max species (Melmetal)
            break;
        case pksm::saves::Generation::LGPE:
            maxSpecies = 151;  // LGPE only contains Gen 1 Pokémon (plus Meltan/Melmetal but we'll default to 151)
            break;
        case pksm::saves::Generation::EIGHT:
            maxSpecies = 898;  // Gen 8 max species (Calyrex)
            break;
        case pksm::saves::Generation::NINE:
            maxSpecies = 1025;  // Gen 9 max species (Pecharunt)
            break;
        default:
            maxSpecies = 1025;  // Default to all species
    }

    // Distribution for generating random values
    std::uniform_int_distribution<int> emptyDist(1, 100);  // For determining if slot is empty
    std::uniform_int_distribution<u16> speciesDist(1, maxSpecies);  // Random species
    std::uniform_int_distribution<int> shinyDist(1, 100);  // For determining if Pokémon is shiny

    // Resize the box to hold the correct number of slots
    boxData.resize(SLOTS_PER_BOX);

    // Fill each box with Pokémon data
    for (int slot = 0; slot < SLOTS_PER_BOX; slot++) {
        // 20% chance of empty slot
        if (emptyDist(seededRng) <= 20) {
            boxData[slot] = pksm::ui::BoxPokemonData();
        } else {
            // Random species within the generation's range
            u16 species = speciesDist(seededRng);

            // 10% chance of shiny
            bool shiny = (shinyDist(seededRng) <= 10);

            // Use form 0 to avoid issues with missing forms
            boxData[slot] = pksm::ui::BoxPokemonData(species, 0, shiny);
        }
    }

    return boxData;
}

bool MockBoxDataProvider::SetBoxData(
    const pksm::saves::SaveData::Ref& saveData,
    int boxIndex,
    const pksm::ui::BoxData& boxData
) {
    // Verify the save data is valid
    if (!saveData) {
        pksm::utils::Logger::Error("SetBoxData: Invalid save data");
        return false;
    }

    // Generate save key
    std::string saveKey = GenerateSaveKey(saveData);

    // Create cache entry if it doesn't exist
    if (boxDataCache.find(saveKey) == boxDataCache.end()) {
        boxDataCache[saveKey] = std::vector<pksm::ui::BoxData>(GetBoxCount(saveData));
    }

    // Verify box index is valid
    if (boxIndex < 0 || boxIndex >= static_cast<int>(boxDataCache[saveKey].size())) {
        pksm::utils::Logger::Error("SetBoxData: Invalid box index: " + std::to_string(boxIndex));
        return false;
    }

    // Update the cache
    boxDataCache[saveKey][boxIndex] = boxData;
    return true;
}

bool MockBoxDataProvider::SetPokemonData(
    const pksm::saves::SaveData::Ref& saveData,
    int boxIndex,
    int slotIndex,
    const pksm::ui::BoxPokemonData& pokemonData
) {
    // Verify the save data is valid
    if (!saveData) {
        pksm::utils::Logger::Error("SetPokemonData: Invalid save data");
        return false;
    }

    // Get current box data (will generate it if not in cache)
    pksm::ui::BoxData boxData = GetBoxData(saveData, boxIndex);

    // Verify slot index is valid
    if (slotIndex < 0 || slotIndex >= static_cast<int>(boxData.size())) {
        pksm::utils::Logger::Error("SetPokemonData: Invalid slot index: " + std::to_string(slotIndex));
        return false;
    }

    // Update the Pokémon data
    boxData[slotIndex] = pokemonData;

    // Update the box data
    return SetBoxData(saveData, boxIndex, boxData);
}