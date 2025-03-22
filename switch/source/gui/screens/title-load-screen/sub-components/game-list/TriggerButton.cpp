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
    isPressed(false),
    lastTouchTime(0) {
    outline = pksm::ui::PulsingOutline::New(
        x - TRIGGER_BUTTON_OUTLINE_PADDING,
        y - TRIGGER_BUTTON_OUTLINE_PADDING,
        width + (TRIGGER_BUTTON_OUTLINE_PADDING * 2),
        height + (TRIGGER_BUTTON_OUTLINE_PADDING * 2),
        pu::ui::Color(0, 150, 255, 255),
        cornerRadius
    );
    outline->SetVisible(false);

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
            textBlock->SetText("\ue0f6");
            break;
        case Side::Right:
            textBlock->SetText("\ue0f7");
            break;
    }

    // Position text and create initial texture
    UpdateTextPosition();
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
        outline->OnRender(drawer, x - TRIGGER_BUTTON_OUTLINE_PADDING, y - TRIGGER_BUTTON_OUTLINE_PADDING);
    }
}

void TriggerButton::SetFocused(bool focus) {
    LOG_DEBUG("Trigger button SetFocused: " + std::to_string(focus));
    this->focused = focus;
    outline->SetVisible(focus);
}

bool TriggerButton::IsFocused() const {
    return focused;
}

void TriggerButton::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void TriggerButton::SetOnTouchNavigation(std::function<void()> callback) {
    onTouchNavigationCallback = callback;
}

void TriggerButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (!touch_pos.IsEmpty() && touch_pos.HitsRegion(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight())) {
        LOG_DEBUG("Trigger button touched");

        // Get current time for debouncing
        u64 currentTime = SDL_GetTicks64();

        // Only process touch if we're not in a debounce period
        if (currentTime - lastTouchTime >= TOUCH_DEBOUNCE_TIME) {
            lastTouchTime = currentTime;

            if (!isPressed) {
                isPressed = true;

                // If not focused, trigger select callback
                if (!focused && onTouchSelectCallback) {
                    onTouchSelectCallback();
                }
                // If already focused, trigger navigation callback
                else if (focused && onTouchNavigationCallback) {
                    onTouchNavigationCallback();
                }
            }
        }
    } else if (isPressed) {
        // Touch released outside button
        isPressed = false;
    }
}

std::vector<pksm::ui::HelpItem> TriggerButton::GetHelpItems() const {
    if (!IsFocused()) {
        return {};
    }
    return {{{pksm::ui::HelpButton::A}, "Go to " + navigationText + " games"}, {{pksm::ui::HelpButton::B}, "Back"}};
}
}  // namespace pksm::ui