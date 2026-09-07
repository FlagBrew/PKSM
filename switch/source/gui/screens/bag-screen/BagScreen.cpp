#include "gui/screens/bag-screen/BagScreen.hpp"

#include <algorithm>
#include <switch.h>

#include "utils/Logger.hpp"
#include "utils/PouchGlyphs.hpp"
#include "utils/SoftwareKeyboard.hpp"

namespace {

// Numeric keyboard bounded to [min, max]; nothing when cancelled
std::optional<int> PromptQuantity(const std::string& header, int initial, int min, int max) {
    const auto entered = pksm::utils::ShowKeyboard(
        header,
        std::to_string(initial),
        std::to_string(max).size(),
        [min, max](const std::string& text) -> std::optional<std::string> {
            const bool numeric = !text.empty() && text.find_first_not_of("0123456789") == std::string::npos;
            const int value = numeric ? std::stoi(text) : -1;
            if (value < min || value > max) {
                return "Enter a number from " + std::to_string(min) + " to " + std::to_string(max);
            }
            return std::nullopt;
        },
        true
    );
    if (!entered) {
        return std::nullopt;
    }
    return std::stoi(*entered);
}

// What a pouch holds, for the count and the sort prompt
std::string Noun(const pksm::bag::Pouch& pouch) {
    return pouch.pouch == ::pksm::Sav::Pouch::Donut ? "donut" : "item";
}


}  // namespace

