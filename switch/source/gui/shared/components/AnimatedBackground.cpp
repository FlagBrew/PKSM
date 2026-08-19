#include "gui/shared/components/AnimatedBackground.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <pu/ui/render/render_SDL2.hpp>

#include "utils/Logger.hpp"
#include "utils/SDLHelper.hpp"

namespace pksm::ui {

AnimatedBackground::AnimatedBackground()
  : Element(),
    bg_textures{nullptr, nullptr, nullptr},
    static_bg_texture(nullptr),
    bg_positions{0, 0, 0},
    bg_y_offsets{0, 0, 0},
    lastFrameTime(SDL_GetTicks64()),
    startTime(SDL_GetTicks64()),
    textureWidths{0, 0, 0},
    textureHeights{0, 0, 0},
    staticBgWidth(0),
    staticBgHeight(0),
    customScaleFactor(1.5f) {
    InitializeBackground();
    ConfigureDefaultAnimations();
}

AnimatedBackground::~AnimatedBackground() = default;

void AnimatedBackground::InitializeBackground() {
    LOG_DEBUG("Initializing animated background");

    // Decoded once per app run and shared across screens. Deliberately leaked:
    // a static handle would destroy its texture after Renderer::Finalize
    struct SharedTextures {
        pu::sdl2::TextureHandle::Ref staticBg;
        pu::sdl2::TextureHandle::Ref layers[3];
    };
    static SharedTextures* shared = nullptr;
    if (!shared) {
        shared = new SharedTextures();
        pu::sdl2::Texture staticTexture = pu::ui::render::LoadImage("romfs:/gfx/ui/bg_style.png");
        if (!staticTexture) {
            LOG_ERROR("Failed to load static background texture");
        } else {
            shared->staticBg = pu::sdl2::TextureHandle::New(staticTexture);
        }
        const char* layerPaths[] = {
            "romfs:/gfx/ui/anim_squares_1.png",
            "romfs:/gfx/ui/anim_squares_2.png",
            "romfs:/gfx/ui/anim_squares_3.png"
        };
        for (int i = 0; i < 3; i++) {
            pu::sdl2::Texture loadedTexture = pu::ui::render::LoadImage(layerPaths[i]);
            if (!loadedTexture) {
                LOG_ERROR("Failed to load animated squares texture");
                continue;
            }
            shared->layers[i] = pu::sdl2::TextureHandle::New(loadedTexture);
        }
    }

    if (shared->staticBg) {
        static_bg_texture = shared->staticBg;
        staticBgWidth = pu::ui::render::GetTextureWidth(static_bg_texture->Get());
        staticBgHeight = pu::ui::render::GetTextureHeight(static_bg_texture->Get());
    }

    for (int i = 0; i < 3; i++) {
        if (!shared->layers[i]) {
            continue;
        }
        bg_textures[i] = shared->layers[i];

        // Get actual texture dimensions
        textureWidths[i] = pu::ui::render::GetTextureWidth(bg_textures[i]->Get());
        textureHeights[i] = pu::ui::render::GetTextureHeight(bg_textures[i]->Get());

        // Set blending mode for smooth animation
        SDL_SetTextureBlendMode(bg_textures[i]->Get(), pksm::sdl::BlendModeBlend());

        // Initialize scroll position
        bg_positions[i] = 0;
    }

    LOG_DEBUG("Background initialized successfully");
}

void AnimatedBackground::RenderStaticBackground(
    pu::ui::render::Renderer::Ref& drawer,
    const pu::i32 screenWidth,
    const pu::i32 screenHeight
) {
    if (!static_bg_texture) {
        return;
    }

    // Calculate scaling factors for width and height
    float scaleX = static_cast<float>(screenWidth) / staticBgWidth;
    float scaleY = static_cast<float>(screenHeight) / staticBgHeight;

    // Use the larger scale factor to ensure the image fills the screen
    float scale = std::max(scaleX, scaleY);

    // Calculate dimensions that maintain aspect ratio and fill screen
    pu::i32 renderWidth = static_cast<pu::i32>(staticBgWidth * scale);
    pu::i32 renderHeight = static_cast<pu::i32>(staticBgHeight * scale);

    // Calculate position to center the image
    pu::i32 xPos = (screenWidth - renderWidth) / 2;
    pu::i32 yPos = (screenHeight - renderHeight) / 2;

    // Create render options
    pu::ui::render::TextureRenderOptions opts;
    opts.width = renderWidth;
    opts.height = renderHeight;

    // Render the static background
    drawer->RenderTexture(static_bg_texture->Get(), xPos, yPos, opts);
}

void AnimatedBackground::ConfigureDefaultAnimations() {
    ConfigureLayer(0, 80.0f, true, 7.0f, 0.3f, 0.0f);  // most muted layer
    ConfigureLayer(1, 100.0f, true, 4.0f, 0.5f, 0.0f);
    ConfigureLayer(2, 120.0f, false);  // least muted layer
}

void AnimatedBackground::ConfigureLayer(
    int layer,
    float scrollSpeed,
    bool bobEnabled,
    float bobAmplitude,
    float bobFrequency,
    float bobDelay
) {
    if (layer < 0 || layer >= NUM_LAYERS) {
        return;
    }

    layerConfigs[layer].enabled = true;
    layerConfigs[layer].scrollSpeed = scrollSpeed;
    layerConfigs[layer].bobEnabled = bobEnabled;
    layerConfigs[layer].bobAmplitude = bobAmplitude;
    layerConfigs[layer].bobFrequency = bobFrequency;
    layerConfigs[layer].bobDelay = bobDelay;
}

void AnimatedBackground::UpdateBackgroundAnimation() {
    // Time delta, clamped so a stall doesn't teleport every layer in one frame
    const auto currentTime = SDL_GetTicks64();
    const auto deltaTime = std::min<u64>(currentTime - lastFrameTime, 50);
    lastFrameTime = currentTime;

    // Time since animation started (for bobbing delays)
    const auto timeSinceStart = currentTime - startTime;

    auto [screenWidth, screenHeight] = pu::ui::render::GetDimensions();

    for (int i = 0; i < NUM_LAYERS; i++) {
        if (!bg_textures[i] || !layerConfigs[i].enabled)
            continue;

        // Update horizontal scrolling
        const float moveAmount = (layerConfigs[i].scrollSpeed * deltaTime) / 1000.0f;
        bg_positions[i] -= moveAmount;

        // Calculate scaling to match what's used in rendering
        float baseScale = std::min((float)screenWidth / textureWidths[i], (float)screenHeight / textureHeights[i]);
        float finalScale = baseScale * customScaleFactor;
        float scaledWidth = textureWidths[i] * finalScale;

        // Reset when completely off screen, using scaled width
        if (bg_positions[i] <= -scaledWidth) {
            bg_positions[i] += scaledWidth;
        }

        // Update bobbing animation
        if (layerConfigs[i].bobEnabled && timeSinceStart >= layerConfigs[i].bobDelay) {
            float timeForBob = (timeSinceStart - layerConfigs[i].bobDelay) / 1000.0f;
            bg_y_offsets[i] = layerConfigs[i].bobAmplitude *
                std::sin(2.0f * M_PI * layerConfigs[i].bobFrequency * timeForBob);
        }
    }
}

void AnimatedBackground::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Get current screen dimensions
    auto [screenWidth, screenHeight] = pu::ui::render::GetDimensions();

