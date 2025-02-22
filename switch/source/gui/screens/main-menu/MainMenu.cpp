#include "gui/screens/main-menu/MainMenu.hpp"

namespace pksm::layout {

MainMenu::MainMenu(std::function<void()> onBack) : Layout(), onBack(onBack) {
    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Set up input handling
    this->SetOnInput(
        std::bind(&MainMenu::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );
}

MainMenu::~MainMenu() = default;

void MainMenu::OnInput(u64 down, u64 up, u64 held) {
    if (down & HidNpadButton_B) {
        if (onBack) {
            onBack();
        }
    }
}

}  // namespace pksm::layout