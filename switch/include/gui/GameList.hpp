#pragma once

#include <pu/Plutonium>
#include "titles/Title.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/PulsingOutline.hpp"
#include "gui/DirectionalInputHandler.hpp"
#include <vector>
#include <memory>
#include <functional>

class GameList : public pu::ui::elm::Element {
private:
    enum class SelectionState {
        GameCard,
        InstalledGame
    };

    // Layout constants (copied from TitleLoadScreen for consistency)
    static constexpr u32 GAME_SECTION_Y = 180;
    static constexpr u32 GAME_CARD_SIZE = 350;
    static constexpr u32 GAME_CARD_X = 150;
    static constexpr u32 SECTION_DIVIDER_PADDING = 100;
    static constexpr u32 SECTION_DIVIDER = GAME_CARD_X + GAME_CARD_SIZE + SECTION_DIVIDER_PADDING;
    static constexpr u32 INSTALLED_START_X = SECTION_DIVIDER + SECTION_DIVIDER_PADDING;
    static constexpr u32 INSTALLED_GAME_SIZE = 240;
    static constexpr u32 GAME_SPACING = 280;
    static constexpr u32 INSTALLED_GAME_ITEMS_PER_ROW = 4;
    static constexpr u32 GAME_OUTLINE_PADDING = 15;

    // Background margin constants
    static constexpr u32 MARGIN_LEFT = 75;     // Left padding from game card
    static constexpr u32 MARGIN_RIGHT = 24;    // Right padding after last installed game
    static constexpr u32 MARGIN_TOP = 20;      // Space between background and section titles
    static constexpr u32 MARGIN_BOTTOM = 150;  // Space below game icons
    
    // Section spacing constants
    static constexpr u32 SECTION_TITLE_SPACING = 75;  // Space between section titles and game icons

    // UI Elements
    pu::ui::elm::Rectangle::Ref divider;
    pu::ui::elm::TextBlock::Ref cartridgeText;
    pu::ui::elm::TextBlock::Ref installedText;
    FocusableImage::Ref gameCardImage;
    std::vector<FocusableImage::Ref> installedGameImages;

    // Data
    std::vector<titles::TitleRef> titles;

    // State
    SelectionState selectionState;
    size_t selectedGameIndex;
    bool focused;
    pu::ui::Color backgroundColor;

    // Input handling
    DirectionalInputHandler inputHandler;

    // Callbacks
    std::function<void()> onSelectionChangedCallback;

    // Helper methods
    void UpdateHighlights();
    void HandleOnSelectionChanged();

public:
    GameList(const pu::i32 x, const pu::i32 y);
    PU_SMART_CTOR(GameList)

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
    void SetBackgroundColor(const pu::ui::Color& color);
    void SetDataSource(const std::vector<titles::TitleRef>& titles);
    titles::TitleRef GetSelectedTitle() const;
    void SetOnSelectionChanged(std::function<void()> callback);

    // Movement methods
    void MoveLeft();
    void MoveRight();
}; 