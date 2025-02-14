#pragma once
#include <pu/Plutonium>

namespace pksm::ui {

class StaticOutlineBase : public pu::ui::elm::Element {
protected:
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::ui::Color color;
    bool visible;
    u32 borderWidth;

    StaticOutlineBase(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::ui::Color color,
        const u32 borderWidth
    );

public:
    virtual ~StaticOutlineBase() = default;

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void SetX(const pu::i32 x);
    void SetY(const pu::i32 y);
    void SetWidth(const pu::i32 width);
    void SetHeight(const pu::i32 height);
    void SetVisible(bool visible);
    bool IsVisible() const;
    void SetBorderWidth(const u32 width);
    u32 GetBorderWidth() const;

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override = 0;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    static constexpr u32 DEFAULT_BORDER_WIDTH = 8;
};

class CircularOutline : public StaticOutlineBase {
public:
    CircularOutline(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 diameter,
        const pu::ui::Color color,
        const u32 borderWidth = DEFAULT_BORDER_WIDTH
    );
    PU_SMART_CTOR(CircularOutline)

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
};

}  // namespace pksm::ui