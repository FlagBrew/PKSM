#include "data/providers/BoxDataProvider.hpp"

#include <algorithm>
#include <cctype>
#include <string>

#include "data/saves/BoxListRefs.hpp"
#include "data/saves/BoxNameRules.hpp"
#include "data/summary/SummaryBuilder.hpp"
#include "utils/Logger.hpp"

namespace {

// The UI grid is always 6x5
constexpr int GRID_SLOTS = 30;

// The party box exposes the six party slots as its first grid row
constexpr int PARTY_SLOTS = 6;

// International Gen 1/2 saves hold 20 Pokémon per box; the games show them
// 4 wide, so keep the outer grid columns empty and center each row
bool SaveUsesTwentySlotBoxes(const ::pksm::Sav& sav) {
    const int boxes = sav.maxBoxes();
    return boxes > 0 && (sav.maxSlot() / boxes) == 20;
}

int ToGridSlot(int saveSlot) {
    return (saveSlot / 4) * 6 + 1 + (saveSlot % 4);
}

// Stored box names may be NUL-padded; strip that before display or judging
// blankness. A blank name (empty or whitespace-only) has no visible content.
std::string VisibleName(std::string name) {
    name.erase(std::remove(name.begin(), name.end(), '\0'), name.end());
    return name;
}

bool IsBlankName(const std::string& name) {
    return std::all_of(name.begin(), name.end(), [](unsigned char c) { return std::isspace(c) != 0; });
}

pksm::ui::BoxPokemonData VisualFromPkx(const ::pksm::PKX& pk) {
    const u16 form = pk.alternativeForm();
    return pksm::ui::BoxPokemonData(
        static_cast<u16>(pk.species()),
        form > 0xFF ? 0 : static_cast<u8>(form),
        pk.shiny(),
        pk.gender()
    );
}

}  // namespace

BoxDataProvider::BoxDataProvider(SaveDataAccessor::Ref saveDataAccessor)
  : saveDataAccessor(std::move(saveDataAccessor)) {}

::pksm::Sav* BoxDataProvider::CurrentSav(const pksm::saves::SaveData::Ref& saveData) const {
    if (!saveData || saveData != saveDataAccessor->getCurrentSaveData()) {
        return nullptr;
    }
    return saveDataAccessor->currentSav();
}

std::unique_ptr<::pksm::PKX>
BoxDataProvider::OccupantAt(::pksm::Sav& sav, int boxIndex, int gridSlot, int& saveSlot) const {
    saveSlot = GridToSaveSlot(sav, boxIndex, gridSlot);
    if (saveSlot < 0) {
        return nullptr;
    }
    auto pk = IsPartyBox(boxIndex) ? sav.pkm(static_cast<u8>(saveSlot))
                                   : sav.pkm(static_cast<u8>(boxIndex - 1), static_cast<u8>(saveSlot));
    if (!pk) {
        return nullptr;
    }
    if (pk->isEncrypted()) {
        pk->decrypt();
    }
    if (static_cast<u16>(pk->species()) == 0) {
        return nullptr;
    }
    return pk;
}

int BoxDataProvider::GridToSaveSlot(const ::pksm::Sav& sav, int boxIndex, int gridSlot) const {
    if (gridSlot < 0 || gridSlot >= GRID_SLOTS) {
        return -1;
    }
    if (IsPartyBox(boxIndex)) {
        return gridSlot < PARTY_SLOTS ? gridSlot : -1;
    }
    const int box = boxIndex - 1;
    if (box < 0 || box >= sav.maxBoxes()) {
        return -1;
    }
    const bool twentySlot = SaveUsesTwentySlotBoxes(sav);
    int saveSlot = gridSlot;
    if (twentySlot) {
        const int col = gridSlot % 6;
        if (col == 0 || col == 5) {
            return -1;
        }
        saveSlot = (gridSlot / 6) * 4 + (col - 1);
    }
    const int slotsPerBox = twentySlot ? 20 : GRID_SLOTS;
    if (box * slotsPerBox + saveSlot >= sav.maxSlot()) {
        return -1;
    }
    return saveSlot;
}

int BoxDataProvider::LivePartyCount(::pksm::Sav& sav) const {
    int count = 0;
    for (int slot = 0; slot < PARTY_SLOTS; slot++) {
        auto pk = sav.pkm(static_cast<u8>(slot));
        if (!pk) {
            continue;
        }
        if (pk->isEncrypted()) {
            pk->decrypt();
        }
        if (static_cast<u16>(pk->species()) != 0) {
            count++;
        }
    }
    return count;
}

bool BoxDataProvider::WouldEmptyParty(::pksm::Sav& sav, int boxIndex, int saveSlot) const {
    const bool partyMember = IsPartyBox(boxIndex) ||
        pksm::saves::ListRefAt(sav, (boxIndex - 1) * GRID_SLOTS + saveSlot) >= 0;
    return partyMember && LivePartyCount(sav) <= 1;
}

