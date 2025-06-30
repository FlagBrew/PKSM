#include "gui/screens/title-load-screen/TitleLoadScreen.hpp"

#include <algorithm>
#include <sys/stat.h>

#include "gui/screens/title-load-screen/sub-components/game-list/GameListCommon.hpp"
#include "gui/shared/UIConstants.hpp"
#include "titles/Title.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

pksm::layout::TitleLoadScreen::TitleLoadScreen(
    ITitleDataProvider::Ref titleProvider,
    ISaveDataProvider::Ref saveProvider,
    data::AccountManager& accountManager,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    std::function<void(pksm::titles::Title::Ref, pksm::saves::Save::Ref)> onSaveLoaded
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    titleProvider(titleProvider),
    saveProvider(saveProvider),
    accountManager(accountManager),
    onSaveLoaded(onSaveLoaded) {
    LOG_DEBUG("Initializing TitleLoadScreen...");
    this->SetBackgroundColor(pksm::ui::global::BACKGROUND_BLUE);

    // Initialize focus managers
    titleLoadFocusManager = pksm::input::FocusManager::New("TitleLoadScreen Manager");
    titleLoadFocusManager->SetActive(true);  // since this is the root manager
    gameListManager = pksm::input::FocusManager::New("GameList Manager");
    titleLoadFocusManager->RegisterChildManager(gameListManager);

    // Create user icon button
    this->userIconButton = pksm::ui::UserIconButton::New(
        USER_ICON_MARGIN,
        HEADER_TOP_MARGIN + (HEADER_HEIGHT - USER_ICON_SIZE) / 2,  // Vertically center in header
        USER_ICON_SIZE,
        accountManager
    );
    this->userIconButton->SetName("UserIconButton Element");
    this->accountManager.SetOnAccountSelected([this](AccountUid newUserId) {
        this->userIconButton->UpdateAccountInfo();
        this->gameList->OnAccountChanged(newUserId);
    });
    this->userIconButton->SetOnCancel(std::bind(&TitleLoadScreen::FocusGameSection, this));
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
        titleProvider,
        accountManager.GetCurrentAccount()
    );
    this->gameList->SetName("GameList Element");
    this->gameList->EstablishOwningRelationship();
    this->gameList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::LoadSaves, this));
    this->gameList->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnGameTouchSelect, this));
    this->gameList->SetOnSelect(std::bind(&TitleLoadScreen::FocusSaveList, this));
    this->gameList->SetOnGameListChanged(std::bind(&TitleLoadScreen::OnGameListChanged, this));
    this->userIconButton->SetVisible(this->gameList->IsGameListDependentOnUser());

    // Set up help text updates
    this->gameList->SetOnShouldUpdateHelpText([this]() {
        if (this->gameList->IsFocused()) {
            this->UpdateHelpItems(this->gameList);
        }
    });
    this->Add(this->gameList);
    LOG_DEBUG("Setting up UI components...");

    this->saveList = pksm::ui::SaveList::New(SAVE_LIST_X, GetBottomSectionY(), SAVE_LIST_WIDTH);
    this->saveList->SetName("SaveList Element");

    // Enable scrolling and selection handling
    this->saveList->SetOnSelectionChanged(std::bind(&TitleLoadScreen::OnSaveSelected, this));
    this->saveList->SetOnTouchSelect(std::bind(&TitleLoadScreen::OnSaveListTouchSelect, this));
    this->saveList->SetOnCancel(std::bind(&TitleLoadScreen::FocusGameSection, this));
    this->saveList->SetOnSelect(std::bind(&TitleLoadScreen::TransitionToButtons, this));

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
    this->loadButton->SetOnCancel(std::bind(&TitleLoadScreen::FocusSaveList, this));
    this->loadButton->SetContentFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_BUTTON));
    this->loadButton->SetHelpText("Load Save");
    titleLoadFocusManager->RegisterFocusable(this->loadButton);

    // Create wireless button
    this->wirelessButton = pksm::ui::FocusableButton::New(
        SAVE_LIST_X + SAVE_LIST_WIDTH + BUTTON_SPACING,
        GetBottomSectionY() + BUTTON_HEIGHT + BUTTON_SPACING,  // Below load button
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        "Wireless"
    );
    this->wirelessButton->SetName("WirelessButton Element");
    this->wirelessButton->SetOnClick(std::bind(&TitleLoadScreen::OnWirelessButtonClick, this));
    this->wirelessButton->SetOnCancel(std::bind(&TitleLoadScreen::FocusSaveList, this));
    this->wirelessButton->SetContentFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_BUTTON));
    this->wirelessButton->SetHelpText("Load Wireless");
    titleLoadFocusManager->RegisterFocusable(this->wirelessButton);

    // Add elements to layout
    this->Add(this->saveList);
    this->Add(this->loadButton);
    this->Add(this->wirelessButton);
    InitializeHelpFooter();

    // Let container prepare elements
    this->PreRender();

    // Set up input handling for the layout
    this->SetOnInput(
        std::bind(&TitleLoadScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    // Set up button handler
    buttonDirectionalHandler.SetOnMoveUp([this]() { MoveButtonSelectionUp(); });
    buttonDirectionalHandler.SetOnMoveDown([this]() { MoveButtonSelectionDown(); });
    buttonDirectionalHandler.SetOnMoveLeft([this]() { this->FocusSaveList(); });
    buttonDirectionalHandler.SetOnMoveRight([this]() { MoveButtonSelectionRight(); });

    // Set up save list handler
    saveListDirectionalHandler.SetOnMoveLeft([this]() { saveList->shakeOutOfBounds(pksm::ui::ShakeDirection::LEFT); });
    saveListDirectionalHandler.SetOnMoveRight([this]() { TransitionToButtons(); });
    saveListDirectionalHandler.SetOnMoveUp([this]() {
        if (this->saveList->ShouldResignUpFocus()) {
            FocusGameSection();
        }
    });

    // Set up game list handler
    gameListDirectionalHandler.SetOnMoveDown([this]() {
        if (gameList->ShouldResignDownFocus()) {
            FocusSaveList();
        }
    });
    gameListDirectionalHandler.SetOnMoveUp([this]() {
        if (gameList->ShouldResignUpFocus()) {
            this->FocusUserIcon();
        }
    });

    userIconButtonDirectionalHandler.SetOnMoveDown([this]() { gameList->RequestFocus(); });
    userIconButtonDirectionalHandler.SetOnMoveRight([this]() {
        this->userIconButton->shakeOutOfBounds(pksm::ui::ShakeDirection::RIGHT);
    });
    userIconButtonDirectionalHandler.SetOnMoveLeft([this]() {
        this->userIconButton->shakeOutOfBounds(pksm::ui::ShakeDirection::LEFT);
    });
    userIconButtonDirectionalHandler.SetOnMoveUp([this]() {
        this->userIconButton->shakeOutOfBounds(pksm::ui::ShakeDirection::UP);
    });

    this->LoadSaves();  // Load initial saves
    this->gameList->RequestFocus();  // Set initial focus
    UpdateHelpItems(this->gameList);  // Update help items
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

        // Update save list with current user
        auto saves = saveProvider->GetSavesForTitle(title, accountManager.GetCurrentAccount());
        LOG_DEBUG("Found " + std::to_string(saves.size()) + " saves for title");
        this->saveList->SetDataSource(saves);
    }
}

