#include "gui/shared/components/BoxItem.hpp"

#include "utils/Logger.hpp"

pksm::ui::BoxItem::BoxItem(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    pu::sdl2::TextureHandle::Ref image,
    const pu::i32 outlinePadding,
    const pu::ui::Color defaultBgColor,
    const pu::ui::Color selectedBgColor
)
  : Element(),
    ShakeableWithOutline(
        pksm::ui::PulsingOutline::New(
            x - outlinePadding,
            y - outlinePadding,
            width + (outlinePadding * 2),
            height + (outlinePadding * 2),
            pksm::ui::global::OUTLINE_COLOR,
            0,
            OUTLINE_BORDER_WIDTH
        )
    ),
    focused(false),
    selected(false),
    outlinePadding(outlinePadding),
    x(x),
    y(y),
    width(width),
    height(height),
    defaultBgColor(defaultBgColor),
    selectedBgColor(selectedBgColor) {
    // Create container with elements
    container = pu::ui::Container::New(0, 0, width, height);
    background = pu::ui::elm::Rectangle::New(0, 0, width, height, defaultBgColor);
    this->image = pu::ui::elm::Image::New(0, 0, image);

    // Set image dimensions to match container
    this->image->SetWidth(width);
    this->image->SetHeight(height);

    // Add elements to container
    container->Add(background);
    container->Add(this->image);

    // Create the regular outline
    outline = pksm::ui::RectangularOutline::New(
        x - outlinePadding,
        y - outlinePadding,
        width + (outlinePadding * 2),
        height + (outlinePadding * 2),
        outlineColor,
        0,
        OUTLINE_BORDER_WIDTH
    );

    // Set initial visibility
    outline->SetVisible(true);
    pulsingOutline->SetVisible(false);
}

void pksm::ui::BoxItem::SetWidth(const pu::i32 width) {
    this->width = width;
    container->SetWidth(width);
    background->SetWidth(width);
    image->SetWidth(width);

    // Update outlines
    pu::i32 outlineWidth = width + (outlinePadding * 2);
    outline->SetWidth(outlineWidth);
    pulsingOutline->SetWidth(outlineWidth);
}

void pksm::ui::BoxItem::SetHeight(const pu::i32 height) {
    this->height = height;
    container->SetHeight(height);
    background->SetHeight(height);
    image->SetHeight(height);

    // Update outlines
    pu::i32 outlineHeight = height + (outlinePadding * 2);
    outline->SetHeight(outlineHeight);
    pulsingOutline->SetHeight(outlineHeight);
}

void pksm::ui::BoxItem::SetX(const pu::i32 x) {
    this->x = x;
    pu::i32 outlineX = x - outlinePadding;
    outline->SetX(outlineX);
    pulsingOutline->SetX(outlineX);
}

void pksm::ui::BoxItem::SetY(const pu::i32 y) {
    this->y = y;
    pu::i32 outlineY = y - outlinePadding;
    outline->SetY(outlineY);
    pulsingOutline->SetY(outlineY);
}

void pksm::ui::BoxItem::SetOutlinePadding(const pu::i32 padding) {
    outlinePadding = padding;

    // Update both outlines
    pu::i32 outlineX = x - padding;
    pu::i32 outlineY = y - padding;
    pu::i32 outlineWidth = width + (padding * 2);
    pu::i32 outlineHeight = height + (padding * 2);

    outline->SetX(outlineX);
    outline->SetY(outlineY);
    outline->SetWidth(outlineWidth);
    outline->SetHeight(outlineHeight);

    pulsingOutline->SetX(outlineX);
    pulsingOutline->SetY(outlineY);
    pulsingOutline->SetWidth(outlineWidth);
    pulsingOutline->SetHeight(outlineHeight);
}

pu::i32 pksm::ui::BoxItem::GetX() {
    return x;
}

pu::i32 pksm::ui::BoxItem::GetY() {
    return y;
}

pu::i32 pksm::ui::BoxItem::GetWidth() {
    return width;
}

pu::i32 pksm::ui::BoxItem::GetHeight() {
    return height;
}

void pksm::ui::BoxItem::SetImage(pu::sdl2::TextureHandle::Ref newImage) {
    image->SetImage(newImage);
    image->SetWidth(width);
    image->SetHeight(height);
}

pu::ui::elm::Image::Ref pksm::ui::BoxItem::GetImage() {
    return image;
}

void pksm::ui::BoxItem::SetSelected(bool select) {
    this->selected = select;
    background->SetColor(select ? selectedBgColor : defaultBgColor);
}

bool pksm::ui::BoxItem::IsSelected() const {
    return selected;
}

void pksm::ui::BoxItem::SetFocused(bool focus) {
    if (focus) {
        LOG_DEBUG("[BoxItem] Setting focused to true");
        RequestSelection();
    } else {
        LOG_DEBUG("[BoxItem] Setting focused to false");
    }

    this->focused = focus;
    outline->SetVisible(!focus);
    pulsingOutline->SetVisible(focus);
}

bool pksm::ui::BoxItem::IsFocused() const {
    return focused;
}

void pksm::ui::BoxItem::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Draw the appropriate outline first
    if (focused) {
        pulsingOutline->OnRender(drawer, x - outlinePadding, y - outlinePadding);
    } else {
        outline->OnRender(drawer, x - outlinePadding, y - outlinePadding);
    }

    // Draw container elements
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, x + element->GetX(), y + element->GetY());
    }
}

void pksm::ui::BoxItem::SetOnTouchSelect(std::function<void()> callback) {
    onTouchSelectCallback = callback;
}

void pksm::ui::BoxItem::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Pass input to container elements
    for (auto& element : container->GetElements()) {
        element->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }

    // Handle touch selection
    if (!touch_pos.IsEmpty() && touch_pos.HitsRegion(this->x, this->y, this->width, this->height) && !focused &&
        onTouchSelectCallback) {
        LOG_DEBUG("BoxItem Tapped");
        onTouchSelectCallback();
    }
}