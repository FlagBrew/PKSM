#include "gui/FocusableButton.hpp"

FocusableButton::FocusableButton(
    const pu::i32 x, const pu::i32 y, const pu::i32 w, const pu::i32 h,
    const std::string& content,
    const pu::ui::Color& normalColor,
    const pu::ui::Color& focusedColor
) : Button(x, y, w, h, content, pu::ui::Color(255, 255, 255, 255), normalColor), 
    focused(false), 
    normalColor(normalColor), 
    focusedColor(focusedColor) {
}

void FocusableButton::SetFocused(bool focus) {
    this->focused = focus;
    this->SetBackgroundColor(focus ? focusedColor : normalColor);
}

bool FocusableButton::IsFocused() const {
    return focused;
} 