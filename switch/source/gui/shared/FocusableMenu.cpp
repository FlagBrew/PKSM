#include "gui/shared/components/FocusableMenu.hpp"

#include <algorithm>

#include "utils/Logger.hpp"

pksm::ui::FocusableMenu::FocusableMenu(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::ui::Color items_clr,
    const pu::ui::Color items_focus_clr,
    const pu::i32 items_height,
    const u32 items_to_show
)
  : Menu(x, y, width, items_clr, items_focus_clr, items_height, items_to_show), focused(false), lastPosition(0) {
    outline = pksm::ui::PulsingOutline::New(x, y, width, items_height * items_to_show, pu::ui::Color(0, 150, 255, 255));
    outline->SetVisible(false);

    // Set up input handler
    inputHandler.SetOnMoveUp([this]() { MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { MoveDown(); });
}

void pksm::ui::FocusableMenu::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    Menu::OnRender(drawer, x, y);
    // Calculate actual height based on number of items currently in the menu
    pu::i32 actualHeight = static_cast<pu::i32>(std::min<size_t>(
                               this->GetItems().size(),
                               static_cast<size_t>(this->GetNumberOfItemsToShow())
                           )) *
        this->GetItemsHeight();
    outline->SetHeight(actualHeight);

    // Render the outline if focused
    if (focused) {
        outline->OnRender(drawer, x, y);
    }
}

void pksm::ui::FocusableMenu::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (!touch_pos.IsEmpty() && touch_pos.HitsRegion(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight())) {
        // Let the menu handle the touch input first
        Menu::OnInput(keys_down, keys_up, keys_held, touch_pos);

        // Notify about touch selection if we're not already focused
        if (!focused && onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    } else if (focused) {
        if (inputHandler.HandleInput(keys_down, keys_held)) {
            return;  // Input was handled by directional handler
        }

        Menu::OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

void pksm::ui::FocusableMenu::MoveUp() {
    if (this->GetSelectedIndex() > 0) {
        this->SetSelectedIndex(this->GetSelectedIndex() - 1);
    }
}

void pksm::ui::FocusableMenu::MoveDown() {
    if (!this->GetItems().empty()) {
        size_t nextIndex = this->GetSelectedIndex() + 1;
        if (nextIndex >= this->GetItems().size()) {
            // Wrap around to top
            nextIndex = 0;
        }
        this->SetSelectedIndex(nextIndex);
    }
}

void pksm::ui::FocusableMenu::SetFocused(bool focused) {
    if (focused) {
        LOG_DEBUG("[FocusableMenu] Setting focused to true");
    } else {
        LOG_DEBUG("[FocusableMenu] Setting focused to false");
    }
    if (this->focused != focused) {
        this->focused = focused;
        outline->SetVisible(focused);

        if (focused) {
            RestorePosition();
        } else {
            StorePosition();
        }
    }
}

bool pksm::ui::FocusableMenu::IsFocused() const {
    return focused;
}

void pksm::ui::FocusableMenu::StorePosition() {
    lastPosition = this->GetSelectedIndex();
}

void pksm::ui::FocusableMenu::RestorePosition() {
    // Only restore if we have items and the position is valid
    if (!this->GetItems().empty() && lastPosition < static_cast<int>(this->GetItems().size())) {
        this->SetSelectedIndex(lastPosition);
    }
}

void pksm::ui::FocusableMenu::ResetPosition() {
    lastPosition = 0;
    if (!this->GetItems().empty()) {
        this->SetSelectedIndex(0);
    }
}

void pksm::ui::FocusableMenu::SetDataSource(const std::vector<std::string>& items) {
    // Check if data source has actually changed
    if (items != currentDataSource) {
        currentDataSource = items;

        // Clear and rebuild menu items
        this->ClearItems();
        for (const auto& item : currentDataSource) {
            auto menuItem = pu::ui::elm::MenuItem::New(item);
            menuItem->SetColor(pu::ui::Color(255, 255, 255, 255));
            this->AddItem(menuItem);
        }

        // Reset position since data source changed
        ResetPosition();
    }
}

const std::vector<std::string>& pksm::ui::FocusableMenu::GetDataSource() const {
    return currentDataSource;
}

void pksm::ui::FocusableMenu::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

std::string pksm::ui::FocusableMenu::GetSelectedItemText() const {
    if (!currentDataSource.empty()) {
        if (selected_item_idx < currentDataSource.size()) {
            return currentDataSource[selected_item_idx];
        }
    }
    return "";
}