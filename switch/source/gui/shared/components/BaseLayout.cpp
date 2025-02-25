#include "gui/shared/components/BaseLayout.hpp"

namespace pksm::layout {

BaseLayout::BaseLayout(std::function<void(pu::ui::Overlay::Ref)> onShowOverlay, std::function<void()> onHideOverlay)
  : Layout(), onShowOverlay(onShowOverlay), onHideOverlay(onHideOverlay), isHelpOverlayVisible(false) {
    LOG_DEBUG("Initializing BaseLayout...");
}

void BaseLayout::InitializeHelpFooter() {
    LOG_DEBUG("Initializing help footer");
    helpFooter = pksm::ui::HelpFooter::New(0, GetHeight() - pksm::ui::HelpFooter::FOOTER_HEIGHT, GetWidth());
    this->Add(helpFooter);
}

void BaseLayout::UpdateHelpItems(pksm::ui::IHelpProvider::Ref helpItemProvider) {
    if (helpFooter) {
        helpFooter->SetHelpItems(helpItemProvider->GetHelpItems());
    }
}

void BaseLayout::ShowHelpOverlay(const std::vector<pksm::ui::HelpItem>& helpItems) {
    LOG_DEBUG("Showing help overlay");
    if (!isHelpOverlayVisible) {
        // Create and show overlay
        auto overlay = pksm::ui::HelpOverlay::New(0, 0, GetWidth(), GetHeight());
        overlay->SetHelpItems(helpItems);
        onShowOverlay(overlay);
        isHelpOverlayVisible = true;

        // Call virtual method to allow derived classes to disable UI elements
        OnHelpOverlayShown();
    }
}

void BaseLayout::HideHelpOverlay() {
    LOG_DEBUG("Hiding help overlay");
    if (isHelpOverlayVisible) {
        onHideOverlay();
        isHelpOverlayVisible = false;

        // Call virtual method to allow derived classes to re-enable UI elements
        OnHelpOverlayHidden();
    }
}

bool BaseLayout::HandleHelpInput(u64 down) {
    if (down & HidNpadButton_Minus) {
        LOG_DEBUG("Minus button pressed");
        if (isHelpOverlayVisible) {
            HideHelpOverlay();
        } else {
            ShowHelpOverlay(GetHelpOverlayItems());
        }
        return true;  // Input was handled
    }

    if (isHelpOverlayVisible) {
        if (down & HidNpadButton_B) {
            LOG_DEBUG("B button pressed while help overlay visible");
            HideHelpOverlay();
        }
        return true;  // Input was handled (block other input while help is visible)
    }

    return false;  // Input was not handled
}

std::vector<pksm::ui::HelpItem> BaseLayout::GetHelpOverlayItems() const {
    // Default implementation with common help items
    return {
        {{{pksm::ui::HelpButton::A}}, "Select"},
        {{{pksm::ui::HelpButton::B}}, "Back"},
        {{{pksm::ui::HelpButton::DPad}}, "Navigate"},
        {{{pksm::ui::HelpButton::Minus}}, "Close Help"}
    };
}

}  // namespace pksm::layout