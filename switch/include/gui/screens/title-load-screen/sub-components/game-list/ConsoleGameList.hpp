#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/screens/title-load-screen/sub-components/game-list/GameGrid.hpp"
#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/shared/components/FocusableImage.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "input/visual-feedback/interfaces/ISelectable.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {

class ConsoleGameList : public pu::ui::elm::Element, public IFocusable {
public:
    ConsoleGameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const GameListLayoutConfig& config,
        input::FocusManager::Ref parentFocusManager
    );
    PU_SMART_CTOR(ConsoleGameList)

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

    bool ShouldResignUpFocus() const {
        return selectionState == SelectionState::GameCard || installedGames->InOnTopRow();
    }
    bool ShouldResignDownFocus() const {
        return selectionState == SelectionState::GameCard ||
            (selectionState == SelectionState::InstalledGame && installedGames->IsOnBottomRow());
    }

    // Returns a value between 0.0 and 1.0 indicating the relative horizontal position
    // of the current selection (0.0 = leftmost, 1.0 = rightmost)
    float GetSelectionHorizontalPosition() const {
        if (selectionState == SelectionState::GameCard) {
            return 0.0f;  // Game card is always on the left
        }
        // For grid items, calculate relative position based on column
        size_t column = installedGames->GetSelectedIndex() % GRID_ITEMS_PER_ROW;
        return static_cast<float>(column) / (GRID_ITEMS_PER_ROW - 1);  // Normalize to [0,1]
    }

private:
    enum class SelectionState { GameCard, InstalledGame };

    // Grid layout constants
    static constexpr size_t GRID_ITEMS_PER_ROW = 4;

    void HandleOnSelectionChanged();

    // State
    SelectionState selectionState;
    bool focused = false;
    bool selected = false;
    std::function<void()> onSelectionChangedCallback;

    // Position tracking
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::i32 gameCardX;
    pu::i32 installedStartX;

    // Layout config
    GameListLayoutConfig config;

    // Core components
    pu::ui::Container::Ref container;
    pksm::input::DirectionalInputHandler inputHandler;

    // UI Elements
    pu::ui::elm::TextBlock::Ref cartridgeText;
    pu::ui::elm::TextBlock::Ref installedText;
    pu::ui::elm::Rectangle::Ref divider;
    FocusableImage::Ref gameCardImage;
    GameGrid::Ref installedGames;

    // Data
    std::vector<titles::Title::Ref> titles;

    // Callbacks
    std::function<void()> onTouchSelectCallback;

    // Console-specific layout constants
    static constexpr pu::i32 GAME_CARD_SIZE = 350;
    static constexpr pu::i32 SECTION_DIVIDER_WIDTH = 20;
    static constexpr pu::i32 SECTION_DIVIDER_PADDING = 80;

    // Focus management
    input::FocusManager::Ref installedGamesManager;

    // Selection management
    input::SelectionManager::Ref consoleGameListSelectionManager;
    input::SelectionManager::Ref gameGridSelectionManager;
};

}  // namespace pksm::ui