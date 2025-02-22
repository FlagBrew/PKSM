#pragma once

#include <functional>
#include <pu/Plutonium>

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

    void OnInput(u64 down, u64 up, u64 held);
};

}  // namespace pksm::layout