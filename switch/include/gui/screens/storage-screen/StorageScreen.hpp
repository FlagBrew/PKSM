#pragma once

#include <functional>
#include <pu/Plutonium>

#include "gui/shared/components/AnimatedBackground.hpp"
#include "gui/shared/components/BaseLayout.hpp"
#include "gui/shared/components/BoxGrid.hpp"
#include "gui/shared/components/HelpFooter.hpp"
#include "gui/shared/components/HelpOverlay.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "input/directional/DirectionalInputHandler.hpp"
#include "input/visual-feedback/FocusManager.hpp"
#include "input/visual-feedback/SelectionManager.hpp"

namespace pksm::layout {

class StorageScreen : public BaseLayout {
public:
    StorageScreen(
        std::function<void()> onBack,
        std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
        std::function<void()> onHideOverlay
    );
    PU_SMART_CTOR(StorageScreen)
    ~StorageScreen();

private:
    pu::ui::elm::Element::Ref background;
    pu::ui::Color bgColor = pu::ui::Color(5, 171, 49, 255);
    std::function<void()> onBack;
    pksm::ui::BoxGrid::Ref boxGrid;

    // Layout constants
    static constexpr pu::i32 BOX_GRID_SIDE_MARGIN = 80;  // Margin from left edge
    static constexpr pu::i32 BOX_GRID_TOP_MARGIN = 120;  // Margin from top
    static constexpr pu::i32 BOX_ITEM_SIZE = 134;  // Size of each box item

    // Focus and selection management
    pksm::input::FocusManager::Ref storageScreenFocusManager;
    pksm::input::SelectionManager::Ref storageScreenSelectionManager;
    pksm::input::FocusManager::Ref boxGridFocusManager;
    pksm::input::SelectionManager::Ref boxGridSelectionManager;

    // Input handlers
    pksm::input::DirectionalInputHandler boxGridHandler;

    void OnInput(u64 down, u64 up, u64 held);
    void InitializeBoxGrid();
    void LoadMockPokemonData();
    void InitializeFocusManagement();

    // Override BaseLayout methods
    std::vector<pksm::ui::HelpItem> GetHelpOverlayItems() const override;
    void OnHelpOverlayShown() override;
    void OnHelpOverlayHidden() override;
};

}  // namespace pksm::layout