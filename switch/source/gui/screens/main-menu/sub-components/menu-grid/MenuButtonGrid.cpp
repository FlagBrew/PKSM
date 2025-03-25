#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"

#include "utils/Logger.hpp"

namespace pksm::ui {

MenuButtonGrid::MenuButtonGrid(const pu::i32 x, const pu::i32 y, const pu::i32 width)
  : pu::ui::elm::Element(), IGrid(BUTTONS_PER_ROW, {}), x(x), y(y), width(width) {
    LOG_DEBUG("Initializing MenuButtonGrid...");

    // Initialize container
    container = pu::ui::Container::New(x, y, width, GetHeight());  // Use GetHeight() for height

    // Initialize focus manager
    focusManager = input::FocusManager::New("MenuButtonGrid Manager");
    focusManager->SetActive(true);  // since this is the root manager

    // Set up input handling
    inputHandler.SetOnMoveLeft([this]() { IGrid::MoveLeft(); });
    inputHandler.SetOnMoveRight([this]() { IGrid::MoveRight(); });
    inputHandler.SetOnMoveUp([this]() { IGrid::MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { IGrid::MoveDown(); });

    // Calculate button size based on available width
    const pu::i32 availableWidth = width - (GRID_PADDING * 2) - (BUTTON_SPACING * (BUTTONS_PER_ROW - 1));
    buttonSize = std::max(MIN_BUTTON_SIZE, availableWidth / BUTTONS_PER_ROW);

    InitializeButtons();

    // Update container height and prepare elements
    container->SetHeight(GetHeight());  // Use GetHeight() instead of height variable
    container->PreRender();

    // Set initial selection after buttons are created
    if (!buttons.empty()) {
        buttons[0]->RequestFocus();
        LOG_DEBUG("Initial button focused");
    }

    LOG_DEBUG("MenuButtonGrid initialization complete");
}

void MenuButtonGrid::InitializeButtons() {
    // Create buttons with icons and labels
    const std::vector<std::pair<MenuButtonType, std::pair<std::string, std::string>>> buttonData = {
        {MenuButtonType::Storage, {"Storage", "icon_storage"}},
        {MenuButtonType::Editor, {"Editor", "icon_editor"}},
        {MenuButtonType::Events, {"Events", "icon_events"}},
        {MenuButtonType::Bag, {"Bag", "icon_bag"}},
        {MenuButtonType::Scripts, {"Scripts", "icon_scripts"}},
        {MenuButtonType::Settings, {"Settings", "icon_settings"}}
    };

    for (size_t i = 0; i < buttonData.size(); i++) {
        // Calculate position using IGrid's helper method
        auto position = CalculateItemPosition(i);

        // Apply padding offset
        const pu::i32 buttonX = position.first - x + GRID_PADDING;  // Adjust for container position
        const pu::i32 buttonY = position.second - y + GRID_PADDING;  // Adjust for container position

        auto button = MenuButton::New(
            x + buttonX,
            y + buttonY,
            buttonSize,
            buttonSize,
            buttonData[i].second.first,
            buttonData[i].second.second
        );
        button->IFocusable::SetName("MenuButton Element: " + buttonData[i].second.first);

        // Store the button type
        MenuButtonType buttonType = buttonData[i].first;
        const size_t index = i;  // Store index for lambda capture

        // Set up touch select handling (first tap - focus request)
        button->SetOnTouchSelect([this, index]() { SetSelectedIndex(index); });

        // Set up action handling (second tap or A button - execute action)
        button->SetOnSelect([this, buttonType]() {
            // Execute the callback if registered
            if (buttonCallbacks.count(buttonType) > 0) {
                LOG_DEBUG("Executing callback for button: " + std::to_string(static_cast<int>(buttonType)));
                buttonCallbacks[buttonType]();
            }
        });

        focusManager->RegisterFocusable(button);
        buttons.push_back(button);
        container->Add(button);  // Add to container
    }
}

void MenuButtonGrid::RegisterButtonCallback(MenuButtonType type, MenuButtonCallback callback) {
    buttonCallbacks[type] = callback;
}

bool MenuButtonGrid::HandleSelectInput(const u64 keys_down) {
    // No longer needed as buttons now handle their own input via ButtonInputHandler
    return false;
}

void MenuButtonGrid::SetSelectedIndex(size_t index) {
    if (index < buttons.size() && selectedIndex != index) {
        selectedIndex = index;

        // Request focus on the newly selected button
        buttons[selectedIndex]->RequestFocus();
    }
}

pu::i32 MenuButtonGrid::GetX() {
    return x;
}

pu::i32 MenuButtonGrid::GetY() {
    return y;
}

void MenuButtonGrid::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render all elements in the container
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, element->GetX(), element->GetY());
    }
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

        // Let each element handle its own input
        for (auto& element : container->GetElements()) {
            element->OnInput(keys_down, keys_up, keys_held, touch_pos);
        }
    }
}

void MenuButtonGrid::SetDisabled(bool disabled) {
    this->disabled = disabled;
}

}  // namespace pksm::ui