namespace pksm::layout {

BagScreen::BagScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    std::function<int(const pksm::ui::ChoiceDialog&)> requestChoice,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBagDataProvider::Ref bagDataProvider
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    onBack(onBack),
    requestChoice(requestChoice),
    saveDataAccessor(saveDataAccessor),
    bagDataProvider(bagDataProvider) {
    LOG_DEBUG("Initializing BagScreen...");

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    focusManager = pksm::input::FocusManager::New("BagScreen Manager");
    focusManager->SetActive(true);  // since this is the root manager
    itemListFocusManager = pksm::input::FocusManager::New("BagItemList Manager");
    focusManager->RegisterChildManager(itemListFocusManager);

    const u64 t0 = armGetSystemTick();
    bag = bagDataProvider->GetBag(saveDataAccessor->getCurrentSaveData());
    editedStacks.Reset(bag);
    size_t itemCount = 0;
    for (const auto& pouch : bag.pouches) {
        itemCount += pouch.items.size();
    }
    LOG_DEBUG(
        "Bag read: " + std::to_string(bag.pouches.size()) + " pouches, " + std::to_string(itemCount) + " items, " +
        std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
    InitializePouchColumn();
    InitializeItemList();
    shortcuts = std::make_unique<BagShortcuts>(
        bag,
        itemList,
        bagDataProvider,
        saveDataAccessor,
        BagShortcuts::Hooks{
            .requestChoice = requestChoice,
            .applyPouch =
                [this](pksm::bag::Pouch pouch) { ApplyPouch(std::move(pouch), itemList->GetSelectedIndex(), true); },
            .shakeCursor =
                [this]() {
                    if (auto row = itemList->GetItemAtIndex(itemList->GetSelectedIndex())) {
                        row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);
                    }
                },
            .beforeDialog = [this]() { adjustRepeat.Reset(); },
        }
    );
    shortcuts->PushGlyphs();
    InitializePicker();
    InitializeHelpFooter();

    pouchDirectionalHandler.SetOnMoveUp([this]() {
        if (currentPouch > 0) {
            pouchColumn->RequestFocus(currentPouch - 1);
        } else {
            pouchColumn->ShakeSelected(ui::ShakeDirection::UP);
        }
    });
    pouchDirectionalHandler.SetOnMoveDown([this]() {
        if (currentPouch + 1 < bag.pouches.size()) {
            pouchColumn->RequestFocus(currentPouch + 1);
        } else {
            pouchColumn->ShakeSelected(ui::ShakeDirection::DOWN);
        }
    });
    pouchDirectionalHandler.SetOnMoveLeft([this]() { pouchColumn->ShakeSelected(ui::ShakeDirection::LEFT); });
    pouchDirectionalHandler.SetOnMoveRight([this]() { FocusItemList(); });
    listDirectionalHandler.SetOnMoveLeft([this]() { pouchColumn->RequestFocus(currentPouch); });

    buttonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() { HandleBackButton(); });
    buttonHandler.RegisterButton(HidNpadButton_L, nullptr, [this]() { StepPouch(-1); });
    buttonHandler.RegisterButton(HidNpadButton_R, nullptr, [this]() { StepPouch(1); });
    const auto listFocused = [this]() { return itemList->IsFocused(); };
    buttonHandler.RegisterButton(HidNpadButton_A, nullptr, [this]() { PromptCount(); }, listFocused);
    buttonHandler.RegisterButton(HidNpadButton_X, nullptr, [this]() { RemoveItem(); }, listFocused);
    // Stick clicks, kept out of the footer: the help overlay names them where a format keeps the marks
    buttonHandler.RegisterButton(HidNpadButton_StickR, nullptr, [this]() { ToggleMark(true); }, listFocused);
    buttonHandler.RegisterButton(HidNpadButton_StickL, nullptr, [this]() { ToggleMark(false); }, listFocused);
    // The handler keeps one callback per button, so Y serves the list and the column from one
    buttonHandler.RegisterButton(HidNpadButton_Y, nullptr, [this]() {
        if (itemList->IsFocused()) {
            LiftItem();
        } else {
            SortPouch();
        }
    });
    buttonHandler.RegisterButton(HidNpadButton_Plus, nullptr, [this]() { OpenPicker(); });
    carryButtonHandler.RegisterButton(HidNpadButton_Y, nullptr, [this]() { DropItem(); });
    carryButtonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() { PutBack(); });
    pickerButtonHandler.RegisterButton(HidNpadButton_A, nullptr, [this]() { PickItem(); });
    pickerButtonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() { CancelPicker(); });
    pickerButtonHandler.RegisterButton(HidNpadButton_Y, nullptr, [this]() { SearchPicker(); }, [this]() {
        return picker->HasCandidates();
    });
    pickerButtonHandler.RegisterButton(HidNpadButton_X, nullptr, [this]() { ClearSearch(); }, [this]() {
        return !picker->GetSearch().empty();
    });

    if (!bag.pouches.empty()) {
        pouchColumn->RequestFocus(0);
    }

    this->SetOnInput(
        std::bind(&BagScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("BagScreen initialization complete");
    PreRender();
}

void BagScreen::InitializePouchColumn() {
    std::vector<pksm::ui::PouchColumn::Entry> pouches;
    for (const auto& pouch : bag.pouches) {
        pouches.push_back({pouch.name, utils::PouchGlyphs::Get(pouch.pouch, bag.storageFormat)});
    }
    pouchColumn = pksm::ui::PouchColumn::New(SIDE_MARGIN, TOP_MARGIN, LIST_X, TOP_MARGIN - 10, pouches, focusManager);
    pouchColumn->SetOnPouchFocused([this](size_t i) {
        // Focus returning from the list lands on the pouch already shown
        if (i != currentPouch) {
            ShowPouch(i);
        }
        UpdateHelpItems();
    });
    pouchColumn->SetOnPouchActivated([this]() { FocusItemList(); });
    this->Add(pouchColumn);
}

pu::ui::elm::TextBlock::Ref
BagScreen::AddText(pu::i32 x, pu::i32 y, const std::string& text, const std::string& font) {
    auto block = pu::ui::elm::TextBlock::New(x, y, text);
    block->SetColor(pksm::ui::global::TEXT_WHITE);
    block->SetFont(font);
    this->Add(block);
    return block;
}

void BagScreen::InitializeItemList() {
    pouchCount = AddText(0, TOP_MARGIN, "", pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_ACCOUNT_NAME));
    emptyNotice = AddText(
        LIST_X,
        LIST_Y + 16,
        "This pouch is empty",
        pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION)
    );
    emptyNotice->SetVisible(false);

    itemList = pksm::ui::BagItemList::New(LIST_X, LIST_Y, ListWidth(), ListHeight(), ROW_HEIGHT, itemListFocusManager);
    itemList->SetName("BagItemList Element");
    itemList->EstablishOwningRelationship();
    itemList->SetOnFocusChanged([this]() {
        adjustRepeat.Reset();  // a hold cannot span a focus change
        UpdateHelpItems();
    });
    this->Add(itemList);
}

