#include "gui/screens/title-load-screen/TitleLoadScreen.hpp"

#include <algorithm>
#include <sys/stat.h>

#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/shared/UIConstants.hpp"
#include "titles/Title.hpp"
#include "utils/Logger.hpp"

pksm::layout::TitleLoadScreen::TitleLoadScreen(
    ITitleDataProvider::Ref titleProvider,
    ISaveDataProvider::Ref saveProvider,
    data::AccountManager& accountManager
)
  : Layout::Layout(), titleProvider(titleProvider), saveProvider(saveProvider), accountManager(accountManager) {
    LOG_DEBUG("Initializing TitleLoadScreen...");

    // Initialize focus managers
    titleLoadFocusManager = pksm::input::FocusManager::New("TitleLoadScreen Manager");
    titleLoadFocusManager->SetActive(true);  // since this is the root manager
    gameListManager = pksm::input::FocusManager::New("GameList Manager");

    // Set up focus manager hierarchy
    titleLoadFocusManager->RegisterChildManager(gameListManager);

    // Set background color to match DS version - deeper blue
    this->SetBackgroundColor(pksm::ui::global::BACKGROUND_BLUE);

    // Create user icon button in top left corner
    this->userIconButton = pksm::ui::UserIconButton::New(
        USER_ICON_MARGIN,
        HEADER_TOP_MARGIN + (HEADER_HEIGHT - USER_ICON_SIZE) / 2,  // Vertically center in header
        USER_ICON_SIZE,
        accountManager
    );
    this->userIconButton->SetName("UserIconButton Element");
    this->userIconButton->SetOnClick([this]() { this->accountManager.ShowAccountSelector(); });
    this->accountManager.SetOnAccountSelected([this](AccountUid) { this->userIconButton->UpdateAccountInfo(); });
    titleLoadFocusManager->RegisterFocusable(this->userIconButton);
    this->Add(this->userIconButton);

    // Create header text (initially empty, will be updated in LoadSaves)
    this->headerText = pu::ui::elm::TextBlock::New(0, 0, "");
    this->headerText->SetColor(pksm::ui::global::TEXT_WHITE);
    this->headerText->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TITLE));
    this->Add(this->headerText);

    // Set up UI components
    this->gameList = pksm::ui::GameList::New(
        GAME_LIST_SIDE_MARGIN,
        HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN,
        GetWidth() - (GAME_LIST_SIDE_MARGIN * 2),  // Width is screen width minus margins
        GetHeight() - (HEADER_TOTAL_VERTICAL_SPACE + SAVE_LIST_TOTAL_VERTICAL_SPACE),
        gameListManager,
        titleProvider
    );
    this->gameList->SetName("GameList Element");
    this->gameList->EstablishOwningRelationship();
    this->gameList->RequestFocus();
    this->gameList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::LoadSaves, this));
    this->gameList->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnGameTouchSelect, this));

    this->Add(this->gameList);

    LOG_DEBUG("Setting up UI components...");
    // Create save list menu with a dark semi-transparent background
    this->saveList = pksm::ui::SaveList::New(
        SAVE_LIST_X,
        GetBottomSectionY(),
        SAVE_LIST_WIDTH,
        pu::ui::Color(0, 0, 0, 200),  // Semi-transparent black
        pu::ui::Color(0, 150, 255, 255),  // Selection color
        SAVE_ITEM_HEIGHT,
        SAVE_LIST_MAX_VISIBLE_ITEMS  // Show 5 items at once
    );
    this->saveList->SetName("SaveList Element");
    // Enable scrolling and selection handling
    this->saveList->SetScrollbarColor(pu::ui::Color(0, 150, 255, 255));
    this->saveList->SetScrollbarWidth(16);
    this->saveList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::OnSaveSelected, this));
    this->saveList->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnSaveListTouchSelect, this));
    titleLoadFocusManager->RegisterFocusable(this->saveList);

    // Create load button
    this->loadButton = pksm::ui::FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY(),  // Same Y as save list
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Load Save"
    );
    this->loadButton->SetName("LoadSaveButton Element");
    this->loadButton->SetOnClick(std::bind(&TitleLoadScreen::OnLoadButtonClick, this));
    this->loadButton->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnLoadButtonClick, this));
    this->loadButton->SetContentFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_BUTTON));
    titleLoadFocusManager->RegisterFocusable(this->loadButton);

    // Create settings button
    this->wirelessButton = pksm::ui::FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY() + BUTTON_HEIGHT + BUTTON_SPACING,  // Below load button
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Wireless"
    );
    this->wirelessButton->SetName("WirelessButton Element");
    this->wirelessButton->SetOnClick(std::bind(&TitleLoadScreen::OnWirelessButtonClick, this));
    this->wirelessButton->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnWirelessButtonClick, this));
    this->wirelessButton->SetContentFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_BUTTON));
    titleLoadFocusManager->RegisterFocusable(this->wirelessButton);

    // Add elements to layout
    this->Add(this->saveList);
    this->Add(this->loadButton);
    this->Add(this->wirelessButton);

    // Let container prepare elements
    this->PreRender();

    // Set up input handling for the layout
    this->SetOnInput(
        std::bind(&TitleLoadScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    // Set up button handler
    buttonHandler.SetOnMoveUp([this]() { MoveButtonSelectionUp(); });
    buttonHandler.SetOnMoveDown([this]() { MoveButtonSelectionDown(); });
    buttonHandler.SetOnMoveLeft([this]() { this->saveList->RequestFocus(); });

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

    gameListHandler.SetOnMoveUp([this]() {
        if (gameList->ShouldResignUpFocus()) {
            this->userIconButton->RequestFocus();
        }
    });

    userIconButtonHandler.SetOnMoveDown([this]() { gameList->RequestFocus(); });

    // Load initial saves
    this->LoadSaves();

    // Set initial focus
    this->gameList->RequestFocus();

    LOG_DEBUG("TitleLoadScreen initialization complete");
}

void pksm::layout::TitleLoadScreen::LoadSaves() {
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
        this->headerText->SetX((GetWidth() - textWidth) / 2);

        // Vertical center within header area
        this->headerText->SetY(HEADER_TOP_MARGIN + (HEADER_HEIGHT - textHeight) / 2);

        // Update save list
        auto saves = saveProvider->GetSavesForTitle(title);
        LOG_DEBUG("Found " + std::to_string(saves.size()) + " saves for title");
        this->saveList->SetDataSource(saves);
    }
}

