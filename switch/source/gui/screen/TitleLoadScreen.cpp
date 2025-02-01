#include "gui/screen/TitleLoadScreen.hpp"
#include "titles/Title.hpp"
#include "gui/UIConstants.hpp"
#include <algorithm>
#include <sys/stat.h>

TitleLoadScreen::TitleLoadScreen(std::shared_ptr<ITitleDataProvider> titleProvider, std::shared_ptr<ISaveDataProvider> saveProvider)
    : Layout::Layout(), titleProvider(titleProvider), saveProvider(saveProvider),
      selectionState(TitleSelectionState::GameCard), lastSelectionState(TitleSelectionState::GameCard) {
    
    // Set background color to match DS version - deeper blue
    this->SetBackgroundColor(UIConstants::BACKGROUND_BLUE);

    // Create header text (initially empty, will be updated in LoadSaves)
    this->headerText = pu::ui::elm::TextBlock::New(0, 0, "");
    this->headerText->SetColor(UIConstants::TEXT_WHITE);
    this->Add(this->headerText);

    // Create game list at the adjusted Y position
    this->gameList = GameList::New(
        GAME_LIST_LEFT_MARGIN, 
        HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN
    );
    this->gameList->SetFocused(true);
    this->gameList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::LoadSaves, this));
    
    // Set initial data
    std::vector<titles::TitleRef> titles;
    auto cartTitle = titleProvider->GetGameCardTitle();
    if (cartTitle) {
        titles.push_back(cartTitle);
    }
    auto installedTitles = titleProvider->GetInstalledTitles();
    titles.insert(titles.end(), installedTitles.begin(), installedTitles.end());
    this->gameList->SetDataSource(titles);
    
    this->Add(this->gameList);

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

    // Create load button
    this->loadButton = FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY(),  // Same Y as save list
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Load Save"
    );

    // Create settings button
    this->wirelessButton = FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY() + BUTTON_HEIGHT + BUTTON_SPACING,  // Below load button
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Wireless"
    );

    // Set up button click handlers
    this->loadButton->SetOnClick(std::bind(&TitleLoadScreen::OnLoadButtonClick, this));
    this->wirelessButton->SetOnClick(std::bind(&TitleLoadScreen::OnWirelessButtonClick, this));

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
    gameListHandler.SetOnMoveDown([this]() { FocusSaveList(); });

    // Load initial saves
    this->LoadSaves();
}

void TitleLoadScreen::LoadSaves() {
    auto title = GetSelectedTitle();
    if (title) {
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
        this->saveList->SetDataSource(saves);
    }
}

titles::TitleRef TitleLoadScreen::GetSelectedTitle() const {
    return this->gameList->GetSelectedTitle();
}

void TitleLoadScreen::MoveButtonSelectionUp() {
    if (this->wirelessButton->IsFocused()) {
        this->loadButton->SetFocused(true);
        this->wirelessButton->SetFocused(false);
    }
    else if (this->loadButton->IsFocused()) {
        this->wirelessButton->SetFocused(true);
        this->loadButton->SetFocused(false);
    }
}

void TitleLoadScreen::MoveButtonSelectionDown() {
    if (this->loadButton->IsFocused()) {
        this->loadButton->SetFocused(false);
        this->wirelessButton->SetFocused(true);
    }
    else if (this->wirelessButton->IsFocused()) {
        this->wirelessButton->SetFocused(false);
        this->loadButton->SetFocused(true);
    }
}

void TitleLoadScreen::FocusGameSection() {
    this->saveList->SetFocused(false);  // This will automatically store position
    selectionState = lastSelectionState;
    this->gameList->SetFocused(true);
}

void TitleLoadScreen::FocusSaveList() {
    lastSelectionState = selectionState;
    this->gameList->SetFocused(false);
    selectionState = TitleSelectionState::InSaveList;
    this->saveList->SetFocused(true);  // This will automatically restore position
}

void TitleLoadScreen::TransitionToSaveList() {
    FocusSaveList();
}

void TitleLoadScreen::TransitionToButtons() {
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
                this->loadButton->SetFocused(false);
                this->wirelessButton->SetFocused(false);
                this->saveList->SetFocused(true);
            }
            else if (down & HidNpadButton_A) {
                if (this->loadButton->IsFocused()) {
                    this->OnLoadButtonClick();
                } else if (this->wirelessButton->IsFocused()) {
                    this->OnWirelessButtonClick();
                }
            }
        } else {
            // Save list is focused
            this->saveList->SetFocused(true);
            
            if (saveListHandler.HandleInput(down, held)) {
                // Input was handled by directional handler
            }
            else if (down & HidNpadButton_B) {
                FocusGameSection();
            }
            else if (down & HidNpadButton_A) {
                TransitionToButtons();
            }
        }
    } else {
        // Game selection mode
        if (gameListHandler.HandleInput(down, held)) {
            // Input was handled by directional handler
        }
        else if (down & HidNpadButton_A) {
            FocusSaveList();
        }
    }
}

void TitleLoadScreen::OnLoadButtonClick() {
    // TODO: Implement load functionality
}

void TitleLoadScreen::OnWirelessButtonClick() {
    // TODO: Implement settings functionality
}

void TitleLoadScreen::OnSaveSelected() {
    // TODO: This is when the save to be loaded on load press would be stored
}

pu::i32 TitleLoadScreen::GetBottomSectionY() const {
    return HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN + this->gameList->GetHeight() + GAME_LIST_BOTTOM_MARGIN;
}