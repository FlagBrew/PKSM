#pragma once

#include "data/saves/SaveData.hpp"
#include "gui/shared/components/BoxPokemonData.hpp"

// The single hand that edits storage; the held Pokémon lives provider-side.
// Box indices are display indices (box 0 = party); slot verbs need the hand empty, hand verbs full.
class IStorageHand {
public:
    PU_SMART_CTOR(IStorageHand)
    virtual ~IStorageHand() = default;

    virtual bool PickUpPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Occupied targets swap into the hand; refused if it would leave the party empty
    virtual bool PlaceDownPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Return the held Pokémon to its origin slot; a cloned hand is discarded
    virtual bool CancelHold(const pksm::saves::SaveData::Ref& saveData) = 0;

    virtual bool ClonePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    virtual bool ReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    virtual bool ReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) = 0;

    // False when the Pokémon is the party's last member
    virtual bool CanReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const = 0;
    virtual bool CanReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) const = 0;

    virtual bool HasHeldPokemon() const = 0;
    virtual pksm::ui::BoxPokemonData GetHeldPokemon() const = 0;

    // Box the current carry started from; -1 when nothing is held
    virtual int GetHeldOriginBox() const = 0;

    virtual bool IsHeldPokemonClone() const = 0;
};