void BoxDataProvider::ClearHand(::pksm::Sav& sav) {
    if (heldTouchedParty) {
        // Settle the deferred repair: close the party holes the carry left
        // and refresh the stored party count. Safe only here - no reference
        // token is live once the hand empties.
        sav.fixParty();
    }
    heldPkm.reset();
    heldVisual = pksm::ui::BoxPokemonData();
    heldOriginBox = -1;
    heldOriginSlot = -1;
    heldSwapped = false;
    heldIsClone = false;
    heldListRef = -1;
    heldTouchedParty = false;
}

size_t BoxDataProvider::GetBoxCount(const pksm::saves::SaveData::Ref& saveData) const {
    auto* sav = CurrentSav(saveData);
    return sav ? static_cast<size_t>(std::max(0, sav->maxBoxes())) + 1 : 0;
}

pksm::ui::BoxData BoxDataProvider::GetBoxData(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const {
    auto* sav = CurrentSav(saveData);

    if (IsPartyBox(boxIndex)) {
        pksm::ui::BoxData boxData("Party");
        boxData.resize(GRID_SLOTS);
        for (int gridSlot = PARTY_SLOTS; gridSlot < GRID_SLOTS; gridSlot++) {
            boxData[gridSlot].unusable = true;
        }
        if (!sav) {
            return boxData;
        }
        try {
            for (int slot = 0; slot < PARTY_SLOTS; slot++) {
                auto pk = sav->pkm(static_cast<u8>(slot));
                if (!pk) {
                    continue;
                }
                if (pk->isEncrypted()) {
                    pk->decrypt();
                }
                if (static_cast<u16>(pk->species()) == 0) {
                    continue;
                }
                boxData[slot] = VisualFromPkx(*pk);
                boxData[slot].partyNumber = static_cast<u8>(slot + 1);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Core threw while reading the party: " + std::string(e.what()));
        }
        return boxData;
    }

    const int box = boxIndex - 1;
    pksm::ui::BoxData boxData("Box " + std::to_string(box + 1));
    boxData.resize(GRID_SLOTS);

    if (!sav || box < 0 || box >= sav->maxBoxes()) {
        return boxData;
    }

    for (int gridSlot = 0; gridSlot < GRID_SLOTS; gridSlot++) {
        boxData[gridSlot].unusable = GridToSaveSlot(*sav, boxIndex, gridSlot) < 0;
    }

    try {
        // Some saves have no box names by design (e.g. LGPE)
        const std::string name = VisibleName(sav->boxName(static_cast<u8>(box)));
        if (!IsBlankName(name)) {
            boxData.name = name;
        }

        const bool twentySlot = SaveUsesTwentySlotBoxes(*sav);
        const int slotsPerBox = twentySlot ? 20 : GRID_SLOTS;
        // The last box may be partial (LGPE's 1000-slot list ends mid-box)
        const int slotsInBox = std::min(slotsPerBox, sav->maxSlot() - box * slotsPerBox);

        for (int slot = 0; slot < slotsInBox; slot++) {
            auto pk = sav->pkm(static_cast<u8>(box), static_cast<u8>(slot));
            if (!pk) {
                continue;
            }
            if (pk->isEncrypted()) {
                pk->decrypt();
            }
            if (static_cast<u16>(pk->species()) == 0) {
                continue;
            }
            const int gridSlot = twentySlot ? ToGridSlot(slot) : slot;
            boxData[gridSlot] = VisualFromPkx(*pk);
            // Party members living in the box list wear their party number
            const int ref = pksm::saves::ListRefAt(*sav, box * GRID_SLOTS + slot);
            if (ref >= 0) {
                boxData[gridSlot].partyNumber = static_cast<u8>(ref + 1);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Core threw while reading box " + std::to_string(box) + ": " + e.what());
    }

    return boxData;
}

bool BoxDataProvider::PickUpPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) {
    auto* sav = CurrentSav(saveData);
    if (!sav || heldPkm) {
        return false;
    }
    int saveSlot = -1;
    auto pk = OccupantAt(*sav, boxIndex, slotIndex, saveSlot);
    if (!pk) {
        return false;
    }

    heldVisual = VisualFromPkx(*pk);
    heldPkm = std::move(pk);
    heldOriginBox = boxIndex;
    heldOriginSlot = saveSlot;
    heldSwapped = false;
    heldIsClone = false;
    if (IsPartyBox(boxIndex)) {
        // The party setter empties the slot (and, on LGPE, the box-list
        // entry backing it); the hole it leaves is repaired at hand-empty
        heldListRef = -1;
        heldTouchedParty = true;
        sav->pkm(*sav->emptyPkm(), static_cast<u8>(saveSlot));
    } else {
        heldListRef = pksm::saves::ListRefAt(*sav, (boxIndex - 1) * GRID_SLOTS + saveSlot);
        sav->pkm(*sav->emptyPkm(), static_cast<u8>(boxIndex - 1), static_cast<u8>(saveSlot), false);
    }
    return true;
}

bool BoxDataProvider::ClonePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) {
    auto* sav = CurrentSav(saveData);
    if (!sav || heldPkm) {
        return false;
    }
    int saveSlot = -1;
    auto pk = OccupantAt(*sav, boxIndex, slotIndex, saveSlot);
    if (!pk) {
        return false;
    }

    // The copy carries no external reference of its own; the original keeps
    // both its slot and any reference naming it
    heldVisual = VisualFromPkx(*pk);
    heldPkm = std::move(pk);
    heldOriginBox = boxIndex;
    heldOriginSlot = saveSlot;
    heldSwapped = false;
    heldIsClone = true;
    heldListRef = -1;
    return true;
}

bool BoxDataProvider::PlaceDownPokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) {
    auto* sav = CurrentSav(saveData);
    if (!sav || !heldPkm) {
        return false;
    }
    int saveSlot = -1;
    auto occupant = OccupantAt(*sav, boxIndex, slotIndex, saveSlot);
    if (saveSlot < 0) {
        return false;
    }
    const bool occupied = occupant != nullptr;

    int occupantListRef = -1;
    if (IsPartyBox(boxIndex)) {
        // The party setter admits the held Pokémon (on LGPE by writing it
        // into the box list and referencing it). A reference it still
        // carried from its old box slot is superseded by the new
        // membership; detach it, leaving the repair for hand-empty.
        pksm::saves::DetachListRef(*sav, heldListRef);
        heldListRef = -1;
        heldTouchedParty = true;
        sav->pkm(*heldPkm, static_cast<u8>(saveSlot));
        if (!occupied) {
            // LGPE's party setter fails silently when the box list has no
            // free entry. Only a cloned hand can meet a full list (a real
            // pickup frees the entry it came from), so nothing above needs
            // undoing - the copy just stays in hand.
            auto placed = sav->pkm(static_cast<u8>(saveSlot));
            if (placed && placed->isEncrypted()) {
                placed->decrypt();
            }
            if (!placed || static_cast<u16>(placed->species()) == 0) {
                return false;
            }
        }
    } else {
        // A hand-emptying placement outside the party may not leave the
        // party empty; its last member can still be swapped or put back
        if (!occupied && LivePartyCount(*sav) == 0) {
            return false;
        }
        // Read before the list changes, excluding the held Pokémon's own ref -
        // after an earlier swap it transiently points at this very target
        const int listIndex = (boxIndex - 1) * GRID_SLOTS + saveSlot;
        occupantListRef = occupied ? pksm::saves::ListRefAt(*sav, listIndex, heldListRef) : -1;
        sav->pkm(*heldPkm, static_cast<u8>(boxIndex - 1), static_cast<u8>(saveSlot), false);
        pksm::saves::MoveListRef(*sav, heldListRef, listIndex);
    }

    if (occupied) {
        // Swap: the displaced Pokémon stays in hand, origin unchanged so a
        // later cancel still returns to where the carry started. A cloned
        // hand owes nothing to its source slot - the displaced Pokémon's
        // origin becomes the slot the copy just took, so cancelling undoes
        // the swap instead of overwriting the untouched original.
        if (heldIsClone) {
            heldOriginBox = boxIndex;
            heldOriginSlot = saveSlot;
            heldIsClone = false;
        }
        heldPkm = std::move(occupant);
        heldVisual = VisualFromPkx(*heldPkm);
        heldSwapped = true;
        heldListRef = occupantListRef;
        saveDataAccessor->markDirty();
    } else {
        const bool backHome =
            !heldSwapped && !heldIsClone && boxIndex == heldOriginBox && saveSlot == heldOriginSlot;
        if (!backHome) {
            saveDataAccessor->markDirty();
        }
        ClearHand(*sav);
    }
    return true;
}

std::optional<pksm::summary::SummaryData>
BoxDataProvider::GetPokemonSummary(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) const {
    auto* sav = CurrentSav(saveData);
    if (!sav) {
        return std::nullopt;
    }
    int saveSlot = -1;
    const auto pk = OccupantAt(*sav, boxIndex, slotIndex, saveSlot);
    if (!pk) {
        return std::nullopt;
    }
    return pksm::summary::BuildSummary(*pk);
}

bool BoxDataProvider::CancelHold(const pksm::saves::SaveData::Ref& saveData) {
    auto* sav = CurrentSav(saveData);
    if (!sav || !heldPkm) {
        return false;
    }

    // A cloned hand is discarded outright - its original never left the
    // origin slot. Otherwise the origin slot was emptied by this carry's own
    // pickup and only this hand can refill it, so the put-back always
    // succeeds (a party origin regains its freed LGPE list entry too).
    if (!heldIsClone) {
        if (IsPartyBox(heldOriginBox)) {
            sav->pkm(*heldPkm, static_cast<u8>(heldOriginSlot));
        } else {
            sav->pkm(*heldPkm, static_cast<u8>(heldOriginBox - 1), static_cast<u8>(heldOriginSlot), false);
            pksm::saves::MoveListRef(*sav, heldListRef, (heldOriginBox - 1) * GRID_SLOTS + heldOriginSlot);
        }
    }
    ClearHand(*sav);
    return true;
}

bool BoxDataProvider::ReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex) {
    auto* sav = CurrentSav(saveData);
    if (!sav || heldPkm) {
        return false;
    }
    int saveSlot = -1;
    if (!OccupantAt(*sav, boxIndex, slotIndex, saveSlot)) {
        return false;
    }
    if (WouldEmptyParty(*sav, boxIndex, saveSlot)) {
        return false;
    }

    if (IsPartyBox(boxIndex)) {
        sav->pkm(*sav->emptyPkm(), static_cast<u8>(saveSlot));
        sav->fixParty();
    } else {
        sav->pkm(*sav->emptyPkm(), static_cast<u8>(boxIndex - 1), static_cast<u8>(saveSlot), false);
        pksm::saves::ClearListRefsAt(*sav, (boxIndex - 1) * GRID_SLOTS + saveSlot);
    }
    saveDataAccessor->markDirty();
    return true;
}

