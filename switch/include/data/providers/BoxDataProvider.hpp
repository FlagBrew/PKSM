#pragma once

#include <memory>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "pkx/PKX.hpp"

// Reads and edits box contents on the accessor's single owned Sav. Mutations
// stay in memory until the accessor's saveChanges writes them back.
class BoxDataProvider : public IBoxDataProvider {
private:
    SaveDataAccessor::Ref saveDataAccessor;

    // The Pokémon currently in hand during pick/place, with its origin so a
    // cancel can put it back
    std::unique_ptr<::pksm::PKX> heldPkm;
    pksm::ui::BoxPokemonData heldVisual;
    int heldOriginBox = -1;
    int heldOriginSlot = -1;
    bool heldSwapped = false;
    // A cloned hand left the original in its slot, so nothing is owed back
    // to the origin on cancel
    bool heldIsClone = false;
    // External reference into the box list attached to the held Pokémon
    // (pksm::saves::ListRefAt token, -1 if none); moved to wherever the
    // hand finally empties
    int heldListRef = -1;

    // Non-null only when `saveData` is the save the accessor currently owns
    ::pksm::Sav* CurrentSav(const pksm::saves::SaveData::Ref& saveData) const;

    // Save-file slot for a grid slot; -1 for the padding columns of 20-slot
    // saves and for slots past the save's last box entry
    int GridToSaveSlot(const ::pksm::Sav& sav, int boxIndex, int gridSlot) const;

    // Decrypted occupant of a grid slot, with its save-file slot through
    // saveSlot; null when the slot is out of range (saveSlot -1) or empty
    std::unique_ptr<::pksm::PKX> OccupantAt(::pksm::Sav& sav, int boxIndex, int gridSlot, int& saveSlot) const;

public:
    explicit BoxDataProvider(SaveDataAccessor::Ref saveDataAccessor);
    PU_SMART_CTOR(BoxDataProvider)

    // IBoxDataProvider implementation
    size_t GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const override;
    pksm::ui::BoxData GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const override;
    bool SetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const pksm::ui::BoxData& boxData)
        override;
    bool SetPokemonData(
        const pksm::saves::SaveData::Ref& saveData,
        int boxIndex,
        int slotIndex,
        const pksm::ui::BoxPokemonData& pokemonData
    ) override;
    bool PickUpPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool PlaceDownPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool CancelHold(const pksm::saves::SaveData::Ref& saveData) override;
    bool ClonePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool ReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool ReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) override;
    bool HasHeldPokemon() const override { return heldPkm != nullptr; }
    pksm::ui::BoxPokemonData GetHeldPokemon() const override { return heldVisual; }
    int GetHeldOriginBox() const override { return heldOriginBox; }
    bool IsHeldPokemonClone() const override { return heldPkm != nullptr && heldIsClone; }
};
