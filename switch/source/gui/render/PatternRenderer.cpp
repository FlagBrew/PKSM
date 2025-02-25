#include "gui/render/PatternRenderer.hpp"

#include <pu/ui/render/render_Renderer.hpp>
#include <sstream>

#include "utils/Logger.hpp"
#include "utils/SDLHelper.hpp"

namespace pksm::ui::render {

bool PatternRenderer::IsInRoundedCorner(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 cornerX,
    const pu::i32 cornerY,
    const pu::i32 radius
) {
    pu::i32 dx = x - cornerX;
    pu::i32 dy = y - cornerY;
    return (dx * dx + dy * dy) <= (radius * radius);
}

bool PatternBackground::IsInRoundedCorner(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 cornerX,
    const pu::i32 cornerY,
    const pu::i32 radius
) {
    pu::i32 dx = x - cornerX;
    pu::i32 dy = y - cornerY;
    return (dx * dx + dy * dy) <= (radius * radius);
}

PatternBackground::PatternBackground(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::i32 cornerRadius,
    const pu::ui::Color& backgroundColor,
    const pu::ui::Color& lineColor,
    const pu::i32 lineSpacing,
    const pu::i32 lineThickness
)
  : Element(),
    x(x),
    y(y),
    width(width),
    height(height),
    cornerRadius(cornerRadius),
    lineSpacing(lineSpacing),
    lineThickness(lineThickness),
    lineColor(lineColor),
    backgroundColor(backgroundColor),
    backgroundTexture(nullptr) {
    CreateBackgroundTexture();
}

PatternBackground::~PatternBackground() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

void PatternBackground::SetDimensions(const pu::i32 width, const pu::i32 height) {
    if (this->width != width || this->height != height) {
        this->width = width;
        this->height = height;
        CreateBackgroundTexture();
    }
}

void PatternBackground::SetCornerRadius(const pu::i32 radius) {
    if (this->cornerRadius != radius) {
        this->cornerRadius = radius;
        CreateBackgroundTexture();
    }
}

void PatternBackground::SetBackgroundColor(const pu::ui::Color& color) {
    if (this->backgroundColor.r != color.r || this->backgroundColor.g != color.g ||
        this->backgroundColor.b != color.b || this->backgroundColor.a != color.a) {
        this->backgroundColor = color;
        CreateBackgroundTexture();
    }
}

void PatternBackground::SetLineColor(const pu::ui::Color& color) {
    if (this->lineColor.r != color.r || this->lineColor.g != color.g || this->lineColor.b != color.b ||
        this->lineColor.a != color.a) {
        this->lineColor = color;
        CreateBackgroundTexture();
    }
}

void PatternBackground::SetLineProperties(const pu::i32 spacing, const pu::i32 thickness) {
    if (this->lineSpacing != spacing || this->lineThickness != thickness) {
        this->lineSpacing = spacing;
        this->lineThickness = thickness;
        CreateBackgroundTexture();
    }
}

void PatternBackground::CreateBackgroundTexture() {
    // Clean up existing texture
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }

    // Create new texture using our existing pattern generation code
    backgroundTexture =
        PatternRenderer::CreateDiagonalLinePattern(width, height, cornerRadius, lineSpacing, lineThickness, lineColor);
}

void PatternBackground::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Draw solid background first
    drawer->RenderRoundedRectangleFill(backgroundColor, x, y, width, height, cornerRadius);

    // Draw pattern overlay
    if (backgroundTexture) {
        SDL_Rect destRect = {x, y, width, height};
        SDL_RenderCopy(pu::ui::render::GetMainRenderer(), backgroundTexture, nullptr, &destRect);
    }
}

