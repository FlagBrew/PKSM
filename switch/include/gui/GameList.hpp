#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/ConsoleGameList.hpp"
#include "gui/DirectionalInputHandler.hpp"
#include "gui/EmulatorGameList.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/GameGrid.hpp"
#include "gui/GameListCommon.hpp"
#include "gui/PulsingOutline.hpp"
#include "gui/TriggerButton.hpp"
#include "gui/UIConstants.hpp"
#include "input/FocusManager.hpp"
#include "titles/Title.hpp"
#include "ui/render/PatternRenderer.hpp"

namespace pksm::ui {

class GameList : public pu::ui::elm::Element, public IFocusable {
private:
    // Layout constants
    static constexpr u32 CORNER_RADIUS = 30;  // Radius for rounded corners

    // Background margin constants
    static constexpr u32 PADDING_LEFT = 80;  // Left padding of the component
    static constexpr u32 PADDING_RIGHT = 80;  // Right padding of the component
    static constexpr u32 PADDING_TOP = 20;  // Space between background and section titles
    static constexpr u32 PADDING_BOTTOM = 120;  // Space below game icons

    // Section spacing constants
    static constexpr u32 SECTION_TITLE_SPACING = 70;  // Space between section titles and game icons
    static constexpr u32 GAME_OUTLINE_PADDING = 15;  // Padding for the selection outline

    // Trigger button constants
    static constexpr u32 TRIGGER_BUTTON_WIDTH = 180;  // Width of L/R trigger buttons
    static constexpr u32 TRIGGER_BUTTON_HEIGHT = 100;  // Height of L/R trigger buttons
    static constexpr u32 TRIGGER_HORIZONTAL_OFFSET = 16;  // Offset from component edges
    static constexpr u32 TRIGGER_VERTICAL_OFFSET = 28;  // Offset from component edges
    static constexpr pu::ui::Color TRIGGER_BUTTON_COLOR = pu::ui::Color(135, 135, 135, 255);
    static constexpr pu::ui::Color TRIGGER_BUTTON_COLOR_PRESSED = pu::ui::Color(70, 70, 70, 255);

    // State
    bool focused = false;
    pu::ui::Color backgroundColor;
    std::function<void()> onSelectionChangedCallback;
    std::function<void()> onTouchSelectCallback;

    // Position tracking
    pu::i32 x;  // Component's x position
    pu::i32 y;  // Component's y position
    pu::i32 width;  // Component's width
    pu::i32 height;  // Component's height

    // Focus management
    input::FocusManager::Ref emulatorGameListManager;
    input::FocusManager::Ref leftTriggerFocusManager;
    input::FocusManager::Ref rightTriggerFocusManager;

    // Core components
    pu::ui::Container::Ref container;
    pksm::input::DirectionalInputHandler inputHandler;

    // UI Elements
    ui::TriggerButton::Ref leftTrigger;
    ui::TriggerButton::Ref rightTrigger;
    ui::render::PatternBackground::Ref background;
    EmulatorGameList::Ref emulatorGameList;

    // Data
    std::vector<titles::Title::Ref> titles;

    // Trigger button methods
    void CreateTriggerButtons();
    void OnTriggerButtonPressed(ui::TriggerButton::Side side);
    void OnTriggerButtonReleased(ui::TriggerButton::Side side);

public:
    GameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        input::FocusManager::Ref parentFocusManager
    );
    PU_SMART_CTOR(GameList)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    // Handle non-directional button presses
    bool HandleNonDirectionalInput(const u64 keys_down, const u64 keys_up, const u64 keys_held);

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;
    void SetFocusManager(std::shared_ptr<input::FocusManager> manager);

    // Public interface
    void SetBackgroundColor(const pu::ui::Color& color);
    void SetDataSource(const std::vector<titles::Title::Ref>& titles);
    titles::Title::Ref GetSelectedTitle() const;
    void SetOnSelectionChanged(std::function<void()> callback);
    void SetOnTouchSelect(std::function<void()> callback);

    // Focus management
    bool ShouldResignDownFocus() const;
};
}  // namespace pksm::ui