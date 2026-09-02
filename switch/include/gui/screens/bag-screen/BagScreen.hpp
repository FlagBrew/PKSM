#pragma once

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
        ISaveDataAccessor::Ref saveDataAccessor,
        IBagDataProvider::Ref bagDataProvider
    );
    PU_SMART_CTOR(BagScreen)

private:
    pu::ui::elm::Element::Ref background;
    pu::ui::Color bgColor = pu::ui::Color(176, 112, 16, 255);
    std::function<void()> onBack;
    ISaveDataAccessor::Ref saveDataAccessor;
    IBagDataProvider::Ref bagDataProvider;

    pksm::bag::BagData bag;
    static constexpr size_t NO_POUCH = SIZE_MAX;
    size_t currentPouch = NO_POUCH;

    std::vector<pksm::ui::FocusableButton::Ref> pouchButtons;
    pu::ui::elm::Rectangle::Ref pouchMarker;
    // One title per pouch, rasterized up front: a switch only flips visibility
    std::vector<pu::ui::elm::TextBlock::Ref> pouchTitles;
    pu::ui::elm::TextBlock::Ref pouchCount;
    pu::ui::elm::TextBlock::Ref emptyNotice;
    pksm::ui::BagItemList::Ref itemList;

    // Layout constants
    static constexpr pu::i32 SIDE_MARGIN = 80;
    static constexpr pu::i32 TOP_MARGIN = 120;
    static constexpr pu::i32 POUCH_WIDTH = 400;
    static constexpr pu::i32 POUCH_HEIGHT = 72;
    static constexpr pu::i32 POUCH_SPACING = 12;
    static constexpr pu::i32 MARKER_WIDTH = 8;
    static constexpr pu::i32 MARKER_GAP = 10;
    static constexpr pu::i32 LIST_X = 600;
    static constexpr pu::i32 LIST_Y = 190;
    static constexpr pu::i32 LIST_BOTTOM_MARGIN = 10;
    static constexpr pu::i32 ROW_HEIGHT = 88;

    // Focus management
    pksm::input::FocusManager::Ref focusManager;
    pksm::input::FocusManager::Ref itemListFocusManager;

    // Input handlers
    pksm::input::DirectionalInputHandler pouchDirectionalHandler;
    pksm::input::DirectionalInputHandler listDirectionalHandler;
    pksm::input::ButtonInputHandler buttonHandler;

    static constexpr pu::i32 PouchY(size_t index) {
        return TOP_MARGIN + static_cast<pu::i32>(index) * (POUCH_HEIGHT + POUCH_SPACING);
    }
    pu::i32 ListWidth() { return GetWidth() - LIST_X - SIDE_MARGIN; }

    pu::ui::elm::TextBlock::Ref AddText(pu::i32 x, pu::i32 y, const std::string& text, const std::string& font);
    void OnInput(u64 down, u64 up, u64 held);
    void InitializePouchColumn();
    void InitializeItemList();
    void ShowPouch(size_t index);
    void FocusPouch(size_t index);
    void FocusItemList();
    void StepPouch(int delta);
    void HandleBackButton();
    void UpdateHelpItems();

    // Override BaseLayout methods
    std::vector<pksm::ui::HelpItem> GetHelpOverlayItems() const override;
    void OnHelpOverlayShown() override;
    void OnHelpOverlayHidden() override;
};

}  // namespace pksm::layout
