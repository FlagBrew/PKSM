#include "gui/FocusableImage.hpp"

FocusableImage::FocusableImage(
    const pu::i32 x, const pu::i32 y,
    pu::sdl2::TextureHandle::Ref image,
    const u8 unfocusedAlpha,
    const pu::i32 outlinePadding
) : Image(x, y, image), focused(false), selected(false), outlinePadding(outlinePadding) {
    // Create an overlay rectangle with the same dimensions
    overlay = pu::ui::elm::Rectangle::New(x, y, 0, 0, pu::ui::Color(0, 0, 0, unfocusedAlpha));
    // Create a pulsing outline with padding
    outline = PulsingOutline::New(
        x - outlinePadding, 
        y - outlinePadding, 
        0, 
        0, 
        pu::ui::Color(0, 150, 255, 255)
    );
    outline->SetVisible(false);
}

void FocusableImage::SetWidth(const pu::i32 width) {
    Image::SetWidth(width);
    overlay->SetWidth(width);
    outline->SetWidth(width + (outlinePadding * 2));  // Add padding to both sides
}

void FocusableImage::SetHeight(const pu::i32 height) {
    Image::SetHeight(height);
    overlay->SetHeight(height);
    outline->SetHeight(height + (outlinePadding * 2));  // Add padding to top and bottom
}

void FocusableImage::SetX(const pu::i32 x) {
    Image::SetX(x);
    overlay->SetX(x);
    outline->SetX(x - outlinePadding);  // Adjust for padding
}

void FocusableImage::SetY(const pu::i32 y) {
    Image::SetY(y);
    overlay->SetY(y);
    outline->SetY(y - outlinePadding);  // Adjust for padding
}

void FocusableImage::SetOutlinePadding(const pu::i32 padding) {
    outlinePadding = padding;
    // Update outline position and size
    outline->SetX(GetX() - padding);
    outline->SetY(GetY() - padding);
    outline->SetWidth(GetWidth() + (padding * 2));
    outline->SetHeight(GetHeight() + (padding * 2));
}

void FocusableImage::SetSelected(bool select) {
    this->selected = select;
    overlay->SetVisible(!select);  // Show overlay for unselected items
}

bool FocusableImage::IsSelected() const {
    return selected;
}

void FocusableImage::SetFocused(bool focus) {
    this->focused = focus;
    outline->SetVisible(focus && selected);  // Only show outline when both focused and selected
}

bool FocusableImage::IsFocused() const {
    return focused;
}

pu::ui::elm::Rectangle::Ref FocusableImage::GetOverlay() {
    return overlay;
}

void FocusableImage::OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) {
    Image::OnRender(drawer, x, y);
    
    // Draw the overlay for unselected items
    if (!selected) {
        overlay->OnRender(drawer, x, y);
    }
    
    // Draw the outline only for the focused and selected item
    if (focused && selected) {
        outline->OnRender(drawer, x, y);
    }
} 