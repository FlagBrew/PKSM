#pragma once

#include <pu/Plutonium>
#include "titles/Title.hpp"
#include "gui/FocusableButton.hpp"
#include "gui/FocusableImage.hpp"
#include "gui/FocusableMenu.hpp"
#include "gui/GameList.hpp"
#include "gui/DirectionalInputHandler.hpp"
#include "data/ITitleDataProvider.hpp"
#include "data/ISaveDataProvider.hpp"
#include <vector>
#include <string>
#include <memory>

class TitleLoadScreen : public pu::ui::Layout {
private:
    enum class TitleSelectionState {
        InSaveList,
        GameCard,
        InstalledGame
    };

    // Layout constants
    static constexpr u32 SCREEN_WIDTH = 1920;
    static constexpr u32 SCREEN_HEIGHT = 1080;

    // Header sectionimage.png
    static constexpr u32 HEADER_TOP_MARGIN = 24;     // Space between screen top and header
    static constexpr u32 HEADER_HEIGHT = 80;         // Height of header text area
    static constexpr u32 HEADER_BOTTOM_MARGIN = 24;  // Space between header and game list
    
    // Game list section
    static constexpr u32 GAME_LIST_LEFT_MARGIN = 75;  // Left margin for game list component
    static constexpr u32 GAME_LIST_BOTTOM_MARGIN = 48;  // Space between game list and save list
    
    // Save list section (bottom)
    static constexpr u32 SAVE_LIST_WIDTH = 1200;
    static constexpr u32 SAVE_LIST_HEIGHT = 300;
    static constexpr u32 SAVE_LIST_X = GAME_LIST_LEFT_MARGIN;
    
    // Buttons
    static constexpr u32 BUTTON_WIDTH = 508;
    static constexpr u32 BUTTON_HEIGHT = 117;
    static constexpr u32 BUTTON_SPACING = 20;
    static constexpr u32 SAVE_ITEM_HEIGHT = 48;

    // Input handling
    DirectionalInputHandler buttonHandler;
    DirectionalInputHandler saveListHandler;
    DirectionalInputHandler gameListHandler;

    // UI Elements
    pu::ui::elm::TextBlock::Ref headerText;
    GameList::Ref gameList;
    FocusableMenu::Ref saveList;
    FocusableButton::Ref loadButton;
    FocusableButton::Ref wirelessButton;

    // Data providers
    std::shared_ptr<ITitleDataProvider> titleProvider;
    std::shared_ptr<ISaveDataProvider> saveProvider;

    // State
    TitleSelectionState selectionState;
    TitleSelectionState lastSelectionState;

    // Event handlers
    void OnSaveSelected();
    void OnLoadButtonClick();
    void OnWirelessButtonClick();
    void OnSaveItemKey();
    void OnInput(u64 down, u64 up, u64 held);

    // Helper methods
    void LoadSaves();
    titles::TitleRef GetSelectedTitle() const;
    pu::i32 GetBottomSectionY() const;  // Helper to calculate Y position for save list and buttons
    
    // Navigation helpers
    void MoveButtonSelectionUp();
    void MoveButtonSelectionDown();
    void TransitionToSaveList();
    void TransitionToButtons();
    void FocusGameSection();
    void FocusSaveList();

public:
    TitleLoadScreen(std::shared_ptr<ITitleDataProvider> titleProvider, std::shared_ptr<ISaveDataProvider> saveProvider);
    PU_SMART_CTOR(TitleLoadScreen)
}; 