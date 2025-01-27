#pragma once

#include <pu/Plutonium>
#include "gui/PulsingOutline.hpp"

class FocusableImage : public pu::ui::elm::Image {
private:
    bool focused;
    bool selected;
    pu::ui::elm::Rectangle::Ref overlay;
    PulsingOutline::Ref outline;
    pu::i32 outlinePadding;  // Padding between image and outline

public:
    FocusableImage(
        const pu::i32 x, const pu::i32 y,
        pu::sdl2::TextureHandle::Ref image,
        const u8 unfocusedAlpha = 128,
        const pu::i32 outlinePadding = 0
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

    PU_SMART_CTOR(FocusableImage)

    void SetWidth(const pu::i32 width) {
        Image::SetWidth(width);
        overlay->SetWidth(width);
        outline->SetWidth(width + (outlinePadding * 2));  // Add padding to both sides
    }

    void SetHeight(const pu::i32 height) {
        Image::SetHeight(height);
        overlay->SetHeight(height);
        outline->SetHeight(height + (outlinePadding * 2));  // Add padding to top and bottom
    }

    void SetX(const pu::i32 x) {
        Image::SetX(x);
        overlay->SetX(x);
        outline->SetX(x - outlinePadding);  // Adjust for padding
    }

    void SetY(const pu::i32 y) {
        Image::SetY(y);
        overlay->SetY(y);
        outline->SetY(y - outlinePadding);  // Adjust for padding
    }

    void SetOutlinePadding(const pu::i32 padding) {
        outlinePadding = padding;
        // Update outline position and size
        outline->SetX(GetX() - padding);
        outline->SetY(GetY() - padding);
        outline->SetWidth(GetWidth() + (padding * 2));
        outline->SetHeight(GetHeight() + (padding * 2));
    }

    // Controls whether this image is the currently selected one
    void SetSelected(bool select) {
        this->selected = select;
        overlay->SetVisible(!select);  // Hide overlay when selected
    }

    // Controls whether we're navigating in the games section
    void SetFocused(bool focus) {
        this->focused = focus;
        outline->SetVisible(focus && selected);  // Only show outline when both focused and selected
    }

    bool IsFocused() const {
        return focused;
    }

    bool IsSelected() const {
        return selected;
    }

    pu::ui::elm::Rectangle::Ref GetOverlay() {
        return overlay;
    }

    void OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) override {
        Image::OnRender(drawer, x, y);
        if (focused && selected) {
            outline->OnRender(drawer, x, y);
        }
    }
}; 