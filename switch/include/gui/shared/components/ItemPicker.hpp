#pragma once

#include <optional>
#include <pu/Plutonium>
#include <string>
#include <vector>

#include "data/bag/BagData.hpp"
#include "gui/shared/components/BagItemList.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::ui {

// Chooses one item to add to a pouch from the candidates it is given. Takes the bag list's
// place on the screen while open: a header names the pouch, a search narrows the rows by name,
// and the same list the bag uses shows what is left. The screen decides what a choice does
class ItemPicker : public pu::ui::elm::Element {
private:
    static constexpr pu::i32 STATUS_Y = 10;  // the count text sits a little below the title
    static constexpr pu::i32 NOTICE_Y = 16;  // an empty-state line where the first row would be

    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;
    bool open = false;

    pu::ui::elm::TextBlock::Ref title;
    pu::ui::elm::TextBlock::Ref status;
    pu::ui::elm::TextBlock::Ref notice;
    BagItemList::Ref list;

    std::vector<bag::Slot> candidates;
    std::vector<bag::Slot> matches;  // candidates whose names contain the search
    std::string search;
    std::string emptyNotice;  // shown when there are no candidates at all
    ::pksm::Generation storageFormat = ::pksm::Generation::UNUSED;
    ::pksm::Sav::Pouch pouch = ::pksm::Sav::Pouch::NormalItem;

    // Rebinds the rows and header to the current search
    void Refresh();

public:
    // The header band runs from y to listY, where the list starts; both x and width are shared
    ItemPicker(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 listY,
        const pu::i32 width,
        const pu::i32 listHeight,
        const pu::i32 rowHeight,
        input::FocusManager::Ref focusManager
    );
    PU_SMART_CTOR(ItemPicker)

    // Element implementation
    pu::i32 GetX() override { return x; }
    pu::i32 GetY() override { return y; }
    pu::i32 GetWidth() override { return width; }
    pu::i32 GetHeight() override { return height; }
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void
    OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override;

    // Shows the candidates for the named pouch with the search cleared and takes focus, so the
    // view it replaced lets go of its own; emptyNotice explains an empty candidate list. The
    // generation and pouch are sprite keys, opaque here as they are to the list
    void Open(
        const std::string& pouchName,
        std::vector<bag::Slot> candidates,
        ::pksm::Generation storageFormat,
        ::pksm::Sav::Pouch pouch,
        const std::string& emptyNotice
    );
    // Hides the picker and lets its rows go; focus is the caller's to place
    void Close();
    bool IsOpen() const { return open; }

    // Narrows the rows to names containing text, ignoring ASCII case; empty shows every candidate
    void SetSearch(const std::string& text);
    const std::string& GetSearch() const { return search; }

    bool HasCandidates() const { return !candidates.empty(); }
    bool HasRows() const { return !matches.empty(); }
    // The row under the cursor, if any
    std::optional<bag::Slot> GetSelected() const;
    // Shakes the cursor row, for a choice that could not be taken
    void ShakeSelected();

    // Ignore input (help overlay)
    void SetDisabled(bool disabled) { list->SetDisabled(disabled); }
};

}  // namespace pksm::ui
