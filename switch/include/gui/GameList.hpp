#pragma once

#include <pu/Plutonium>
#include "titles/Title.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/PulsingOutline.hpp"
#include "gui/DirectionalInputHandler.hpp"
#include "gui/GameGrid.hpp"
#include "gui/UIConstants.hpp"
#include <vector>
#include <memory>
#include <functional>
#include "ui/render/PatternRenderer.hpp"

namespace pksm {

    class GameList : public pu::ui::elm::Element {
    private:
        enum class SelectionState {
            GameCard,
            InstalledGame
        };

        // Layout constants
        static constexpr u32 GAME_CARD_SIZE = 350;        // Size of the game card image
        static constexpr u32 SECTION_DIVIDER_WIDTH = 20;   // Width of the section divider
        static constexpr u32 SECTION_DIVIDER_PADDING = 80; // Padding around the divider
        static constexpr u32 GAME_OUTLINE_PADDING = 15;   // Padding for the selection outline
        static constexpr u32 CORNER_RADIUS = 30;          // Radius for rounded corners

        // Background margin constants
        static constexpr u32 MARGIN_LEFT = 80;     // Left padding of the component
        static constexpr u32 MARGIN_RIGHT = 80;    // Right padding of the component
        static constexpr u32 MARGIN_TOP = 85;      // Space between background and section titles
        static constexpr u32 MARGIN_BOTTOM = 150;  // Space below game icons
        
        // Section spacing constants
        static constexpr u32 SECTION_TITLE_SPACING = 70;  // Space between section titles and game icons
        static constexpr u32 SECTION_BOTTOM_SPACING = SECTION_TITLE_SPACING; // Space below the installed games section

        // State
        SelectionState selectionState;
        bool focused;
        pu::ui::Color backgroundColor;
        std::function<void()> onSelectionChangedCallback;
        std::function<void()> onTouchSelectCallback;

        // Position tracking
        pu::i32 x;                  // Component's x position
        pu::i32 y;                  // Component's y position
        pu::i32 gameCardX;          // X position of game card
        pu::i32 installedStartX;    // Starting X position of installed games section

        // UI Elements
        pu::ui::elm::Rectangle::Ref divider;
        pu::ui::elm::TextBlock::Ref cartridgeText;
        pu::ui::elm::TextBlock::Ref installedText;
        pu::ui::elm::TextBlock::Ref LButtonText;
        FocusableImage::Ref gameCardImage;
        GameGrid::Ref installedGames;

        // Data
        std::vector<titles::TitleRef> titles;

        // Input handling
        DirectionalInputHandler inputHandler;

        // Replace SDL_Texture* with PatternBackground
        std::shared_ptr<ui::render::PatternBackground> background;

        // Helper methods
        void UpdateHighlights();
        void HandleOnSelectionChanged();

    public:
        GameList(const pu::i32 x, const pu::i32 y);
        ~GameList();
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
        void SetOnTouchSelect(std::function<void()> callback);

        // Focus management
        bool ShouldResignDownFocus() const { return selectionState == SelectionState::GameCard || (selectionState == SelectionState::InstalledGame && installedGames->IsOnLastRow()); }
    };
} 