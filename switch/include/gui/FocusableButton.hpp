#pragma once

#include <pu/Plutonium>

class FocusableButton : public pu::ui::elm::Button {
private:
    bool focused;
    pu::ui::Color normalColor;
    pu::ui::Color focusedColor;

public:
    FocusableButton(
        const pu::i32 x, const pu::i32 y, const pu::i32 w, const pu::i32 h,
        const std::string& content,
        const pu::ui::Color& normalColor = pu::ui::Color(0, 100, 200, 255),
        const pu::ui::Color& focusedColor = pu::ui::Color(0, 150, 255, 255)
    ) : Button(x, y, w, h, content, pu::ui::Color(255, 255, 255, 255), normalColor), 
        focused(false), 
        normalColor(normalColor), 
        focusedColor(focusedColor) {
    }

    PU_SMART_CTOR(FocusableButton)

    void SetFocused(bool focus) {
        this->focused = focus;
        this->SetBackgroundColor(focus ? focusedColor : normalColor);
    }

    bool IsFocused() const {
        return focused;
    }
}; 