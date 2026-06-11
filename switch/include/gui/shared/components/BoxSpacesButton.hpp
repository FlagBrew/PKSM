#pragma once

#include <pu/Plutonium>
#include <string>

#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

class BoxSpacesButton : public pu::ui::elm::Element, public IFocusable, public ShakeableWithOutline {
private:
    // Text constant
    static constexpr const char* BUTTON_TEXT = "Box Spaces";

    // Layout constants
    static constexpr pu::i32 CORNER_RADIUS = 25;  // Corner radius for the pill-shaped container
    static constexpr pu::i32 PADDING_HORIZONTAL = 20;  // Horizontal padding inside the pill
    static constexpr pu::i32 ICON_TEXT_MARGIN = 20;  // Space between icon and text
    static constexpr pu::i32 ICON_SIZE = 34;  // Size of the icon

    // Color constants
    static constexpr pu::ui::Color DEFAULT_CONTAINER_COLOR = pu::ui::Color(100, 100, 100, 180);  // White container
    static constexpr pu::ui::Color DEFAULT_TEXT_COLOR = pksm::ui::global::TEXT_WHITE;

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;

    // Colors
    pu::ui::Color containerColor;

    // Components
    pu::ui::Container::Ref container;  // Container for child elements
    pu::ui::elm::TextBlock::Ref textBlock;  // Text display
    pu::ui::elm::Image::Ref icon;  // Icon image

    // Input handlers
    pksm::input::TouchInputHandler touchHandler;

    // State
    bool focused = false;

    // Internal methods
    void CalculateWidth();  // Update width based on content
    void UpdateLayout();  // Position elements correctly

public:
    BoxSpacesButton(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 height,
        pu::sdl2::TextureHandle::Ref iconTexture = nullptr
    );
    ~BoxSpacesButton() = default;
    PU_SMART_CTOR(BoxSpacesButton)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Focus handling (IFocusable implementation)
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    // Position
    void SetX(pu::i32 x);
    void SetY(pu::i32 y);
};

}  // namespace pksm::ui