#include "gui/FocusableButton.hpp"

FocusableButton::FocusableButton(
    const pu::i32 x, const pu::i32 y, const pu::i32 w, const pu::i32 h,
    const std::string& content,
    const pu::ui::Color& normalColor,
    const pu::ui::Color& focusedColor
) : Element(), focused(false), normalColor(normalColor), focusedColor(focusedColor),
    onClickCallback(nullptr), onTouchSelectCallback(nullptr),
    x(x), y(y), width(w), height(h), isPressed(false) {
    
    // Create background rectangle
    background = pu::ui::elm::Rectangle::New(x, y, w, h, normalColor);
    
    // Create text block
    text = pu::ui::elm::TextBlock::New(0, 0, content);  // Position will be centered in SetContent
    text->SetColor(pu::ui::Color(255, 255, 255, 255));  // White text
    
    // Center the text
    SetContent(content);
}

pu::i32 FocusableButton::GetX() {
    return x;
}

pu::i32 FocusableButton::GetY() {
    return y;
}

pu::i32 FocusableButton::GetWidth() {
    return width;
}

pu::i32 FocusableButton::GetHeight() {
    return height;
}

void FocusableButton::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    background->OnRender(drawer, x, y);
    text->OnRender(drawer, text->GetX(), text->GetY());
}

void FocusableButton::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    if (!touch_pos.IsEmpty()) {
        if (touch_pos.HitsRegion(x, y, width, height)) {
            if (!isPressed) {
                isPressed = true;
                
                // If we're not focused, trigger touch select
                if (!focused && onTouchSelectCallback) {
                    onTouchSelectCallback();
                    SetFocused(true);
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

void FocusableButton::SetFocused(bool focus) {
    this->focused = focus;
    background->SetColor(focus ? focusedColor : normalColor);
}

bool FocusableButton::IsFocused() const {
    return focused;
}

void FocusableButton::SetContent(const std::string& content) {
    text->SetText(content);
    
    // Center the text in the button
    pu::i32 textX = x + ((width - text->GetWidth()) / 2);
    pu::i32 textY = y + ((height - text->GetHeight()) / 2);
    text->SetX(textX);
    text->SetY(textY);
}

std::string FocusableButton::GetContent() const {
    return text->GetText();
}

void FocusableButton::SetContentFont(const std::string& font) {
    text->SetFont(font);
    // Re-center text with new font
    SetContent(text->GetText());
}

void FocusableButton::SetContentColor(const pu::ui::Color& color) {
    text->SetColor(color);
}

void FocusableButton::SetBackgroundColor(const pu::ui::Color& color) {
    background->SetColor(color);
}

void FocusableButton::SetOnClick(std::function<void()> callback) {
    onClickCallback = callback;
}

void FocusableButton::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
} 