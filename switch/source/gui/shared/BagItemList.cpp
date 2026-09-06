#include "gui/shared/components/BagItemList.hpp"

#include <algorithm>
#include <switch.h>

#include "utils/ItemSpriteManager.hpp"
#include "utils/Logger.hpp"

pksm::ui::BagItemList::BagItemList(
    const pu::i32 x,
    const pu::i32 y,
    const pu::i32 width,
    const pu::i32 height,
    const pu::i32 rowHeight,
    input::FocusManager::Ref parentFocusManager
)
  : pu::ui::elm::Element(),
    IFocusable(),
    IGrid(1),
    x(x),
    y(y),
    width(width),
    height(height),
    rowHeight(rowHeight) {
    // The viewport includes the outline padding so the cursor row's outline is never clipped
    scrollView = ScrollView::New(
        x - OUTLINE_PADDING,
        y - OUTLINE_PADDING,
        width + (OUTLINE_PADDING * 2),
        height + (OUTLINE_PADDING * 2)
    );

    inputHandler.SetOnMoveUp([this]() { IGrid::MoveUp(); });
    inputHandler.SetOnMoveDown([this]() { IGrid::MoveDown(); });
    inputHandler.SetOnMoveRight([this]() { IGrid::MoveRight(); });

    scrollView->SetOnScrolled([this](pu::i32 offset) { ReleaseOffscreenRows(offset); });

    SetFocusManager(parentFocusManager);
}

void pksm::ui::BagItemList::SetDataSource(
    const std::vector<bag::Slot>& items,
    ::pksm::Generation storageFormat,
    ::pksm::Sav::Pouch pouch,
    size_t selected,
    bool keepScroll
) {
    const u64 t0 = armGetSystemTick();
    const pu::i32 previousOffset = scrollView->GetScrollOffset();
    const size_t built = rows.size();
    while (rows.size() < items.size()) {
        const size_t i = rows.size();
        const auto position = CalculateItemPosition(i);
        auto row = BagItemRow::New(position.first, position.second, width, rowHeight);
        row->SetName("BagItemRow " + std::to_string(i));
        row->SetOnTouchSelect([this, i]() { SetSelectedIndex(i); });
        if (auto fm = this->focusManager.lock()) {
            fm->RegisterFocusable(row);
        }
        rows.push_back(row);
    }
    // Only the rows in use live in the scroll view; the rest of the pool sits idle
    scrollView->Clear();
    for (size_t i = 0; i < items.size(); i++) {
        const auto& slot = items[i];
        const u32 spriteKey = pouch == ::pksm::Sav::Pouch::Donut
            ? utils::ItemSpriteManager::DonutKey(slot.itemId, slot.variant)
            : utils::ItemSpriteManager::ItemKey(slot.itemId, storageFormat);
        // A row without a detail shows its count, unless there is none to show (a picker candidate)
        const std::string detail = !slot.detail.empty() ? slot.detail
            : slot.count > 0                            ? "×" + std::to_string(slot.count)
                                                        : "";
        rows[i]->SetItem(spriteKey, slot.name, detail);
        rows[i]->SetSelected(false);
        scrollView->Add(rows[i]);
    }
    for (size_t i = items.size(); i < shown; i++) {
        rows[i]->Release();
        rows[i]->SetSelected(false);
    }
    shown = items.size();

    scrollView->SetContentHeight(
        shown == 0 ? 0 : static_cast<pu::i32>(shown) * (rowHeight + ROW_SPACING) - ROW_SPACING + OUTLINE_PADDING * 2
    );
    // ScrollToOffset clamps, so a shorter list stops at its new end
    scrollView->ScrollToOffset(keepScroll ? previousOffset : 0, false);
    LOG_DEBUG(
        "Bag rows: " + std::to_string(shown) + " shown, " + std::to_string(rows.size() - built) + " newly built, " +
        std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
    selectedIndex = shown == 0 ? 0 : std::min(selected, shown - 1);
    if (shown > 0) {
        rows[selectedIndex]->SetSelected(true);
        EnsureRowVisible(selectedIndex, false);  // a rebuilt list appears in place, no glide
        if (focused) {
            rows[selectedIndex]->RequestFocus();
        }
    }
}

void pksm::ui::BagItemList::SetSelectedIndex(size_t index) {
    if (index >= shown || index == selectedIndex) {
        return;
    }
    rows[selectedIndex]->SetSelected(false);
    selectedIndex = index;
    rows[index]->SetSelected(true);
    EnsureRowVisible(index);
    if (focused) {
        rows[index]->RequestFocus();
    }
}

void pksm::ui::BagItemList::EnsureRowVisible(size_t index, bool animated) {
    // Row extents in scroll-content coordinates, outline included
    const pu::i32 top = static_cast<pu::i32>(index) * (rowHeight + ROW_SPACING);
    const pu::i32 bottom = top + rowHeight + (OUTLINE_PADDING * 2);
    const pu::i32 offset = scrollView->GetScrollOffset();
    if (top < offset) {
        scrollView->ScrollToOffset(top, animated);
    } else if (bottom > offset + scrollView->GetHeight()) {
        scrollView->ScrollToOffset(bottom - scrollView->GetHeight(), animated);
    }
}

void pksm::ui::BagItemList::SetFocused(bool focus) {
    if (focused == focus) {
        return;
    }
    focused = focus;
    scrollView->SetFocused(focus);
    if (!focus) {
        inputHandler.ClearState();
        pageRepeat.Reset();  // a hold cannot span a focus change
    } else if (selectedIndex < shown) {
        rows[selectedIndex]->RequestFocus();
    }
    if (onFocusChangedCallback) {
        onFocusChangedCallback();
    }
}

bool pksm::ui::BagItemList::IsFocused() const {
    return focused;
}

void pksm::ui::BagItemList::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    scrollView->OnRender(drawer, x, y);
}

