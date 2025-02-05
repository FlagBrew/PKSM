#include "ui/render/PatternRenderer.hpp"
#include <pu/ui/render/render_Renderer.hpp>
#include "utils/Logger.hpp"
#include <sstream>

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

        #ifdef DEBUG
        {
            std::stringstream ss;
            ss << "Creating texture with dimensions: " << width << "x" << height
               << ", spacing: " << lineSpacing << ", thickness: " << lineThickness;
            LOG_DEBUG(ss.str());
        }
        #endif

        // Create render target texture
        SDL_Texture* texture = SDL_CreateTexture(
            pu::ui::render::GetMainRenderer(),
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        );

        if (!texture) {
            LOG_ERROR("Failed to create SDL texture");
            return nullptr;
        }
        LOG_DEBUG("SDL texture created successfully");

        // Set blend mode for transparency
        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) < 0) {
            LOG_ERROR("Failed to set texture blend mode");
            SDL_DestroyTexture(texture);
            return nullptr;
        }

        // Set this texture as the render target
        if (SDL_SetRenderTarget(pu::ui::render::GetMainRenderer(), texture) < 0) {
            LOG_ERROR("Failed to set render target");
            SDL_DestroyTexture(texture);
            return nullptr;
        }

        LOG_DEBUG("Starting to draw diagonal lines");
        
        // Clear with transparent background
        SDL_SetRenderDrawColor(pu::ui::render::GetMainRenderer(), 0, 0, 0, 0);
        SDL_RenderClear(pu::ui::render::GetMainRenderer());

        // Draw diagonal lines
        const pu::i32 startX = -static_cast<pu::i32>(height);  // Start off screen to the left
        const pu::i32 endX = width + height;  // End off screen to the right

        for (pu::i32 i = startX; i <= endX; i += lineSpacing) {
            // For each point along the line
            for (pu::i32 y = 0; y < height; y++) {
                pu::i32 x = i + y;
                
                // Skip if outside the rounded corners
                if (y < cornerRadius) {
                    // Top-left corner
                    if (x < cornerRadius) {
                        if (!IsInRoundedCorner(x, y, cornerRadius, cornerRadius, cornerRadius)) {
                            continue;
                        }
                    }
                    // Top-right corner
                    if (x + lineThickness > width - cornerRadius) {
                        if (!IsInRoundedCorner(x + lineThickness, y, width - cornerRadius, cornerRadius, cornerRadius)) {
                            continue;
                        }
                    }
                }
                if (y > height - cornerRadius) {
                    // Bottom-left corner
                    if (x < cornerRadius) {
                        if (!IsInRoundedCorner(x, y, cornerRadius, height - cornerRadius, cornerRadius)) {
                            continue;
                        }
                    }
                    // Bottom-right corner
                    if (x + lineThickness > width - cornerRadius) {
                        if (!IsInRoundedCorner(x + lineThickness, y, width - cornerRadius, height - cornerRadius, cornerRadius)) {
                            continue;
                        }
                    }
                }

                // Draw the point if it's within bounds
                if (x >= 0 && x < width) {
                    for (pu::i32 t = 0; t < lineThickness; t++) {
                        if (x + t < width) {
                            SDL_SetRenderDrawColor(pu::ui::render::GetMainRenderer(), 
                                lineColor.r, lineColor.g, lineColor.b, lineColor.a);
                            SDL_RenderDrawPoint(pu::ui::render::GetMainRenderer(), x + t, y);
                        }
                    }
                }
            }
        }

        // Reset render target
        if (SDL_SetRenderTarget(pu::ui::render::GetMainRenderer(), nullptr) < 0) {
            LOG_ERROR("Failed to reset render target");
            SDL_DestroyTexture(texture);
            return nullptr;
        }

        LOG_DEBUG("Pattern texture creation complete");
        return texture;
    }

} 