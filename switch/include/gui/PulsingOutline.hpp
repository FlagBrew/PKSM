#pragma once
#include <pu/Plutonium>
#include <chrono>

class PulsingOutline : public pu::ui::elm::Element {
private:
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::ui::Color color;
    std::chrono::steady_clock::time_point startTime;
    bool visible;

public:
    using Ref = std::shared_ptr<PulsingOutline>;
    
    PulsingOutline(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height, const pu::ui::Color color);
    
    static Ref New(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height, const pu::ui::Color color) {
        return std::make_shared<PulsingOutline>(x, y, width, height, color);
    }

    void OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

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
}; 