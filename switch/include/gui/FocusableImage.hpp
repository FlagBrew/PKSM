#pragma once

#include <pu/Plutonium>
#include "gui/PulsingOutline.hpp"
#include "gui/IFocusable.hpp"

class FocusableImage : public pu::ui::elm::Image, public IFocusable {
private:
    bool focused;
    bool selected;
    pu::ui::elm::Rectangle::Ref overlay;
    PulsingOutline::Ref outline;
    pu::i32 outlinePadding;  // Padding between image and outline
    std::function<void()> onTouchSelectCallback;  // New callback for touch selection

public:
    FocusableImage(
        const pu::i32 x, const pu::i32 y,
        pu::sdl2::TextureHandle::Ref image,
        const u8 unfocusedAlpha = 128,
        const pu::i32 outlinePadding = 0
    );

    static Ref New(
        const pu::i32 x, const pu::i32 y,
        pu::sdl2::TextureHandle::Ref image,
        const u8 unfocusedAlpha = 128,
        const pu::i32 outlinePadding = 0
    ) {
        return std::make_shared<FocusableImage>(x, y, image, unfocusedAlpha, outlinePadding);
    }

    PU_SMART_CTOR(FocusableImage)

    void SetWidth(const pu::i32 width);
    void SetHeight(const pu::i32 height);
    void SetX(const pu::i32 x);
    void SetY(const pu::i32 y);
    void SetOutlinePadding(const pu::i32 padding);

    // Controls whether this image is the currently selected one
    void SetSelected(bool select);
    bool IsSelected() const;

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    pu::ui::elm::Rectangle::Ref GetOverlay();

    void OnRender(pu::ui::render::Renderer::Ref &drawer, const pu::i32 x, const pu::i32 y) override;

    // Touch selection callback
    void SetOnTouchSelect(std::function<void()> callback);

    // Override OnInput to handle touch
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;
}; 