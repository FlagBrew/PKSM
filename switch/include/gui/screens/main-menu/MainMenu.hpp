#pragma once

#include <functional>
#include <pu/Plutonium>

#include "gui/screens/main-menu/sub-components/TrainerInfo.hpp"
#include "gui/shared/components/AnimatedBackground.hpp"

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

    // Layout constants
    static constexpr pu::i32 TRAINER_INFO_SIDE_MARGIN = 40;  // Margin from screen edges
    static constexpr pu::i32 TRAINER_INFO_TOP_MARGIN = 60;  // Margin from top
    static constexpr pu::i32 TRAINER_INFO_WIDTH = 660;  // Fixed width for trainer info panel

    void OnInput(u64 down, u64 up, u64 held);
};

}  // namespace pksm::layout