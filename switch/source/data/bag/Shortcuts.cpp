#include "data/bag/Shortcuts.hpp"

#include "utils/CoreStrings.hpp"

namespace {

::pksm::bag::ShortcutButton ButtonOf(const ::pksm::Sav& sav, u8 slot) {
    switch (sav.generation()) {
        case ::pksm::Generation::TWO:
        case ::pksm::Generation::THREE:
            return ::pksm::bag::ShortcutButton::Select;
        case ::pksm::Generation::FOUR:
            return slot == 1 ? ::pksm::bag::ShortcutButton::TouchScreen : ::pksm::bag::ShortcutButton::Y;
        case ::pksm::Generation::SIX:
            return ::pksm::bag::ShortcutButton::Position;
        default:
            return ::pksm::bag::ShortcutButton::Y;
    }
}

}  // namespace

namespace pksm::bag {

std::vector<Shortcut> ShortcutsOf(const ::pksm::Sav& sav) {
    std::vector<Shortcut> shortcuts;
    for (u8 slot = 0; slot < sav.registeredItemSlots(); slot++) {
        Shortcut shortcut{ButtonOf(sav, slot), static_cast<u8>(slot + 1), sav.registeredItem(slot)};
        if (shortcut.itemId != 0) {
            shortcut.name = pksm::strings::ItemName(shortcut.itemId, sav.generation());
        }
        shortcuts.push_back(std::move(shortcut));
    }
    return shortcuts;
}

int ShortcutOf(const ::pksm::Sav& sav, u16 itemId) {
    for (u8 slot = 0; slot < sav.registeredItemSlots(); slot++) {
        if (sav.registeredItem(slot) == itemId) {
            return slot;
        }
    }
    return -1;
}

}  // namespace pksm::bag
