#pragma once

#include <functional>
#include <map>
#include <pu/Plutonium>
#include <vector>

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButton.hpp"
#include "gui/shared/components/IGrid.hpp"
#include "gui/shared/components/IShakeable.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::ui {

// Define menu button types
enum class MenuButtonType { Storage, Editor, Events, Bag, Scripts, Settings };

// Callback type for menu button actions
using MenuButtonCallback = std::function<void()>;

class MenuButtonGrid : public IGrid {
private:
    // Layout constants
    static constexpr pu::i32 BUTTON_SPACING = 24;  // Spacing between buttons
    static constexpr pu::i32 BUTTONS_PER_ROW = 3;  // Number of buttons per row
    static constexpr pu::i32 GRID_PADDING = 32;  // Padding around the entire grid
    static constexpr pu::i32 MIN_BUTTON_SIZE = 120;  // Minimum size for buttons

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 buttonSize;  // Calculated button size

    // State
    bool disabled = false;

    // Components
    pu::ui::Container::Ref container;
    std::vector<MenuButton::Ref> buttons;
    input::FocusManager::Ref focusManager;
    pksm::input::DirectionalInputHandler inputHandler;

    // Map of button types to callbacks
    std::map<MenuButtonType, MenuButtonCallback> buttonCallbacks;

    // Helper methods
    void InitializeButtons();

    // IGrid layout method implementations
    pu::i32 GetItemWidth() const override { return buttonSize; }
    pu::i32 GetItemHeight() const override { return buttonSize; }
    pu::i32 GetHorizontalSpacing() const override { return BUTTON_SPACING; }
    pu::i32 GetVerticalSpacing() const override { return BUTTON_SPACING; }

public:
    MenuButtonGrid(const pu::i32 x, const pu::i32 y, const pu::i32 width);
    PU_SMART_CTOR(MenuButtonGrid)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override { return width; }
    pu::i32 GetHeight() override { return CalculateDefaultGridHeight(); }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // IGrid implementations (required abstract methods only)
    size_t GetItemCount() const override { return buttons.size(); }
    ShakeableWithOutline::Ref GetItemAtIndex(size_t index) override {
        if (index < buttons.size()) {
            return buttons[index];
        }
        return nullptr;
    }
    void SetSelectedIndex(size_t index) override;

    // Disable/enable functionality
    void SetDisabled(bool disabled);

    // Register callback for a specific button type
    void RegisterButtonCallback(MenuButtonType type, MenuButtonCallback callback);

    // Handle A button press on selected button
    bool HandleSelectInput(const u64 keys_down);
};

}  // namespace pksm::ui