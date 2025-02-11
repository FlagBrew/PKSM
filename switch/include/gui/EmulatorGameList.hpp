#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/DirectionalInputHandler.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/GameGrid.hpp"
#include "gui/GameListCommon.hpp"
#include "gui/IFocusable.hpp"
#include "gui/ISelectable.hpp"
#include "input/FocusManager.hpp"
#include "input/SelectionManager.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {

class EmulatorGameList : public pu::ui::elm::Element, public IFocusable {
public:
    EmulatorGameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const GameListLayoutConfig& config,
        input::FocusManager::Ref parentFocusManager
    );
    PU_SMART_CTOR(EmulatorGameList)

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    void SetFocused(bool focused) override;
    bool IsFocused() const override;
    void SetFocusManager(std::shared_ptr<input::FocusManager> manager);

    void SetDataSource(const std::vector<titles::Title::Ref>& titles);
    titles::Title::Ref GetSelectedTitle() const;
    void SetOnSelectionChanged(std::function<void()> callback);
    void SetOnTouchSelect(std::function<void()> callback);

    bool ShouldResignUpFocus() const { return gameGrid->InOnTopRow(); }
    bool ShouldResignDownFocus() const { return gameGrid->IsOnBottomRow(); }

    // Returns a value between 0.0 and 1.0 indicating the relative horizontal position
    // of the current selection (0.0 = leftmost, 1.0 = rightmost)
    float GetSelectionHorizontalPosition() const {
        size_t column = gameGrid->GetSelectedIndex() % GRID_ITEMS_PER_ROW;
        return static_cast<float>(column) / (GRID_ITEMS_PER_ROW - 1);  // Normalize to [0,1]
    }

private:
    // Grid layout constants
    static constexpr size_t GRID_ITEMS_PER_ROW = 6;

    void HandleOnSelectionChanged();

    // State
    bool focused = false;
    std::function<void()> onSelectionChangedCallback;

    // Position tracking
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;

    // Layout config
    GameListLayoutConfig config;

    // Core components
    pu::ui::Container::Ref container;
    pksm::input::DirectionalInputHandler inputHandler;

    // UI Elements
    pu::ui::elm::TextBlock::Ref titleText;
    GameGrid::Ref gameGrid;

    // Data
    std::vector<titles::Title::Ref> titles;

    // Callbacks
    std::function<void()> onTouchSelectCallback;

    // Focus management
    input::FocusManager::Ref gameGridManager;

    // Selection management
    input::SelectionManager::Ref gameGridSelectionManager;
};

}  // namespace pksm::ui