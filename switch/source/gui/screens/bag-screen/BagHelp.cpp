#include "gui/screens/bag-screen/BagHelp.hpp"

namespace pksm::layout {

using pksm::ui::global::ButtonGlyph;

std::vector<pksm::ui::HelpItem> BagFooterHelp(const BagHelpState& state) {
    std::vector<pksm::ui::HelpItem> items;
    if (state.pickerOpen) {
        if (state.pickerHasRows) {
            items.push_back({{ButtonGlyph::A}, "Add"});
        }
        if (state.pickerHasCandidates) {
            items.push_back({{ButtonGlyph::Y}, "Search"});
        }
        if (state.searching) {
            items.push_back({{ButtonGlyph::X}, "Clear Search"});
        }
        items.push_back({{ButtonGlyph::B}, "Cancel"});
        return items;
    }
    if (state.carrying) {
        items.push_back({{ButtonGlyph::Y}, "Drop"});
        items.push_back({{ButtonGlyph::B}, "Put Back"});
        return items;
    }
    if (state.listFocused) {
        if (state.canEditCount) {
            items.push_back({{ButtonGlyph::A}, "Set Quantity"});
            items.push_back({{ButtonGlyph::ZL, ButtonGlyph::ZR}, "Adjust"});
        }
        if (state.canRemove) {
            items.push_back({{ButtonGlyph::X}, "Remove"});
        }
        if (state.canReorder) {
            items.push_back({{ButtonGlyph::Y}, "Move"});
        }
    } else {
        if (state.pouchOpenable) {
            items.push_back({{ButtonGlyph::A}, "Open"});
        }
        if (state.canSort) {
            items.push_back({{ButtonGlyph::Y}, "Sort"});
        }
        items.push_back({{ButtonGlyph::B}, "Back"});
    }
    if (state.canAdd) {
        items.push_back({{ButtonGlyph::Plus}, "Add"});
    }
    // B out of the list and the d-pad go without saying
    items.push_back({{ButtonGlyph::L, ButtonGlyph::R}, "Switch Pouch"});
    return items;
}

std::vector<pksm::ui::HelpItem> BagOverlayHelp(const BagHelpState& state) {
    std::vector<pksm::ui::HelpItem> items;
    if (state.pickerOpen) {
        items.push_back({{ButtonGlyph::A}, "Add Item"});
        items.push_back({{ButtonGlyph::Y}, "Search by Name"});
        items.push_back({{ButtonGlyph::X}, "Clear Search"});
        items.push_back({{ButtonGlyph::RightAnalogStick}, "Page Up/Down"});
        items.push_back({{ButtonGlyph::B}, "Cancel"});
        items.push_back({{ButtonGlyph::DPad, ButtonGlyph::LeftAnalogStick}, "Navigate"});
        return items;
    }
    if (state.carrying) {
        items.push_back({{ButtonGlyph::Y}, "Drop Here"});
        items.push_back({{ButtonGlyph::B}, "Put Back"});
        items.push_back({{ButtonGlyph::RightAnalogStick}, "Page Up/Down"});
        items.push_back({{ButtonGlyph::DPad, ButtonGlyph::LeftAnalogStick}, "Carry"});
        return items;
    }
    if (state.listFocused) {
        if (state.canEditCount) {
            items.push_back({{ButtonGlyph::A}, "Set Quantity"});
            items.push_back({{ButtonGlyph::ZL, ButtonGlyph::ZR}, "Adjust Quantity"});
        }
        if (state.canRemove) {
            items.push_back({{ButtonGlyph::X}, "Remove Item"});
        }
        if (state.canReorder) {
            items.push_back({{ButtonGlyph::Y}, "Move Item"});
        }
        if (state.canFavorite) {
            items.push_back(
                {{ButtonGlyph::RightAnalogStick}, state.rowFavorite ? "Press: Unfavorite" : "Press: Favorite"}
            );
        }
        if (state.canMarkSeen) {
            items.push_back({{ButtonGlyph::LeftAnalogStick}, state.rowNew ? "Press: Mark Seen" : "Press: Mark Unseen"});
        }
        items.push_back({{ButtonGlyph::RightAnalogStick}, "Page Up/Down"});
        items.push_back({{ButtonGlyph::B}, "Back to Pouches"});
    } else {
        items.push_back({{ButtonGlyph::A}, "Open Pouch"});
        if (state.canSort) {
            items.push_back({{ButtonGlyph::Y}, "Sort Pouch"});
        }
        items.push_back({{ButtonGlyph::B}, "Back to Main Menu"});
    }
    if (state.canAdd) {
        items.push_back({{ButtonGlyph::Plus}, "Add Item"});
    }
    items.push_back({{ButtonGlyph::L, ButtonGlyph::R}, "Switch Pouch"});
    items.push_back({{ButtonGlyph::DPad, ButtonGlyph::LeftAnalogStick}, "Navigate"});
    return items;
}

}  // namespace pksm::layout
