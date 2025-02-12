#include "gui/shared/components/PulsingOutline.hpp"

pksm::ui::PulsingOutline::PulsingOutline(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::ui::Color color,
    const pu::i32 radius
)
  : x(x),
    y(y),
    width(width),
    height(height),
    color(color),
    radius(radius),
    startTime(std::chrono::steady_clock::now()),
    visible(true) {}

void pksm::ui::PulsingOutline::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (!visible)
        return;

    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    float time = (duration % 1000) / 1000.0f;  // Convert to 0-1 range

    // Calculate pulse intensity (0.0 to 1.0)
    float intensity = std::abs(time - 0.5f) * 2.0f;  // Creates a triangle wave

    // Interpolate color
    pu::ui::Color pulseColor = {
        static_cast<u8>(color.r + ((255 - color.r) * intensity)),
        static_cast<u8>(color.g + ((255 - color.g) * intensity)),
        static_cast<u8>(color.b + ((255 - color.b) * intensity)),
        255
    };

    constexpr u32 BORDER_WIDTH = 3;

    // Draw concentric rounded rectangles to create border effect
    for (u32 i = 0; i < BORDER_WIDTH; i++) {
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

void pksm::ui::PulsingOutline::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // No input handling needed
}

pu::i32 pksm::ui::PulsingOutline::GetX() {
    return x;
}

pu::i32 pksm::ui::PulsingOutline::GetY() {
    return y;
}

pu::i32 pksm::ui::PulsingOutline::GetWidth() {
    return width;
}

pu::i32 pksm::ui::PulsingOutline::GetHeight() {
    return height;
}

void pksm::ui::PulsingOutline::SetX(const pu::i32 x) {
    this->x = x;
}

void pksm::ui::PulsingOutline::SetY(const pu::i32 y) {
    this->y = y;
}

void pksm::ui::PulsingOutline::SetWidth(const pu::i32 width) {
    this->width = width;
}

void pksm::ui::PulsingOutline::SetHeight(const pu::i32 height) {
    this->height = height;
}

void pksm::ui::PulsingOutline::SetVisible(bool visible) {
    this->visible = visible;
}

bool pksm::ui::PulsingOutline::IsVisible() const {
    return visible;
}