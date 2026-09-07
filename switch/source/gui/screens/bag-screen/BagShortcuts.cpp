#include "gui/screens/bag-screen/BagShortcuts.hpp"

#include <algorithm>

#include "gui/shared/UIConstants.hpp"

namespace {

using pksm::bag::Shortcut;
using pksm::bag::ShortcutButton;

// How the bag names a slot: on the row's pill, in the dialog and in the help overlay
std::string ShortcutGlyph(const Shortcut& shortcut) {
    switch (shortcut.button) {
        case ShortcutButton::Select:
            return "SELECT";  // the console has no such button, so the word stands in
        case ShortcutButton::Y:
            return pksm::ui::global::GetButtonGlyphString(pksm::ui::global::ButtonGlyph::Y);
        case ShortcutButton::TouchScreen:
            return pksm::ui::global::GetButtonGlyphString(pksm::ui::global::ButtonGlyph::TouchScreen);
        case ShortcutButton::Position:
            return std::to_string(shortcut.position);
    }
    return "";
}

// The slot in a sentence
std::string ShortcutName(const Shortcut& shortcut) {
    switch (shortcut.button) {
        case ShortcutButton::Select:
            return "Select";
        case ShortcutButton::Y:
            return "Y";
        case ShortcutButton::TouchScreen:
            return "the touch screen";
        case ShortcutButton::Position:
            return "the Ready menu";
    }
    return "";
}

// The slot as a dialog row's label
std::string ShortcutLabel(const Shortcut& shortcut) {
    switch (shortcut.button) {
        case ShortcutButton::Select:
            return "Select button";
        case ShortcutButton::Y:
            return "Y button";
        case ShortcutButton::TouchScreen:
            return "Touch screen";
        case ShortcutButton::Position:
            return "Position " + std::to_string(shortcut.position);
    }
    return "";
}

size_t Held(const std::vector<Shortcut>& shortcuts) {
    return static_cast<size_t>(std::count_if(shortcuts.begin(), shortcuts.end(), [](const Shortcut& s) {
        return s.itemId != 0;
    }));
}

}  // namespace

namespace pksm::layout {

BagShortcuts::BagShortcuts(
    pksm::bag::BagData& bag,
    pksm::ui::BagItemList::Ref itemList,
    IBagDataProvider::Ref bagDataProvider,
    ISaveDataAccessor::Ref saveDataAccessor,
    Hooks hooks
)
  : bag(bag),
    itemList(std::move(itemList)),
    bagDataProvider(std::move(bagDataProvider)),
    saveDataAccessor(std::move(saveDataAccessor)),
    hooks(std::move(hooks)) {}

bool BagShortcuts::CanRegister(const pksm::bag::Pouch& pouch, const pksm::bag::Slot* slot) const {
    return !bag.keepsFavorite && !bag.shortcuts.empty() && slot != nullptr &&
           pouch.pouch == ::pksm::Sav::Pouch::KeyItem;
}

bool BagShortcuts::Direct() const {
    return bag.shortcuts.size() == 1 || Held(bag.shortcuts) == 0;
}

std::string BagShortcuts::Action(const pksm::bag::Pouch& pouch, const pksm::bag::Slot* slot) const {
    if (!CanRegister(pouch, slot)) {
        return "";
    }
    if (slot->shortcut >= 0) {
        return "Unregister from " + ShortcutName(bag.shortcuts[slot->shortcut]);
    }
    if (Direct()) {
        return "Register to " + ShortcutName(bag.shortcuts.front());
    }
    return "Register…";
}

void BagShortcuts::Toggle(const pksm::bag::Pouch& pouch, const pksm::bag::Slot& slot) {
    const auto save = saveDataAccessor->getCurrentSaveData();
    if (slot.shortcut >= 0) {
        Apply(bagDataProvider->Unregister(save, pouch.pouch, slot.slot));
        return;
    }
    if (Direct()) {
        Apply(bagDataProvider->Register(save, pouch.pouch, slot.slot, std::nullopt));
        return;
    }
    // The held slots, then only the next free one: the games keep the list packed
    pksm::ui::ChoiceDialog choice;
    std::vector<u8> targets;
    const bool positions = bag.shortcuts.front().button == ShortcutButton::Position;
    for (size_t i = 0; i < bag.shortcuts.size(); i++) {
        const auto& shortcut = bag.shortcuts[i];
        const bool free = shortcut.itemId == 0;
        choice.options.push_back((positions ? "" : ShortcutGlyph(shortcut) + "  ") + ShortcutLabel(shortcut));
        choice.notes.push_back(free ? "Free" : "Replaces " + shortcut.name);
        targets.push_back(static_cast<u8>(i));
        if (free) {
            break;
        }
    }
    choice.options.push_back("Cancel");
    choice.title = "Register " + slot.name;
    choice.message = positions ? "Which position in the Ready menu?" : "Which button?";
    choice.vertical = true;
    hooks.beforeDialog();
    const int picked = hooks.requestChoice(choice);
    if (picked < 0) {
        return;
    }
    Apply(bagDataProvider->Register(save, pouch.pouch, slot.slot, targets[picked]));
}

void BagShortcuts::Apply(std::optional<pksm::bag::Pouch> updated) {
    if (!updated) {
        hooks.shakeCursor();
        return;
    }
    hooks.applyPouch(std::move(*updated));
}

void BagShortcuts::Refresh() {
    bag.shortcuts = bagDataProvider->GetShortcuts(saveDataAccessor->getCurrentSaveData());
    PushGlyphs();
}

void BagShortcuts::PushGlyphs() {
    std::vector<std::string> glyphs;
    for (const auto& shortcut : bag.shortcuts) {
        glyphs.push_back(ShortcutGlyph(shortcut));
    }
    itemList->SetShortcutGlyphs(std::move(glyphs));
}

}  // namespace pksm::layout
