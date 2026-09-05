#pragma once

#include <functional>
#include <pu/Plutonium>
#include <string>

#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "gui/shared/components/SpriteImage.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

// One bag slot as a row: sprite, name and a detail (a quantity, a donut's quality).
// Sprite and text are resolved on first draw, so rows scrolled out of view never pay for them.
class BagItemRow : public pu::ui::elm::Element, public IFocusable, public ShakeableWithOutline {
private:
    static constexpr pu::i32 SPRITE_SIZE = 64;
    static constexpr pu::i32 PADDING = 16;
    static constexpr pu::i32 OUTLINE_PADDING = 4;
    static constexpr u32 OUTLINE_BORDER_WIDTH = 4;
    static constexpr pu::ui::Color DEFAULT_BG_COLOR = pu::ui::Color(0, 0, 0, 60);
    static constexpr pu::ui::Color SELECTED_BG_COLOR = pu::ui::Color(255, 255, 255, 70);
    static constexpr pu::ui::Color EDITED_TEXT_COLOR = pu::ui::Color(255, 214, 90, 255);

    bool focused = false;
    bool selected = false;
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::ui::elm::Rectangle::Ref background;
    SpriteImage::Ref sprite;
    bool spriteResolved = false;
    u32 spriteKey = 0;
    std::string name;
    std::string detail;
    bool edited = false;  // Tints the detail until the save is written
    pu::sdl2::TextureHandle::Ref nameTexture;
    pu::sdl2::TextureHandle::Ref detailTexture;

    std::function<void()> onTouchSelectCallback;
    pksm::input::TouchInputHandler touchHandler;

    // Draws the text vertically centered; alignRight anchors it to the row's right padding
    void DrawText(
        pu::ui::render::Renderer::Ref& drawer,
        pu::sdl2::TextureHandle::Ref& texture,
        const std::string& text,
        pu::ui::Color color,
        pu::i32 rowX,
        pu::i32 rowY,
        bool alignRight
    );

public:
    BagItemRow(const pu::i32 x, const pu::i32 y, const pu::i32 width, const pu::i32 height);
    PU_SMART_CTOR(BagItemRow)

    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // spriteKey is an ItemSpriteManager key; detail is the right-aligned text (a count, a quality)
    void SetItem(u32 spriteKey, const std::string& itemName, const std::string& detail);
    // Re-rasterizes only the detail; edited draws it in the edited tint
    void SetDetail(const std::string& detail, bool edited);

    // The cursor row; stays highlighted while focus is elsewhere
    void SetSelected(bool selected);

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    void SetOnTouchSelect(std::function<void()> callback) { onTouchSelectCallback = callback; }
};

}  // namespace pksm::ui