void pksm::ui::BagItemList::OnInput(
    const u64 keys_down,
    const u64 keys_up,
    const u64 keys_held,
    const pu::ui::TouchPoint touch_pos
) {
    if (disabled) {
        return;
    }
    if (focused) {
        // The right stick pages; the row-by-row handler must not also step on it
        inputHandler.HandleInput(keys_down & ~PAGE_BUTTONS, keys_held & ~PAGE_BUTTONS);
        if (pageRepeat.Update(keys_down & PAGE_BUTTONS, keys_held & PAGE_BUTTONS)) {
            PageBy((keys_held & HidNpadButton_StickRDown) ? 1 : -1);
        }
    }
    // The scroll view owns touch for its rows
    scrollView->OnInput(keys_down, keys_up, keys_held, touch_pos);
    if (touch_pos.IsEmpty() && selectedIndex < shown) {
        rows[selectedIndex]->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}

void pksm::ui::BagItemList::SetRowDetail(size_t index, const std::string& detail, bool edited) {
    if (index < shown) {
        rows[index]->SetDetail(detail, edited);
    }
}

void pksm::ui::BagItemList::PageBy(int pages) {
    if (shown == 0) {
        return;
    }
    const int visible = std::max<pu::i32>(1, height / (rowHeight + ROW_SPACING));
    const int last = static_cast<int>(shown) - 1;
    const int target = std::clamp(static_cast<int>(selectedIndex) + pages * visible, 0, last);
    if (target == static_cast<int>(selectedIndex)) {
        rows[selectedIndex]->shakeOutOfBounds(pages > 0 ? ShakeDirection::DOWN : ShakeDirection::UP);
        return;
    }
    SetSelectedIndex(static_cast<size_t>(target));
}

void pksm::ui::BagItemList::ReleaseOffscreenRows(pu::i32 offset) {
    const pu::i32 stride = rowHeight + ROW_SPACING;
    const size_t first = static_cast<size_t>(std::max<pu::i32>(0, offset / stride));
    const size_t last = static_cast<size_t>(std::max<pu::i32>(0, (offset + height) / stride));
    for (size_t i = 0; i < shown; i++) {
        if (i + RELEASE_MARGIN < first || i > last + RELEASE_MARGIN) {
            rows[i]->Release();
        }
    }
}
