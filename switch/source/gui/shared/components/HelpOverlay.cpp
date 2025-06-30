#include "gui/shared/components/HelpOverlay.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

HelpOverlay::HelpOverlay(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height)
  : pu::ui::Overlay(x, y, width, height, HELP_OVERLAY_BACKGROUND_COLOR) {
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
            buttonText += pksm::ui::global::GetButtonGlyphString(item.buttons[i]);
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

}  // namespace pksm::ui