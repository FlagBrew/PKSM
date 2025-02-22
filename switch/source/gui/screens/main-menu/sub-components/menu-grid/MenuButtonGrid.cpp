#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"

#include "utils/Logger.hpp"

namespace pksm::ui {

MenuButtonGrid::MenuButtonGrid(const pu::i32 x, const pu::i32 y, const pu::i32 width)
  : Element(), x(x), y(y), width(width) {
    LOG_DEBUG("Initializing MenuButtonGrid...");
    focusManager = input::FocusManager::New("MenuButtonGrid Manager");
    focusManager->SetActive(true);  // since this is the root manager

    // Set up input handling
    inputHandler.SetOnMoveLeft([this]() { MoveLeft(); });
    inputHandler.SetOnMoveRight([this]() { MoveRight(); });
    inputHandler.SetOnMoveUp([this]() { MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { MoveDown(); });

    // Calculate button size based on available width
    const pu::i32 availableWidth = width - (GRID_PADDING * 2) - (BUTTON_SPACING * (BUTTONS_PER_ROW - 1));
    buttonSize = std::max(MIN_BUTTON_SIZE, availableWidth / BUTTONS_PER_ROW);

    InitializeButtons();

    LOG_DEBUG("MenuButtonGrid initialization complete");
}

void MenuButtonGrid::InitializeButtons() {
    // Create buttons with icons and labels
    const std::vector<std::pair<std::string, std::string>> buttonData = {
        {"Storage", "icon_storage"},
        {"Editor", "icon_editor"},
        {"Events", "icon_events"},
        {"Bag", "icon_bag"},
        {"Scripts", "icon_scripts"},
        {"Settings", "icon_settings"}
    };

    // Calculate total rows needed
    const size_t totalRows = (buttonData.size() + BUTTONS_PER_ROW - 1) / BUTTONS_PER_ROW;

    // Calculate total height needed
    height = (totalRows * buttonSize) + ((totalRows - 1) * BUTTON_SPACING) + (GRID_PADDING * 2);

    // Calculate starting position to center the grid vertically
    const pu::i32 startY = y + GRID_PADDING;

    for (size_t i = 0; i < buttonData.size(); i++) {
        const size_t row = i / BUTTONS_PER_ROW;
        const size_t col = i % BUTTONS_PER_ROW;

        const pu::i32 buttonX = x + GRID_PADDING + (col * (buttonSize + BUTTON_SPACING));
        const pu::i32 buttonY = startY + (row * (buttonSize + BUTTON_SPACING));

        auto button =
            MenuButton::New(buttonX, buttonY, buttonSize, buttonSize, buttonData[i].first, buttonData[i].second);
        button->IFocusable::SetName("MenuButton Element: " + buttonData[i].first);
        button->SetOnClick([this, button]() { button->RequestFocus(); });
        focusManager->RegisterFocusable(button);
        buttons.push_back(button);
    }
}

void MenuButtonGrid::MoveFocus(size_t newIndex) {
    if (newIndex < buttons.size()) {
        buttons[newIndex]->RequestFocus();
    }
}

void MenuButtonGrid::MoveLeft() {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i]->IsFocused()) {
            if (!IsFirstInRow(i)) {
                MoveFocus(i - 1);
            }
            break;
        }
    }
}

void MenuButtonGrid::MoveRight() {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i]->IsFocused()) {
            if (!IsLastInRow(i)) {
                MoveFocus(i + 1);
            }
            break;
        }
    }
}

void MenuButtonGrid::MoveUp() {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i]->IsFocused()) {
            if (!IsInFirstRow(i)) {
                MoveFocus(i - BUTTONS_PER_ROW);
            }
            break;
        }
    }
}

void MenuButtonGrid::MoveDown() {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i]->IsFocused()) {
            if (!IsInLastRow(i) && (i + BUTTONS_PER_ROW) < buttons.size()) {
                MoveFocus(i + BUTTONS_PER_ROW);
            }
            break;
        }
    }
}

pu::i32 MenuButtonGrid::GetX() {
    return x;
}

pu::i32 MenuButtonGrid::GetY() {
    return y;
}

pu::i32 MenuButtonGrid::GetWidth() {
    return width;
}

pu::i32 MenuButtonGrid::GetHeight() {
    return height;
}

void MenuButtonGrid::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render each button
    for (auto& button : buttons) {
        button->OnRender(drawer, button->GetX(), button->GetY());
    }

    RequestInitialFocusIfNeeded();
}

void MenuButtonGrid::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (!disabled) {
        // Handle directional input
        inputHandler.HandleInput(keys_down, keys_held);

        // Let each button handle its own input for touch/click events
        for (auto& button : buttons) {
            button->OnInput(keys_down, keys_up, keys_held, touch_pos);
        }
    }
}

void MenuButtonGrid::RequestInitialFocusIfNeeded() {
    if (!hasBeenFocused && !buttons.empty()) {
        buttons[0]->RequestFocus();
        hasBeenFocused = true;
    }
}

void MenuButtonGrid::SetDisabled(bool disabled) {
    this->disabled = disabled;
}

}  // namespace pksm::ui