pksm::titles::Title::Ref pksm::layout::TitleLoadScreen::GetSelectedTitle() const {
    return this->gameList->GetSelectedTitle();
}

pksm::saves::Save::Ref pksm::layout::TitleLoadScreen::GetSelectedSave() const {
    int selectedIndex = this->saveList->GetSelectedIndex();
    if (selectedIndex >= 0) {
        auto saves = saveProvider->GetSavesForTitle(GetSelectedTitle(), accountManager.GetCurrentAccount());
        if (selectedIndex < static_cast<int>(saves.size())) {
            return saves[selectedIndex];
        }
    }
    return nullptr;
}

void pksm::layout::TitleLoadScreen::MoveButtonSelectionUp() {
    if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection up from Wireless to Load Save");
        this->FocusLoadButton();
    } else if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection up from Load Save to Wireless");
        this->loadButton->shakeOutOfBounds(pksm::ui::ShakeDirection::UP);
    }
}

void pksm::layout::TitleLoadScreen::MoveButtonSelectionDown() {
    if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection down from Load Save to Wireless");
        this->FocusWirelessButton();
    } else if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection down from Wireless to Load Save");
        this->wirelessButton->shakeOutOfBounds(pksm::ui::ShakeDirection::DOWN);
    }
}

void pksm::layout::TitleLoadScreen::MoveButtonSelectionRight() {
    if (this->wirelessButton->IsFocused()) {
        LOG_DEBUG("Moving button selection right from Load Save to Wireless");
        this->wirelessButton->shakeOutOfBounds(pksm::ui::ShakeDirection::RIGHT);
    } else if (this->loadButton->IsFocused()) {
        LOG_DEBUG("Moving button selection right from Load Save to Wireless");
        this->loadButton->shakeOutOfBounds(pksm::ui::ShakeDirection::RIGHT);
    }
}

void pksm::layout::TitleLoadScreen::FocusGameSection() {
    LOG_DEBUG("Focusing game section");
    this->gameList->RequestFocus();
    UpdateHelpItems(this->gameList);
}

void pksm::layout::TitleLoadScreen::FocusSaveList() {
    LOG_DEBUG("Focusing save list");
    this->saveList->RequestFocus();
    UpdateHelpItems(this->saveList);
}

