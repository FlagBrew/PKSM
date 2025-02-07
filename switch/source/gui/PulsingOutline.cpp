#include "gui/PulsingOutline.hpp"

PulsingOutline::PulsingOutline(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height, const pu::ui::Color color)
    : x(x), y(y), width(width), height(height), color(color), startTime(std::chrono::steady_clock::now()), visible(true) {
}

void PulsingOutline::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    if (!visible) return;

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

    // Draw outline (4 rectangles)
    constexpr u32 BORDER_WIDTH = 2;
    
    // Top
    drawer->RenderRectangle(pulseColor, x, y, width, BORDER_WIDTH);
    // Bottom
    drawer->RenderRectangle(pulseColor, x, y + height - BORDER_WIDTH, width, BORDER_WIDTH);
    // Left
    drawer->RenderRectangle(pulseColor, x, y, BORDER_WIDTH, height);
    // Right
    drawer->RenderRectangle(pulseColor, x + width - BORDER_WIDTH, y, BORDER_WIDTH, height);
}

void PulsingOutline::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
    // No input handling needed
}

pu::i32 PulsingOutline::GetX() {
    return x;
}

pu::i32 PulsingOutline::GetY() {
    return y;
}

pu::i32 PulsingOutline::GetWidth() {
    return width;
}

pu::i32 PulsingOutline::GetHeight() {
    return height;
}

void PulsingOutline::SetX(const pu::i32 x) {
    this->x = x;
}

void PulsingOutline::SetY(const pu::i32 y) {
    this->y = y;
}

void PulsingOutline::SetWidth(const pu::i32 width) {
    this->width = width;
}

void PulsingOutline::SetHeight(const pu::i32 height) {
    this->height = height;
}

void PulsingOutline::SetVisible(bool visible) {
    this->visible = visible;
}

bool PulsingOutline::IsVisible() const {
    return visible;
} 