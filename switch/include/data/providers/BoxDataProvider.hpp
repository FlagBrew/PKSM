#pragma once

#include <memory>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "data/providers/interfaces/IBoxNameEditor.hpp"
#include "data/providers/interfaces/IStorageHand.hpp"
#include "pkx/PKX.hpp"

// Reads and edits box contents on the accessor's single owned Sav. Mutations
// stay in memory until the accessor's saveChanges writes them back.
// Display box 0 presents the party as a box; the save's own boxes follow,
// shifted up by one.
class BoxDataProvider : public IBoxDataProvider, public IStorageHand, public IBoxNameEditor {
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
    // The carry emptied or filled party slots; fixParty is owed when the
    // hand empties (never earlier - the compaction would shift the origin
    // slot and, on LGPE, the party references while heldListRef is live)
    bool heldTouchedParty = false;

    // Non-null only when `saveData` is the save the accessor currently owns
    ::pksm::Sav* CurrentSav(const pksm::saves::SaveData::Ref& saveData) const;

    // Save-side slot for a grid slot of a display box: a party slot for the
    // party box, a box-file slot otherwise; -1 for slots with no backing
    // storage (party slots past six, padding columns of 20-slot saves,
    // slots past the save's last box entry)
    int GridToSaveSlot(const ::pksm::Sav& sav, int boxIndex, int gridSlot) const;

    // Decrypted occupant of a display box's grid slot, with its save-side
    // slot through saveSlot; null when the slot is out of range (saveSlot
    // -1) or empty
    std::unique_ptr<::pksm::PKX> OccupantAt(::pksm::Sav& sav, int boxIndex, int gridSlot, int& saveSlot) const;

    // Occupied party slots counted live from the slots themselves; the
    // save's stored party count goes stale while a carry holds a hole open
    int LivePartyCount(::pksm::Sav& sav) const;

    // Whether removing the occupant of this display slot would leave the
    // party without a member: it is a party slot, or a box slot the party
    // references, and no other member remains
    bool WouldEmptyParty(::pksm::Sav& sav, int boxIndex, int saveSlot) const;

    // Empty the hand, settling any deferred party compaction first
    void ClearHand(::pksm::Sav& sav);

    static bool IsPartyBox(int boxIndex) { return boxIndex == PARTY_BOX_INDEX; }

public:
    explicit BoxDataProvider(SaveDataAccessor::Ref saveDataAccessor);
    PU_SMART_CTOR(BoxDataProvider)

    // IBoxDataProvider implementation
    size_t GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const override;
    pksm::ui::BoxData GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const override;
    std::optional<pksm::summary::SummaryData>
    GetPokemonSummary(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const override;

    // IStorageHand implementation
    bool PickUpPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool PlaceDownPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool CancelHold(const pksm::saves::SaveData::Ref& saveData) override;
    bool ClonePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool ReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) override;
    bool ReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) override;
    bool CanReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const override;
    bool CanReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) const override;
    bool HasHeldPokemon() const override { return heldPkm != nullptr; }
    pksm::ui::BoxPokemonData GetHeldPokemon() const override { return heldVisual; }
    int GetHeldOriginBox() const override { return heldOriginBox; }
    bool IsHeldPokemonClone() const override { return heldPkm != nullptr && heldIsClone; }

    // IBoxNameEditor implementation
    bool CanRenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const override;
    size_t GetBoxNameMaxLength(const pksm::saves::SaveData::Ref& saveData) const override;
    std::optional<std::string>
    FirstUnstorableBoxNameChar(const pksm::saves::SaveData::Ref& saveData, const std::string& name) const override;
    bool RenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const std::string& name) override;
};