bool BoxDataProvider::ReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) {
    auto* sav = CurrentSav(saveData);
    if (!sav || !heldPkm) {
        return false;
    }

    // A copy vanishes without a trace; a real held Pokémon already left its
    // slot at pickup, so dropping it removes it from the save - unless the
    // party has no other member left to its name
    if (!heldIsClone) {
        if (LivePartyCount(*sav) == 0) {
            return false;
        }
        pksm::saves::ClearListRef(*sav, heldListRef);
        saveDataAccessor->markDirty();
    }
    ClearHand(*sav);
    return true;
}

bool BoxDataProvider::CanReleasePokemon(const pksm::saves::SaveData::Ref& saveData, int boxIndex, int slotIndex)
    const {
    auto* sav = CurrentSav(saveData);
    if (!sav || heldPkm) {
        return false;
    }
    int saveSlot = -1;
    if (!OccupantAt(*sav, boxIndex, slotIndex, saveSlot)) {
        return false;
    }
    return !WouldEmptyParty(*sav, boxIndex, saveSlot);
}

bool BoxDataProvider::CanReleaseHeldPokemon(const pksm::saves::SaveData::Ref& saveData) const {
    auto* sav = CurrentSav(saveData);
    if (!sav || !heldPkm) {
        return false;
    }
    return heldIsClone || LivePartyCount(*sav) > 0;
}

