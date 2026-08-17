#pragma once

#include "data/saves/SaveData.hpp"
#include "gui/shared/components/BoxPokemonData.hpp"

// The single hand that edits storage: pick up, place, clone and release,
// with the held Pokémon living provider-side so the UI only ever sees
// visuals. Box indices are display indices - box 0 is the party, higher
// boxes are the save's own. All verbs are gated on the hand's state (slot
// verbs need it empty, hand verbs need it full).
class IStorageHand {
public:
    PU_SMART_CTOR(IStorageHand)
    virtual ~IStorageHand() = default;

    // Take the slot's Pokémon into the empty hand, leaving the slot empty
    virtual bool PickUpPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) = 0;

    // Place the held Pokémon; an occupied target slot swaps into the hand.
    // Refused when emptying the hand would leave the party empty.
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

    // Whether the release verbs would succeed, so the UI can gate hints and
    // confirmation prompts; false when the Pokémon is the party's last member
    virtual bool CanReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const = 0;
    virtual bool CanReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) const = 0;

    virtual bool HasHeldPokemon() const = 0;
    virtual pksm::ui::BoxPokemonData GetHeldPokemon() const = 0;

    // Box the current carry started from; -1 when nothing is held
    virtual int GetHeldOriginBox() const = 0;

    // Whether the hand holds a copy whose original is still in its slot
    virtual bool IsHeldPokemonClone() const = 0;
};
