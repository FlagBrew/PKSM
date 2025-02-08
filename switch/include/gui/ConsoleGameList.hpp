#pragma once

#include <functional>
#include <memory>
#include <pu/Plutonium>
#include <vector>

#include "gui/DirectionalInputHandler.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/GameGrid.hpp"
#include "gui/IFocusable.hpp"
#include "gui/ISelectable.hpp"
#include "input/FocusManager.hpp"
#include "input/SelectionManager.hpp"
#include "titles/Title.hpp"

namespace pksm::ui {

class ConsoleGameList : public pu::ui::elm::Element, public IFocusable {
public:
    struct LayoutConfig {
        pu::i32 paddingLeft;
        pu::i32 paddingRight;
        pu::i32 paddingTop;
        pu::i32 paddingBottom;
        pu::i32 sectionTitleSpacing;
        pu::i32 gameOutlinePadding;
    };

    ConsoleGameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const LayoutConfig& config,
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

    bool ShouldResignDownFocus() const {
        return selectionState == SelectionState::GameCard ||
            (selectionState == SelectionState::InstalledGame && installedGames->IsOnLastRow());
    }

private:
    enum class SelectionState { GameCard, InstalledGame };

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
    LayoutConfig config;

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

}  // namespace pksm