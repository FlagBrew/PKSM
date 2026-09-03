#include "gui/shared/components/BagItemList.hpp"

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

    SetFocusManager(parentFocusManager);
}

void pksm::ui::BagItemList::SetDataSource(
    const std::vector<bag::Slot>& items,
    ::pksm::Generation storageFormat,
    ::pksm::Sav::Pouch pouch
) {
    const u64 tTeardown = armGetSystemTick();
    const size_t oldRows = rows.size();
    for (auto& row : rows) {
        if (auto fm = this->focusManager.lock()) {
            fm->UnregisterFocusable(row);
        }
    }
    rows.clear();
    scrollView->Clear();

    const u64 t0 = armGetSystemTick();
    for (size_t i = 0; i < items.size(); i++) {
        const auto position = CalculateItemPosition(i);
        auto row = BagItemRow::New(position.first, position.second, width, rowHeight);
        row->SetName("BagItemRow " + std::to_string(i));
        const auto& slot = items[i];
        const u32 spriteKey = pouch == ::pksm::Sav::Pouch::Donut
            ? utils::ItemSpriteManager::DonutKey(slot.itemId, slot.variant)
            : utils::ItemSpriteManager::ItemKey(slot.itemId, storageFormat);
        row->SetItem(spriteKey, slot.name, slot.detail.empty() ? "×" + std::to_string(slot.count) : slot.detail);
        row->SetOnTouchSelect([this, i]() { SetSelectedIndex(i); });
        if (auto fm = this->focusManager.lock()) {
            fm->RegisterFocusable(row);
        }
        rows.push_back(row);
        scrollView->Add(row);
    }

    scrollView->SetContentHeight(
        rows.empty() ? 0 : static_cast<pu::i32>(rows.size()) * (rowHeight + ROW_SPACING) - ROW_SPACING + OUTLINE_PADDING * 2
    );
    scrollView->ScrollToOffset(0, false);
    LOG_DEBUG(
        "Bag rows: " + std::to_string(rows.size()) + " built in " +
        std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms, " + std::to_string(oldRows) +
        " old rows torn down in " + std::to_string(armTicksToNs(t0 - tTeardown) / 1000000) + " ms"
    );
    selectedIndex = 0;
    if (!rows.empty()) {
        rows[0]->SetSelected(true);
        if (focused) {
            rows[0]->RequestFocus();
        }
    }
}

void pksm::ui::BagItemList::SetSelectedIndex(size_t index) {
    if (index >= rows.size() || index == selectedIndex) {
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

void pksm::ui::BagItemList::EnsureRowVisible(size_t index) {
    // Row extents in scroll-content coordinates, outline included
    const pu::i32 top = static_cast<pu::i32>(index) * (rowHeight + ROW_SPACING);
    const pu::i32 bottom = top + rowHeight + (OUTLINE_PADDING * 2);
    const pu::i32 offset = scrollView->GetScrollOffset();
    if (top < offset) {
        scrollView->ScrollToOffset(top, true);
    } else if (bottom > offset + scrollView->GetHeight()) {
        scrollView->ScrollToOffset(bottom - scrollView->GetHeight(), true);
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
    } else if (selectedIndex < rows.size()) {
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
        inputHandler.HandleInput(keys_down, keys_held);
    }
    // The scroll view owns touch for its rows
    scrollView->OnInput(keys_down, keys_up, keys_held, touch_pos);
    if (touch_pos.IsEmpty() && selectedIndex < rows.size()) {
        rows[selectedIndex]->OnInput(keys_down, keys_up, keys_held, touch_pos);
    }
}
