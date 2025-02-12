#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/shared/UIConstants.hpp"
#include "gui/shared/components/FocusableImage.hpp"
#include "gui/shared/components/ScrollView.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/SelectionManager.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {
class GameGrid : public pu::ui::elm::Element, public ISelectable {
private:
    // Layout constants
    static constexpr u32 INSTALLED_GAME_SIZE = 240;  // Size of installed game images
    static constexpr u32 GAME_SPACING = 280;  // Horizontal spacing between games
    static constexpr u32 ROW_SPACING = 40;  // Vertical spacing between rows
    static constexpr u32 GAME_OUTLINE_PADDING = 15;  // Padding for the selection outline

    // State
    size_t selectedIndex = 0;
    size_t itemsPerRow;  // Number of items per row (moved from constant to member)
    bool focused = false;
    bool selected = false;  // Whether any item in the grid is selected
    std::function<void()> onSelectionChangedCallback;
    std::function<void()> onTouchSelectCallback;

    // Position tracking
    pu::i32 x;  // Component's x position
    pu::i32 y;  // Component's y position
    pu::i32 height;  // Component's height

    // Core components
    pu::ui::Container::Ref container;

    // UI Elements
    std::vector<FocusableImage::Ref> gameImages;
    ScrollView::Ref scrollView;

    // Data
    std::vector<titles::Title::Ref> titles;

    // Input handling
    pksm::input::DirectionalInputHandler inputHandler;

    // Helper methods
    void HandleOnSelectionChanged();
    void EnsureRowVisible(size_t row);

public:
    GameGrid(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 height,
        const size_t itemsPerRow,
        input::FocusManager::Ref parentFocusManager,
        input::SelectionManager::Ref parentSelectionManager
    );
    PU_SMART_CTOR(GameGrid)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    // Public interface
    void SetFocused(bool focused) override;
    bool IsFocused() const override;
    bool IsSelected() const override;
    void SetSelected(bool selected) override;
    void SetDataSource(const std::vector<titles::Title::Ref>& titles);
    titles::Title::Ref GetSelectedTitle() const;
    size_t GetSelectedIndex() const;
    void SetSelectedIndex(size_t index);
    void SetOnSelectionChanged(std::function<void()> callback);
    void SetOnTouchSelect(std::function<void()> callback);

    // Movement methods
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

    // Grid state queries
    bool IsFirstInRow() const { return selectedIndex % itemsPerRow == 0; }
    bool InOnTopRow() const { return selectedIndex < itemsPerRow; }
    bool IsOnBottomRow() const { return (selectedIndex / itemsPerRow) == ((gameImages.size() - 1) / itemsPerRow); }
};
}  // namespace pksm::ui