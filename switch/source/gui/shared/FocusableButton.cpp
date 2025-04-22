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
    ShakeableWithOutline(pksm::ui::RectangularPulsingOutline::New(x, y, w, h)),
    focused(false),
    normalColor(normalColor),
    focusedColor(focusedColor),
    onClickCallback(nullptr),
    onCancelCallback(nullptr),
    disabled(false),
    x(x),
    y(y),
    width(w),
    height(h),
    touchHandler(),
    buttonHandler() {
    // Create background rectangle
    background = pu::ui::elm::Rectangle::New(x, y, w, h, normalColor);

    // Create text block
    text = pu::ui::elm::TextBlock::New(0, 0, content);  // Position will be centered in SetContent
    text->SetColor(global::TEXT_WHITE);

    // Center the text
    SetContent(content);

    // Setup touch handler callbacks
    touchHandler.SetOnTouchUpInside([this]() {
        if (!focused) {
            RequestFocus();
        } else if (focused && onClickCallback) {
            onClickCallback();
        }
    });

    // Register A button with the condition to only process when focused
    buttonHandler.RegisterButton(
        HidNpadButton_A,
        nullptr,
        [this]() {
            if (onClickCallback) {
                onClickCallback();
            }
        },
        [this]() { return this->focused; }
    );

    // Register B button with callback
    buttonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() {
        if (focused && onCancelCallback) {
            onCancelCallback();
        }
    });
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
        pulsingOutline->OnRender(drawer, x, y);
    }
}

void pksm::ui::FocusableButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Don't process input if disabled
    if (disabled) {
        return;
    }

    // Process touch using the touch handler with current bounds
    touchHandler.HandleInput(touch_pos, x, y, width, height);

    // Process button inputs
    buttonHandler.HandleInput(keys_down, keys_up, keys_held);
}

void pksm::ui::FocusableButton::SetFocused(bool focus) {
    LOG_DEBUG("FocusableButton SetFocused: " + std::to_string(focus));
    this->focused = focus;
    background->SetColor(focus ? focusedColor : normalColor);
    pulsingOutline->SetVisible(focused);
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

void pksm::ui::FocusableButton::SetHelpText(const std::string& text) {
    helpText = text;
}

std::vector<pksm::ui::HelpItem> pksm::ui::FocusableButton::GetHelpItems() const {
    if (!IsFocused()) {
        return {};
    }
    return {
        {{pksm::ui::global::ButtonGlyph::A}, helpText.empty() ? GetContent() : helpText},
        {{pksm::ui::global::ButtonGlyph::B}, "Back"}
    };
}

void pksm::ui::FocusableButton::SetDisabled(bool disabled) {
    LOG_DEBUG("FocusableButton SetDisabled: " + std::to_string(disabled));
    this->disabled = disabled;
}

bool pksm::ui::FocusableButton::IsDisabled() const {
    return disabled;
}