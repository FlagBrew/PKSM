#pragma once

#include <memory>
#include <vector>

#include "data/saves/SaveData.hpp"
#include "data/summary/SummaryData.hpp"
#include "gui/shared/components/BoxPokemonData.hpp"

// Read model of a save's storage as display boxes: box 0 is the party
// (six usable slots, the rest unusable), higher boxes are the save's own
// boxes shifted by one. Editing goes through IStorageHand, box renames
// through IBoxNameEditor.
class IBoxDataProvider {
public:
    PU_SMART_CTOR(IBoxDataProvider)
    virtual ~IBoxDataProvider() = default;

    // Display box presenting the party; the save's own boxes start at 1
    static constexpr int PARTY_BOX_INDEX = 0;

    // Get the number of boxes available for a save
    virtual size_t GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const = 0;

    // Get the data for a specific box
    virtual pksm::ui::BoxData GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const = 0;

    // Display model of the slot's Pokémon for the summary view; empty for
    // empty and out-of-range slots
    virtual std::optional<pksm::summary::SummaryData>
    GetPokemonSummary(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const = 0;
};
