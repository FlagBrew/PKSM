#include "gui/TriggerButton.hpp"

#include "gui/UIConstants.hpp"
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
    const pu::ui::Color& textColor
)
  : Element(),
    x(x),
    y(y),
    width(width),
    height(height),
    cornerRadius(cornerRadius),
    side(side),
    backgroundColor(backgroundColor),
    textColor(textColor) {
    outline = pksm::ui::PulsingOutline::New(
        x - TRIGGER_BUTTON_OUTLINE_PADDING,
        y - TRIGGER_BUTTON_OUTLINE_PADDING,
        width + (TRIGGER_BUTTON_OUTLINE_PADDING * 2),
        height + (TRIGGER_BUTTON_OUTLINE_PADDING * 2),
        pu::ui::Color(0, 150, 255, 255),
        cornerRadius
    );
    focused = false;
    outline->SetVisible(false);
    // Create text block
    textBlock = pu::ui::elm::TextBlock::New(x, y + TRIGGER_BUTTON_GLYPH_Y_OFFSET, "");
    textBlock->SetColor(textColor);
    textBlock->SetFont(pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_BUTTON));
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

void TriggerButton::UpdateTextPosition() {
    // Center text in button
    pu::i32 textX = x + (width - textBlock->GetWidth()) / 2;
    textBlock->SetX(textX);
}

void TriggerButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    drawer->RenderRoundedRectangleFill(backgroundColor, x, y, width, height, cornerRadius);

    // Render text
    if (textBlock) {
        textBlock->OnRender(drawer, textBlock->GetX(), textBlock->GetY());
    }

    if (focused) {
        // Pass the adjusted position directly to the outline
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
        if (!focused && onTouchSelectCallback)
            onTouchSelectCallback();
        else if (focused && onTouchNavigationCallback)
            onTouchNavigationCallback();
    }
}
}  // namespace pksm::ui