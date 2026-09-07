#pragma once

#include <string>
#include <vector>

namespace pksm::ui {

// A blocking choice put to the player; the last option cancels. vertical lists the options one
// per row, each with its note (parallel to options, may be shorter) on the right
struct ChoiceDialog {
    std::string title;
    std::string message;
    std::vector<std::string> options;
    std::vector<std::string> notes;
    bool vertical = false;
};

}  // namespace pksm::ui
