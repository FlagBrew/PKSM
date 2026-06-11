#include "gui/screens/title-load-screen/sub-components/game-list/TriggerButton.hpp"

#include <SDL2/SDL.h>

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {
TriggerButton::TriggerButton(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::i32 cornerRadius,
    const Side side,
    const pu::ui::Color& backgroundColor,
    const pu::ui::Color& textColor,
    const std::string& navigationText,
    const pu::ui::Color& navigationTextColor
)
  : Element(),
    ShakeableWithOutline(
        pksm::ui::PulsingOutline::New(
            x - TRIGGER_BUTTON_OUTLINE_PADDING,
            y - TRIGGER_BUTTON_OUTLINE_PADDING,
            width + (TRIGGER_BUTTON_OUTLINE_PADDING * 2),
            height + (TRIGGER_BUTTON_OUTLINE_PADDING * 2),
            pksm::ui::global::OUTLINE_COLOR,
            cornerRadius
        )
    ),
    x(x),
    y(y),
    width(width),
    height(height),
    cornerRadius(cornerRadius),
    side(side),
    backgroundColor(backgroundColor),
    textColor(textColor),
    navigationText(navigationText),
    navigationTextColor(navigationTextColor),
    focused(false),
    touchHandler(),
    buttonHandler() {
    // Create text block for the trigger button glyph
    textBlock = pu::ui::elm::TextBlock::New(x, y + TRIGGER_BUTTON_GLYPH_Y_OFFSET, "");
    textBlock->SetColor(textColor);
    textBlock->SetFont(pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_BUTTON));

    // Create text block for navigation text
    navigationTextBlock = pu::ui::elm::TextBlock::New(x, y + TRIGGER_BUTTON_GLYPH_Y_OFFSET, "");
    navigationTextBlock->SetColor(navigationTextColor);
    navigationTextBlock->SetFont(
        pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION)
    );

    switch (side) {
        case Side::Left:
            textBlock->SetText(pksm::ui::global::GetButtonGlyphString(pksm::ui::global::ButtonGlyph::TriggerLeft));
            // Register L button callbacks - always process regardless of focus
            buttonHandler.RegisterButton(
                HidNpadButton_L,
                [this]() { OnTriggerButtonPressed(); },
                [this]() { OnTriggerButtonReleased(); }
            );
            break;
        case Side::Right:
            textBlock->SetText(pksm::ui::global::GetButtonGlyphString(pksm::ui::global::ButtonGlyph::TriggerRight));
            // Register R button callbacks - always process regardless of focus
            buttonHandler.RegisterButton(
                HidNpadButton_R,
                [this]() { OnTriggerButtonPressed(); },
                [this]() { OnTriggerButtonReleased(); }
            );
            break;
    }

    // Register A button - only process when focused
    buttonHandler.RegisterButton(
        HidNpadButton_A,
        [this]() { OnTriggerButtonPressed(); },
        [this]() { OnTriggerButtonReleased(); },
        [this]() { return this->focused; }  // Only process when focused
    );

    // Position text and create initial texture
    UpdateTextPosition();

    // Setup touch handler callbacks
    touchHandler.SetOnTouchUpInside([this]() {
        LOG_DEBUG("TriggerButton Touch Up Inside");
        if (!focused && onTouchSelectCallback) {
            onTouchSelectCallback();
        } else if (focused && onSelectCallback) {
            onSelectCallback();
        }
    });
}

void TriggerButton::SetBackgroundColor(const pu::ui::Color& color) {
    backgroundColor = color;
}

void TriggerButton::SetTextColor(const pu::ui::Color& color) {
    textColor = color;
    textBlock->SetColor(color);
}

void TriggerButton::SetNavigationText(const std::string& text) {
    navigationText = text;
    navigationTextBlock->SetText(text);
    UpdateTextPosition();
}

void TriggerButton::SetNavigationTextColor(const pu::ui::Color& color) {
    navigationTextColor = color;
    navigationTextBlock->SetColor(color);
}

void TriggerButton::UpdateTextPosition() {
    // Center glyph text in button
    pu::i32 textX = x + (width - textBlock->GetWidth()) / 2;
    textBlock->SetX(textX);

    // Position navigation text based on side
    if (navigationText.empty()) {
        navigationTextBlock->SetVisible(false);
        return;
    }

    navigationTextBlock->SetVisible(true);
    if (side == Side::Left) {
        navigationTextBlock->SetX(x + width + NAVIGATION_TEXT_PADDING);
    } else {
        navigationTextBlock->SetX(x - navigationTextBlock->GetWidth() - NAVIGATION_TEXT_PADDING);
    }
}

void TriggerButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    drawer->RenderRoundedRectangleFill(backgroundColor, x, y, width, height, cornerRadius);

    // Render glyph text
    if (textBlock) {
        textBlock->OnRender(drawer, textBlock->GetX(), textBlock->GetY());
    }

    // Render navigation text if set
    if (!navigationText.empty() && navigationTextBlock) {
        navigationTextBlock->OnRender(drawer, navigationTextBlock->GetX(), navigationTextBlock->GetY());
    }

    if (focused) {
        pulsingOutline->OnRender(drawer, x - TRIGGER_BUTTON_OUTLINE_PADDING, y - TRIGGER_BUTTON_OUTLINE_PADDING);
    }
}

void TriggerButton::SetFocused(bool focus) {
    LOG_DEBUG("Trigger button SetFocused: " + std::to_string(focus));
    this->focused = focus;
    pulsingOutline->SetVisible(focus);
}

bool TriggerButton::IsFocused() const {
    return focused;
}

void TriggerButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Handle touch using our touch handler with current bounds
    touchHandler.HandleInput(touch_pos, x, y, width, height);

    // Process all button inputs
    buttonHandler.HandleInput(keys_down, keys_up, keys_held);
}

std::vector<pksm::ui::HelpItem> TriggerButton::GetHelpItems() const {
    if (!IsFocused()) {
        return {};
    }
    return {
        {{pksm::ui::global::ButtonGlyph::A}, "Go to " + navigationText + " games"},
        {{pksm::ui::global::ButtonGlyph::B}, "Back"}
    };
}

void pksm::ui::TriggerButton::OnTriggerButtonPressed() {
    this->SetBackgroundColor(TRIGGER_BUTTON_COLOR_PRESSED);
}

void pksm::ui::TriggerButton::OnTriggerButtonReleased() {
    this->SetBackgroundColor(TRIGGER_BUTTON_COLOR);
    if (onSelectCallback) {
        onSelectCallback();
    }
}

}  // namespace pksm::ui