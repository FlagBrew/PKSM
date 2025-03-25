#pragma once

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "data/saves/SaveData.hpp"
#include "gui/shared/components/BoxPokemonData.hpp"

class MockBoxDataProvider : public IBoxDataProvider {
private:
    // Map to store box data for different save data
    // Key is a hash of save data info, value is the box data
    mutable std::map<std::string, std::vector<pksm::ui::BoxData>> boxDataCache;

    // Generate a hash key for a save data
    std::string GenerateSaveKey(const pksm::saves::SaveData::Ref& saveData) const;

    // Create a seeded random number generator based on save data
    std::mt19937 CreateSeededRNG(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const;

    // Generate mock data for a box
    pksm::ui::BoxData GenerateBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const;

public:
    MockBoxDataProvider();
    virtual ~MockBoxDataProvider() = default;

    // IBoxDataProvider implementation
    size_t GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const override;

    pksm::ui::BoxData GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const override;

    bool SetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const pksm::ui::BoxData& boxData) override;

    bool SetPokemonData(
        const pksm::saves::SaveData::Ref& saveData,
        int boxIndex,
        int slotIndex,
        const pksm::ui::BoxPokemonData& pokemonData
    ) override;
};