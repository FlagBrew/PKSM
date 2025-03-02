#include "gui/shared/components/HelpOverlay.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

HelpOverlay::HelpOverlay(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height)
  : pu::ui::Overlay(x, y, width, height, pu::ui::Color(0, 0, 0, 210)) {
    LOG_DEBUG("Initializing HelpOverlay...");
    this->SetRadius(0);
    this->SetMaxFadeAlpha(210);
    this->SetFadeAlphaVariation(15);
    LOG_DEBUG("HelpOverlay initialization complete");
}

void HelpOverlay::SetHelpItems(const std::vector<HelpItem>& items) {
    LOG_DEBUG("Setting help items, count: " + std::to_string(items.size()));
    helpItems = items;
    UpdateHelpTexts();
}

void HelpOverlay::Show() {
    LOG_DEBUG("Showing help overlay");
    this->NotifyEnding(false);
}

void HelpOverlay::Hide() {
    LOG_DEBUG("Hiding help overlay");
    this->NotifyEnding(true);
}

void HelpOverlay::UpdateHelpTexts() {
    LOG_DEBUG("Updating help texts");

    // Clear all elements
    this->Clear();
    helpTexts.clear();

    // Start from top margin
    pu::i32 currentY = OVERLAY_TOP_MARGIN;

    // Process items in order
    for (const auto& item : helpItems) {
        // Create button text
        std::string buttonText;
        for (size_t i = 0; i < item.buttons.size(); i++) {
            if (i > 0) {
                buttonText += "/";
            }
            buttonText += GetButtonGlyph(item.buttons[i]);
        }

        // Create button glyph text block
        auto buttonTextBlock = pu::ui::elm::TextBlock::New(OVERLAY_PADDING, currentY + GLYPH_Y_OFFSET, buttonText);
        buttonTextBlock->SetFont(pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_BUTTON));
        buttonTextBlock->SetColor(pksm::ui::global::TEXT_WHITE);

        // Create action description text block
        auto actionTextBlock = pu::ui::elm::TextBlock::New(
            OVERLAY_PADDING + buttonTextBlock->GetWidth() + BUTTON_ACTION_SPACING,
            currentY,
            item.actionDescription
        );
        actionTextBlock->SetColor(pksm::ui::global::TEXT_WHITE);

        // Add spacing for next item based on the taller of the two text blocks
        pu::i32 itemHeight = std::max(buttonTextBlock->GetHeight(), actionTextBlock->GetHeight());
        currentY += itemHeight + ITEM_VERTICAL_SPACING;

        // Add elements to overlay
        this->Add(buttonTextBlock);
        this->Add(actionTextBlock);

        // Store references
        helpTexts.push_back(buttonTextBlock);
        helpTexts.push_back(actionTextBlock);
    }
}

std::string HelpOverlay::GetButtonGlyph(HelpButton button) const {
    switch (button) {
        case HelpButton::A:
            return "\uE0E0";
        case HelpButton::B:
            return "\uE0E1";
        case HelpButton::X:
            return "\uE0E2";
        case HelpButton::Y:
            return "\uE0E3";
        case HelpButton::L:
            return "\uE0E4";
        case HelpButton::R:
            return "\uE0E5";
        case HelpButton::ZL:
            return "\uE0E6";
        case HelpButton::ZR:
            return "\uE0E7";
        case HelpButton::Plus:
            return "\uE0EF";
        case HelpButton::Minus:
            return "\uE0F0";
        case HelpButton::Home:
            return "\uE0F4";
        case HelpButton::AnalogStick:
            return "\uE100";
        case HelpButton::LeftAnalogStick:
            return "\uE101";
        case HelpButton::RightAnalogStick:
            return "\uE102";
        case HelpButton::DPad:
            return "\uE110";
        case HelpButton::TouchScreen:
            return "\uE121";
        default:
            return "?";
    }
}

}  // namespace pksm::ui