void BagScreen::InitializePicker() {
    pickerFocusManager = pksm::input::FocusManager::New("ItemPicker Manager");
    focusManager->RegisterChildManager(pickerFocusManager);
    // Its header takes the pouch title's band, its list the bag list's place
    picker = pksm::ui::ItemPicker::New(
        LIST_X, TOP_MARGIN - 10, LIST_Y, ListWidth(), ListHeight(), ROW_HEIGHT, pickerFocusManager
    );
    this->Add(picker);
}

void BagScreen::ShowPouch(size_t index) {
    const u64 t0 = armGetSystemTick();
    currentPouch = index;
    const auto& pouch = bag.pouches[index];
    pouchColumn->Select(index);
    pouchCount->SetText(std::to_string(pouch.items.size()) + " " + Noun(pouch) + (pouch.items.size() == 1 ? "" : "s"));
    pouchCount->SetX(LIST_X + ListWidth() - pouchCount->GetWidth());
    itemList->SetDataSource(pouch.items, bag.storageFormat, pouch.pouch);
    for (size_t i = 0; i < pouch.items.size(); i++) {
        if (editedStacks.IsEdited(index, pouch.items[i])) {
            itemList->SetRowDetail(i, "×" + std::to_string(pouch.items[i].count), true);
        }
    }
    itemList->SetVisible(!pouch.items.empty());
    emptyNotice->SetVisible(pouch.items.empty());
    LOG_DEBUG(
        "Pouch switch to " + pouch.name + ": " + std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
}

void BagScreen::FocusItemList() {
    if (bag.pouches[currentPouch].items.empty()) {
        pouchColumn->ShakeSelected(ui::ShakeDirection::RIGHT);
        return;
    }
    itemList->RequestFocus();
}

void BagScreen::StepPouch(int delta) {
    if (bag.pouches.empty()) {
        return;
    }
    const size_t count = bag.pouches.size();
    const size_t next = (currentPouch + (delta < 0 ? count - 1 : 1)) % count;
    if (itemList->IsFocused() && !bag.pouches[next].items.empty()) {
        ShowPouch(next);
    } else {
        pouchColumn->RequestFocus(next);
    }
}

void BagScreen::HandleBackButton() {
    if (itemList->IsFocused()) {
        pouchColumn->RequestFocus(currentPouch);
        return;
    }
    LOG_DEBUG("B button pressed, returning to main menu");
    if (onBack) {
        onBack();
    }
}

BagHelpState BagScreen::HelpState() const {
    return {
        .pickerOpen = picker->IsOpen(),
        .pickerHasRows = picker->HasRows(),
        .pickerHasCandidates = picker->HasCandidates(),
        .searching = !picker->GetSearch().empty(),
        .carrying = liftedFrom != NOT_LIFTED,
        .listFocused = itemList->IsFocused(),
        .canEditCount = CanEditCount(),
        .canRemove = CanRemove(),
        .canReorder = CanReorder(),
        .canSort = CanSort(),
        .canAdd = CanAdd(),
        .pouchOpenable = currentPouch < bag.pouches.size() && !bag.pouches[currentPouch].items.empty(),
        .canFavorite = bag.keepsFavorite && CursorMarkable(),
        .canMarkSeen = bag.keepsNewMark && CursorMarkable(),
        .rowFavorite = CursorSlot() != nullptr && CursorSlot()->favorite,
        .rowNew = CursorSlot() != nullptr && CursorSlot()->isNew,
        .shortcutAction =
            currentPouch < bag.pouches.size() ? shortcuts->Action(bag.pouches[currentPouch], CursorSlot()) : "",
    };
}

// Donuts are baked in-game and carry no marks, whatever their save keeps for items
bool BagScreen::CursorMarkable() const {
    return CursorSlot() != nullptr && bag.pouches[currentPouch].pouch != ::pksm::Sav::Pouch::Donut;
}

const pksm::bag::Slot* BagScreen::CursorSlot() const {
    if (currentPouch >= bag.pouches.size()) {
        return nullptr;
    }
    const auto& items = bag.pouches[currentPouch].items;
    const size_t index = itemList->GetSelectedIndex();
    return index < items.size() ? &items[index] : nullptr;
}

void BagScreen::ToggleMark(bool favoriteMark) {
    const auto* slot = CursorSlot();
    if (favoriteMark && slot && shortcuts->CanRegister(bag.pouches[currentPouch], slot)) {
        shortcuts->Toggle(bag.pouches[currentPouch], *slot);
        return;
    }
    if (!CursorMarkable() || !(favoriteMark ? bag.keepsFavorite : bag.keepsNewMark)) {
        if (auto row = itemList->GetItemAtIndex(itemList->GetSelectedIndex())) {
            row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);  // nothing here to mark
        }
        return;
    }
    const auto& pouch = bag.pouches[currentPouch];
    const u16 itemId = slot->itemId;
    const bool isNew = favoriteMark ? slot->isNew : !slot->isNew;
    const bool favorite = favoriteMark ? !slot->favorite : slot->favorite;
    auto updated =
        bagDataProvider->SetMarks(saveDataAccessor->getCurrentSaveData(), pouch.pouch, slot->slot, isNew, favorite);
    if (!updated) {
        return;
    }
    // A mark can move the row under a newest-first or favorites-first sort: rebind and follow it
    const auto& items = updated->items;
    const auto row = std::find_if(items.begin(), items.end(), [&](const pksm::bag::Slot& s) {
        return s.itemId == itemId;
    });
    const size_t index = row == items.end() ? itemList->GetSelectedIndex() : static_cast<size_t>(row - items.begin());
    ApplyPouch(std::move(*updated), index, true);
}

