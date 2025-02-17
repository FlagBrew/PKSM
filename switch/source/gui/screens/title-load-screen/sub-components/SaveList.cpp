#include "gui/screens/title-load-screen/sub-components/SaveList.hpp"

#include "utils/Logger.hpp"

namespace pksm::ui {

SaveList::SaveList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::ui::Color items_clr,
    const pu::ui::Color items_focus_clr,
    const pu::i32 items_height,
    const u32 items_to_show
)
  : FocusableMenu(x, y, width, items_clr, items_focus_clr, items_height, items_to_show) {
    LOG_DEBUG("Initializing SaveList");
}

void SaveList::SetDataSource(const std::vector<saves::Save::Ref>& saves) {
    LOG_DEBUG("Setting SaveList data source with " + std::to_string(saves.size()) + " saves");
    this->saves = saves;

    // Convert saves to display strings
    std::vector<std::string> displayStrings;
    for (const auto& save : saves) {
        displayStrings.push_back(save->getName());
    }

    // Set display strings in base menu
    FocusableMenu::SetDataSource(displayStrings);
}

saves::Save::Ref SaveList::GetSelectedSave() const {
    pu::i32 selectedIndex = GetSelectedIndex();
    if (selectedIndex >= 0 && static_cast<size_t>(selectedIndex) < saves.size()) {
        return saves[selectedIndex];
    }
    return nullptr;
}

std::string SaveList::GetSelectedSaveText() const {
    return GetSelectedItemText();
}

void SaveList::SetOnSelectionChanged(std::function<void()> callback) {
    onSelectionChangedCallback = callback;
    // Set base menu callback to trigger our callback
    FocusableMenu::SetOnSelectionChanged([this]() {
        if (onSelectionChangedCallback) {
            onSelectionChangedCallback();
        }
    });
}

std::vector<HelpItem> SaveList::GetHelpItems() const {
    if (!IsFocused()) {
        return {};
    }
    return {{{pksm::ui::HelpButton::A}, "Select Save"}, {{pksm::ui::HelpButton::B}, "Back"}};
}

}  // namespace pksm::ui