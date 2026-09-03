#include "gui/screens/bag-screen/BagScreen.hpp"

#include <switch.h>

#include "gui/shared/components/SpriteImage.hpp"
#include "utils/Logger.hpp"
#include "utils/PouchGlyphs.hpp"

namespace pksm::layout {

BagScreen::BagScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBagDataProvider::Ref bagDataProvider
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    onBack(onBack),
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
            ShowPouch(i);
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

    const pu::i32 listHeight = GetHeight() - LIST_Y - pksm::ui::HelpFooter::FOOTER_HEIGHT - LIST_BOTTOM_MARGIN;
    itemList = pksm::ui::BagItemList::New(LIST_X, LIST_Y, ListWidth(), listHeight, ROW_HEIGHT, itemListFocusManager);
    itemList->SetName("BagItemList Element");
    itemList->EstablishOwningRelationship();
    itemList->SetOnFocusChanged([this]() { UpdateHelpItems(); });
    this->Add(itemList);
}

void BagScreen::ShowPouch(size_t index) {
    // Focus returning from the list lands on the pouch already shown
    if (index == currentPouch) {
        return;
    }
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
    if (itemList->IsFocused()) {
        helpItems.push_back({{pksm::ui::global::ButtonGlyph::B}, "Pouches"});
    } else {
        if (currentPouch < bag.pouches.size() && !bag.pouches[currentPouch].items.empty()) {
            helpItems.push_back({{pksm::ui::global::ButtonGlyph::A}, "Open"});
        }
        helpItems.push_back({{pksm::ui::global::ButtonGlyph::B}, "Back"});
    }
    helpItems.push_back({{pksm::ui::global::ButtonGlyph::L, pksm::ui::global::ButtonGlyph::R}, "Switch Pouch"});
    helpItems.push_back({{pksm::ui::global::ButtonGlyph::DPad}, "Navigate"});
    helpFooter->SetHelpItems(helpItems);
}

void BagScreen::OnInput(u64 down, u64 up, u64 held) {
    if (HandleHelpInput(down)) {
        return;
    }
    if (itemList->IsFocused()) {
        listDirectionalHandler.HandleInput(down, held);
    } else {
        pouchDirectionalHandler.HandleInput(down, held);
    }
    buttonHandler.HandleInput(down, up, held);
}

std::vector<pksm::ui::HelpItem> BagScreen::GetHelpOverlayItems() const {
    std::vector<pksm::ui::HelpItem> items;
    if (itemList->IsFocused()) {
        items.push_back({{pksm::ui::global::ButtonGlyph::B}, "Back to Pouches"});
    } else {
        items.push_back({{pksm::ui::global::ButtonGlyph::A}, "Open Pouch"});
        items.push_back({{pksm::ui::global::ButtonGlyph::B}, "Back to Main Menu"});
    }
    items.push_back({{pksm::ui::global::ButtonGlyph::L, pksm::ui::global::ButtonGlyph::R}, "Switch Pouch"});
    items.push_back({{pksm::ui::global::ButtonGlyph::DPad}, "Navigate"});
    return items;
}

void BagScreen::OnHelpOverlayShown() {
    for (auto& button : pouchButtons) {
        button->SetDisabled(true);
    }
    itemList->SetDisabled(true);
}

void BagScreen::OnHelpOverlayHidden() {
    for (auto& button : pouchButtons) {
        button->SetDisabled(false);
    }
    itemList->SetDisabled(false);
}

}  // namespace pksm::layout
