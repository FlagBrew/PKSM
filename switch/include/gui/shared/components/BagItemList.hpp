#pragma once

#include <functional>
#include <pu/Plutonium>
#include <vector>

#include "data/bag/BagData.hpp"
#include "gui/shared/components/BagItemRow.hpp"
#include "gui/shared/components/IGrid.hpp"
#include "gui/shared/components/ScrollView.hpp"
#include "input/HoldRepeat.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::ui {

// Scrolling single-column list of bag rows; Up/Down move the cursor, the parent owns Left.
// Rows are pooled: a row is built and registered for focus once and then rebound to whatever
// item lands at its index, so switching pouches never pays for teardown or registration.
// Only rows near the visible window hold textures; the rest release theirs as they scroll
// away, so the sprite and text caches are the only long-term owners and a texture dies one at
// a time on eviction rather than by the hundred on a pouch switch.
class BagItemList : public IFocusable, public IGrid {
private:
    static constexpr pu::i32 ROW_SPACING = 12;
    static constexpr pu::i32 OUTLINE_PADDING = 4;
    static constexpr size_t RELEASE_MARGIN = 8;  // rows kept resolved beyond the visible window

    bool focused = false;
    bool disabled = false;
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    pu::i32 rowHeight;
    std::vector<BagItemRow::Ref> rows;  // The pool; the first shown rows are in use
    size_t shown = 0;
    ScrollView::Ref scrollView;
    pksm::input::DirectionalInputHandler inputHandler;
    pksm::input::HoldRepeat pageRepeat{350, 120};
    std::function<void()> onFocusChangedCallback;

    void EnsureRowVisible(size_t index, bool animated = true);
    void ReleaseOffscreenRows(pu::i32 scrollOffset);
    // Moves the cursor a screenful of rows; shakes at the ends
    void PageBy(int pages);

    // IGrid layout
    pu::i32 GetItemWidth() const override { return width; }
    pu::i32 GetItemHeight() const override { return rowHeight; }
    pu::i32 GetHorizontalSpacing() const override { return 0; }
    pu::i32 GetVerticalSpacing() const override { return ROW_SPACING; }

public:
    BagItemList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const pu::i32 rowHeight,
        input::FocusManager::Ref parentFocusManager
    );
    PU_SMART_CTOR(BagItemList)

    // Element implementation
    pu::i32 GetX() override { return x; }
    pu::i32 GetY() override { return y; }
    pu::i32 GetWidth() override { return width; }
    pu::i32 GetHeight() override { return height; }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // IGrid implementation
    size_t GetItemCount() const override { return shown; }
    ShakeableWithOutline::Ref GetItemAtIndex(size_t index) override {
        if (index < shown) {
            return rows[index];
        }
        return nullptr;
    }
    void SetSelectedIndex(size_t index) override;

    // IFocusable implementation
    void SetFocused(bool focused) override;
    bool IsFocused() const override;

    // storageFormat and pouch key the sprites; the cursor lands on selected (clamped to the rows).
    // keepScroll holds the view where it was, for a rebind of the same list; a new list starts at
    // the top. Either way the cursor row ends up in view
    void SetDataSource(
        const std::vector<bag::Slot>& items,
        ::pksm::Generation storageFormat,
        ::pksm::Sav::Pouch pouch,
        size_t selected = 0,
        bool keepScroll = false
    );
    size_t GetSelectedIndex() const { return selectedIndex; }
    // The right stick pages the list; a parent keeps it out of its own directional handling
    static constexpr u64 PAGE_BUTTONS = HidNpadButton_StickRUp | HidNpadButton_StickRDown;
    // Redraws one row's right-hand text; edited tints it
    void SetRowDetail(size_t index, const std::string& detail, bool edited);

    // Ignore input (help overlay)
    void SetDisabled(bool disabled) { this->disabled = disabled; }

    // Focus arriving or leaving, whichever way (navigation, touch, pouch switch)
    void SetOnFocusChanged(std::function<void()> callback) { onFocusChangedCallback = callback; }
};

}  // namespace pksm::ui
