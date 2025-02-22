#include "gui/screens/main-menu/MainMenu.hpp"

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

MainMenu::MainMenu(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay
)
  : Layout(), onBack(onBack), onShowOverlay(onShowOverlay), onHideOverlay(onHideOverlay), isHelpOverlayVisible(false) {
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

    // Create help footer at the bottom of the screen
    helpFooter = pksm::ui::HelpFooter::New(0, GetHeight() - pksm::ui::HelpFooter::FOOTER_HEIGHT, GetWidth());
    this->Add(helpFooter);

    // Set initial help items
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::HelpButton::A}}, "Select"},
        {{{pksm::ui::HelpButton::B}}, "Back to Game Selection"},
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
    if (down & HidNpadButton_Minus) {
        LOG_DEBUG("Minus button pressed");
        if (isHelpOverlayVisible) {
            EndOverlay();
        } else {
            StartOverlay();
        }
        return;  // Don't process other input while toggling help
    }

    if (isHelpOverlayVisible) {
        if (down & HidNpadButton_B) {
            LOG_DEBUG("B button pressed while help overlay visible");
            EndOverlay();
        }
        return;  // Don't process other input while help is visible
    }

    // Only process input if not in help overlay
    if (!isHelpOverlayVisible) {
        if (down & HidNpadButton_B) {
            LOG_DEBUG("B button pressed, returning to game selection");
            if (onBack) {
                onBack();
            }
        }
    }
}

void MainMenu::UpdateHelpItems(pksm::ui::IHelpProvider::Ref helpItemProvider) {
    if (helpFooter) {
        helpFooter->SetHelpItems(helpItemProvider->GetHelpItems());
    }
}

void MainMenu::StartOverlay() {
    LOG_DEBUG("Starting help overlay");
    if (!isHelpOverlayVisible) {
        // Create and show overlay
        auto overlay = pksm::ui::HelpOverlay::New(0, 0, GetWidth(), GetHeight());
        std::vector<pksm::ui::HelpItem> helpItems = {
            {{{pksm::ui::HelpButton::A}}, "Select Highlighted"},
            {{{pksm::ui::HelpButton::B}}, "Back to Game Selection"},
            {{{pksm::ui::HelpButton::DPad}}, "Navigate"},
            {{{pksm::ui::HelpButton::Minus}}, "Close Help"}
        };

        overlay->SetHelpItems(helpItems);
        onShowOverlay(overlay);
        menuGrid->SetDisabled(true);  // Disable menu grid interaction
        isHelpOverlayVisible = true;
    }
}

void MainMenu::EndOverlay() {
    LOG_DEBUG("Ending help overlay");
    if (isHelpOverlayVisible) {
        onHideOverlay();
        menuGrid->SetDisabled(false);  // Re-enable menu grid interaction
        isHelpOverlayVisible = false;
    }
}

}  // namespace pksm::layout