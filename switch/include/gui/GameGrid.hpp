#pragma once

#include <pu/Plutonium>
#include "titles/Title.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/DirectionalInputHandler.hpp"
#include "gui/ScrollView.hpp"
#include "gui/UIConstants.hpp"
#include <vector>
#include <memory>
#include <functional>

class GameGrid : public pu::ui::elm::Element {
private:
    // Layout constants
    static constexpr u32 INSTALLED_GAME_SIZE = 240;   // Size of installed game images
    static constexpr u32 GAME_SPACING = 280;          // Horizontal spacing between games
    static constexpr u32 ROW_SPACING = 40;            // Vertical spacing between rows
    static constexpr u32 ITEMS_PER_ROW = 4;          // Number of games per row
    static constexpr u32 GAME_OUTLINE_PADDING = 15;   // Padding for the selection outline

    // State
    size_t selectedIndex;
    bool focused;
    bool selected;  // Whether any item in the grid is selected
    std::function<void()> onSelectionChangedCallback;
    std::function<void()> onTouchSelectCallback;
    std::function<void()> onTopRowCallback;      // Called when trying to move up from top row
    std::function<void()> onBottomRowCallback;   // Called when trying to move down from bottom row

    // Position tracking
    pu::i32 x;                  // Component's x position
    pu::i32 y;                  // Component's y position

    // UI Elements
    std::vector<FocusableImage::Ref> gameImages;
    std::unique_ptr<ScrollView> scrollView;

    // Data
    std::vector<titles::TitleRef> titles;

    // Input handling
    DirectionalInputHandler inputHandler;

    // Helper methods
    void UpdateHighlights();
    void HandleOnSelectionChanged();
    bool IsInTopRow() const;
    bool IsInBottomRow() const;
    void EnsureRowVisible(size_t row);

public:
    GameGrid(const pu::i32 x, const pu::i32 y);
    ~GameGrid() = default;
    PU_SMART_CTOR(GameGrid)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Public interface
    void SetFocused(bool focused);
    bool IsFocused() const;
    void SetSelected(bool selected);
    bool IsSelected() const;
    void SetDataSource(const std::vector<titles::TitleRef>& titles);
    titles::TitleRef GetSelectedTitle() const;
    size_t GetSelectedIndex() const;
    void SetSelectedIndex(size_t index);
    void SetOnSelectionChanged(std::function<void()> callback);
    void SetOnTouchSelect(std::function<void()> callback);
    void SetOnTopRow(std::function<void()> callback);
    void SetOnBottomRow(std::function<void()> callback);

    // Movement methods
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

    // Grid state queries
    bool IsFirstInRow() const { return selectedIndex % ITEMS_PER_ROW == 0; }
    bool IsOnLastRow() const { return (selectedIndex / ITEMS_PER_ROW) == ((gameImages.size() - 1) / ITEMS_PER_ROW); }

    void SetAvailableHeight(pu::i32 height);
}; 