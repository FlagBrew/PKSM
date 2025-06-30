#include "gui/render/LineRenderer.hpp"

#include <SDL2/SDL2_gfxPrimitives.h>
#include <pu/ui/render/render_Renderer.hpp>

namespace pksm::ui::render {

void LineRenderer::RenderLine(
    const pu::ui::Color& clr,
    const pu::i32 x1,
    const pu::i32 y1,
    const pu::i32 x2,
    const pu::i32 y2,
    const pu::i32 thickness
) {
    auto renderer = pu::ui::render::GetMainRenderer();
    if (thickness <= 1) {
        // Use SDL2_gfx's anti-aliased line drawing for better quality
        aalineRGBA(renderer, x1, y1, x2, y2, clr.r, clr.g, clr.b, clr.a);
    } else {
        // For thicker lines, use SDL2_gfx's thick line drawing
        thickLineRGBA(renderer, x1, y1, x2, y2, thickness, clr.r, clr.g, clr.b, clr.a);
    }
}

}  // namespace pksm::ui::render