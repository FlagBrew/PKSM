#include "gui/screen/TitleLoadScreen.hpp"
#include "titles/Title.hpp"
#include <algorithm>
#include <sys/stat.h>

TitleLoadScreen::TitleLoadScreen() : Layout::Layout() {
    // Initialize mock data
    mockCartridgeTitle = std::make_shared<titles::Title>(
        "Pokémon Ultra Sun", 
        "romfs:/gfx/ui/gameselector_cart.png", 
        0x00175E00
    );

    mockInstalledTitles = {
        std::make_shared<titles::Title>("Pokémon Sun", "romfs:/gfx/ui/gameselector_cart.png", 0x00164800),
        std::make_shared<titles::Title>("Pokémon Moon", "romfs:/gfx/ui/gameselector_cart.png", 0x00164900),
        std::make_shared<titles::Title>("Pokémon Ultra Moon", "romfs:/gfx/ui/gameselector_cart.png", 0x00175F00)
    };

    selectedTitle = -1;  // Start with game card selected
    lastSelectedTitle = -1;  // Initialize last selected title
    
    // Set background color to match DS version - deeper blue
    this->SetBackgroundColor(pu::ui::Color(10, 15, 75, 255));

    // Create header text
    this->headerText = pu::ui::elm::TextBlock::New(
        10, 10, "Choose a save to open. Press Y for absent games.");
    this->headerText->SetColor(pu::ui::Color(255, 255, 255, 255));
    this->Add(this->headerText);

    // Add vertical divider between sections
    auto divider = pu::ui::elm::Rectangle::New(
        SECTION_DIVIDER, HEADER_HEIGHT + HEADER_BOTTOM_MARGIN,
        2, (SAVE_LIST_Y - HEADER_HEIGHT) - SAVE_LIST_TOP_MARGIN,
        pu::ui::Color(255, 255, 255, 128)  // Semi-transparent white
    );
    this->Add(divider);

    // Add cartridge section text
    this->cartridgeText = pu::ui::elm::TextBlock::New(
        GAME_CARD_X + (GAME_CARD_SIZE/2 - 105), HEADER_HEIGHT + HEADER_BOTTOM_MARGIN, "Game Card");
    this->cartridgeText->SetColor(pu::ui::Color(255, 255, 255, 255));
    this->Add(this->cartridgeText);

    // Add installed games text
    this->installedText = pu::ui::elm::TextBlock::New(
        INSTALLED_START_X + (INSTALLED_GAME_SIZE/2 + 165), HEADER_HEIGHT + HEADER_BOTTOM_MARGIN, "Installed Games");
    this->installedText->SetColor(pu::ui::Color(255, 255, 255, 255));
    this->Add(this->installedText);

    // Create game card image
    if (mockCartridgeTitle) {
        this->gameCardImage = FocusableImage::New(
            GAME_CARD_X,
            GAME_SECTION_Y,
            mockCartridgeTitle->getIcon(),
            128,  // Default alpha
            GAME_OUTLINE_PADDING  // Add padding to the outline
        );
        gameCardImage->SetWidth(GAME_CARD_SIZE);
        gameCardImage->SetHeight(GAME_CARD_SIZE);
        gameCardImage->SetSelected(true);  // Initially selected
        gameCardImage->SetFocused(true);   // Initially focused
        this->Add(gameCardImage);
        this->Add(gameCardImage->GetOverlay());
    }

    // Add installed game images
    for (size_t i = 0; i < mockInstalledTitles.size(); i++) {
        auto gameImage = FocusableImage::New(
            INSTALLED_START_X + (i * GAME_SPACING),
            GAME_SECTION_Y,
            mockInstalledTitles[i]->getIcon(),
            128,  // Default alpha
            GAME_OUTLINE_PADDING  // Add padding to the outline
        );
        gameImage->SetWidth(INSTALLED_GAME_SIZE);
        gameImage->SetHeight(INSTALLED_GAME_SIZE);
        gameImage->SetFocused(false);
        this->Add(gameImage);
        this->Add(gameImage->GetOverlay());
        installedGameImages.push_back(gameImage);
    }

    // Create save list menu with a dark semi-transparent background
    this->saveList = FocusableMenu::New(
        SAVE_LIST_X,
        SAVE_LIST_Y,
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
        SAVE_LIST_Y,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Load Save"
    );

    // Create settings button
    this->wirelessButton = FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        SAVE_LIST_Y + BUTTON_HEIGHT + BUTTON_SPACING,
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

    // Set up game selection handler
    gameSelectionHandler.SetOnMoveLeft([this]() { MoveGameSelectionLeft(); });
    gameSelectionHandler.SetOnMoveRight([this]() { MoveGameSelectionRight(); });
    gameSelectionHandler.SetOnMoveDown([this]() { FocusSaveList(); });

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

    // Load initial saves
    this->LoadSaves();
}

