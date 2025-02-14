#include "gui/shared/components/StaticOutline.hpp"

// StaticOutlineBase implementation
pksm::ui::StaticOutlineBase::StaticOutlineBase(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::ui::Color color,
    const u32 borderWidth
)
  : x(x), y(y), width(width), height(height), color(color), visible(true), borderWidth(borderWidth) {}

pu::i32 pksm::ui::StaticOutlineBase::GetX() {
    return x;
}
pu::i32 pksm::ui::StaticOutlineBase::GetY() {
    return y;
}
pu::i32 pksm::ui::StaticOutlineBase::GetWidth() {
    return width;
}
pu::i32 pksm::ui::StaticOutlineBase::GetHeight() {
    return height;
}
void pksm::ui::StaticOutlineBase::SetX(const pu::i32 x) {
    this->x = x;
}
void pksm::ui::StaticOutlineBase::SetY(const pu::i32 y) {
    this->y = y;
}
void pksm::ui::StaticOutlineBase::SetWidth(const pu::i32 width) {
    this->width = width;
}
void pksm::ui::StaticOutlineBase::SetHeight(const pu::i32 height) {
    this->height = height;
}
void pksm::ui::StaticOutlineBase::SetVisible(bool visible) {
    this->visible = visible;
}
bool pksm::ui::StaticOutlineBase::IsVisible() const {
    return visible;
}
void pksm::ui::StaticOutlineBase::SetBorderWidth(const u32 width) {
    borderWidth = width;
}
u32 pksm::ui::StaticOutlineBase::GetBorderWidth() const {
    return borderWidth;
}

void pksm::ui::StaticOutlineBase::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // No input handling needed
}

// CircularOutline implementation
pksm::ui::CircularOutline::CircularOutline(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 diameter,
    const pu::ui::Color color,
    const u32 borderWidth
)
  : StaticOutlineBase(x, y, diameter, diameter, color, borderWidth) {}

void pksm::ui::CircularOutline::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    if (!visible)
        return;

    pu::i32 baseRadius = width / 2;
    pu::i32 centerX = x + baseRadius;
    pu::i32 centerY = y + baseRadius;

    for (u32 i = 0; i < borderWidth; i++) {
        drawer->RenderCircle(color, centerX, centerY, baseRadius + i);
    }
}