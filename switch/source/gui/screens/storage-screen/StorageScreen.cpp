#include "gui/screens/storage-screen/StorageScreen.hpp"

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"

namespace pksm::layout {

StorageScreen::StorageScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay
)
  : BaseLayout(onShowOverlay, onHideOverlay), onBack(onBack) {
    LOG_DEBUG("Initializing StorageScreen...");

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Initialize help footer
    InitializeHelpFooter();

    // Set initial help items
    std::vector<pksm::ui::HelpItem> helpItems = {
        {{{pksm::ui::HelpButton::A}}, "Select"},
        {{{pksm::ui::HelpButton::B}}, "Back to Main Menu"},
        {{{pksm::ui::HelpButton::Minus}}, "Help"}
    };
    helpFooter->SetHelpItems(helpItems);

    // Set up input handling
    this->SetOnInput(
        std::bind(&StorageScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("StorageScreen initialization complete");
}

StorageScreen::~StorageScreen() = default;

void StorageScreen::OnInput(u64 down, u64 up, u64 held) {
    // First handle help-related input
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    // Only process other input if not in help overlay
    if (down & HidNpadButton_B) {
        LOG_DEBUG("B button pressed, returning to main menu");
        if (onBack) {
            onBack();
        }
    }
}

std::vector<pksm::ui::HelpItem> StorageScreen::GetHelpOverlayItems() const {
    return {
        {{{pksm::ui::HelpButton::A}}, "Select"},
        {{{pksm::ui::HelpButton::B}}, "Back to Main Menu"},
        {{{pksm::ui::HelpButton::DPad}}, "Navigate"},
        {{{pksm::ui::HelpButton::Minus}}, "Close Help"}
    };
}

void StorageScreen::OnHelpOverlayShown() {
    LOG_DEBUG("Help overlay shown, disabling UI elements");
}

void StorageScreen::OnHelpOverlayHidden() {
    LOG_DEBUG("Help overlay hidden, re-enabling UI elements");
}

}  // namespace pksm::layout