pksm::titles::Title::Ref pksm::layout::TitleLoadScreen::GetSelectedTitle() const {
    return this->gameList->GetSelectedTitle();
}

void pksm::layout::TitleLoadScreen::MoveButtonSelectionUp() {
    if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection up from Wireless to Load Save");
        this->loadButton->RequestFocus();
    } else if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection up from Load Save to Wireless");
        this->wirelessButton->RequestFocus();
    }
}

void pksm::layout::TitleLoadScreen::MoveButtonSelectionDown() {
    if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection down from Load Save to Wireless");
        this->wirelessButton->RequestFocus();
    } else if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection down from Wireless to Load Save");
        this->loadButton->RequestFocus();
    }
}

void pksm::layout::TitleLoadScreen::FocusGameSection() {
    LOG_DEBUG("Focusing game section");
    this->gameList->RequestFocus();
}

void pksm::layout::TitleLoadScreen::FocusSaveList() {
    LOG_DEBUG("Focusing save list");
    this->saveList->RequestFocus();
}

void pksm::layout::TitleLoadScreen::TransitionToSaveList() {
    LOG_DEBUG("Transitioning focus to save list");
    FocusSaveList();
}

void pksm::layout::TitleLoadScreen::TransitionToButtons() {
    LOG_DEBUG("Transitioning focus to button region");
    this->loadButton->RequestFocus();
}

void pksm::layout::TitleLoadScreen::OnInput(u64 down, u64 up, u64 held) {
    if (this->userIconButton->IsFocused()) {
        if (userIconButtonHandler.HandleInput(down, held)) {
            // Input was handled by directional handler
        } else if (down & HidNpadButton_A) {
            LOG_DEBUG("User icon button activated via A button");
            this->accountManager.ShowAccountSelector();
        } else if (down & HidNpadButton_B || down & HidNpadButton_Down) {
            LOG_DEBUG("Moving focus from user icon to game list");
            FocusGameSection();
        }
    } else if (this->loadButton->IsFocused() || this->wirelessButton->IsFocused()) {
        // Handle button group navigation
        if (buttonHandler.HandleInput(down, held)) {
            // Input was handled by directional handler
        } else if (down & HidNpadButton_B) {
            LOG_DEBUG("Returning to save list from button region");
            this->saveList->RequestFocus();
        } else if (down & HidNpadButton_A) {
            if (this->loadButton->IsFocused()) {
                LOG_DEBUG("Load button activated via A button");
                this->OnLoadButtonClick();
            } else if (this->wirelessButton->IsFocused()) {
                LOG_DEBUG("Wireless button activated via A button");
                this->OnWirelessButtonClick();
            }
        }
    } else if (saveList->IsFocused()) {
        // Save list is focused
        if (saveListHandler.HandleInput(down, held)) {
            // Input was handled by directional handler
        } else if (down & HidNpadButton_B) {
            LOG_DEBUG("Returning to game selection from save list");
            FocusGameSection();
        } else if (down & HidNpadButton_A) {
            LOG_DEBUG("Transitioning from save list to button region");
            TransitionToButtons();
        }
    } else if (gameList->IsFocused()) {
        // Game selection mode
        if (gameListHandler.HandleInput(down, held)) {
            // Input was handled by directional handler
        } else if (gameList->HandleNonDirectionalInput(down, up, held)) {
            // Input was handled by internal game list handler
        } else if (down & HidNpadButton_A) {
            LOG_DEBUG("Transitioning from game selection to save list");
            FocusSaveList();
        } else if (down & HidNpadButton_Up) {
            LOG_DEBUG("Moving focus from game list to user icon");
            this->userIconButton->RequestFocus();
        }
    }
}

void pksm::layout::TitleLoadScreen::HandleButtonInteraction(pksm::ui::FocusableButton::Ref& buttonToFocus) {
    LOG_DEBUG("Handling button interaction, focusing " + buttonToFocus->GetContent());
    buttonToFocus->RequestFocus();
}

void pksm::layout::TitleLoadScreen::OnLoadButtonClick() {
    LOG_DEBUG("Load button clicked");
    HandleButtonInteraction(this->loadButton);
}

void pksm::layout::TitleLoadScreen::OnWirelessButtonClick() {
    LOG_DEBUG("Wireless button clicked");
    HandleButtonInteraction(this->wirelessButton);
}

void pksm::layout::TitleLoadScreen::OnSaveSelected() {
    LOG_DEBUG("Save selected: " + saveList->GetSelectedItemText());
}

void pksm::layout::TitleLoadScreen::OnGameTouchSelect() {
    LOG_DEBUG("Game selected via touch");
}

void pksm::layout::TitleLoadScreen::OnSaveListTouchSelect() {
    LOG_DEBUG("Save list selected via touch");
    // When save list is touched, focus it but preserve game selection
    FocusSaveList();
}

pu::i32 pksm::layout::TitleLoadScreen::GetBottomSectionY() const {
    return HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN + this->gameList->GetHeight() +
        SAVE_LIST_TOP_MARGIN;
}