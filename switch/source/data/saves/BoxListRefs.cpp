#include "data/saves/BoxListRefs.hpp"

#include "sav/SavLGPE.hpp"

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

}  // namespace pksm::saves
