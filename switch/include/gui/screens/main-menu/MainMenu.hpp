#pragma once

#include <functional>
#include <pu/Plutonium>

#include "gui/screens/main-menu/sub-components/TrainerInfo.hpp"
#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "gui/shared/components/AnimatedBackground.hpp"
#include "input/visual-feedback/FocusManager.hpp"

namespace pksm::layout {

class MainMenu : public pu::ui::Layout {
public:
    MainMenu(std::function<void()> onBack);
    PU_SMART_CTOR(MainMenu)
    ~MainMenu();

private:
    pu::ui::elm::Element::Ref background;
    pu::ui::Color bgColor = pu::ui::Color(39, 66, 164, 255);
    std::function<void()> onBack;
    pksm::ui::TrainerInfo::Ref trainerInfo;
    pksm::ui::MenuButtonGrid::Ref menuGrid;

    // Focus management
    input::FocusManager::Ref mainMenuFocusManager;
    input::FocusManager::Ref menuGridFocusManager;

    // Layout constants
    static constexpr pu::i32 TRAINER_INFO_SIDE_MARGIN = 40;  // Margin from screen edges
    static constexpr pu::i32 TRAINER_INFO_TOP_MARGIN = 60;  // Margin from top
    static constexpr pu::i32 TRAINER_INFO_WIDTH = 660;  // Fixed width for trainer info panel
    static constexpr pu::i32 MENU_GRID_TOP_MARGIN = 120;  // Margin from top
    static constexpr pu::i32 MENU_GRID_MARGIN = 32;  // Margin between TrainerInfo and MenuButtonGrid

    void OnInput(u64 down, u64 up, u64 held);
};

}  // namespace pksm::layout