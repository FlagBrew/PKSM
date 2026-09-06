#include "gui/screens/bag-screen/BagScreen.hpp"

#include <algorithm>
#include <switch.h>

#include "gui/shared/components/SpriteImage.hpp"
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

}  // namespace

namespace pksm::layout {

BagScreen::BagScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    std::function<bool(const std::string& title, const std::string& message, const std::string& confirmLabel)>
        requestConfirmation,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBagDataProvider::Ref bagDataProvider
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    onBack(onBack),
    requestConfirmation(requestConfirmation),
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
    for (const auto& pouch : bag.pouches) {
        auto& stacks = originalStacks.emplace_back();
        for (const auto& slot : pouch.items) {
            stacks.insert(StackKey(slot));
        }
    }
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
    InitializePicker();
    InitializeHelpFooter();

    pouchDirectionalHandler.SetOnMoveUp([this]() {
        if (currentPouch > 0) {
            FocusPouch(currentPouch - 1);
        } else {
            pouchButtons[currentPouch]->shakeOutOfBounds(ui::ShakeDirection::UP);
        }
    });
    pouchDirectionalHandler.SetOnMoveDown([this]() {
        if (currentPouch + 1 < pouchButtons.size()) {
            FocusPouch(currentPouch + 1);
        } else {
            pouchButtons[currentPouch]->shakeOutOfBounds(ui::ShakeDirection::DOWN);
        }
    });
    pouchDirectionalHandler.SetOnMoveLeft([this]() {
        pouchButtons[currentPouch]->shakeOutOfBounds(ui::ShakeDirection::LEFT);
    });
    pouchDirectionalHandler.SetOnMoveRight([this]() { FocusItemList(); });
    listDirectionalHandler.SetOnMoveLeft([this]() { FocusPouch(currentPouch); });

    buttonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() { HandleBackButton(); });
    buttonHandler.RegisterButton(HidNpadButton_L, nullptr, [this]() { StepPouch(-1); });
    buttonHandler.RegisterButton(HidNpadButton_R, nullptr, [this]() { StepPouch(1); });
    const auto listFocused = [this]() { return itemList->IsFocused(); };
    buttonHandler.RegisterButton(HidNpadButton_A, nullptr, [this]() { PromptCount(); }, listFocused);
    buttonHandler.RegisterButton(HidNpadButton_X, nullptr, [this]() { RemoveItem(); }, listFocused);
    buttonHandler.RegisterButton(HidNpadButton_Plus, nullptr, [this]() { OpenPicker(); });
    pickerButtonHandler.RegisterButton(HidNpadButton_A, nullptr, [this]() { PickItem(); });
    pickerButtonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() { CancelPicker(); });
    pickerButtonHandler.RegisterButton(HidNpadButton_Y, nullptr, [this]() { SearchPicker(); }, [this]() {
        return picker->HasCandidates();
    });
    pickerButtonHandler.RegisterButton(HidNpadButton_X, nullptr, [this]() { ClearSearch(); }, [this]() {
        return !picker->GetSearch().empty();
    });

    if (!pouchButtons.empty()) {
        pouchButtons[0]->RequestFocus();
    }

    this->SetOnInput(
        std::bind(&BagScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("BagScreen initialization complete");
    PreRender();
}

void BagScreen::InitializePouchColumn() {
    for (size_t i = 0; i < bag.pouches.size(); i++) {
        auto button = pksm::ui::FocusableButton::New(
            SIDE_MARGIN,
            PouchY(i),
            POUCH_WIDTH,
            POUCH_HEIGHT,
            bag.pouches[i].name,
            pu::ui::Color(0, 0, 0, 70),
            pu::ui::Color(255, 255, 255, 80)
        );
        button->SetContentFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_ACCOUNT_NAME));
        button->SetName("Pouch button: " + bag.pouches[i].name);
        button->SetOnFocus([this, i]() {
            // Focus returning from the list lands on the pouch already shown
            if (i != currentPouch) {
                ShowPouch(i);
            }
            UpdateHelpItems();
        });
        button->SetOnClick([this]() { FocusItemList(); });
        focusManager->RegisterFocusable(button);
        this->Add(button);
        pouchButtons.push_back(button);
        // Drawn over the button, left of its centred name
        this->Add(pksm::ui::SpriteImage::New(
            SIDE_MARGIN + GLYPH_INSET,
            PouchY(i) + (POUCH_HEIGHT - GLYPH_SIZE) / 2,
            GLYPH_SIZE,
            GLYPH_SIZE,
            utils::PouchGlyphs::Get(bag.pouches[i].pouch, bag.storageFormat)
        ));

        auto title = AddText(
            LIST_X,
            TOP_MARGIN - 10,
            bag.pouches[i].name,
            pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_HEADER)
        );
        title->SetVisible(false);
        pouchTitles.push_back(title);
    }

    pouchMarker = pu::ui::elm::Rectangle::New(
        SIDE_MARGIN - MARKER_GAP - MARKER_WIDTH,
        PouchY(0),
        MARKER_WIDTH,
        POUCH_HEIGHT,
        pksm::ui::global::TEXT_WHITE
    );
    pouchMarker->SetVisible(!pouchButtons.empty());
    this->Add(pouchMarker);
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
    if (currentPouch != NO_POUCH) {
        pouchTitles[currentPouch]->SetVisible(false);
    }
    currentPouch = index;
    const auto& pouch = bag.pouches[index];
    pouchMarker->SetY(PouchY(index));
    pouchTitles[index]->SetVisible(true);
    const std::string noun = pouch.pouch == ::pksm::Sav::Pouch::Donut ? " donut" : " item";
    pouchCount->SetText(std::to_string(pouch.items.size()) + noun + (pouch.items.size() == 1 ? "" : "s"));
    pouchCount->SetX(LIST_X + ListWidth() - pouchCount->GetWidth());
    itemList->SetDataSource(pouch.items, bag.storageFormat, pouch.pouch);
    for (size_t i = 0; i < pouch.items.size(); i++) {
        if (IsEdited(pouch.items[i])) {
            itemList->SetRowDetail(i, "×" + std::to_string(pouch.items[i].count), true);
        }
    }
    itemList->SetVisible(!pouch.items.empty());
    emptyNotice->SetVisible(pouch.items.empty());
    LOG_DEBUG(
        "Pouch switch to " + pouch.name + ": " + std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
}

void BagScreen::FocusPouch(size_t index) {
    pouchButtons[index]->RequestFocus();
}

void BagScreen::FocusItemList() {
    if (bag.pouches[currentPouch].items.empty()) {
        pouchButtons[currentPouch]->shakeOutOfBounds(ui::ShakeDirection::RIGHT);
        return;
    }
    itemList->RequestFocus();
}

void BagScreen::StepPouch(int delta) {
    if (pouchButtons.empty()) {
        return;
    }
    const size_t count = pouchButtons.size();
    const size_t next = (currentPouch + (delta < 0 ? count - 1 : 1)) % count;
    if (itemList->IsFocused() && !bag.pouches[next].items.empty()) {
        ShowPouch(next);
    } else {
        FocusPouch(next);
    }
}

void BagScreen::HandleBackButton() {
    if (itemList->IsFocused()) {
        FocusPouch(currentPouch);
        return;
    }
    LOG_DEBUG("B button pressed, returning to main menu");
    if (onBack) {
        onBack();
    }
}

