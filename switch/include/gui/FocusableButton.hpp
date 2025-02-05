#pragma once

#include <pu/Plutonium>
#include "gui/IFocusable.hpp"

class FocusableButton : public pu::ui::elm::Element, public IFocusable {
private:
    bool focused;
    pu::ui::Color normalColor;
    pu::ui::Color focusedColor;
    std::function<void()> onClickCallback;
    std::function<void()> onTouchSelectCallback;
    
    // UI Elements
    pu::ui::elm::Rectangle::Ref background;
    pu::ui::elm::TextBlock::Ref text;
    
    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    
    // Touch state
    bool isPressed;

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

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    // Button specific methods
    void SetContent(const std::string& content);
    std::string GetContent() const;
    void SetContentFont(const std::string& font);
    void SetContentColor(const pu::ui::Color& color);
    void SetBackgroundColor(const pu::ui::Color& color);
    void SetOnClick(std::function<void()> callback);
    void SetOnTouchSelect(std::function<void()> callback);
}; 