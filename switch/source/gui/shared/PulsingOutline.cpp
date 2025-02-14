#include "gui/shared/components/PulsingOutline.hpp"

pksm::ui::PulsingOutlineBase::PulsingOutlineBase(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::ui::Color color,
    const u32 borderWidth
)
  : StaticOutlineBase(x, y, width, height, color, borderWidth), startTime(std::chrono::steady_clock::now()) {}

pu::ui::Color pksm::ui::PulsingOutlineBase::calculatePulseColor() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    float time = (duration % 1000) / 1000.0f;  // Convert to 0-1 range

    // Calculate pulse intensity (0.0 to 1.0)
    float intensity = std::abs(time - 0.5f) * 2.0f;  // Creates a triangle wave

    // Interpolate color
    return pu::ui::Color{
        static_cast<u8>(color.r + ((255 - color.r) * intensity)),
        static_cast<u8>(color.g + ((255 - color.g) * intensity)),
        static_cast<u8>(color.b + ((255 - color.b) * intensity)),
        255
    };
}

// RectangularPulsingOutline implementation
pksm::ui::RectangularPulsingOutline::RectangularPulsingOutline(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::ui::Color color,
    const pu::i32 radius,
    const u32 borderWidth
)
  : PulsingOutlineBase(x, y, width, height, color, borderWidth), radius(radius) {}

void pksm::ui::RectangularPulsingOutline::OnRender(
    pu::ui::render::Renderer::Ref& drawer,
    const pu::i32 x,
    const pu::i32 y
) {
    if (!visible)
        return;

    pu::ui::Color pulseColor = calculatePulseColor();

    for (u32 i = 0; i < borderWidth; i++) {
        drawer->RenderRoundedRectangle(
            pulseColor,
            x + i,
            y + i,
            width - (2 * i),
            height - (2 * i),
            radius > static_cast<pu::i32>(i) ? radius - static_cast<pu::i32>(i) : 0
        );
    }
}

// CircularPulsingOutline implementation
pksm::ui::CircularPulsingOutline::CircularPulsingOutline(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 diameter,
    const pu::ui::Color color,
    const u32 borderWidth
)
  : PulsingOutlineBase(x, y, diameter, diameter, color, borderWidth) {}

void pksm::ui::CircularPulsingOutline::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (!visible)
        return;

    pu::ui::Color pulseColor = calculatePulseColor();

    pu::i32 baseRadius = this->width / 2;  // treat 'width' as diameter for circles
    pu::i32 centerX = x + baseRadius;
    pu::i32 centerY = y + baseRadius;

    // Draw from inside out, adding to radius each time
    for (u32 i = 0; i < borderWidth; i++) {
        drawer->RenderCircle(pulseColor, centerX, centerY, baseRadius + i);
    }
}