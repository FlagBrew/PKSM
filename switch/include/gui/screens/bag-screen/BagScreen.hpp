#pragma once

#include <functional>
#include <pu/Plutonium>
#include <vector>

#include "data/bag/BagData.hpp"
#include "data/bag/EditedStacks.hpp"
#include "data/providers/interfaces/IBagDataProvider.hpp"
#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "gui/screens/bag-screen/BagHelp.hpp"
#include "gui/shared/components/AnimatedBackground.hpp"
#include "gui/shared/components/BagItemList.hpp"
#include "gui/shared/components/BaseLayout.hpp"
#include "gui/shared/components/ItemPicker.hpp"
#include "gui/shared/components/PouchColumn.hpp"
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
        std::function<
            int(const std::string& title, const std::string& message, const std::vector<std::string>& options)>
            requestChoice,
        ISaveDataAccessor::Ref saveDataAccessor,
        IBagDataProvider::Ref bagDataProvider
    );
    PU_SMART_CTOR(BagScreen)

private:
    pu::ui::elm::Element::Ref background;
    pu::ui::Color bgColor = pu::ui::Color(176, 112, 16, 255);
    std::function<void()> onBack;
    // Blocking choice among options, the last one cancelling; the index taken, negative for none.
    // Plutonium dialogs are application-level
    std::function<int(const std::string& title, const std::string& message, const std::vector<std::string>& options)>
        requestChoice;
    ISaveDataAccessor::Ref saveDataAccessor;
    IBagDataProvider::Ref bagDataProvider;

    pksm::bag::BagData bag;
    pksm::bag::EditedStacks editedStacks;
    // ZL/ZR: by one, then by ten and a hundred the longer the hold
    pksm::input::HoldRepeat adjustRepeat{450, 90};
    static constexpr size_t NO_POUCH = SIZE_MAX;
    size_t currentPouch = NO_POUCH;

    pksm::ui::PouchColumn::Ref pouchColumn;
    pu::ui::elm::TextBlock::Ref pouchCount;
    pu::ui::elm::TextBlock::Ref emptyNotice;
    pksm::ui::BagItemList::Ref itemList;
    // Stands in for the list while an item is being added
    pksm::ui::ItemPicker::Ref picker;
    bool pickerFromList = false;  // where focus returns when the picker closes without a choice
    // The row Y picked up, by its index before the carry; the list carries it, the save is untouched until the drop
    static constexpr size_t NOT_LIFTED = SIZE_MAX;
    size_t liftedFrom = NOT_LIFTED;

    // Layout constants
    static constexpr pu::i32 SIDE_MARGIN = 80;
    static constexpr pu::i32 TOP_MARGIN = 120;
    static constexpr pu::i32 LIST_X = 600;
    static constexpr pu::i32 LIST_Y = 190;
    static constexpr pu::i32 LIST_BOTTOM_MARGIN = 10;
    static constexpr pu::i32 ROW_HEIGHT = 88;

    // Focus management
    pksm::input::FocusManager::Ref focusManager;
    pksm::input::FocusManager::Ref itemListFocusManager;
    pksm::input::FocusManager::Ref pickerFocusManager;

    // Input handlers; the picker's buttons replace the others while it is open, the carry's while a row is lifted
    pksm::input::DirectionalInputHandler pouchDirectionalHandler;
    pksm::input::DirectionalInputHandler listDirectionalHandler;
    pksm::input::ButtonInputHandler buttonHandler;
    pksm::input::ButtonInputHandler pickerButtonHandler;
    pksm::input::ButtonInputHandler carryButtonHandler;

    pu::i32 ListWidth() { return GetWidth() - LIST_X - SIDE_MARGIN; }
    pu::i32 ListHeight() { return GetHeight() - LIST_Y - pksm::ui::HelpFooter::FOOTER_HEIGHT - LIST_BOTTOM_MARGIN; }

    pu::ui::elm::TextBlock::Ref AddText(pu::i32 x, pu::i32 y, const std::string& text, const std::string& font);
    void OnInput(u64 down, u64 up, u64 held);
    void InitializePouchColumn();
    void InitializeItemList();
    void InitializePicker();
    void ShowPouch(size_t index);
    void FocusItemList();
    void StepPouch(int delta);
    void HandleBackButton();
    BagHelpState HelpState() const;
    void UpdateHelpItems();
    // The column is out of reach while the help overlay, the picker or a carry owns the screen
    void UpdatePouchColumn();

    // Editing the focused row; every write goes through the provider and refreshes the pouch
    bool CanEditCount() const;
    bool CanRemove() const;
    void AdjustCount(int delta);
    void HandleHolds(u64 down, u64 held);
    void PromptCount();
    void RemoveItem();
    // rebind redraws every row even when their number held (a mark can reorder a sorted pouch)
    void ApplyPouch(pksm::bag::Pouch pouch, size_t selected, bool rebind = false);
    // The game's marks on the cursor row: the right stick's click flips favourite, the left's the red dot
    const pksm::bag::Slot* CursorSlot() const;
    void ToggleMark(bool favoriteMark);

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

    // Reordering, where the game keeps a slot array. In the list Y lifts a row, the cursor carries
    // it, Y drops it into the save and B puts it back where it was; on the pouch column Y sorts.
    // A pouch the game orders itself offers the game's own options there instead
    bool CanReorder() const;
    bool CanSort() const;
    void LiftItem();
    void DropItem();
    void PutBack();
    void EndCarry();
    void SortPouch();
    void ChoosePouchSort();

    // Override BaseLayout methods
    std::vector<pksm::ui::HelpItem> GetHelpOverlayItems() const override;
    void OnHelpOverlayShown() override;
    void OnHelpOverlayHidden() override;
};

}  // namespace pksm::layout
