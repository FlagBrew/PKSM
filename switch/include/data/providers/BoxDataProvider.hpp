#pragma once

#include <memory>

#include "data/providers/SaveDataAccessor.hpp"
#include "data/providers/interfaces/IBoxDataProvider.hpp"
#include "data/providers/interfaces/IBoxNameEditor.hpp"
#include "data/providers/interfaces/IStorageHand.hpp"
#include "pkx/PKX.hpp"

// Reads and edits box contents on the accessor's single owned Sav.
// Display box 0 presents the party as a box; the save's own boxes follow,
// shifted up by one.
class BoxDataProvider : public IBoxDataProvider, public IStorageHand, public IBoxNameEditor {
private:
    SaveDataAccessor::Ref saveDataAccessor;

    // The Pokémon in hand during pick/place, with its origin for cancel
    std::unique_ptr<::pksm::PKX> heldPkm;
    pksm::ui::BoxPokemonData heldVisual;
    int heldOriginBox = -1;
    int heldOriginSlot = -1;
    bool heldSwapped = false;
    bool heldIsClone = false;
    // pksm::saves::ListRefAt token riding with the held Pokémon, -1 if none
    int heldListRef = -1;
    // fixParty is owed when the hand empties, never earlier (compaction would
    // shift the origin slot and, on LGPE, live party references)
    bool heldTouchedParty = false;

    // Non-null only when `saveData` is the save the accessor currently owns
    ::pksm::Sav* CurrentSav(const pksm::saves::SaveData::Ref& saveData) const;

    // Save-side slot for a display grid slot; -1 for slots with no backing storage
    int GridToSaveSlot(const ::pksm::Sav& sav, int boxIndex, int gridSlot) const;

    // Decrypted occupant of a grid slot; null when empty or out of range
    std::unique_ptr<::pksm::PKX> OccupantAt(::pksm::Sav& sav, int boxIndex, int gridSlot, int& saveSlot) const;

    // Counted from the slots; the stored party count goes stale mid-carry
    int LivePartyCount(::pksm::Sav& sav) const;

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
