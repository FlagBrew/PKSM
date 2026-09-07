#pragma once

#include <vector>

#include "gui/shared/components/HelpItem.hpp"

namespace pksm::layout {

// Where the bag screen stands, as far as its help is concerned; the screen fills it in
struct BagHelpState {
    bool pickerOpen;
    bool pickerHasRows;
    bool pickerHasCandidates;
    bool searching;
    bool carrying;
    bool listFocused;
    bool canEditCount;
    bool canRemove;
    bool canReorder;
    bool canSort;
    bool canAdd;
    bool pouchOpenable;
    // The cursor row's marks, where the format keeps them; the stick clicks flip them
    bool canFavorite;
    bool canMarkSeen;
    bool rowFavorite;
    bool rowNew;
};

// The footer keeps to what a newcomer needs; the Minus overlay lists everything
std::vector<pksm::ui::HelpItem> BagFooterHelp(const BagHelpState& state);
std::vector<pksm::ui::HelpItem> BagOverlayHelp(const BagHelpState& state);

}  // namespace pksm::layout
