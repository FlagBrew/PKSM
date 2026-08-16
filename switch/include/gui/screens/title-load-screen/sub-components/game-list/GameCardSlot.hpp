#pragma once

#include <pu/Plutonium>

#include "data/titles/Title.hpp"
#include "gui/shared/components/PulsingOutline.hpp"
#include "gui/shared/components/ShakeableWithOutline.hpp"
#include "input/ButtonInputHandler.hpp"
#include "input/TouchInputHandler.hpp"
#include "input/visual-feedback/interfaces/IFocusable.hpp"
#include "input/visual-feedback/interfaces/ISelectable.hpp"

namespace pksm::ui {

// The title screen's Game Card slot: a cartridge frame with a transparent
// label window through which the inserted game's icon shows. Dimming for
// the unselected state color-modulates the textures themselves, so the
// frame's transparency is respected (no overlay rectangle over the
// bounding box). Owns its empty (no cartridge) state.
class GameCardSlot : public pu::ui::elm::Element, public ISelectable, public ShakeableWithOutline {
public:
    GameCardSlot(const pu::i32 x, const pu::i32 y, const pu::i32 cardSize, const pu::i32 outlinePadding = 0);
    PU_SMART_CTOR(GameCardSlot)

    // Sets the cartridge title (nullptr = no game card -> empty state)
    void SetTitle(pksm::titles::Title::Ref title);
    pksm::titles::Title::Ref GetTitle() const { return title; }

    // pu::ui::elm::Element
    pu::i32 GetX() override { return x; }
    pu::i32 GetY() override { return y; }
    pu::i32 GetWidth() override { return cardSize; }
    pu::i32 GetHeight() override { return cardSize; }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    void SetX(const pu::i32 x);
    void SetY(const pu::i32 y);

    // ISelectable
    void SetSelected(bool select) override;
    bool IsSelected() const override;

    // IFocusable
    void SetFocused(bool focus) override;
    bool IsFocused() const override;

    void SetOnTouchSelect(std::function<void()> callback) { onTouchSelectCallback = callback; }
    void SetOnSelect(std::function<void()> callback) { onSelectCallback = callback; }

private:
    // Frame art geometry: 232px square canvas, transparent label window at
    // x47 y63, 137x140; all scaled by cardSize at render time
    static constexpr pu::i32 FRAME_CANVAS = 232;
    static constexpr pu::i32 WINDOW_X = 47;
    static constexpr pu::i32 WINDOW_Y = 63;
    static constexpr pu::i32 WINDOW_WIDTH = 137;
    static constexpr pu::i32 WINDOW_HEIGHT = 140;
    static constexpr pu::i32 FRAME_CORNER_RADIUS = 24;

    // Brightness factor for the unselected state (matches the previous
    // black-overlay-at-alpha-94 look: 255 - 94 = 161)
    static constexpr u8 DIM_COLOR_MOD = 161;

    // Ghosted alpha for the empty (no cartridge) frame
    static constexpr u8 EMPTY_FRAME_ALPHA = 140;

    pu::i32 Scaled(const pu::i32 v) const { return v * cardSize / FRAME_CANVAS; }

    pu::i32 x;
    pu::i32 y;
    pu::i32 cardSize;
    pu::i32 outlinePadding;
    bool focused;
    bool selected;
    pksm::titles::Title::Ref title;
    pu::sdl2::TextureHandle::Ref frameTexture;
    pu::sdl2::TextureHandle::Ref emptyFrameTexture;
    pu::ui::elm::TextBlock::Ref emptyText;
    std::function<void()> onTouchSelectCallback;
    std::function<void()> onSelectCallback;
    pksm::input::TouchInputHandler touchHandler;
    pksm::input::ButtonInputHandler buttonHandler;
};

}  // namespace pksm::ui
