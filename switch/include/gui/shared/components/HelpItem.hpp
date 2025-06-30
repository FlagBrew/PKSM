#pragma once

#include <string>
#include <vector>

#include "gui/shared/UIConstants.hpp"

namespace pksm::ui {

// Struct to represent a help item (button + action description)
struct HelpItem {
    std::vector<pksm::ui::global::ButtonGlyph> buttons;  // Multiple buttons can be grouped (separated by /)
    std::string actionDescription;
};

}  // namespace pksm::ui