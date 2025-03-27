#include "gui/shared/components/BoxSpacesButton.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

BoxSpacesButton::BoxSpacesButton(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 height,
    pu::sdl2::TextureHandle::Ref iconTexture
)
  : Element(),
    ShakeableWithOutline(pksm::ui::PulsingOutline::New(x, y, 0, height, pksm::ui::global::OUTLINE_COLOR, CORNER_RADIUS)),
    x(x),
    y(y),
    width(0),
    height(height),
    containerColor(DEFAULT_CONTAINER_COLOR),
    focused(false) {
    // Create container for child elements
    container = pu::ui::Container::New(0, 0, 0, height);

    // Create the text block
    textBlock = pu::ui::elm::TextBlock::New(0, 0, BUTTON_TEXT);
    textBlock->SetColor(DEFAULT_TEXT_COLOR);
    textBlock->SetFont(global::MakeMediumFontName(global::FONT_SIZE_BOX_SPACES_BUTTON));

    // Create the icon image
    icon = pu::ui::elm::Image::New(0, 0, iconTexture);
    icon->SetWidth(ICON_SIZE);
    icon->SetHeight(ICON_SIZE);

    // Add elements to container
    container->Add(icon);
    container->Add(textBlock);

    // Setup touch handler
    touchHandler.SetOnTouchUpInside([this]() { this->RequestFocus(); });

    // Calculate width and update layout
    CalculateWidth();
    UpdateLayout();

    LOG_DEBUG("BoxSpacesButton component initialization complete");
}

void BoxSpacesButton::CalculateWidth() {
    pu::i32 textWidth = textBlock->GetWidth();
    pu::i32 iconWidth = icon->GetWidth();

    // Calculate total width
    width = PADDING_HORIZONTAL;

    // Add icon width and margin if icon exists
    if (iconWidth > 0) {
        width += iconWidth + ICON_TEXT_MARGIN;
    }

    // Add text width and right padding
    width += textWidth + PADDING_HORIZONTAL;

    // Update outline width
    pulsingOutline->SetWidth(width);
}

void BoxSpacesButton::UpdateLayout() {
    // Start positioning from left padding
    pu::i32 currentX = PADDING_HORIZONTAL;

    pu::i32 iconY = (height - icon->GetHeight()) / 2;
    icon->SetX(currentX);
    icon->SetY(iconY);
    currentX += icon->GetWidth() + ICON_TEXT_MARGIN;

    // Position text vertically centered
    pu::i32 textY = (height - textBlock->GetHeight()) / 2;
    textBlock->SetX(currentX);
    textBlock->SetY(textY);
}

pu::i32 BoxSpacesButton::GetX() {
    return x;
}

pu::i32 BoxSpacesButton::GetY() {
    return y;
}

pu::i32 BoxSpacesButton::GetWidth() {
    return width;
}

pu::i32 BoxSpacesButton::GetHeight() {
    return height;
}

void BoxSpacesButton::SetX(pu::i32 newX) {
    x = newX;
    pulsingOutline->SetX(newX);
}

void BoxSpacesButton::SetY(pu::i32 newY) {
    y = newY;
    pulsingOutline->SetY(newY);
}

void BoxSpacesButton::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render the pill-shaped container
    drawer->RenderRoundedRectangleFill(containerColor, x, y, width, height, CORNER_RADIUS);

    // Render container elements (icon and text)
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, x + element->GetX(), y + element->GetY());
    }

    // Render focus outline if focused
    if (focused) {
        pulsingOutline->OnRender(drawer, pulsingOutline->GetX(), pulsingOutline->GetY());
    }
}

void BoxSpacesButton::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    touchHandler.HandleInput(touch_pos, GetX(), GetY(), GetWidth(), GetHeight());
}

void BoxSpacesButton::SetFocused(bool focus) {
    LOG_DEBUG("[BoxSpacesButton] SetFocused: " + std::to_string(focus));
    this->focused = focus;
    pulsingOutline->SetVisible(focus);
}

bool BoxSpacesButton::IsFocused() const {
    return focused;
}

}  // namespace pksm::ui