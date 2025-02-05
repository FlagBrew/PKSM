#include "gui/screen/TitleLoadScreen.hpp"
#include "titles/Title.hpp"
#include "gui/UIConstants.hpp"
#include "gui/GameList.hpp"
#include <algorithm>
#include <sys/stat.h>
#include "utils/Logger.hpp"

TitleLoadScreen::TitleLoadScreen(std::shared_ptr<ITitleDataProvider> titleProvider, std::shared_ptr<ISaveDataProvider> saveProvider)
    : Layout::Layout(), titleProvider(titleProvider), saveProvider(saveProvider),
      selectionState(TitleSelectionState::GameCard), lastSelectionState(TitleSelectionState::GameCard) {
    
    LOG_DEBUG("Initializing TitleLoadScreen...");
    
    // Set background color to match DS version - deeper blue
    this->SetBackgroundColor(UIConstants::BACKGROUND_BLUE);

    // Create header text (initially empty, will be updated in LoadSaves)
    this->headerText = pu::ui::elm::TextBlock::New(0, 0, "");
    this->headerText->SetColor(UIConstants::TEXT_WHITE);
    this->headerText->SetFont(UIConstants::MakeHeavyFontName(UIConstants::FONT_SIZE_TITLE));
    this->Add(this->headerText);

    // Create game list at the adjusted Y position
    this->gameList = GameList::New(
        GAME_LIST_LEFT_MARGIN, 
        HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN
    );
    this->gameList->SetFocused(true);
    this->gameList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::LoadSaves, this));
    this->gameList->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnGameTouchSelect, this));
    
    // Set initial data
    std::vector<titles::TitleRef> titles;
    auto cartTitle = titleProvider->GetGameCardTitle();
    if (cartTitle) {
        LOG_DEBUG("Game card title found: " + cartTitle->getName());
        titles.push_back(cartTitle);
    } else {
        LOG_DEBUG("No game card title found");
    }
    
    auto installedTitles = titleProvider->GetInstalledTitles();
    LOG_DEBUG("Found " + std::to_string(installedTitles.size()) + " installed titles");
    titles.insert(titles.end(), installedTitles.begin(), installedTitles.end());
    this->gameList->SetDataSource(titles);
    
    this->Add(this->gameList);

    LOG_DEBUG("Setting up UI components...");
    // Create save list menu with a dark semi-transparent background
    this->saveList = FocusableMenu::New(
        SAVE_LIST_X,
        GetBottomSectionY(),
        SAVE_LIST_WIDTH,
        pu::ui::Color(0, 0, 0, 200),  // Semi-transparent black
        pu::ui::Color(0, 150, 255, 255),  // Selection color
        SAVE_ITEM_HEIGHT,
        5  // Show 5 items at once
    );

    // Enable scrolling and selection handling
    this->saveList->SetScrollbarColor(pu::ui::Color(0, 150, 255, 255));
    this->saveList->SetScrollbarWidth(16);
    this->saveList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::OnSaveSelected, this));
    this->saveList->SetFocused(false);  // Initially not focused
    this->saveList->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnSaveListTouchSelect, this));

    // Create load button
    this->loadButton = FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY(),  // Same Y as save list
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Load Save"
    );
    this->loadButton->SetOnClick(std::bind(&TitleLoadScreen::OnLoadButtonClick, this));
    this->loadButton->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnLoadButtonClick, this));
    this->loadButton->SetContentFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_BUTTON));

    // Create settings button
    this->wirelessButton = FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY() + BUTTON_HEIGHT + BUTTON_SPACING,  // Below load button
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Wireless"
    );
    this->wirelessButton->SetOnClick(std::bind(&TitleLoadScreen::OnWirelessButtonClick, this));
    this->wirelessButton->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnWirelessButtonClick, this));
    this->wirelessButton->SetContentFont(UIConstants::MakeMediumFontName(UIConstants::FONT_SIZE_BUTTON));

    // Add elements to layout
    this->Add(this->saveList);
    this->Add(this->loadButton);
    this->Add(this->wirelessButton);

    // Set up input handling for the layout
    this->SetOnInput(std::bind(&TitleLoadScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // Set up button handler
    buttonHandler.SetOnMoveUp([this]() { MoveButtonSelectionUp(); });
    buttonHandler.SetOnMoveDown([this]() { MoveButtonSelectionDown(); });
    buttonHandler.SetOnMoveLeft([this]() { 
        this->loadButton->SetFocused(false);
        this->wirelessButton->SetFocused(false);
        this->saveList->SetFocused(true);
    });

    // Set up save list handler
    saveListHandler.SetOnMoveRight([this]() { TransitionToButtons(); });
    saveListHandler.SetOnMoveUp([this]() {
        // Only move up to games if we're at the top of the list
        if (this->saveList->GetSelectedIndex() == 0) {
            FocusGameSection();
        }
    });

    // Set up game list handler
    gameListHandler.SetOnMoveDown([this]() {
        if (gameList->ShouldResignDownFocus()) {
            FocusSaveList();
        }
    });

    // Load initial saves
    this->LoadSaves();

    LOG_DEBUG("TitleLoadScreen initialization complete");
}

void TitleLoadScreen::LoadSaves() {
    auto title = GetSelectedTitle();
    if (title) {
        LOG_DEBUG("Loading saves for title: " + title->getName());
        
        // Update header text with selected title
        std::string titleText = title->getName();
        this->headerText->SetText(titleText);
        
        // Center the header text horizontally and vertically
        pu::i32 textWidth = this->headerText->GetWidth();
        pu::i32 textHeight = this->headerText->GetHeight();
        
        // Horizontal center
        this->headerText->SetX((SCREEN_WIDTH - textWidth) / 2);
        
        // Vertical center within header area
        this->headerText->SetY(HEADER_TOP_MARGIN + (HEADER_HEIGHT - textHeight) / 2);
        
        // Update save list
        auto saves = saveProvider->GetSavesForTitle(title);
        LOG_DEBUG("Found " + std::to_string(saves.size()) + " saves for title");
        this->saveList->SetDataSource(saves);
    }
}

titles::TitleRef TitleLoadScreen::GetSelectedTitle() const {
    return this->gameList->GetSelectedTitle();
}

void TitleLoadScreen::MoveButtonSelectionUp() {
    if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection up from Wireless to Load Save");
        this->loadButton->SetFocused(true);
        this->wirelessButton->SetFocused(false);
    }
    else if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection up from Load Save to Wireless");
        this->wirelessButton->SetFocused(true);
        this->loadButton->SetFocused(false);
    }
}

