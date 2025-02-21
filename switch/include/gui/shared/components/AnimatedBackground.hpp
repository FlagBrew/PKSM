#pragma once

#include <memory>
#include <optional>
#include <pu/Plutonium>

namespace pksm::ui {

class AnimatedBackground : public pu::ui::elm::Element {
private:
    static constexpr float SCROLL_SPEED = 100.0f;  // Pixels per second

    pu::sdl2::TextureHandle::Ref bg_texture1;  // Animated squares texture
    pu::sdl2::TextureHandle::Ref static_bg_texture;  // Static background texture
    float bg_x1;
    u64 lastFrameTime;
    pu::i32 textureWidth;
    pu::i32 textureHeight;
    pu::i32 staticBgWidth;  // Dimensions for the static background
    pu::i32 staticBgHeight;
    std::optional<pu::ui::Color> tintColor;  // Optional tint color
    float customScaleFactor;  // Scale factor for the background

    void InitializeBackground();
    void UpdateBackgroundAnimation();
    void RenderStaticBackground(
        pu::ui::render::Renderer::Ref& drawer,
        const pu::i32 screenWidth,
        const pu::i32 screenHeight
    );

public:
    AnimatedBackground();
    PU_SMART_CTOR(AnimatedBackground)
    ~AnimatedBackground() override;

    // Set a tint color for the background texture
    void SetTintColor(const pu::ui::Color& color);

    // Set a custom scale factor for the background
    void SetCustomScale(float scale);

    // Required Element overrides
    pu::i32 GetX() override { return 0; }  // Background is always at 0,0
    pu::i32 GetY() override { return 0; }
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override {}

    // Other overrides
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
};

}  // namespace pksm::ui