void BagScreen::UpdateHelpItems() {
    std::vector<pksm::ui::HelpItem> helpItems;
    if (picker->IsOpen()) {
        if (picker->HasRows()) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::A}, "Add"});
        }
        if (picker->HasCandidates()) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::Y}, "Search"});
        }
        if (!picker->GetSearch().empty()) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::X}, "Clear Search"});
        }
        helpItems.push_back({{pksm::ui::global::ButtonGlyph::B}, "Cancel"});
        helpFooter->SetHelpItems(helpItems);
        return;
    }
    if (itemList->IsFocused()) {
        if (CanEditCount()) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::A}, "Set Quantity"});
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::ZL, pksm::ui::global::ButtonGlyph::ZR}, "Adjust"});
        }
        if (bag.pouches[currentPouch].pouch != ::pksm::Sav::Pouch::Donut) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::X}, "Remove"});
        }
    } else {
        if (currentPouch < bag.pouches.size() && !bag.pouches[currentPouch].items.empty()) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::A}, "Open"});
        }
        helpItems.push_back({{pksm::ui::global::ButtonGlyph::B}, "Back"});
    }
    if (CanAdd()) {
        helpItems.push_back({{pksm::ui::global::ButtonGlyph::Plus}, "Add"});
    }
    // The footer keeps to what a newcomer needs; B out of the list and the d-pad go without saying
    helpItems.push_back({{pksm::ui::global::ButtonGlyph::L, pksm::ui::global::ButtonGlyph::R}, "Switch Pouch"});
    helpFooter->SetHelpItems(helpItems);
}