void BagScreen::UpdateHelpItems() {
    helpFooter->SetHelpItems(BagFooterHelp(HelpState()));
}

void BagScreen::UpdatePouchColumn() {
    const bool reachable = !isHelpOverlayVisible && !picker->IsOpen() && liftedFrom == NOT_LIFTED;
    pouchColumn->SetDisabled(!reachable);
}

void BagScreen::OnInput(u64 down, u64 up, u64 held) {
    if (HandleHelpInput(down)) {
        return;
    }
    if (picker->IsOpen()) {
        pickerButtonHandler.HandleInput(down, up, held);  // its list moves itself
        return;
    }
    if (liftedFrom != NOT_LIFTED) {
        carryButtonHandler.HandleInput(down, up, held);  // the list carries the row on its own Up/Down
        return;
    }
    if (itemList->IsFocused()) {
        // The list pages on the right stick; it must not also count as directional input here
        constexpr u64 PAGE = ui::BagItemList::PAGE_BUTTONS;
        listDirectionalHandler.HandleInput(down & ~PAGE, held & ~PAGE);
        HandleHolds(down, held);
    } else {
        pouchDirectionalHandler.HandleInput(down, held);
    }
    buttonHandler.HandleInput(down, up, held);
}

bool BagScreen::CanEditCount() const {
    if (currentPouch >= bag.pouches.size()) {
        return false;
    }
    const auto& pouch = bag.pouches[currentPouch];
    return pouch.maxCount > 1 && !pouch.items.empty();  // one-of-each pouches have nothing to set
}

// The same rule as CanAdd today: donuts are the game's to make and unmake
bool BagScreen::CanRemove() const {
    return currentPouch < bag.pouches.size() && bag.pouches[currentPouch].pouch != ::pksm::Sav::Pouch::Donut;
}

void BagScreen::ApplyPouch(pksm::bag::Pouch pouch, size_t selected, bool rebind) {
    const bool sameRows = !rebind && pouch.items.size() == bag.pouches[currentPouch].items.size();
    bag.pouches[currentPouch] = std::move(pouch);
    if (!bag.shortcuts.empty() && bag.pouches[currentPouch].pouch == ::pksm::Sav::Pouch::KeyItem) {
        shortcuts->Refresh();  // a removal or a registration changed the slots the rows show
    }
    const auto& items = bag.pouches[currentPouch].items;
    if (items.empty()) {
        // Nothing left to hold the cursor; the pouch column takes it
        ShowPouch(currentPouch);
        pouchColumn->RequestFocus(currentPouch);
        UpdateHelpItems();
        return;
    }
    if (!sameRows) {
        // A removal shifts the rows; a quantity change only re-rasterizes its own detail, and
        // after a move the rows already stand in the save's new order
        itemList->SetDataSource(items, bag.storageFormat, bag.pouches[currentPouch].pouch, selected, true);
        const std::string noun = Noun(bag.pouches[currentPouch]);
        pouchCount->SetText(std::to_string(items.size()) + " " + noun + (items.size() == 1 ? "" : "s"));
        pouchCount->SetX(LIST_X + ListWidth() - pouchCount->GetWidth());
    }
    for (size_t i = 0; i < items.size(); i++) {
        // The edited row is redrawn either way: a count put back to the save's loses its tint
        const bool edited = editedStacks.IsEdited(currentPouch, items[i]);
        if (edited || i == selected) {
            itemList->SetRowDetail(i, "×" + std::to_string(items[i].count), edited);
        }
    }
}

