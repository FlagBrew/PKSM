#include "gui/shared/components/HelpFooter.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

HelpFooter::HelpFooter(const pu::i32 x, const pu::i32 y, const pu::i32 width) : Element(), x(x), y(y), width(width) {
    LOG_DEBUG("Initializing HelpFooter...");

    // Create white divider line - now goes to the top
    divider = pu::ui::elm::Rectangle::New(
        x + DIVIDER_MARGIN,
        y,  // Start from top
        width - (DIVIDER_MARGIN * 2),
        DIVIDER_HEIGHT,
        pksm::ui::global::TEXT_WHITE
    );

    // Create minus button text
    minusButtonText = pu::ui::elm::TextBlock::New(x, y, GetButtonGlyph(HelpButton::Minus));
    minusButtonText->SetFont(pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_BUTTON));
    minusButtonText->SetColor(pksm::ui::global::TEXT_WHITE);

    // Create "Help" text
    helpText = pu::ui::elm::TextBlock::New(x, y, "Help");
    helpText->SetColor(pksm::ui::global::TEXT_WHITE);

    // Position the elements vertically centered
    PositionElements();

    LOG_DEBUG("HelpFooter initialization complete");
}

void HelpFooter::PositionElements() {
    // Calculate vertical center position
    const pu::i32 verticalCenter = y + (FOOTER_HEIGHT / 2);

    // Position minus button text starting from divider + padding
    const pu::i32 minusButtonY = verticalCenter - (minusButtonText->GetHeight() / 2);
    minusButtonText->SetX(x + DIVIDER_MARGIN + INTERNAL_PADDING);
    minusButtonText->SetY(minusButtonY + GLYPH_Y_OFFSET);

    // Position help text next to minus button
    const pu::i32 helpTextY = verticalCenter - (helpText->GetHeight() / 2);
    helpText->SetX(minusButtonText->GetX() + minusButtonText->GetWidth() + BUTTON_ACTION_SPACING);
    helpText->SetY(helpTextY);
}

pu::i32 HelpFooter::GetX() {
    return x;
}

pu::i32 HelpFooter::GetY() {
    return y;
}

pu::i32 HelpFooter::GetWidth() {
    return width;
}

pu::i32 HelpFooter::GetHeight() {
    return FOOTER_HEIGHT;
}

void HelpFooter::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {}

void HelpFooter::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render divider
    divider->OnRender(drawer, divider->GetX(), divider->GetY());

    // Render minus button and help text
    minusButtonText->OnRender(drawer, minusButtonText->GetX(), minusButtonText->GetY());
    helpText->OnRender(drawer, helpText->GetX(), helpText->GetY());

    // Render dynamic help texts
    for (const auto& text : dynamicHelpTexts) {
        text->OnRender(drawer, text->GetX(), text->GetY());
    }
}

void HelpFooter::SetHelpItems(const std::vector<HelpItem>& items) {
    LOG_DEBUG("Setting help items, count: " + std::to_string(items.size()));
    helpItems = items;
    UpdateDynamicHelpTexts();
}

std::string HelpFooter::GetButtonGlyph(HelpButton button) const {
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

void HelpFooter::UpdateDynamicHelpTexts() {
    LOG_DEBUG("Updating dynamic help texts");
    dynamicHelpTexts.clear();

    // Calculate vertical center
    const pu::i32 verticalCenter = y + (FOOTER_HEIGHT / 2);

    // Start from the right margin, accounting for divider margin and internal padding
    pu::i32 currentX = width - (DIVIDER_MARGIN + INTERNAL_PADDING);

    // Process items in normal order
    for (const auto& item : helpItems) {
        // Create button text
        std::string buttonText;
        for (size_t i = 0; i < item.buttons.size(); i++) {
            if (i > 0) {
                buttonText += "/";
            }
            buttonText += GetButtonGlyph(item.buttons[i]);
        }

        // First create both text blocks to measure their widths
        auto actionTextBlock = pu::ui::elm::TextBlock::New(0, 0, item.actionDescription);
        actionTextBlock->SetColor(pksm::ui::global::TEXT_WHITE);

        auto buttonTextBlock = pu::ui::elm::TextBlock::New(0, 0, buttonText);
        buttonTextBlock->SetFont(pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_BUTTON));
        buttonTextBlock->SetColor(pksm::ui::global::TEXT_WHITE);

        // Position action text from the right
        currentX -= actionTextBlock->GetWidth();
        actionTextBlock->SetX(currentX);
        actionTextBlock->SetY(verticalCenter - (actionTextBlock->GetHeight() / 2));

        // Position button text to the left of the action text
        currentX -= (buttonTextBlock->GetWidth() + BUTTON_ACTION_SPACING);
        buttonTextBlock->SetX(currentX);
        buttonTextBlock->SetY(verticalCenter - (buttonTextBlock->GetHeight() / 2) + GLYPH_Y_OFFSET);

        // Add spacing for next item
        currentX -= HELP_ITEM_SPACING;

        // Add elements in order
        dynamicHelpTexts.push_back(buttonTextBlock);
        dynamicHelpTexts.push_back(actionTextBlock);
    }
}

}  // namespace pksm::ui