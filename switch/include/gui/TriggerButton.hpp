#pragma once

#include <pu/Plutonium>

namespace pksm::ui {

class TriggerButton : public pu::ui::elm::Element {
public:
    enum class Side {
        Left,
        Right
    };

    TriggerButton(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::i32 cornerRadius,
        const Side side,
        const pu::ui::Color& backgroundColor = pu::ui::Color(70, 70, 70, 255),
        const pu::ui::Color& textColor = pu::ui::Color(255, 255, 255, 255)
    );

    static Ref New(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::i32 cornerRadius,
        const Side side,
        const pu::ui::Color& backgroundColor = pu::ui::Color(70, 70, 70, 255),
        const pu::ui::Color& textColor = pu::ui::Color(255, 255, 255, 255)
    ) {
        return std::make_shared<TriggerButton>(x, y, width, height, cornerRadius, side, backgroundColor, textColor);
    }

    PU_SMART_CTOR(TriggerButton)
    ~TriggerButton();

    // Element implementation
    pu::i32 GetX() override { return x; }
    pu::i32 GetY() override { return y; }
    pu::i32 GetWidth() override { return width; }
    pu::i32 GetHeight() override { return height; }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override {}

    // Customization methods
    void SetBackgroundColor(const pu::ui::Color& color);
    void SetTextColor(const pu::ui::Color& color);

private:
    void UpdateTextPosition();

    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::i32 cornerRadius;
    Side side;
    pu::ui::Color backgroundColor;
    pu::ui::Color textColor;
    std::shared_ptr<pu::ui::elm::TextBlock> textBlock;
};

} 