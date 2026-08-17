#pragma once

#include <memory>
#include <vector>

#include "data/saves/SaveData.hpp"
#include "data/summary/SummaryData.hpp"
#include "gui/shared/components/BoxPokemonData.hpp"

class IBoxDataProvider {
public:
    PU_SMART_CTOR(IBoxDataProvider)
    virtual ~IBoxDataProvider() = default;

    // Get the number of boxes available for a save
    virtual size_t GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const = 0;

    // Get the data for a specific box
    virtual pksm::ui::BoxData GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const = 0;

    // Set data for a specific box (returns success/failure)
    virtual bool
    SetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const pksm::ui::BoxData& boxData) = 0;

    // Set data for a specific Pokémon in a box (returns success/failure)
    virtual bool SetPokemonData(
        const pksm::saves::SaveData::Ref& saveData,
        int boxIndex,
        int slotIndex,
        const pksm::ui::BoxPokemonData& pokemonData
    ) = 0;

    // Pick up / place / put back for box editing. Grid slot indices; the held
    // Pokémon lives with the provider so the UI only ever sees visuals.
    virtual bool PickUpPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Place the held Pokémon; an occupied target slot swaps into the hand
    virtual bool PlaceDownPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Return the held Pokémon to its origin slot; a cloned hand is discarded
    virtual bool CancelHold(const pksm::saves::SaveData::Ref& saveData) = 0;

    // Take a copy of the slot's Pokémon into the empty hand; the slot keeps
    // the original
    virtual bool ClonePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Remove the slot's Pokémon from the save; hand must be empty
    virtual bool ReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Remove the held Pokémon from the save instead of placing it back
    virtual bool ReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) = 0;

    virtual bool HasHeldPokemon() const = 0;
    virtual pksm::ui::BoxPokemonData GetHeldPokemon() const = 0;

    // Box the current carry started from; -1 when nothing is held
    virtual int GetHeldOriginBox() const = 0;

    // Whether the hand holds a copy whose original is still in its slot
    virtual bool IsHeldPokemonClone() const = 0;

    // Display model of the slot's Pokémon for the summary view; empty for
    // empty and out-of-range slots
    virtual std::optional<pksm::summary::SummaryData>
    GetPokemonSummary(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const = 0;
};