void pksm::layout::TitleLoadScreen::FocusUserIcon() {
    LOG_DEBUG("Focusing user icon");
    this->userIconButton->RequestFocus();
    UpdateHelpItems(this->userIconButton);
}

void pksm::layout::TitleLoadScreen::FocusLoadButton() {
    LOG_DEBUG("Focusing load button");
    this->loadButton->RequestFocus();
    UpdateHelpItems(this->loadButton);
}

void pksm::layout::TitleLoadScreen::FocusWirelessButton() {
    LOG_DEBUG("Focusing wireless button");
    this->wirelessButton->RequestFocus();
    UpdateHelpItems(this->wirelessButton);
}

void pksm::layout::TitleLoadScreen::TransitionToButtons() {
    LOG_DEBUG("Transitioning focus to button region");
    this->FocusLoadButton();
}

void pksm::layout::TitleLoadScreen::OnInput(u64 down, u64 up, u64 held) {
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    if (this->userIconButton->IsFocused()) {
        userIconButtonDirectionalHandler.HandleInput(down, held);
    } else if (this->loadButton->IsFocused() || this->wirelessButton->IsFocused()) {
        buttonDirectionalHandler.HandleInput(down, held);
    } else if (saveList->IsFocused()) {
        saveListDirectionalHandler.HandleInput(down, held);
    } else if (gameList->IsFocused()) {
        gameListDirectionalHandler.HandleInput(down, held);
    }
}

void pksm::layout::TitleLoadScreen::OnLoadButtonClick() {
    LOG_DEBUG("Load button clicked");
    if (onSaveLoaded) {
        auto selectedTitle = GetSelectedTitle();
        auto selectedSave = GetSelectedSave();

        if (selectedTitle && selectedSave) {
            LOG_DEBUG("Loading save: " + selectedSave->getName() + " for title: " + selectedTitle->getName());
            onSaveLoaded(selectedTitle, selectedSave);
        } else {
            LOG_ERROR("Cannot load save: No title or save selected");
        }
    }
}

void pksm::layout::TitleLoadScreen::OnWirelessButtonClick() {
    LOG_DEBUG("Wireless button clicked");
}

void pksm::layout::TitleLoadScreen::OnSaveSelected() {
    LOG_DEBUG("Save selected: " + saveList->GetSelectedItemText());
}

void pksm::layout::TitleLoadScreen::OnGameTouchSelect() {
    LOG_DEBUG("Game selected via touch");
}

void pksm::layout::TitleLoadScreen::OnGameListChanged() {
    LOG_DEBUG("Game list changed");
    this->userIconButton->SetVisible(this->gameList->IsGameListDependentOnUser());
    this->userIconButton->SetDisabled(!this->gameList->IsGameListDependentOnUser());
}

void pksm::layout::TitleLoadScreen::OnSaveListTouchSelect() {
    LOG_DEBUG("Save list selected via touch");
    FocusSaveList();
}

pu::i32 pksm::layout::TitleLoadScreen::GetBottomSectionY() const {
    return HEADER_TOP_MARGIN + HEADER_HEIGHT + HEADER_BOTTOM_MARGIN + this->gameList->GetHeight() +
        SAVE_LIST_TOP_MARGIN;
}

std::vector<pksm::ui::HelpItem> pksm::layout::TitleLoadScreen::GetHelpOverlayItems() const {
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::global::ButtonGlyph::A}}, "Select Highlighted"},
        {{{pksm::ui::global::ButtonGlyph::B}}, "Back"}
    };

    if (gameList->IsGameListDependentOnUser()) {
        helpItems.push_back({{{pksm::ui::global::ButtonGlyph::ZL}}, "Change Player"});
    }

    helpItems.insert(
        helpItems.end(),
        {{{{pksm::ui::global::ButtonGlyph::L, pksm::ui::global::ButtonGlyph::R}}, "Change Game List"},
         {{{pksm::ui::global::ButtonGlyph::AnalogStick, pksm::ui::global::ButtonGlyph::DPad}}, "Navigate"},
         {{{pksm::ui::global::ButtonGlyph::Minus}}, "Close Help"}}
    );

    return helpItems;
}

void pksm::layout::TitleLoadScreen::OnHelpOverlayShown() {
    LOG_DEBUG("Help overlay shown, disabling UI elements");
    gameList->SetDisabled(true);
    saveList->SetDisabled(true);
    loadButton->SetDisabled(true);
    wirelessButton->SetDisabled(true);
    userIconButton->SetDisabled(true);
}

void pksm::layout::TitleLoadScreen::OnHelpOverlayHidden() {
    LOG_DEBUG("Help overlay hidden, re-enabling UI elements");
    gameList->SetDisabled(false);
    saveList->SetDisabled(false);
    loadButton->SetDisabled(false);
    wirelessButton->SetDisabled(false);
    userIconButton->SetDisabled(false);
}

}  // namespace pksm::layout