void BagScreen::AdjustCount(int delta) {
    const auto& pouch = bag.pouches[currentPouch];
    const size_t index = itemList->GetSelectedIndex();
    if (!CanEditCount() || index >= pouch.items.size()) {
        if (auto row = itemList->GetItemAtIndex(index)) {
            row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);
        }
        return;
    }
    const auto& slot = pouch.items[index];
    // Held adjustments stop at one; only Remove empties a slot
    const int next = std::clamp(slot.count + delta, 1, static_cast<int>(pouch.maxCount));
    if (next == slot.count) {
        itemList->GetItemAtIndex(index)->shakeOutOfBounds(delta > 0 ? ui::ShakeDirection::UP : ui::ShakeDirection::DOWN);
        return;
    }
    if (auto updated = bagDataProvider->SetCount(
            saveDataAccessor->getCurrentSaveData(), pouch.pouch, slot.slot, static_cast<u16>(next)
        )) {
        ApplyPouch(std::move(*updated), index);
    }
}

void BagScreen::HandleHolds(u64 down, u64 held) {
    constexpr u64 ADJUST = HidNpadButton_ZL | HidNpadButton_ZR;
    constexpr s64 TENS_AFTER_MS = 2000;
    constexpr s64 HUNDREDS_AFTER_MS = 4000;
    if (adjustRepeat.Update(down & ADJUST, held & ADJUST)) {
        const s64 heldMs = adjustRepeat.HeldMs();
        const int step = heldMs >= HUNDREDS_AFTER_MS ? 100 : heldMs >= TENS_AFTER_MS ? 10 : 1;
        AdjustCount((held & HidNpadButton_ZR) ? step : -step);
    }
}

void BagScreen::PromptCount() {
    const auto& pouch = bag.pouches[currentPouch];
    const size_t index = itemList->GetSelectedIndex();
    adjustRepeat.Reset();  // the keyboard swallows the release
    if (!CanEditCount() || index >= pouch.items.size()) {
        if (auto row = itemList->GetItemAtIndex(index)) {
            row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);
        }
        return;
    }
    const auto& slot = pouch.items[index];
    const auto next = PromptQuantity(
        slot.name + ": quantity, 0 to " + std::to_string(pouch.maxCount) + " (0 removes)", slot.count, 0, pouch.maxCount
    );
    if (!next || *next == slot.count) {
        return;
    }
    if (auto updated = bagDataProvider->SetCount(
            saveDataAccessor->getCurrentSaveData(), pouch.pouch, slot.slot, static_cast<u16>(*next)
        )) {
        ApplyPouch(std::move(*updated), index);
    }
}

bool BagScreen::CanAdd() const {
    return currentPouch < bag.pouches.size() && bag.pouches[currentPouch].pouch != ::pksm::Sav::Pouch::Donut;
}

void BagScreen::ShowPouchView(bool visible) {
    const auto& items = bag.pouches[currentPouch].items;
    pouchColumn->SetTitleVisible(visible);
    pouchCount->SetVisible(visible);
    itemList->SetVisible(visible && !items.empty());
    emptyNotice->SetVisible(visible && items.empty());
}

