#pragma once
#include <pu/ui/ui_Types.hpp>
#include <SDL2/SDL.h>

namespace pksm::ui::render {

    class PatternRenderer {
        public:
            // Creates a texture with diagonal line pattern and rounded corners
            static SDL_Texture* CreateDiagonalLinePattern(
                const pu::i32 width,
                const pu::i32 height,
                const pu::i32 cornerRadius,
                const pu::i32 lineSpacing,
                const pu::i32 lineThickness,
                const pu::ui::Color& lineColor
            );

            // Helper to check if a point is within the rounded corner bounds
            static bool IsInRoundedCorner(
                const pu::i32 x,
                const pu::i32 y,
                const pu::i32 cornerX,
                const pu::i32 cornerY,
                const pu::i32 radius
            );
    };

} 