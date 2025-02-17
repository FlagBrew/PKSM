#include "gui/shared/components/FocusableButton.hpp"

#include "utils/Logger.hpp"
pksm::ui::FocusableButton::FocusableButton(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 w,
    const pu::i32 h,
    const std::string& content,
    const pu::ui::Color& normalColor,
    const pu::ui::Color& focusedColor
)
  : Element(),
    focused(false),
    normalColor(normalColor),
    focusedColor(focusedColor),
    onClickCallback(nullptr),
    onTouchSelectCallback(nullptr),
    x(x),
    y(y),
    width(w),
    height(h),
    isPressed(false) {
    // Create outline
    outline = pksm::ui::PulsingOutline::New(x, y, width, height, pu::ui::Color(0, 150, 255, 255));
    outline->SetVisible(false);

    // Create background rectangle
    background = pu::ui::elm::Rectangle::New(x, y, w, h, normalColor);

    // Create text block
    text = pu::ui::elm::TextBlock::New(0, 0, content);  // Position will be centered in SetContent
    text->SetColor(pu::ui::Color(255, 255, 255, 255));  // White text

    // Center the text
    SetContent(content);
}

pu::i32 pksm::ui::FocusableButton::GetX() {
    return x;
}

pu::i32 pksm::ui::FocusableButton::GetY() {
    return y;
}

pu::i32 pksm::ui::FocusableButton::GetWidth() {
    return width;
}

pu::i32 pksm::ui::FocusableButton::GetHeight() {
    return height;
}

void pksm::ui::FocusableButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    background->OnRender(drawer, x, y);
    text->OnRender(drawer, text->GetX(), text->GetY());

    if (focused) {
        outline->OnRender(drawer, x, y);
    }
}

void pksm::ui::FocusableButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (!touch_pos.IsEmpty()) {
        if (touch_pos.HitsRegion(x, y, width, height)) {
            if (!isPressed) {
                isPressed = true;

                // If we're not focused, trigger touch select
                if (!focused && onTouchSelectCallback) {
                    onTouchSelectCallback();
                    RequestFocus();
                }
            }
        }
    } else if (isPressed) {
        // Touch released
        isPressed = false;

        // If touch was released while still over button, trigger click
        if (onClickCallback) {
            onClickCallback();
        }
    }
}

void pksm::ui::FocusableButton::SetFocused(bool focus) {
    LOG_DEBUG("FocusableButton SetFocused: " + std::to_string(focus));
    this->focused = focus;
    background->SetColor(focus ? focusedColor : normalColor);
    outline->SetVisible(focused);
}

bool pksm::ui::FocusableButton::IsFocused() const {
    return focused;
}

void pksm::ui::FocusableButton::SetContent(const std::string& content) {
    text->SetText(content);

    // Center the text in the button
    pu::i32 textX = x + ((width - text->GetWidth()) / 2);
    pu::i32 textY = y + ((height - text->GetHeight()) / 2);
    text->SetX(textX);
    text->SetY(textY);
}

std::string pksm::ui::FocusableButton::GetContent() const {
    return text->GetText();
}

void pksm::ui::FocusableButton::SetContentFont(const std::string& font) {
    text->SetFont(font);
    // Re-center text with new font
    SetContent(text->GetText());
}

void pksm::ui::FocusableButton::SetContentColor(const pu::ui::Color& color) {
    text->SetColor(color);
}

void pksm::ui::FocusableButton::SetBackgroundColor(const pu::ui::Color& color) {
    background->SetColor(color);
}

void pksm::ui::FocusableButton::SetOnClick(std::function<void()> callback) {
    onClickCallback = callback;
}

void pksm::ui::FocusableButton::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}
void pksm::ui::FocusableButton::SetHelpText(const std::string& text) {
    helpText = text;
}

std::vector<pksm::ui::HelpItem> pksm::ui::FocusableButton::GetHelpItems() const {
    if (!IsFocused()) {
        return {};
    }
    return {
        {{pksm::ui::HelpButton::A}, helpText.empty() ? GetContent() : helpText},
        {{pksm::ui::HelpButton::B}, "Back"}
    };
}