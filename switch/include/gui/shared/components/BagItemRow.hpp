#pragma once

#include <functional>
#include <pu/Plutonium>
#include <string>

#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "gui/shared/components/SpriteImage.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"

namespace pksm::ui {

// One bag slot as a row: sprite, name, the game's marks (new, favourite, the shortcut button
// holding it) and a detail (a quantity, a donut's quality).
// Sprite and text are resolved on first draw, so rows scrolled out of view never pay for them;
// text comes from the shared TextTextureCache, so a row never owns a texture outright.
class BagItemRow : public pu::ui::elm::Element, public IFocusable, public ShakeableWithOutline {
private:
    static constexpr pu::i32 SPRITE_SIZE = 64;
    static constexpr pu::i32 PADDING = 16;
    static constexpr pu::i32 OUTLINE_PADDING = 4;
    static constexpr u32 OUTLINE_BORDER_WIDTH = 4;
    static constexpr pu::ui::Color DEFAULT_BG_COLOR = pu::ui::Color(0, 0, 0, 60);
    static constexpr pu::ui::Color SELECTED_BG_COLOR = pu::ui::Color(255, 255, 255, 70);
    static constexpr pu::ui::Color LIFTED_BG_COLOR = pu::ui::Color(255, 214, 90, 90);
    static constexpr pu::ui::Color EDITED_TEXT_COLOR = pu::ui::Color(255, 214, 90, 255);
    static constexpr pu::ui::Color NEW_MARK_COLOR = pu::ui::Color(235, 75, 75, 255);
    static constexpr pu::ui::Color FAVORITE_MARK_COLOR = pu::ui::Color(255, 110, 150, 255);
    static constexpr pu::i32 MARK_GAP = 12;
    // The shortcut pill: a heart on a solid cap, the button's glyph beside it, outlined
    static constexpr pu::i32 PILL_HEIGHT = 38;
    static constexpr pu::i32 PILL_BORDER = 3;
    static constexpr pu::i32 PILL_CAP = 40;
    static constexpr pu::i32 PILL_PAD_LEFT = 10;
    static constexpr pu::i32 PILL_PAD_RIGHT = 12;
    static constexpr pu::ui::Color PILL_CAP_INK = pu::ui::Color(42, 10, 20, 255);

    bool focused = false;
    bool selected = false;
    bool lifted = false;
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
    bool isNew = false;
    bool favorite = false;
    std::string shortcut;  // The glyph of the shortcut button holding the item, empty for none
    bool edited = false;   // Tints the detail until the save is written
    pu::sdl2::TextureHandle::Ref nameTexture;
    pu::sdl2::TextureHandle::Ref detailTexture;
    pu::sdl2::TextureHandle::Ref newMarkTexture;
    pu::sdl2::TextureHandle::Ref favoriteMarkTexture;
    pu::sdl2::TextureHandle::Ref shortcutTexture;
    pu::sdl2::TextureHandle::Ref pillHeartTexture;

    std::function<void()> onTouchSelectCallback;
    pksm::input::TouchInputHandler touchHandler;

    void UpdateBackground();
    // Draws the shortcut pill with its left edge at pillX and returns its width
    pu::i32 DrawShortcut(pu::ui::render::Renderer::Ref& drawer, pu::i32 pillX, pu::i32 rowY);
    // Draws the text vertically centered at textX (or against the row's right padding) and
    // returns its width, 0 for no text
    pu::i32 DrawText(
        pu::ui::render::Renderer::Ref& drawer,
        pu::sdl2::TextureHandle::Ref& texture,
        const std::string& text,
        pu::ui::Color color,
        pu::i32 textX,
        pu::i32 rowY,
        bool alignRight = false
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

    // spriteKey is an ItemSpriteManager key; detail is the right-aligned text (a count, a quality);
    // the marks follow the name; shortcut is the button glyph of the slot holding the item
    void SetItem(
        u32 spriteKey,
        const std::string& itemName,
        const std::string& detail,
        bool isNew,
        bool favorite,
        const std::string& shortcut
    );
    // Re-rasterizes only the detail; edited draws it in the edited tint
    void SetDetail(const std::string& detail, bool edited);
    // Drops the sprite and text textures; they come back from the caches on the next draw
    void Release();
    // Trades what the two rows show, textures included: a carried row changes place without a redraw
    void SwapContent(BagItemRow& other);

    // The cursor row; stays highlighted while focus is elsewhere
    void SetSelected(bool selected);
    // Picked up by the cursor, drawn as such until it is dropped or put back
    void SetLifted(bool lifted);

    // IFocusable implementation
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    void SetOnTouchSelect(std::function<void()> callback) { onTouchSelectCallback = callback; }
};

}  // namespace pksm::ui
