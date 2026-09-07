#pragma once

#include <functional>
#include <pu/Plutonium>
#include <string>
#include <vector>

#include "gui/shared/components/FocusableButton.hpp"
#include "gui/shared/components/ShakeDirection.hpp"
#include "gui/shared/components/SpriteImage.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "utils/SpriteSheet.hpp"

namespace pksm::ui {

// The bag's pouches as a column of buttons, a glyph on each, a marker beside the selected one
// and its title over the list. The buttons register with the screen's focus manager in order,
// so focus moves among them as it would among the screen's own elements
class PouchColumn : public pu::ui::elm::Element {
private:
    static constexpr pu::i32 POUCH_WIDTH = 400;
    static constexpr pu::i32 POUCH_HEIGHT = 72;
    static constexpr pu::i32 POUCH_SPACING = 12;
    static constexpr pu::i32 GLYPH_SIZE = 44;
    static constexpr pu::i32 GLYPH_INSET = 16;
    static constexpr pu::i32 MARKER_WIDTH = 8;
    static constexpr pu::i32 MARKER_GAP = 10;
    static constexpr size_t NONE = SIZE_MAX;

    pu::i32 x;
    pu::i32 y;
    pu::i32 titleX;
    pu::i32 titleY;
    std::vector<FocusableButton::Ref> buttons;
    std::vector<SpriteImage::Ref> glyphs;
    // One title per pouch, rasterized up front: a switch only flips visibility
    std::vector<pu::ui::elm::TextBlock::Ref> titles;
    pu::ui::elm::Rectangle::Ref marker;
    size_t selected = NONE;
    std::function<void(size_t)> onPouchFocused;
    std::function<void()> onPouchActivated;

    pu::i32 PouchY(size_t index) const { return y + static_cast<pu::i32>(index) * (POUCH_HEIGHT + POUCH_SPACING); }

public:
    struct Entry {
        std::string name;
        utils::SpriteRef glyph;
    };

    // The column's top-left corner, where the selected pouch's title goes, and one button, glyph
    // and title per pouch, top to bottom, registered with the focus manager in that order
    PouchColumn(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 titleX,
        const pu::i32 titleY,
        const std::vector<Entry>& pouches,
        input::FocusManager::Ref focusManager
    );
    PU_SMART_CTOR(PouchColumn)

    // Element implementation
    pu::i32 GetX() override { return x; }
    pu::i32 GetY() override { return y; }
    pu::i32 GetWidth() override { return POUCH_WIDTH; }
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Moves the marker to the pouch and shows its title in place of the last one's
    void Select(size_t index);
    void RequestFocus(size_t index);
    void ShakeSelected(ShakeDirection direction);
    // The selected pouch's title; hidden while something else takes its band
    void SetTitleVisible(bool visible);

    // Ignore input (help overlay, picker, carry)
    void SetDisabled(bool disabled);

    // Focus arriving on a pouch, whether by navigation or touch
    void SetOnPouchFocused(std::function<void(size_t)> callback) { onPouchFocused = callback; }
    // A on the focused pouch, or a touch on it
    void SetOnPouchActivated(std::function<void()> callback) { onPouchActivated = callback; }
};

}  // namespace pksm::ui