void TitleLoadScreen::LoadSaves() {
    // Prepare the data source based on selected title
    std::vector<std::string> saveItems;
    std::string prefix;
    
    if (selectedTitle == -1) {
        // Game card - Ultra Sun
        prefix = "Cartridge";
        saveItems = {
            prefix + " Main Save",
            prefix + " Challenge Mode",
            prefix + " Nuzlocke"
        };
    } else {
        switch(selectedTitle) {
            case 0: // Sun
                prefix = "Digital";
                saveItems = {
                    prefix + " Main Save",
                    prefix + " Speedrun",
                    prefix + " Living Dex",
                    prefix + " Shiny Hunt",
                    prefix + " Competitive",
                    prefix + " Nuzlocke",
                    prefix + " Battle Tree"
                };
                break;
            case 1: // Moon
                prefix = "Digital";
                saveItems = {
                    prefix + " Main Save",
                    prefix + " Nuzlocke"
                };
                break;
            case 2: // Ultra Moon
                prefix = "Digital";
                saveItems = {
                    prefix + " Main Save",
                    prefix + " Challenge Mode",
                    prefix + " Speedrun",
                    prefix + " Living Dex",
                    prefix + " Shiny Hunt",
                    prefix + " Competitive",
                    prefix + " Nuzlocke",
                    prefix + " Battle Tree"
                };
                break;
            default:
                prefix = "Unknown";
                saveItems = { prefix + " Main Save" };
        }
    }
    
    // Update the menu's data source
    this->saveList->SetDataSource(saveItems);
}

void TitleLoadScreen::MoveGameSelectionLeft() {
    if (selectedTitle > 0) {
        selectedTitle--;
    } else if (selectedTitle == 0) {
        selectedTitle = -1;
    }
    UpdateGameHighlights();
}

void TitleLoadScreen::MoveGameSelectionRight() {
    if (selectedTitle == -1) {
        selectedTitle = 0;
    } else if (selectedTitle >= 0 && selectedTitle < (int)installedGameImages.size() - 1) {
        selectedTitle++;
    }
    UpdateGameHighlights();
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
    selectedTitle = lastSelectedTitle; // Restore last game selection
    UpdateGameHighlights();
}

void TitleLoadScreen::FocusSaveList() {
    lastSelectedTitle = selectedTitle; // Store for returning from save list
    bool inGameSelection = false;
    if (mockCartridgeTitle) {
        this->gameCardImage->SetFocused(inGameSelection);
    }
    for (auto& image : installedGameImages) {
        image->SetFocused(inGameSelection);
    }
    
    selectedTitle = -2;  // Move to save list
    this->saveList->SetFocused(true);  // This will automatically restore position
}

void TitleLoadScreen::TransitionToSaveList() {
    FocusSaveList();
}

void TitleLoadScreen::TransitionToButtons() {
    this->saveList->SetFocused(false);
    this->loadButton->SetFocused(true);
}

void TitleLoadScreen::UpdateGameHighlights() {
    bool inGameSelection = selectedTitle >= -1 && selectedTitle != -2;  // True when actively selecting games
    
    // Update game card highlight
    if (mockCartridgeTitle) {
        this->gameCardImage->SetSelected(selectedTitle == -1);  // Remove overlay when selected
        this->gameCardImage->SetFocused(inGameSelection);      // Only pulse when actively navigating games
    }

    // Update installed games highlight
    for (size_t i = 0; i < installedGameImages.size(); i++) {
        installedGameImages[i]->SetSelected(selectedTitle == (int)i);  // Remove overlay when selected
        installedGameImages[i]->SetFocused(inGameSelection);          // Only pulse when actively navigating games
    }

    // Update save list if game selection changed
    if (selectedTitle >= -1) {
        LoadSaves();
    }
}

void TitleLoadScreen::OnInput(u64 down, u64 up, u64 held) {
    // Let menu handle its own input when focused
    if (selectedTitle == -2) {
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
        if (gameSelectionHandler.HandleInput(down, held)) {
            // Store last selected title before potential change
            int previousTitle = selectedTitle;
            
            // Input was handled by directional handler
            // If the selected title changed, reset save list
            if (previousTitle != selectedTitle && selectedTitle >= -1) {
                LoadSaves();
            }
        }
        else if (down & HidNpadButton_A) {
            FocusSaveList();
        }
    }
}

void TitleLoadScreen::OnSaveItemKey() {
    auto selected = this->saveList->GetSelectedItem();
    if (selected) {
        // Handle save item key press
    }
}

void TitleLoadScreen::OnLoadButtonClick() {
    // TODO: Implement load functionality
}

void TitleLoadScreen::OnWirelessButtonClick() {
    // TODO: Implement settings functionality
}

void TitleLoadScreen::OnSaveSelected() {
    // Update colors for all items
    auto items = this->saveList->GetItems();
    for (auto& item : items) {
        item->SetColor(pu::ui::Color(255, 255, 255, 255));
    }
    
    // Highlight selected item
    auto selected = this->saveList->GetSelectedItem();
    if (selected) {
        selected->SetColor(pu::ui::Color(0, 255, 0, 255));
    }
}

bool TitleLoadScreen::LoadSelectedSave() {
    // TODO: Implement loading the selected save
    return false;
} 