void TitleLoadScreen::MoveButtonSelectionDown() {
    if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection down from Load Save to Wireless");
        this->loadButton->SetFocused(false);
        this->wirelessButton->SetFocused(true);
    }
    else if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection down from Wireless to Load Save");
        this->wirelessButton->SetFocused(false);
        this->loadButton->SetFocused(true);
    }
}

void TitleLoadScreen::FocusGameSection() {
    LOG_DEBUG("Focusing game section");
    this->saveList->SetFocused(false);
    selectionState = lastSelectionState;
    this->gameList->SetFocused(true);
}

void TitleLoadScreen::FocusSaveList() {
    LOG_DEBUG("Focusing save list");
    lastSelectionState = selectionState;
    this->gameList->SetFocused(false);
    selectionState = TitleSelectionState::InSaveList;
    this->saveList->SetFocused(true);
}

void TitleLoadScreen::TransitionToSaveList() {
    LOG_DEBUG("Transitioning focus to save list");
    FocusSaveList();
}

void TitleLoadScreen::TransitionToButtons() {
    LOG_DEBUG("Transitioning focus to button region");
    this->saveList->SetFocused(false);
    this->loadButton->SetFocused(true);
}

void TitleLoadScreen::OnInput(u64 down, u64 up, u64 held) {
    if (selectionState == TitleSelectionState::InSaveList) {
        if (this->loadButton->IsFocused() || this->wirelessButton->IsFocused()) {
            // Handle button group navigation
            if (buttonHandler.HandleInput(down, held)) {
                // Input was handled by directional handler
            }
            else if (down & HidNpadButton_B) {
                LOG_DEBUG("Returning to save list from button region");
                this->loadButton->SetFocused(false);
                this->wirelessButton->SetFocused(false);
                this->saveList->SetFocused(true);
            }
            else if (down & HidNpadButton_A) {
                if (this->loadButton->IsFocused()) {
                    LOG_DEBUG("Load button activated via A button");
                    this->OnLoadButtonClick();
                } else if (this->wirelessButton->IsFocused()) {
                    LOG_DEBUG("Wireless button activated via A button");
                    this->OnWirelessButtonClick();
                }
            }
        } else {
            // Save list is focused
            if (saveListHandler.HandleInput(down, held)) {
                // Input was handled by directional handler
            }
            else if (down & HidNpadButton_B) {
                LOG_DEBUG("Returning to game selection from save list");
                FocusGameSection();
            }
            else if (down & HidNpadButton_A) {
                LOG_DEBUG("Transitioning from save list to button region");
                TransitionToButtons();
            }
        }
    } else {
        // Game selection mode
        if (gameListHandler.HandleInput(down, held)) {
            // Input was handled by directional handler
        }
        else if (down & HidNpadButton_A) {
            LOG_DEBUG("Transitioning from game selection to save list");
            FocusSaveList();
        }
    }
}

