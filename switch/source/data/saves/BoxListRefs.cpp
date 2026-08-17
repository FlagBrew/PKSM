#include "data/saves/BoxListRefs.hpp"

#include "sav/SavLGPE.hpp"

namespace {

// 1001 is LGPE's empty-party-slot sentinel; fixParty then compacts the six
// slots so the party has no holes (the same repair the 3DS app performs when
// releasing a party-referenced Pokémon)
constexpr u16 LGPE_EMPTY_PARTY_SLOT = 1001;

}  // namespace

namespace pksm::saves {

// LGPE party members are partyBoxSlot indices into the box list (mirrors the
// party-ref patching the 3DS app does in its StorageScreen). Other saves
// keep party storage separate from boxes and have no list references.

int ListRefAt(::pksm::Sav& sav, int listIndex, int excludeToken) {
    if (sav.generation() != ::pksm::Generation::LGPE) {
        return -1;
    }
    auto& lgpe = static_cast<::pksm::SavLGPE&>(sav);
    for (u8 i = 0; i < 6; i++) {
        if (i != excludeToken && lgpe.partyBoxSlot(i) == listIndex) {
            return i;
        }
    }
    return -1;
}

void MoveListRef(::pksm::Sav& sav, int token, int listIndex) {
    if (token < 0) {
        return;
    }
    static_cast<::pksm::SavLGPE&>(sav).partyBoxSlot(static_cast<u8>(token), static_cast<u16>(listIndex));
}

void ClearListRef(::pksm::Sav& sav, int token) {
    if (token < 0) {
        return;
    }
    DetachListRef(sav, token);
    static_cast<::pksm::SavLGPE&>(sav).fixParty();
}

void DetachListRef(::pksm::Sav& sav, int token) {
    if (token < 0) {
        return;
    }
    static_cast<::pksm::SavLGPE&>(sav).partyBoxSlot(static_cast<u8>(token), LGPE_EMPTY_PARTY_SLOT);
}

void ClearListRefsAt(::pksm::Sav& sav, int listIndex) {
    if (sav.generation() != ::pksm::Generation::LGPE) {
        return;
    }
    auto& lgpe = static_cast<::pksm::SavLGPE&>(sav);
    bool cleared = false;
    for (u8 i = 0; i < 6; i++) {
        if (lgpe.partyBoxSlot(i) == listIndex) {
            lgpe.partyBoxSlot(i, LGPE_EMPTY_PARTY_SLOT);
            cleared = true;
        }
    }
    if (cleared) {
        lgpe.fixParty();
    }
}

void FinalizeBoxList(::pksm::Sav& sav) {
    if (sav.generation() != ::pksm::Generation::LGPE) {
        return;
    }
    // Mirror the 3DS app's storage-exit bookkeeping (3ds StorageScreen.cpp):
    // compact the contiguous list, then recount its length
    auto& lgpe = static_cast<::pksm::SavLGPE&>(sav);
    lgpe.compressBox();
    u16 occupiedSlots = 0;
    for (int i = 0; i < sav.maxSlot(); i++) {
        // 30 slots per LGPE box view; box/slot pairs keep both args in u8
        auto pk = sav.pkm(static_cast<u8>(i / 30), static_cast<u8>(i % 30));
        if (pk && pk->isEncrypted()) {
            pk->decrypt();
        }
        if (!pk || static_cast<u16>(pk->species()) == 0) {
            break;
        }
        occupiedSlots++;
    }
    lgpe.boxedPkm(occupiedSlots);
}

}  // namespace pksm::saves