SDL_Texture* PatternRenderer::CreateDiagonalLinePattern(
    const pu::i32 width,
    const pu::i32 height,
    const pu::i32 cornerRadius,
    const pu::i32 lineSpacing,
    const pu::i32 lineThickness,
    const pu::ui::Color& lineColor
) {
    LOG_DEBUG("Starting CreateDiagonalLinePattern");

    // Safety check
    if (width <= 0 || height <= 0) {
        LOG_ERROR("Invalid dimensions for pattern texture");
        return nullptr;
    }

    // Create a smaller repeating pattern texture (128x128)
    const pu::i32 patternSize = 128;
    SDL_Texture* patternTexture = SDL_CreateTexture(
        pu::ui::render::GetMainRenderer(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        patternSize,
        patternSize
    );

    if (!patternTexture) {
        LOG_ERROR("Failed to create pattern texture");
        return nullptr;
    }
    LOG_DEBUG("Pattern texture created successfully");

    // Set up the pattern texture
    if (SDL_SetTextureBlendMode(patternTexture, pksm::sdl::BlendModeBlend()) < 0) {
        LOG_ERROR("Failed to set pattern texture blend mode");
        SDL_DestroyTexture(patternTexture);
        return nullptr;
    }

    // Draw the repeating pattern
    if (SDL_SetRenderTarget(pu::ui::render::GetMainRenderer(), patternTexture) < 0) {
        LOG_ERROR("Failed to set render target for pattern");
        SDL_DestroyTexture(patternTexture);
        return nullptr;
    }

    // Clear pattern texture
    SDL_SetRenderDrawColor(pu::ui::render::GetMainRenderer(), 0, 0, 0, 0);
    SDL_RenderClear(pu::ui::render::GetMainRenderer());

    // Draw diagonal lines in the pattern
    // Calculate exact number of lines needed for seamless tiling
    const pu::i32 numLines = (patternSize * 2) / lineSpacing + 1;
    const pu::i32 startX = -(patternSize + lineThickness);

    for (pu::i32 i = 0; i < numLines; i++) {
        pu::i32 lineStart = startX + (i * lineSpacing);

        // Draw each diagonal line
        for (pu::i32 y = -lineThickness; y < patternSize + lineThickness; y++) {
            pu::i32 x = lineStart + y;

            // Draw line thickness
            for (pu::i32 t = 0; t < lineThickness; t++) {
                pu::i32 xt = x + t;
                // Wrap around for seamless tiling
                while (xt < 0)
                    xt += patternSize;
                xt %= patternSize;

                if (y >= 0 && y < patternSize) {
                    SDL_SetRenderDrawColor(
                        pu::ui::render::GetMainRenderer(),
                        lineColor.r,
                        lineColor.g,
                        lineColor.b,
                        lineColor.a
                    );
                    SDL_RenderDrawPoint(pu::ui::render::GetMainRenderer(), xt, y);
                }
            }
        }
    }

    // Create the final texture with corner masking
    SDL_Texture* finalTexture = SDL_CreateTexture(
        pu::ui::render::GetMainRenderer(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    if (!finalTexture) {
        LOG_ERROR("Failed to create final texture");
        SDL_DestroyTexture(patternTexture);
        return nullptr;
    }

    // Set up the final texture
    if (SDL_SetTextureBlendMode(finalTexture, pksm::sdl::BlendModeBlend()) < 0) {
        LOG_ERROR("Failed to set final texture blend mode");
        SDL_DestroyTexture(patternTexture);
        SDL_DestroyTexture(finalTexture);
        return nullptr;
    }

    // Set render target to final texture
    if (SDL_SetRenderTarget(pu::ui::render::GetMainRenderer(), finalTexture) < 0) {
        LOG_ERROR("Failed to set render target for final texture");
        SDL_DestroyTexture(patternTexture);
        SDL_DestroyTexture(finalTexture);
        return nullptr;
    }

    // Clear final texture
    SDL_SetRenderDrawColor(pu::ui::render::GetMainRenderer(), 0, 0, 0, 0);
    SDL_RenderClear(pu::ui::render::GetMainRenderer());

    // Tile the pattern texture across the final texture
    for (pu::i32 y = 0; y < height; y += patternSize) {
        for (pu::i32 x = 0; x < width; x += patternSize) {
            SDL_Rect destRect = {x, y, patternSize, patternSize};
            SDL_RenderCopy(pu::ui::render::GetMainRenderer(), patternTexture, nullptr, &destRect);
        }
    }

    // Apply corner radius masking - using existing logic
    SDL_SetRenderDrawBlendMode(pu::ui::render::GetMainRenderer(), pksm::sdl::BlendModeNone());
    for (pu::i32 y = 0; y < height; y++) {
        for (pu::i32 x = 0; x < width; x++) {
            bool shouldMask = false;

            if (y < cornerRadius) {
                // Top-left corner
                if (x < cornerRadius) {
                    if (!IsInRoundedCorner(x, y, cornerRadius, cornerRadius, cornerRadius)) {
                        shouldMask = true;
                    }
                }
                // Top-right corner
                if (x > width - cornerRadius) {
                    if (!IsInRoundedCorner(x, y, width - cornerRadius, cornerRadius, cornerRadius)) {
                        shouldMask = true;
                    }
                }
            }
            if (y > height - cornerRadius) {
                // Bottom-left corner
                if (x < cornerRadius) {
                    if (!IsInRoundedCorner(x, y, cornerRadius, height - cornerRadius, cornerRadius)) {
                        shouldMask = true;
                    }
                }
                // Bottom-right corner
                if (x > width - cornerRadius) {
                    if (!IsInRoundedCorner(x, y, width - cornerRadius, height - cornerRadius, cornerRadius)) {
                        shouldMask = true;
                    }
                }
            }

            if (shouldMask) {
                SDL_SetRenderDrawColor(pu::ui::render::GetMainRenderer(), 0, 0, 0, 0);
                SDL_RenderDrawPoint(pu::ui::render::GetMainRenderer(), x, y);
            }
        }
    }

    // Clean up pattern texture
    SDL_DestroyTexture(patternTexture);

    // Reset render target
    if (SDL_SetRenderTarget(pu::ui::render::GetMainRenderer(), nullptr) < 0) {
        LOG_ERROR("Failed to reset render target");
        SDL_DestroyTexture(finalTexture);
        return nullptr;
    }

    LOG_DEBUG("Pattern texture creation complete");
    return finalTexture;
}

}  // namespace pksm::ui::render