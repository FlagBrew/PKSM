#pragma once

#include <pu/Plutonium>

class FocusableImage : public pu::ui::elm::Image {
private:
    bool focused;
    pu::ui::elm::Rectangle::Ref overlay;

public:
    FocusableImage(
        const pu::i32 x, const pu::i32 y,
        pu::sdl2::TextureHandle::Ref image,
        const u8 unfocusedAlpha = 128
    ) : Image(x, y, image), focused(false) {
        // Create an overlay rectangle with the same dimensions
        overlay = pu::ui::elm::Rectangle::New(x, y, 0, 0, pu::ui::Color(0, 0, 0, unfocusedAlpha));
    }

    PU_SMART_CTOR(FocusableImage)

    void SetWidth(const pu::i32 width) {
        Image::SetWidth(width);
        overlay->SetWidth(width);
    }

    void SetHeight(const pu::i32 height) {
        Image::SetHeight(height);
        overlay->SetHeight(height);
    }

    void SetX(const pu::i32 x) {
        Image::SetX(x);
        overlay->SetX(x);
    }

    void SetY(const pu::i32 y) {
        Image::SetY(y);
        overlay->SetY(y);
    }

    void SetFocused(bool focus) {
        this->focused = focus;
        overlay->SetVisible(!focus);
    }

    bool IsFocused() const {
        return focused;
    }

    pu::ui::elm::Rectangle::Ref GetOverlay() {
        return overlay;
    }
}; 