void BagScreen::OpenPicker() {
    if (!CanAdd()) {
        pouchColumn->ShakeSelected(ui::ShakeDirection::RIGHT);  // donuts are baked in-game
        return;
    }
    const auto& pouch = bag.pouches[currentPouch];
    const u64 t0 = armGetSystemTick();
    adjustRepeat.Reset();
    pickerFromList = itemList->IsFocused();
    // An indexed pouch is never full in this sense: with every item held there is nothing left
    const bool full = !pouch.indexedByItem && pouch.items.size() >= pouch.capacity;
    auto candidates = full ? std::vector<pksm::bag::Slot>{}
                           : bagDataProvider->GetAddable(saveDataAccessor->getCurrentSaveData(), pouch.pouch);
    const size_t count = candidates.size();
    ShowPouchView(false);
    picker->Open(
        pouch.name, std::move(candidates), bag.storageFormat, pouch.pouch, full ? "This pouch is full" : "Nothing left to add"
    );
    UpdatePouchColumn();
    UpdateHelpItems();
    LOG_DEBUG(
        "Picker for " + pouch.name + ": " + std::to_string(count) + " candidates, " +
        std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
}

void BagScreen::LeavePicker() {
    picker->Close();
    UpdatePouchColumn();
}

void BagScreen::RestorePouchView(bool toList) {
    ShowPouchView(true);
    if (toList && !bag.pouches[currentPouch].items.empty()) {
        itemList->RequestFocus();
    } else {
        pouchColumn->RequestFocus(currentPouch);
    }
    UpdateHelpItems();
}

void BagScreen::CancelPicker() {
    LeavePicker();
    RestorePouchView(pickerFromList);
}

void BagScreen::PickItem() {
    const auto chosen = picker->GetSelected();
    if (!chosen) {
        return;
    }
    const auto& pouch = bag.pouches[currentPouch];
    int count = 1;  // one-of-each pouches take the item as is
    if (pouch.maxCount > 1) {
        const auto entered = PromptQuantity(
            "Add " + chosen->name + ": quantity, 1 to " + std::to_string(pouch.maxCount), 1, 1, pouch.maxCount
        );
        if (!entered) {
            return;
        }
        count = *entered;
    }
    auto updated = bagDataProvider->Add(
        saveDataAccessor->getCurrentSaveData(), pouch.pouch, chosen->itemId, static_cast<u16>(count)
    );
    if (!updated) {
        LOG_ERROR("Bag refused to add item " + std::to_string(chosen->itemId) + " to " + pouch.name);
        picker->ShakeSelected();
        return;
    }
    const auto& items = updated->items;
    const auto row = std::find_if(items.begin(), items.end(), [&](const pksm::bag::Slot& slot) {
        return slot.itemId == chosen->itemId;
    });
    const size_t index = row == items.end() ? items.size() - 1 : static_cast<size_t>(row - items.begin());
    LeavePicker();
    ApplyPouch(std::move(*updated), index);
    RestorePouchView(true);  // the new row is the cursor row
}

void BagScreen::SearchPicker() {
    constexpr size_t SEARCH_MAX_CHARS = 24;
    const auto entered =
        utils::ShowKeyboard("Search " + bag.pouches[currentPouch].name, picker->GetSearch(), SEARCH_MAX_CHARS);
    if (!entered) {
        return;
    }
    picker->SetSearch(*entered);
    UpdateHelpItems();
}

void BagScreen::ClearSearch() {
    picker->SetSearch("");
    UpdateHelpItems();
}

void BagScreen::RemoveItem() {
    const auto& pouch = bag.pouches[currentPouch];
    const size_t index = itemList->GetSelectedIndex();
    adjustRepeat.Reset();  // the dialog swallows the release
    if (!CanRemove() || index >= pouch.items.size()) {
        if (auto row = itemList->GetItemAtIndex(index)) {
            row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);
        }
        return;
    }
    const auto& slot = pouch.items[index];
    if (requestChoice({"Remove Item", "Remove " + slot.name + " from the bag?", {"Remove", "Cancel"}}) != 0) {
        return;
    }
    if (auto updated = bagDataProvider->SetCount(saveDataAccessor->getCurrentSaveData(), pouch.pouch, slot.slot, 0)) {
        ApplyPouch(std::move(*updated), index);
    }
}

bool BagScreen::CanReorder() const {
    if (currentPouch >= bag.pouches.size()) {
        return false;
    }
    const auto& pouch = bag.pouches[currentPouch];
    return !pouch.indexedByItem && pouch.pouch != ::pksm::Sav::Pouch::Donut && pouch.items.size() > 1;
}