void TitleLoadScreen::HandleButtonInteraction(FocusableButton::Ref& buttonToFocus) {
    LOG_DEBUG("Handling button interaction, focusing " + buttonToFocus->GetContent());
    
    // When a button is touched/clicked, focus the button area and update selection state
    this->gameList->SetFocused(false);
    this->saveList->SetFocused(false);
    selectionState = TitleSelectionState::InSaveList;  // We're in the save/button section
    
    // Unfocus all buttons
    this->loadButton->SetFocused(false);
    this->wirelessButton->SetFocused(false);
    
    // Focus the selected button
    buttonToFocus->SetFocused(true);
}

void TitleLoadScreen::OnLoadButtonClick() {
    LOG_DEBUG("Load button clicked");
    HandleButtonInteraction(this->loadButton);
    // TODO: Implement load functionality
}

void TitleLoadScreen::OnWirelessButtonClick() {
    LOG_DEBUG("Wireless button clicked");
    HandleButtonInteraction(this->wirelessButton);
    // TODO: Implement settings functionality
}

void TitleLoadScreen::OnSaveSelected() {
    LOG_DEBUG("Save selected: " + saveList->GetSelectedItemText());
}

void TitleLoadScreen::OnGameTouchSelect() {
    LOG_DEBUG("Game selected via touch, restoring game selection mode");
    // When a game is selected via touch, ensure we're in game selection mode
    // and other components are unfocused
    this->loadButton->SetFocused(false);
    this->wirelessButton->SetFocused(false);
    this->saveList->SetFocused(false);
    selectionState = lastSelectionState;  // Restore the game selection state
    this->gameList->SetFocused(true);
}

void TitleLoadScreen::OnSaveListTouchSelect() {
    LOG_DEBUG("Save list selected via touch");
    // When save list is touched, focus it but preserve game selection
    this->loadButton->SetFocused(false);
    this->wirelessButton->SetFocused(false);
    this->gameList->SetFocused(false);
    lastSelectionState = selectionState;  // Store current game selection state
    selectionState = TitleSelectionState::InSaveList;
    this->saveList->SetFocused(true);
}

pu::i32 TitleLoadScreen::GetBottomSectionY() const {
    return HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN + this->gameList->GetHeight() + GAME_LIST_BOTTOM_MARGIN;
}