    // Render static background first
    RenderStaticBackground(drawer, screenWidth, screenHeight);

    // Update animation before rendering
    UpdateBackgroundAnimation();

    // Then render each layer of animated squares
    for (int layer = 0; layer < NUM_LAYERS; layer++) {
        if (!bg_textures[layer] || !layerConfigs[layer].enabled) {
            continue;
        }

        // Calculate base scaling to fit screen while maintaining aspect ratio
        float baseScale =
            std::min((float)screenWidth / textureWidths[layer], (float)screenHeight / textureHeights[layer]);

        // Apply custom scale factor
        float finalScale = baseScale * customScaleFactor;

        pu::i32 scaledWidth = static_cast<pu::i32>(textureWidths[layer] * finalScale);
        pu::i32 scaledHeight = static_cast<pu::i32>(textureHeights[layer] * finalScale);

        // Center vertically and add bobbing offset
        pu::i32 yOffset = (screenHeight - scaledHeight) / 2 + static_cast<pu::i32>(bg_y_offsets[layer]);

        // Create render options for the texture
        pu::ui::render::TextureRenderOptions opts;
        opts.width = scaledWidth;
        opts.height = scaledHeight;

        // Calculate how many textures we need to cover the screen plus extra for smooth scrolling
        int numTextures = (screenWidth / scaledWidth) + 3;  // +3 to ensure smooth transitions

        // Render textures starting from the current scroll position
        for (int i = 0; i < numTextures; i++) {
            float xPos = bg_positions[layer] + (i * scaledWidth);
            drawer->RenderTexture(bg_textures[layer]->Get(), static_cast<pu::i32>(xPos), yOffset, opts);
        }
    }
}

pu::i32 AnimatedBackground::GetWidth() {
    return textureWidths[0];  // Return width of first layer as reference
}

pu::i32 AnimatedBackground::GetHeight() {
    return textureHeights[0];  // Return height of first layer as reference
}

}  // namespace pksm::ui