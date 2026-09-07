#pragma once

#include <string>

#include "data/bag/BagData.hpp"
#include "gui/shared/UIConstants.hpp"

// How the bag names a shortcut slot: on the row's pill, in the dialog and in the help overlay
namespace pksm::layout {

inline std::string ShortcutGlyph(const pksm::bag::Shortcut& shortcut) {
    switch (shortcut.button) {
        case pksm::bag::ShortcutButton::Select:
            return "SELECT";  // the console has no such button, so the word stands in
        case pksm::bag::ShortcutButton::Y:
            return pksm::ui::global::GetButtonGlyphString(pksm::ui::global::ButtonGlyph::Y);
        case pksm::bag::ShortcutButton::TouchScreen:
            return pksm::ui::global::GetButtonGlyphString(pksm::ui::global::ButtonGlyph::TouchScreen);
        case pksm::bag::ShortcutButton::Position:
            return std::to_string(shortcut.position);
    }
    return "";
}

// The slot in a sentence
inline std::string ShortcutName(const pksm::bag::Shortcut& shortcut) {
    switch (shortcut.button) {
        case pksm::bag::ShortcutButton::Select:
            return "Select";
        case pksm::bag::ShortcutButton::Y:
            return "Y";
        case pksm::bag::ShortcutButton::TouchScreen:
            return "the touch screen";
        case pksm::bag::ShortcutButton::Position:
            return "the Ready menu";
    }
    return "";
}

// The slot as a dialog row's label
inline std::string ShortcutLabel(const pksm::bag::Shortcut& shortcut) {
    switch (shortcut.button) {
        case pksm::bag::ShortcutButton::Select:
            return "Select button";
        case pksm::bag::ShortcutButton::Y:
            return "Y button";
        case pksm::bag::ShortcutButton::TouchScreen:
            return "Touch screen";
        case pksm::bag::ShortcutButton::Position:
            return "Position " + std::to_string(shortcut.position);
    }
    return "";
}

}  // namespace pksm::layout
