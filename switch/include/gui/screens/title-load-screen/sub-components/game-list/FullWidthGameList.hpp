#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/screens/title-load-screen/sub-components/game-list/GameGrid.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/IGameList.hpp"
#include "gui/shared/components/FocusableImage.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/SelectionManager.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "input/visual-feedback/interfaces/ISelectable.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {

class FullWidthGameList : public IGameList {
public:
    FullWidthGameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const GameListLayoutConfig& config,
        input::FocusManager::Ref parentFocusManager,
        const std::string& titleText
    );
    PU_SMART_CTOR(FullWidthGameList)

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    void SetFocused(bool focused) override;
    bool IsFocused() const override;
    void SetFocusManager(std::shared_ptr<input::FocusManager> manager) override;

    void SetDataSource(const std::vector<titles::Title::Ref>& titles) override;
    titles::Title::Ref GetSelectedTitle() const override;
    void SetOnSelectionChanged(std::function<void()> callback) override;
    void SetOnTouchSelect(std::function<void()> callback) override;

    bool ShouldResignUpFocus() const override { return gameGrid->InOnTopRow(); }
    bool ShouldResignDownFocus() const override { return gameGrid->IsOnBottomRow(); }

    // Returns a value between 0.0 and 1.0 indicating the relative horizontal position
    // of the current selection (0.0 = leftmost, 1.0 = rightmost)
    float GetSelectionHorizontalPosition() const override {
        size_t column = gameGrid->GetSelectedIndex() % GRID_ITEMS_PER_ROW;
        return static_cast<float>(column) / (GRID_ITEMS_PER_ROW - 1);  // Normalize to [0,1]
    }

protected:
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