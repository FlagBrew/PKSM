#pragma once

#include <pu/Plutonium>

#include "gui/shared/components/PulsingOutline.hpp"
#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "gui/shared/components/StaticOutline.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "input/visual-feedback/interfaces/ISelectable.hpp"

namespace pksm::ui {
class BoxItem : public pu::ui::elm::Element, public ISelectable, public ShakeableWithOutline {
private:
    bool focused;
    bool selected;
    pu::ui::Container::Ref container;
    pu::ui::elm::Rectangle::Ref background;
    pu::ui::elm::Image::Ref image;
    pksm::ui::RectangularOutline::Ref outline;
    pu::i32 outlinePadding;  // Padding between box and outline
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::ui::Color defaultBgColor;
    pu::ui::Color selectedBgColor;
    std::function<void()> onTouchSelectCallback;  // Callback for touch selection
    static constexpr u32 OUTLINE_BORDER_WIDTH = 5;
    pu::ui::Color outlineColor = pu::ui::Color(70, 70, 70, 188);

public:
    BoxItem(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        pu::sdl2::TextureHandle::Ref image,
        const pu::i32 outlinePadding = 2,
        const pu::ui::Color defaultBgColor = pu::ui::Color(200, 200, 200, 58),
        const pu::ui::Color selectedBgColor = pu::ui::Color(255, 255, 0, 128)
    );
    PU_SMART_CTOR(BoxItem)

    void SetWidth(const pu::i32 width);
    void SetHeight(const pu::i32 height);
    void SetX(const pu::i32 x);
    void SetY(const pu::i32 y);
    void SetOutlinePadding(const pu::i32 padding);

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;

    void SetImage(pu::sdl2::TextureHandle::Ref newImage);
    pu::ui::elm::Image::Ref GetImage();

    // Controls whether this box is the currently selected one
    void SetSelected(bool select) override;
    bool IsSelected() const override;

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;

    // Touch selection callback
    void SetOnTouchSelect(std::function<void()> callback);

    // Override OnInput to handle touch
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;
};
}  // namespace pksm::ui