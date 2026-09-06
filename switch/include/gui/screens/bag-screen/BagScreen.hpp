#pragma once

#include <unordered_set>

#include <functional>
#include <pu/Plutonium>
#include <vector>

#include "data/bag/BagData.hpp"
#include "data/providers/interfaces/IBagDataProvider.hpp"
#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "gui/shared/components/AnimatedBackground.hpp"
#include "gui/shared/components/BagItemList.hpp"
#include "gui/shared/components/BaseLayout.hpp"
#include "gui/shared/components/FocusableButton.hpp"
#include "gui/shared/components/ItemPicker.hpp"
#include "input/HoldRepeat.hpp"
#include "input/ButtonInputHandler.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::layout {

// Pouch column on the left, the focused pouch's items on the right
class BagScreen : public BaseLayout {
public:
    BagScreen(
        std::function<void()> onBack,
        std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
        std::function<void()> onHideOverlay,
        std::function<bool(const std::string& title, const std::string& message, const std::string& confirmLabel)>
            requestConfirmation,
        ISaveDataAccessor::Ref saveDataAccessor,
        IBagDataProvider::Ref bagDataProvider
    );
    PU_SMART_CTOR(BagScreen)

private:
    pu::ui::elm::Element::Ref background;
    pu::ui::Color bgColor = pu::ui::Color(176, 112, 16, 255);
    std::function<void()> onBack;
    // Blocking yes/no prompt; Plutonium dialogs are application-level
    std::function<bool(const std::string& title, const std::string& message, const std::string& confirmLabel)>
        requestConfirmation;
    ISaveDataAccessor::Ref saveDataAccessor;
    IBagDataProvider::Ref bagDataProvider;

    pksm::bag::BagData bag;
    // Per pouch, the (item, count) stacks as read from the save: a row not among them is edited
    std::vector<std::unordered_set<u32>> originalStacks;
    // ZL/ZR: by one, then by ten and a hundred the longer the hold
    pksm::input::HoldRepeat adjustRepeat{450, 90};
    static constexpr size_t NO_POUCH = SIZE_MAX;
    size_t currentPouch = NO_POUCH;

    std::vector<pksm::ui::FocusableButton::Ref> pouchButtons;
    pu::ui::elm::Rectangle::Ref pouchMarker;
    // One title per pouch, rasterized up front: a switch only flips visibility
    std::vector<pu::ui::elm::TextBlock::Ref> pouchTitles;
    pu::ui::elm::TextBlock::Ref pouchCount;
    pu::ui::elm::TextBlock::Ref emptyNotice;
    pksm::ui::BagItemList::Ref itemList;
    // Stands in for the list while an item is being added
    pksm::ui::ItemPicker::Ref picker;
    bool pickerFromList = false;  // where focus returns when the picker closes without a choice

    // Layout constants
    static constexpr pu::i32 SIDE_MARGIN = 80;
    static constexpr pu::i32 TOP_MARGIN = 120;
    static constexpr pu::i32 POUCH_WIDTH = 400;
    static constexpr pu::i32 POUCH_HEIGHT = 72;
    static constexpr pu::i32 POUCH_SPACING = 12;
    static constexpr pu::i32 GLYPH_SIZE = 44;
    static constexpr pu::i32 GLYPH_INSET = 16;
    static constexpr pu::i32 MARKER_WIDTH = 8;
    static constexpr pu::i32 MARKER_GAP = 10;
    static constexpr pu::i32 LIST_X = 600;
    static constexpr pu::i32 LIST_Y = 190;
    static constexpr pu::i32 LIST_BOTTOM_MARGIN = 10;
    static constexpr pu::i32 ROW_HEIGHT = 88;

    // Focus management
    pksm::input::FocusManager::Ref focusManager;
    pksm::input::FocusManager::Ref itemListFocusManager;
    pksm::input::FocusManager::Ref pickerFocusManager;

    // Input handlers; the picker's buttons replace the others while it is open
    pksm::input::DirectionalInputHandler pouchDirectionalHandler;
    pksm::input::DirectionalInputHandler listDirectionalHandler;
    pksm::input::ButtonInputHandler buttonHandler;
    pksm::input::ButtonInputHandler pickerButtonHandler;

    static constexpr pu::i32 PouchY(size_t index) {
        return TOP_MARGIN + static_cast<pu::i32>(index) * (POUCH_HEIGHT + POUCH_SPACING);
    }
    pu::i32 ListWidth() { return GetWidth() - LIST_X - SIDE_MARGIN; }
    pu::i32 ListHeight() { return GetHeight() - LIST_Y - pksm::ui::HelpFooter::FOOTER_HEIGHT - LIST_BOTTOM_MARGIN; }

    pu::ui::elm::TextBlock::Ref AddText(pu::i32 x, pu::i32 y, const std::string& text, const std::string& font);
    void OnInput(u64 down, u64 up, u64 held);
    void InitializePouchColumn();
    void InitializeItemList();
    void InitializePicker();
    void ShowPouch(size_t index);
    void FocusPouch(size_t index);
    void FocusItemList();
    void StepPouch(int delta);
    void HandleBackButton();
    void UpdateHelpItems();

    // Editing the focused row; every write goes through the provider and refreshes the pouch
    bool CanEditCount() const;
    void AdjustCount(int delta);
    void HandleHolds(u64 down, u64 held);
    void PromptCount();
    void RemoveItem();
    void ApplyPouch(pksm::bag::Pouch pouch, size_t selected);
    static u32 StackKey(const pksm::bag::Slot& slot);
    bool IsEdited(const pksm::bag::Slot& slot) const;

    // Adding an item: Plus swaps the list for the picker, a choice lands as the cursor row
    bool CanAdd() const;
    void ShowPouchView(bool visible);
    void OpenPicker();
    void LeavePicker();
    void RestorePouchView(bool toList);
    void CancelPicker();
    void PickItem();
    void SearchPicker();
    void ClearSearch();

    // Override BaseLayout methods
    std::vector<pksm::ui::HelpItem> GetHelpOverlayItems() const override;
    void OnHelpOverlayShown() override;
    void OnHelpOverlayHidden() override;
};

}  // namespace pksm::layout
