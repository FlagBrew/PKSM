#pragma once

#include <pu/Plutonium>
#include <string>

#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

class PillTextContainer : public pu::ui::elm::Element, public IFocusable, public ShakeableWithOutline {
private:
    // Layout constants
    static constexpr pu::i32 CORNER_RADIUS = 25;  // Corner radius for the pill-shaped container
    static constexpr pu::ui::Color DEFAULT_CONTAINER_COLOR = pu::ui::Color(255, 255, 255, 255);  // White container
    static constexpr pu::ui::Color DEFAULT_TEXT_COLOR = pu::ui::Color(0, 0, 0, 255);  // Black text

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;

    // Colors
    pu::ui::Color containerColor;

    // Components
    pu::ui::elm::TextBlock::Ref textBlock;  // Text for display

    // Input handlers
    pksm::input::TouchInputHandler touchHandler;

    // Layout update
    void UpdateTextPosition();

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool focused;

public:
    PillTextContainer(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const std::string& text = ""
    );
    ~PillTextContainer() = default;
    PU_SMART_CTOR(PillTextContainer)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Text handling
    void SetText(const std::string& text);
    std::string GetText() const;

    // Font handling
    void SetFont(const std::string& font);

    // Color handling
    void SetContainerColor(const pu::ui::Color& color);
    void SetTextColor(const pu::ui::Color& color);

    // Focus handling
    bool IsFocused() const override;

    // Size and position
    void SetX(pu::i32 x);
    void SetY(pu::i32 y);
    void SetWidth(pu::i32 width);
    void SetHeight(pu::i32 height);
    void SetDimensions(pu::i32 x, pu::i32 y, pu::i32 width, pu::i32 height);
};

}  // namespace pksm::ui