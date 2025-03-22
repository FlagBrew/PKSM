#pragma once

#include <functional>
#include <pu/Plutonium>

#include "data/providers/interfaces/ISaveDataAccessor.hpp"
#include "gui/screens/main-menu/sub-components/TrainerInfo.hpp"
#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "gui/shared/components/AnimatedBackground.hpp"
#include "gui/shared/components/BaseLayout.hpp"
#include "gui/shared/components/HelpFooter.hpp"
#include "gui/shared/components/HelpOverlay.hpp"
#include "gui/shared/interfaces/IHelpProvider.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::layout {

class MainMenu : public BaseLayout {
public:
    MainMenu(
        std::function<void()> onBack,
        std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
        std::function<void()> onHideOverlay,
        ISaveDataAccessor::Ref saveDataAccessor
    );
    PU_SMART_CTOR(MainMenu)
    ~MainMenu();

private:
    pu::ui::elm::Element::Ref background;
    pu::ui::Color bgColor = pu::ui::Color(39, 66, 164, 255);
    std::function<void()> onBack;
    pksm::ui::TrainerInfo::Ref trainerInfo;
    pksm::ui::MenuButtonGrid::Ref menuGrid;
    ISaveDataAccessor::Ref saveDataAccessor;

    // Layout constants
    static constexpr pu::i32 TRAINER_INFO_SIDE_MARGIN = 40;  // Margin from screen edges
    static constexpr pu::i32 TRAINER_INFO_TOP_MARGIN = 60;  // Margin from top
    static constexpr pu::i32 TRAINER_INFO_WIDTH = 660;  // Fixed width for trainer info panel
    static constexpr pu::i32 MENU_GRID_TOP_MARGIN = 120;  // Margin from top
    static constexpr pu::i32 MENU_GRID_MARGIN = 32;  // Margin between TrainerInfo and MenuButtonGrid

    void OnInput(u64 down, u64 up, u64 held);
    void UpdateTrainerInfo();

    // Override BaseLayout methods
    std::vector<pksm::ui::HelpItem> GetHelpOverlayItems() const override;
    void OnHelpOverlayShown() override;
    void OnHelpOverlayHidden() override;
};

}  // namespace pksm::layout