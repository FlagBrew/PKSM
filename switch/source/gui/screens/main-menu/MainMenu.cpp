#include "gui/screens/main-menu/MainMenu.hpp"

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

MainMenu::MainMenu(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    ISaveDataAccessor::Ref saveDataAccessor,
    std::map<pksm::ui::MenuButtonType, std::function<void()>> navigationCallbacks
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    onBack(onBack),
    saveDataAccessor(saveDataAccessor),
    navigationCallbacks(navigationCallbacks) {
    LOG_DEBUG("Initializing MainMenu...");

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Create trainer info component with dynamic height
    trainerInfo = pksm::ui::TrainerInfo::New(
        TRAINER_INFO_SIDE_MARGIN,
        TRAINER_INFO_TOP_MARGIN,
        TRAINER_INFO_WIDTH,
        "",  // OT Name (will be updated)
        0,  // Badges (will be updated)
        0,  // Wonder Cards (will be updated)
        0,  // National Dex Seen (will be updated)
        0,  // National Dex Caught (will be updated)
        0.0f,  // Completion (will be updated)
        ""  // Time played (will be updated)
    );
    this->Add(trainerInfo);

    // Calculate MenuButtonGrid width and position
    const pu::i32 MENU_GRID_MARGIN = 32;  // Margin between TrainerInfo and MenuButtonGrid
    const pu::i32 menuGridWidth = GetWidth() - (TRAINER_INFO_SIDE_MARGIN + TRAINER_INFO_WIDTH + (MENU_GRID_MARGIN * 2));
    const pu::i32 menuGridX = TRAINER_INFO_SIDE_MARGIN + TRAINER_INFO_WIDTH + MENU_GRID_MARGIN;

    // Create menu button grid
    menuGrid = pksm::ui::MenuButtonGrid::New(menuGridX, MENU_GRID_TOP_MARGIN, menuGridWidth);
    this->Add(menuGrid);

    // Register navigation callbacks for the menu buttons
    RegisterNavigationCallbacks();

    // Initialize help footer
    InitializeHelpFooter();

    // Set initial help items
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::global::ButtonGlyph::A}}, "Select"},
        {{{pksm::ui::global::ButtonGlyph::B}}, "Back to Game Selection"},
        {{{pksm::ui::global::ButtonGlyph::Minus}}, "Help"}
    };
    helpFooter->SetHelpItems(helpItems);

    // Set up input handling
    this->SetOnInput(
        std::bind(&MainMenu::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    // Set up save data change callback
    saveDataAccessor->setOnSaveDataChanged([this](pksm::saves::SaveData::Ref saveData) {
        LOG_DEBUG("Save data changed, updating trainer info");
        menuGrid->SetSelectedIndex(0);
        UpdateTrainerInfo();
    });

    // Update trainer info with current save data
    UpdateTrainerInfo();

    LOG_DEBUG("MainMenu initialization complete");
}

void MainMenu::RegisterNavigationCallbacks() {
    LOG_DEBUG("Registering navigation callbacks for menu buttons");

    // Register each callback with the corresponding button type
    for (const auto& [buttonType, callback] : navigationCallbacks) {
        menuGrid->RegisterButtonCallback(buttonType, callback);
        LOG_DEBUG("Registered callback for button type: " + std::to_string(static_cast<int>(buttonType)));
    }
}

void MainMenu::UpdateTrainerInfo() {
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (saveData) {
        LOG_DEBUG("Updating trainer info with save data");

        // Update trainer info with save data
        trainerInfo->SetTrainerInfo(
            saveData->getOTName(),
            saveData->getBadges(),
            saveData->getWonderCards(),
            saveData->getDexSeen(),
            saveData->getDexCaught(),
            saveData->getDexCompletionPercentage() /
                100.0f,  // Convert from percentage (0-100) to 0-1 range for progress bar
            saveData->getPlayedTimeString(),
            saveData->getGeneration()
        );
    } else {
        LOG_DEBUG("No save data available, using default trainer info");

        // Set default values
        trainerInfo->SetTrainerInfo("No Save Loaded", 0, 0, 0, 0, 0.0f, "00:00:00", pksm::saves::Generation::TWO);
    }
}

MainMenu::~MainMenu() = default;

void MainMenu::OnInput(u64 down, u64 up, u64 held) {
    // First handle help-related input
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    // Only process other input if not in help overlay
    if (down & HidNpadButton_B) {
        LOG_DEBUG("B button pressed, returning to game selection");
        if (onBack) {
            onBack();
        }
    }
}

std::vector<pksm::ui::HelpItem> MainMenu::GetHelpOverlayItems() const {
    return {
        {{{pksm::ui::global::ButtonGlyph::A}}, "Select Highlighted"},
        {{{pksm::ui::global::ButtonGlyph::B}}, "Back to Game Selection"},
        {{{pksm::ui::global::ButtonGlyph::DPad}}, "Navigate"},
        {{{pksm::ui::global::ButtonGlyph::Minus}}, "Close Help"}
    };
}

void MainMenu::OnHelpOverlayShown() {
    LOG_DEBUG("Help overlay shown, disabling UI elements");
    menuGrid->SetDisabled(true);
}

void MainMenu::OnHelpOverlayHidden() {
    LOG_DEBUG("Help overlay hidden, re-enabling UI elements");
    menuGrid->SetDisabled(false);
}

}  // namespace pksm::layout