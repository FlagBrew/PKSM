#include "gui/shared/components/FocusableImage.hpp"

#include "utils/Logger.hpp"

pksm::ui::FocusableImage::FocusableImage(
    const pu::i32 x,
    const pu::i32 y,
    pu::sdl2::TextureHandle::Ref image,
    const u8 unfocusedAlpha,
    const pu::i32 outlinePadding
)
  : Image(x, y, image),
    ShakeableWithOutline(pksm::ui::PulsingOutline::New(x - outlinePadding, y - outlinePadding, 0, 0)),
    focused(false),
    selected(false),
    outlinePadding(outlinePadding),
    touchHandler(),
    buttonHandler() {
    // Create an overlay rectangle with the same dimensions
    overlay = pu::ui::elm::Rectangle::New(x, y, 0, 0, pu::ui::Color(0, 0, 0, unfocusedAlpha));

    // Setup touch handler callbacks
    touchHandler.SetOnTouchUpInside([this]() {
        LOG_DEBUG("[FocusableImage] Touch Up Inside");
        if (!focused && onTouchSelectCallback) {
            LOG_DEBUG("[FocusableImage] Requesting focus");
            onTouchSelectCallback();
            RequestFocus();
        } else if (focused && onSelectCallback) {
            onSelectCallback();
        }
    });

    // Register A button with focus check via condition
    buttonHandler.RegisterButton(
        HidNpadButton_A,
        nullptr,  // No press visual feedback needed
        [this]() {
            if (onSelectCallback) {
                onSelectCallback();
            }
        },
        [this]() { return this->focused; }  // Only process when focused
    );
}

void pksm::ui::FocusableImage::SetWidth(const pu::i32 width) {
    Image::SetWidth(width);
    overlay->SetWidth(width);
    pulsingOutline->SetWidth(width + (outlinePadding * 2));  // Add padding to both sides
}

void pksm::ui::FocusableImage::SetHeight(const pu::i32 height) {
    Image::SetHeight(height);
    overlay->SetHeight(height);
    pulsingOutline->SetHeight(height + (outlinePadding * 2));  // Add padding to top and bottom
}

void pksm::ui::FocusableImage::SetX(const pu::i32 x) {
    Image::SetX(x);
    overlay->SetX(x);
    pulsingOutline->SetX(x - outlinePadding);  // Adjust for padding
}

void pksm::ui::FocusableImage::SetY(const pu::i32 y) {
    Image::SetY(y);
    overlay->SetY(y);
    pulsingOutline->SetY(y - outlinePadding);  // Adjust for padding
}

void pksm::ui::FocusableImage::SetOutlinePadding(const pu::i32 padding) {
    outlinePadding = padding;
    pulsingOutline->SetX(GetX() - padding);
    pulsingOutline->SetY(GetY() - padding);
    pulsingOutline->SetWidth(GetWidth() + (padding * 2));
    pulsingOutline->SetHeight(GetHeight() + (padding * 2));
}

void pksm::ui::FocusableImage::SetSelected(bool select) {
    this->selected = select;
    overlay->SetVisible(!select);  // Show overlay for unselected items
}

bool pksm::ui::FocusableImage::IsSelected() const {
    return selected;
}

void pksm::ui::FocusableImage::SetFocused(bool focus) {
    if (focus) {
        LOG_DEBUG("[FocusableImage] Setting focused to true");
        RequestSelection();
    } else {
        LOG_DEBUG("[FocusableImage] Setting focused to false");
    }
    this->focused = focus;
    pulsingOutline->SetVisible(focus && selected);  // Only show outline when both focused and selected
}

bool pksm::ui::FocusableImage::IsFocused() const {
    return focused;
}

pu::ui::elm::Rectangle::Ref pksm::ui::FocusableImage::GetOverlay() {
    return overlay;
}

void pksm::ui::FocusableImage::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Draw the base image at the adjusted position
    Image::OnRender(drawer, x, y);

    // Draw the overlay for unselected items
    if (!selected) {
        overlay->OnRender(drawer, x, y);
    }

    // Draw the outline only for the focused and selected item
    if (focused && selected) {
        // Pass the adjusted position directly to the outline
        pulsingOutline->OnRender(drawer, x - outlinePadding, y - outlinePadding);
    }
}

void pksm::ui::FocusableImage::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    // Let the base image handle its normal touch behavior first
    Image::OnInput(keys_down, keys_up, keys_held, touch_pos);

    // Pass current bounds and touch point to the handler
    touchHandler.HandleInput(touch_pos, GetX(), GetY(), GetWidth(), GetHeight());

    // Process button inputs
    buttonHandler.HandleInput(keys_down, keys_up, keys_held);
}