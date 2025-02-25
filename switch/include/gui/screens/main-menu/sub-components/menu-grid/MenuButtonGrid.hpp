#pragma once

#include <pu/Plutonium>

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButton.hpp"
#include "gui/shared/components/IShakeable.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::ui {

class MenuButtonGrid : public pu::ui::elm::Element {
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
    pu::i32 height;
    pu::i32 buttonSize;  // Calculated button size

    // State
    size_t selectedIndex = 0;
    bool disabled = false;

    // Components
    pu::ui::Container::Ref container;
    std::vector<MenuButton::Ref> buttons;
    input::FocusManager::Ref focusManager;
    pksm::input::DirectionalInputHandler inputHandler;

    // Helper methods
    void InitializeButtons();

    // Navigation methods
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDown();

    // Grid state queries
    bool IsFirstInRow(size_t index) const { return index % BUTTONS_PER_ROW == 0; }
    bool IsLastInRow(size_t index) const { return (index + 1) % BUTTONS_PER_ROW == 0 || index == buttons.size() - 1; }
    bool IsInFirstRow(size_t index) const { return index < BUTTONS_PER_ROW; }
    bool IsInLastRow(size_t index) const { return index >= (buttons.size() - BUTTONS_PER_ROW); }

public:
    MenuButtonGrid(const pu::i32 x, const pu::i32 y, const pu::i32 width);
    PU_SMART_CTOR(MenuButtonGrid)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    // Disable/enable functionality
    void SetDisabled(bool disabled);

    // Get the currently selected index
    size_t GetSelectedIndex() const { return selectedIndex; }
    void SetSelectedIndex(size_t index);
};

}  // namespace pksm::ui