#pragma once

#include <pu/Plutonium>

#include "gui/shared/components/PulsingOutline.hpp"
#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "utils/SDLHelper.hpp"

namespace pksm::ui {

class MenuButton : public pu::ui::elm::Element, public IFocusable, public ShakeableWithOutline {
private:
    // Layout constants
    static constexpr pu::i32 ICON_SIZE = 80;
    static constexpr pu::i32 TEXT_MARGIN = 24;
    static constexpr pu::i32 OUTLINE_PADDING = 8;
    static constexpr pu::i32 CORNER_RADIUS = 12;

    // Colors - matching TrainerInfo alpha values
    static constexpr pu::ui::Color BACKGROUND_COLOR = pu::ui::Color(30, 50, 120, 160);  // Same as stats background
    static constexpr pu::ui::Color TEXT_COLOR = pu::ui::Color(255, 255, 255, 180);
    static constexpr pu::i32 FOCUSED_ALPHA = 255;
    static constexpr pu::i32 UNFOCUSED_ALPHA = 102;

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;

    // State
    bool focused;
    std::string text;
    std::string iconName;

    // UI Elements
    pu::sdl2::TextureHandle::Ref icon;
    pu::ui::elm::TextBlock::Ref textBlock;
    std::function<void()> onClickCallback;

public:
    MenuButton(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const std::string& text,
        const std::string& iconName
    );
    PU_SMART_CTOR(MenuButton)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override;

    // Position setters
    void SetX(const pu::i32 x);
    void SetY(const pu::i32 y);

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;

    // Button functionality
    void SetOnClick(std::function<void()> callback);
};

}  // namespace pksm::ui