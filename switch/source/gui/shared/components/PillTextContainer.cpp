#include "gui/shared/components/PillTextContainer.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

PillTextContainer::PillTextContainer(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const std::string& text
)
  : Element(),
    ShakeableWithOutline(
        pksm::ui::PulsingOutline::New(x, y, width, height, pksm::ui::global::OUTLINE_COLOR, CORNER_RADIUS)
    ),
    x(x),
    y(y),
    width(width),
    height(height),
    containerColor(DEFAULT_CONTAINER_COLOR),
    focused(false) {
    // Create the text block
    textBlock = pu::ui::elm::TextBlock::New(0, 0, text);
    textBlock->SetColor(DEFAULT_TEXT_COLOR);
    textBlock->SetFont(global::MakeMediumFontName(global::FONT_SIZE_HEADER));

    // Setup touch handler
    touchHandler.SetOnTouchUpInside([this]() { this->RequestFocus(); });

    // Position the text
    UpdateTextPosition();

    LOG_DEBUG("PillTextContainer component initialization complete");
}

pu::i32 PillTextContainer::GetX() {
    return x;
}

pu::i32 PillTextContainer::GetY() {
    return y;
}

pu::i32 PillTextContainer::GetWidth() {
    return width;
}

pu::i32 PillTextContainer::GetHeight() {
    return height;
}

void PillTextContainer::SetText(const std::string& text) {
    textBlock->SetText(text);
    UpdateTextPosition();
}

std::string PillTextContainer::GetText() const {
    return textBlock->GetText();
}

void PillTextContainer::SetFont(const std::string& font) {
    textBlock->SetFont(font);
    UpdateTextPosition();
}

void PillTextContainer::SetContainerColor(const pu::ui::Color& color) {
    containerColor = color;
}

void PillTextContainer::SetTextColor(const pu::ui::Color& color) {
    textBlock->SetColor(color);
}

void PillTextContainer::SetX(pu::i32 newX) {
    x = newX;
    UpdateTextPosition();
    pulsingOutline->SetX(newX);
}

void PillTextContainer::SetY(pu::i32 newY) {
    y = newY;
    UpdateTextPosition();
    pulsingOutline->SetY(newY);
}

void PillTextContainer::SetWidth(pu::i32 newWidth) {
    width = newWidth;
    UpdateTextPosition();
    pulsingOutline->SetWidth(newWidth);
}

void PillTextContainer::SetHeight(pu::i32 newHeight) {
    height = newHeight;
    UpdateTextPosition();
    pulsingOutline->SetHeight(newHeight);
}

void PillTextContainer::SetDimensions(pu::i32 newX, pu::i32 newY, pu::i32 newWidth, pu::i32 newHeight) {
    x = newX;
    y = newY;
    width = newWidth;
    height = newHeight;
    pulsingOutline->SetX(newX);
    pulsingOutline->SetY(newY);
    pulsingOutline->SetHeight(newHeight);
    pulsingOutline->SetWidth(newWidth);
    UpdateTextPosition();
}

void PillTextContainer::UpdateTextPosition() {
    // Center the text within the container
    pu::i32 textX = x + (width - textBlock->GetWidth()) / 2;
    pu::i32 textY = y + (height - textBlock->GetHeight()) / 2;

    textBlock->SetX(textX);
    textBlock->SetY(textY);
}

void PillTextContainer::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render the pill-shaped container
    drawer->RenderRoundedRectangleFill(containerColor, x, y, width, height, CORNER_RADIUS);

    // Render the text - note it has absolute coordinates so no adjustment needed
    textBlock->OnRender(drawer, textBlock->GetX(), textBlock->GetY());

    if (focused) {
        pulsingOutline->OnRender(drawer, pulsingOutline->GetX(), pulsingOutline->GetY());
    }
}

void PillTextContainer::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    touchHandler.HandleInput(touch_pos, GetX(), GetY(), GetWidth(), GetHeight());
}

void PillTextContainer::SetFocused(bool focus) {
    LOG_DEBUG("[PillTextContainer] SetFocused: " + std::to_string(focus));
    this->focused = focus;
    pulsingOutline->SetVisible(focus);
}

bool PillTextContainer::IsFocused() const {
    return focused;
}

}  // namespace pksm::ui