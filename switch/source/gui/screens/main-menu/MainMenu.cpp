#include "gui/screens/main-menu/MainMenu.hpp"

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

MainMenu::MainMenu(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay
)
  : BaseLayout(onShowOverlay, onHideOverlay), onBack(onBack) {
    LOG_DEBUG("Initializing MainMenu...");

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Create trainer info component with dynamic height
    trainerInfo = pksm::ui::TrainerInfo::New(
        TRAINER_INFO_SIDE_MARGIN,
        TRAINER_INFO_TOP_MARGIN,
        TRAINER_INFO_WIDTH,
        "Wow",  // OT Name
        3,  // Badges
        0,  // Wonder Cards
        54,  // National Dex Seen
        26,  // National Dex Caught
        0.48f,  // Completion
        "26h 15m"  // Time played
    );
    this->Add(trainerInfo);

    // Calculate MenuButtonGrid width and position
    const pu::i32 MENU_GRID_MARGIN = 32;  // Margin between TrainerInfo and MenuButtonGrid
    const pu::i32 menuGridWidth = GetWidth() - (TRAINER_INFO_SIDE_MARGIN + TRAINER_INFO_WIDTH + (MENU_GRID_MARGIN * 2));
    const pu::i32 menuGridX = TRAINER_INFO_SIDE_MARGIN + TRAINER_INFO_WIDTH + MENU_GRID_MARGIN;

    // Create menu button grid
    menuGrid = pksm::ui::MenuButtonGrid::New(menuGridX, MENU_GRID_TOP_MARGIN, menuGridWidth);
    this->Add(menuGrid);

    // Initialize help footer
    InitializeHelpFooter();

    // Set initial help items
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::HelpButton::A}}, "Select"},
        {{{pksm::ui::HelpButton::B}}, "Back to Game Selection"},
        {{{pksm::ui::HelpButton::Minus}}, "Help"}
    };
    helpFooter->SetHelpItems(helpItems);

    // Set up input handling
    this->SetOnInput(
        std::bind(&MainMenu::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("MainMenu initialization complete");
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
        {{{pksm::ui::HelpButton::A}}, "Select Highlighted"},
        {{{pksm::ui::HelpButton::B}}, "Back to Game Selection"},
        {{{pksm::ui::HelpButton::DPad}}, "Navigate"},
        {{{pksm::ui::HelpButton::Minus}}, "Close Help"}
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