bool BagScreen::CanSort() const {
    return (currentPouch < bag.pouches.size() && !bag.pouches[currentPouch].sortOptions.empty()) || CanReorder();
}

void BagScreen::LiftItem() {
    const size_t index = itemList->GetSelectedIndex();
    if (!CanReorder() || index >= bag.pouches[currentPouch].items.size()) {
        if (auto row = itemList->GetItemAtIndex(index)) {
            row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);  // this pouch keeps its order
        }
        return;
    }
    adjustRepeat.Reset();
    liftedFrom = index;
    itemList->SetCarrying(true);
    UpdatePouchColumn();
    UpdateHelpItems();
}

void BagScreen::DropItem() {
    // The pouch still lists the save's order: the carry so far was only on screen
    const auto& pouch = bag.pouches[currentPouch];
    const size_t index = itemList->GetSelectedIndex();
    if (index == liftedFrom) {
        EndCarry();  // dropped where it was lifted: nothing to write
        return;
    }
    auto updated = bagDataProvider->Move(
        saveDataAccessor->getCurrentSaveData(), pouch.pouch, pouch.items[liftedFrom].slot, pouch.items[index].slot
    );
    if (!updated) {
        LOG_ERROR("Bag refused to move slot " + std::to_string(pouch.items[liftedFrom].slot) + " in " + pouch.name);
        PutBack();
        return;
    }
    EndCarry();
    ApplyPouch(std::move(*updated), index);
}

void BagScreen::PutBack() {
    itemList->SetSelectedIndex(liftedFrom);  // carries the row home
    EndCarry();
}

void BagScreen::EndCarry() {
    liftedFrom = NOT_LIFTED;
    itemList->SetCarrying(false);
    UpdatePouchColumn();
    UpdateHelpItems();
}

void BagScreen::SortPouch() {
    const auto& pouch = bag.pouches[currentPouch];
    if (!pouch.sortOptions.empty()) {
        ChoosePouchSort();
        return;
    }
    if (!CanReorder()) {
        pouchColumn->ShakeSelected(ui::ShakeDirection::RIGHT);  // this pouch keeps its order
        return;
    }
    std::vector<std::string> options{"Name", "Number"};
    std::vector<pksm::bag::SortOrder> orders{pksm::bag::SortOrder::Name, pksm::bag::SortOrder::Number};
    if (pouch.maxCount > 1) {  // one-of-each pouches have no quantities to order by
        options.push_back("Quantity");
        orders.push_back(pksm::bag::SortOrder::Quantity);
    }
    options.push_back("Cancel");
    const int choice = requestChoice({"Sort " + pouch.name, "Put the items in order by", options});
    if (choice < 0) {
        return;
    }
    if (auto updated = bagDataProvider->Sort(saveDataAccessor->getCurrentSaveData(), pouch.pouch, orders[choice])) {
        bag.pouches[currentPouch] = std::move(*updated);
        ShowPouch(currentPouch);  // every row moved: rebind from the top
    }
}

void BagScreen::ChoosePouchSort() {
    const auto& pouch = bag.pouches[currentPouch];
    std::vector<std::string> options = pouch.sortOptions;
    options[pouch.sortOption] += " (current)";
    options.push_back("Cancel");
    const int choice = requestChoice({"Sort " + pouch.name, "The game lists the " + Noun(pouch) + "s", options});
    if (choice < 0 || static_cast<size_t>(choice) == pouch.sortOption) {
        return;
    }
    const auto updated =
        bagDataProvider->SetPouchSort(saveDataAccessor->getCurrentSaveData(), pouch.pouch, static_cast<size_t>(choice));
    if (updated) {
        bag = std::move(*updated);  // a setting some games share across pouches; the stacks are the same
        ShowPouch(currentPouch);
    }
}

std::vector<pksm::ui::HelpItem> BagScreen::GetHelpOverlayItems() const {
    return BagOverlayHelp(HelpState());
}

void BagScreen::OnHelpOverlayShown() {
    UpdatePouchColumn();
    itemList->SetDisabled(true);
    picker->SetDisabled(true);
}

void BagScreen::OnHelpOverlayHidden() {
    UpdatePouchColumn();
    itemList->SetDisabled(false);
    picker->SetDisabled(false);
}

}  // namespace pksm::layout
