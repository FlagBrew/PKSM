#pragma once

#include <pu/Plutonium>
#include <memory>
#include <functional>
#include "titles/Title.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/GameGrid.hpp"
#include "gui/UIConstants.hpp"
#include "gui/DirectionalInputHandler.hpp"

namespace pksm {

    class ConsoleGameList : public pu::ui::elm::Element {
        public:
            struct LayoutConfig {
                pu::i32 marginLeft;
                pu::i32 marginRight;
                pu::i32 marginTop;
                pu::i32 marginBottom;
                pu::i32 sectionTitleSpacing;
                pu::i32 gameOutlinePadding;
            };

            ConsoleGameList(const pu::i32 x, const pu::i32 y, const LayoutConfig& config);
            ~ConsoleGameList() override;
            PU_SMART_CTOR(ConsoleGameList)

            pu::i32 GetX() override;
            pu::i32 GetY() override;
            pu::i32 GetWidth() override;
            pu::i32 GetHeight() override;

            void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
            void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

            void SetFocused(bool focused);
            bool IsFocused() const;
            void SetDataSource(const std::vector<titles::TitleRef>& titles);
            titles::TitleRef GetSelectedTitle() const;
            void SetOnSelectionChanged(std::function<void()> callback);
            void SetOnTouchSelect(std::function<void()> callback);

            bool ShouldResignDownFocus() const { return selectionState == SelectionState::GameCard || (selectionState == SelectionState::InstalledGame && installedGames->IsOnLastRow()); }

        private:
            enum class SelectionState {
                GameCard,
                InstalledGame
            };

            void UpdateHighlights();
            void HandleOnSelectionChanged();

            // State
            SelectionState selectionState;
            bool focused;
            std::function<void()> onSelectionChangedCallback;

            // Position tracking
            pu::i32 x;
            pu::i32 y;
            pu::i32 gameCardX;
            pu::i32 installedStartX;

            // Layout config
            LayoutConfig config;

            // Core components
            std::unique_ptr<pu::ui::Container> container;
            DirectionalInputHandler inputHandler;

            // UI Elements
            pu::ui::elm::TextBlock::Ref cartridgeText;
            pu::ui::elm::TextBlock::Ref installedText;
            pu::ui::elm::Rectangle::Ref divider;
            FocusableImage::Ref gameCardImage;
            GameGrid::Ref installedGames;

            // Data
            std::vector<titles::TitleRef> titles;

            // Callbacks
            std::function<void()> onTouchSelectCallback;

            // Console-specific layout constants
            static constexpr pu::i32 GAME_CARD_SIZE = 350;
            static constexpr pu::i32 SECTION_DIVIDER_WIDTH = 20;
            static constexpr pu::i32 SECTION_DIVIDER_PADDING = 80;
    };

} 