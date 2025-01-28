#pragma once

#include <pu/Plutonium>
#include "gui/IFocusable.hpp"

class FocusableButton : public pu::ui::elm::Button, public IFocusable {
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
    );

    static Ref New(
        const pu::i32 x, const pu::i32 y, const pu::i32 w, const pu::i32 h,
        const std::string& content,
        const pu::ui::Color& normalColor = pu::ui::Color(0, 100, 200, 255),
        const pu::ui::Color& focusedColor = pu::ui::Color(0, 150, 255, 255)
    ) {
        return std::make_shared<FocusableButton>(x, y, w, h, content, normalColor, focusedColor);
    }

    PU_SMART_CTOR(FocusableButton)

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool IsFocused() const override;
}; 