void BagScreen::OnInput(u64 down, u64 up, u64 held) {
    if (HandleHelpInput(down)) {
        return;
    }
    if (picker->IsOpen()) {
        pickerButtonHandler.HandleInput(down, up, held);  // its list moves itself
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

u32 BagScreen::StackKey(const pksm::bag::Slot& slot) {
    return (static_cast<u32>(slot.itemId) << 16) | slot.count;
}

bool BagScreen::IsEdited(const pksm::bag::Slot& slot) const {
    return !originalStacks[currentPouch].contains(StackKey(slot));
}

void BagScreen::ApplyPouch(pksm::bag::Pouch pouch, size_t selected) {
    const bool sameRows = pouch.items.size() == bag.pouches[currentPouch].items.size();
    bag.pouches[currentPouch] = std::move(pouch);
    const auto& items = bag.pouches[currentPouch].items;
    if (items.empty()) {
        // Nothing left to hold the cursor; the pouch column takes it
        ShowPouch(currentPouch);
        FocusPouch(currentPouch);
        UpdateHelpItems();
        return;
    }
    if (!sameRows) {
        // A removal shifts the rows; a quantity change only re-rasterizes its own detail
        itemList->SetDataSource(items, bag.storageFormat, bag.pouches[currentPouch].pouch, selected, true);
        const std::string noun = bag.pouches[currentPouch].pouch == ::pksm::Sav::Pouch::Donut ? " donut" : " item";
        pouchCount->SetText(std::to_string(items.size()) + noun + (items.size() == 1 ? "" : "s"));
        pouchCount->SetX(LIST_X + ListWidth() - pouchCount->GetWidth());
    }
    for (size_t i = 0; i < items.size(); i++) {
        // The edited row is redrawn either way: a count put back to the save's loses its tint
        const bool edited = IsEdited(items[i]);
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
    pouchTitles[currentPouch]->SetVisible(visible);
    pouchCount->SetVisible(visible);
    itemList->SetVisible(visible && !items.empty());
    emptyNotice->SetVisible(visible && items.empty());
}

void BagScreen::OpenPicker() {
    if (!CanAdd()) {
        if (currentPouch < pouchButtons.size()) {
            pouchButtons[currentPouch]->shakeOutOfBounds(ui::ShakeDirection::RIGHT);  // donuts are baked in-game
        }
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
    for (auto& button : pouchButtons) {
        button->SetDisabled(true);  // a tap on the column would change the pouch under the picker
    }
    picker->Open(
        pouch.name, std::move(candidates), bag.storageFormat, pouch.pouch, full ? "This pouch is full" : "Nothing left to add"
    );
    UpdateHelpItems();
    LOG_DEBUG(
        "Picker for " + pouch.name + ": " + std::to_string(count) + " candidates, " +
        std::to_string(armTicksToNs(armGetSystemTick() - t0) / 1000000) + " ms"
    );
}

void BagScreen::LeavePicker() {
    picker->Close();
    for (auto& button : pouchButtons) {
        button->SetDisabled(false);
    }
}

void BagScreen::RestorePouchView(bool toList) {
    ShowPouchView(true);
    if (toList && !bag.pouches[currentPouch].items.empty()) {
        itemList->RequestFocus();
    } else {
        FocusPouch(currentPouch);
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
    if (pouch.pouch == ::pksm::Sav::Pouch::Donut || index >= pouch.items.size()) {
        if (auto row = itemList->GetItemAtIndex(index)) {
            row->shakeOutOfBounds(ui::ShakeDirection::RIGHT);
        }
        return;
    }
    const auto& slot = pouch.items[index];
    if (!requestConfirmation("Remove Item", "Remove " + slot.name + " from the bag?", "Remove")) {
        return;
    }
    if (auto updated = bagDataProvider->SetCount(saveDataAccessor->getCurrentSaveData(), pouch.pouch, slot.slot, 0)) {
        ApplyPouch(std::move(*updated), index);
    }
}

std::vector<pksm::ui::HelpItem> BagScreen::GetHelpOverlayItems() const {
    std::vector<pksm::ui::HelpItem> items;
    if (picker->IsOpen()) {
        items.push_back({{pksm::ui::global::ButtonGlyph::A}, "Add Item"});
        items.push_back({{pksm::ui::global::ButtonGlyph::Y}, "Search by Name"});
        items.push_back({{pksm::ui::global::ButtonGlyph::X}, "Clear Search"});
        items.push_back({{pksm::ui::global::ButtonGlyph::RightAnalogStick}, "Page Up/Down"});
        items.push_back({{pksm::ui::global::ButtonGlyph::B}, "Cancel"});
        items.push_back({{pksm::ui::global::ButtonGlyph::DPad, pksm::ui::global::ButtonGlyph::LeftAnalogStick}, "Navigate"});
        return items;
    }
    if (itemList->IsFocused()) {
        if (CanEditCount()) {
            items.push_back({{pksm::ui::global::ButtonGlyph::A}, "Set Quantity"});
            items.push_back({{pksm::ui::global::ButtonGlyph::ZL, pksm::ui::global::ButtonGlyph::ZR}, "Adjust Quantity"});
        }
        if (bag.pouches[currentPouch].pouch != ::pksm::Sav::Pouch::Donut) {
            items.push_back({{pksm::ui::global::ButtonGlyph::X}, "Remove Item"});
        }
        items.push_back({{pksm::ui::global::ButtonGlyph::RightAnalogStick}, "Page Up/Down"});
        items.push_back({{pksm::ui::global::ButtonGlyph::B}, "Back to Pouches"});
    } else {
        items.push_back({{pksm::ui::global::ButtonGlyph::A}, "Open Pouch"});
        items.push_back({{pksm::ui::global::ButtonGlyph::B}, "Back to Main Menu"});
    }
    if (CanAdd()) {
        items.push_back({{pksm::ui::global::ButtonGlyph::Plus}, "Add Item"});
    }
    items.push_back({{pksm::ui::global::ButtonGlyph::L, pksm::ui::global::ButtonGlyph::R}, "Switch Pouch"});
    items.push_back({{pksm::ui::global::ButtonGlyph::DPad, pksm::ui::global::ButtonGlyph::LeftAnalogStick}, "Navigate"});
    return items;
}

void BagScreen::OnHelpOverlayShown() {
    for (auto& button : pouchButtons) {
        button->SetDisabled(true);
    }
    itemList->SetDisabled(true);
    picker->SetDisabled(true);
}

void BagScreen::OnHelpOverlayHidden() {
    for (auto& button : pouchButtons) {
        button->SetDisabled(!picker->IsOpen());  // the column stays out of reach behind the picker
    }
    itemList->SetDisabled(false);
    picker->SetDisabled(false);
}

}  // namespace pksm::layout
