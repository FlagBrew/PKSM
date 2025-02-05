#pragma once
#include <pu/ui/ui_Types.hpp>
#include <SDL2/SDL.h>
#include <pu/ui/elm/elm_Element.hpp>
#include <pu/ui/render/render_Renderer.hpp>

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

    class PatternBackground : public pu::ui::elm::Element {
        private:
            pu::i32 x;
            pu::i32 y;
            pu::i32 width;
            pu::i32 height;
            pu::i32 cornerRadius;
            pu::i32 lineSpacing;
            pu::i32 lineThickness;
            pu::ui::Color lineColor;
            pu::ui::Color backgroundColor;
            SDL_Texture* backgroundTexture;

            // Helper function for corner calculations
            static bool IsInRoundedCorner(
                const pu::i32 x, const pu::i32 y,
                const pu::i32 cornerX, const pu::i32 cornerY,
                const pu::i32 radius
            );

            // Create or recreate the background texture
            void CreateBackgroundTexture();

        public:
            PatternBackground(
                const pu::i32 x, const pu::i32 y,
                const pu::i32 width, const pu::i32 height,
                const pu::i32 cornerRadius,
                const pu::ui::Color& backgroundColor,
                const pu::ui::Color& lineColor = pu::ui::Color(31, 41, 139, 255),
                const pu::i32 lineSpacing = 14,
                const pu::i32 lineThickness = 8
            );
            
            PU_SMART_CTOR(PatternBackground)
            ~PatternBackground();

            // Element interface implementation
            pu::i32 GetX() override { return x; }
            void SetX(const pu::i32 x) { this->x = x; }
            pu::i32 GetY() override { return y; }
            void SetY(const pu::i32 y) { this->y = y; }
            pu::i32 GetWidth() override { return width; }
            pu::i32 GetHeight() override { return height; }

            // Customization methods
            void SetDimensions(const pu::i32 width, const pu::i32 height);
            void SetCornerRadius(const pu::i32 radius);
            void SetBackgroundColor(const pu::ui::Color& color);
            void SetLineColor(const pu::ui::Color& color);
            void SetLineProperties(const pu::i32 spacing, const pu::i32 thickness);

            // Rendering
            void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
            void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override {}
    };

} 