bool BoxDataProvider::CanRenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const {
    auto* sav = CurrentSav(saveData);
    if (!sav || IsPartyBox(boxIndex)) {
        return false;
    }
    const int box = boxIndex - 1;
    if (box < 0 || box >= sav->maxBoxes()) {
        return false;
    }
    return pksm::saves::BoxNameRulesFor(*sav).renamable;
}

size_t BoxDataProvider::GetBoxNameMaxLength(const pksm::saves::SaveData::Ref& saveData) const {
    auto* sav = CurrentSav(saveData);
    return sav ? pksm::saves::BoxNameRulesFor(*sav).maxLength : 0;
}

std::optional<std::string>
BoxDataProvider::FirstUnstorableBoxNameChar(const pksm::saves::SaveData::Ref& saveData, const std::string& name) const {
    auto* sav = CurrentSav(saveData);
    if (!sav) {
        return std::nullopt;
    }
    return pksm::saves::FirstUnstorableBoxNameChar(*sav, name);
}

bool BoxDataProvider::RenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const std::string& name) {
    if (!CanRenameBox(saveData, boxIndex) || IsBlankName(name)) {
        return false;
    }
    auto* sav = CurrentSav(saveData);
    const u8 box = static_cast<u8>(boxIndex - 1);
    const std::string oldName = sav->boxName(box);
    sav->boxName(box, name);
    const std::string storedName = sav->boxName(box);
    // The setter owns the encoding: it truncates to the field and stops at
    // characters the generation's character set lacks. A name that stored as
    // nothing keeps the old one.
    if (IsBlankName(VisibleName(storedName))) {
        sav->boxName(box, oldName);
        return false;
    }
    if (storedName != oldName) {
        saveDataAccessor->markDirty();
    }
    return true;
}
