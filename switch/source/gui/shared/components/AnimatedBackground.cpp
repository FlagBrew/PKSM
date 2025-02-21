#include "gui/shared/components/AnimatedBackground.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <pu/ui/render/render_SDL2.hpp>

#include "utils/Logger.hpp"

namespace pksm::ui {

AnimatedBackground::AnimatedBackground()
  : Element(),
    bg_texture1(nullptr),
    bg_texture2(nullptr),
    bg_x1(0),
    bg_x2(0),
    lastFrameTime(SDL_GetTicks64()),
    textureWidth(0),
    textureHeight(0),
    tintColor(std::nullopt),
    customScaleFactor(1.5f) {
    InitializeBackground();
}

AnimatedBackground::~AnimatedBackground() = default;

void AnimatedBackground::SetTintColor(const pu::ui::Color& color) {
    tintColor = color;
    if (bg_texture1) {
        SDL_SetTextureColorMod(bg_texture1->Get(), color.r, color.g, color.b);
    }
}

void AnimatedBackground::InitializeBackground() {
    LOG_DEBUG("Initializing animated background");

    // Load background texture using the LoadImage function from the Plutonium codebase
    pu::sdl2::Texture loadedTexture = pu::ui::render::LoadImage("romfs:/gfx/ui/anim_squares.png");
    if (!loadedTexture) {
        LOG_ERROR("Failed to load background texture");
        return;
    }

    // Create a texture handle from the loaded texture
    bg_texture1 = pu::sdl2::TextureHandle::New(loadedTexture);

    // Get actual texture dimensions
    textureWidth = pu::ui::render::GetTextureWidth(bg_texture1->Get());
    textureHeight = pu::ui::render::GetTextureHeight(bg_texture1->Get());

    // Set blending mode for smooth animation
    SDL_SetTextureBlendMode(bg_texture1->Get(), SDL_BLENDMODE_BLEND);

    // Apply tint color if one is set
    if (tintColor) {
        SDL_SetTextureColorMod(bg_texture1->Get(), tintColor->r, tintColor->g, tintColor->b);
    }

    // Initialize scroll position
    bg_x1 = 0;

    LOG_DEBUG("Background initialized successfully");
}

void AnimatedBackground::UpdateBackgroundAnimation() {
    // Calculate time delta for smooth animation
    const auto currentTime = SDL_GetTicks64();
    const auto deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // Update scroll position
    const float moveAmount = (SCROLL_SPEED * deltaTime) / 1000.0f;
    bg_x1 -= moveAmount;

    // Calculate scaling to match what's used in rendering
    auto [screenWidth, screenHeight] = pu::ui::render::GetDimensions();
    float baseScale = std::min((float)screenWidth / textureWidth, (float)screenHeight / textureHeight);
    float finalScale = baseScale * customScaleFactor;
    float scaledWidth = textureWidth * finalScale;

    // Reset when completely off screen, using scaled width
    if (bg_x1 <= -scaledWidth) {
        bg_x1 += scaledWidth;  // Reset by one scaled texture width
    }
}

void AnimatedBackground::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (!bg_texture1) {
        return;
    }

    // Update animation before rendering
    UpdateBackgroundAnimation();

    // Get current screen dimensions
    auto [screenWidth, screenHeight] = pu::ui::render::GetDimensions();

    // Calculate base scaling to fit screen while maintaining aspect ratio
    float baseScale = std::min((float)screenWidth / textureWidth, (float)screenHeight / textureHeight);

    // Apply custom scale factor
    float finalScale = baseScale * customScaleFactor;

    pu::i32 scaledWidth = static_cast<pu::i32>(textureWidth * finalScale);
    pu::i32 scaledHeight = static_cast<pu::i32>(textureHeight * finalScale);

    // Center vertically only
    pu::i32 yOffset = (screenHeight - scaledHeight) / 2;

    // Create render options for the texture
    pu::ui::render::TextureRenderOptions opts;
    opts.width = scaledWidth;
    opts.height = scaledHeight;

    // Calculate how many textures we need to cover the screen plus extra for smooth scrolling
    int numTextures = (screenWidth / scaledWidth) + 3;  // +3 to ensure smooth transitions

    // Render textures starting from the current scroll position
    for (int i = 0; i < numTextures; i++) {
        float xPos = bg_x1 + (i * scaledWidth);
        drawer->RenderTexture(bg_texture1->Get(), static_cast<pu::i32>(xPos), yOffset, opts);
    }
}

pu::i32 AnimatedBackground::GetWidth() {
    return textureWidth;
}

pu::i32 AnimatedBackground::GetHeight() {
    return textureHeight;
}

void AnimatedBackground::SetCustomScale(float scale) {
    customScaleFactor = std::max(0.1f, scale);  // Ensure scale doesn't go too small
}

}  // namespace pksm::ui