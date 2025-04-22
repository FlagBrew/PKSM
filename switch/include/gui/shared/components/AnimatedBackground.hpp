#pragma once

#include <memory>
#include <optional>
#include <pu/Plutonium>

namespace pksm::ui {

class AnimatedBackground : public pu::ui::elm::Element {
private:
    struct LayerConfig {
        bool enabled = true;
        float scrollSpeed = 100.0f;  // Pixels per second

        // Bobbing configuration
        bool bobEnabled = false;
        float bobAmplitude = 10.0f;  // Pixels to move up/down
        float bobFrequency = 2.0f;  // Complete cycles per second
        float bobDelay = 0.0f;  // Milliseconds before starting bob

        LayerConfig() = default;
        LayerConfig(float speed) : scrollSpeed(speed) {}
    };

    static constexpr int NUM_LAYERS = 3;
    LayerConfig layerConfigs[NUM_LAYERS];

    // Three layers of animated squares textures
    pu::sdl2::TextureHandle::Ref bg_textures[NUM_LAYERS];
    pu::sdl2::TextureHandle::Ref static_bg_texture;  // Static background texture
    float bg_positions[NUM_LAYERS];  // X positions for each layer
    float bg_y_offsets[NUM_LAYERS];  // Y offset for bobbing animation
    u64 lastFrameTime;
    u64 startTime;  // For timing bob delays
    pu::i32 textureWidths[NUM_LAYERS];
    pu::i32 textureHeights[NUM_LAYERS];
    pu::i32 staticBgWidth;  // Dimensions for the static background
    pu::i32 staticBgHeight;
    std::optional<pu::ui::Color> tintColor;  // Optional tint color
    float customScaleFactor;  // Scale factor for the background

    void InitializeBackground();
    void UpdateBackgroundAnimation();
    void ConfigureDefaultAnimations();  // New method to set up default animations
    void RenderStaticBackground(
        pu::ui::render::Renderer::Ref& drawer,
        const pu::i32 screenWidth,
        const pu::i32 screenHeight
    );

public:
    AnimatedBackground();
    PU_SMART_CTOR(AnimatedBackground)
    ~AnimatedBackground() override;

    // Animation configuration methods
    void ConfigureLayer(
        int layer,
        float scrollSpeed,
        bool bobEnabled = false,
        float bobAmplitude = 10.0f,
        float bobFrequency = 2.0f,
        float bobDelay = 0.0f
    );
    void EnableLayer(int layer, bool enabled);
    void EnableBobbing(int layer, bool enabled);

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